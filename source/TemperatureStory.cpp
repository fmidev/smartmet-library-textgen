// ======================================================================
/*!
 * \file
 * \brief Implementation of class TemperatureStory
 */
// ======================================================================

#include "TemperatureStory.h"
#include "DefaultAcceptor.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "Integer.h"
#include "IntegerRange.h"
#include "HourPeriodGenerator.h"
#include "MessageLogger.h"
#include "NullPeriodGenerator.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "UnitFactory.h"
#include "WeatherFunction.h"
#include "WeatherParameter.h"
#include "WeatherPeriodTools.h"
#include "WeatherResult.h"
#include "WeekdayTools.h"

#include "boost/lexical_cast.hpp"

using namespace WeatherAnalysis;
using namespace boost;
using namespace std;

namespace
{
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Return temperature comparison phrase
   *
   * \param theMean1 The first mean
   * \param theMean2 The second mean
   * \param theVariable The variable containing the limits
   */
  // ----------------------------------------------------------------------

  string temperature_comparison(int theMean1,
								int theMean2,
								const string & theVariable)
  {
	using namespace Settings;

	const int significantly_higher = require_percentage(theVariable+"::significantly_higher");
	const int significantly_lower = require_percentage(theVariable+"::significantly_lower");
	const int higher = require_percentage(theVariable+"::higher");
	const int lower = require_percentage(theVariable+"::lower");
	const int somewhat_higher = require_percentage(theVariable+"::somewhat_higher");
	const int somewhat_lower = require_percentage(theVariable+"::somewhat_lower");

	if(theMean2 - theMean1 >= significantly_higher)
	  return "huomattavasti korkeampi";
	if(theMean2 - theMean1 >= higher)
	  return "korkeampi";
	if(theMean2 - theMean1 >= somewhat_higher)
	  return "hieman korkeampi";
	if(theMean1 - theMean2 >= significantly_lower)
	  return "huomattavasti alempi";
	if(theMean1 - theMean2 >= lower)
	  return "alempi";
	if(theMean1 - theMean2 >= somewhat_lower)
	  return "hieman alempi";
	return "suunnilleen sama";
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return temperature sentence
   *
   * Possible sentences are
   *
   *  - "noin x astetta"
   *  - "x...y astetta";
   *
   * \param theMinimum The minimum temperature
   * \param theMean The mean temperature
   * \param theMaximum The maximum temperature
   * \param theMinInterval The minimum interval limit
   * \param theZeroFlag True if zero is always intervalled
   * \return The sentence
   */
  // ----------------------------------------------------------------------

  TextGen::Sentence temperature_phrase(int theMinimum,
									   int theMean,
									   int theMaximum,
									   int theMinInterval,
									   bool theZeroFlag)
  {
	using namespace TextGen;
	Sentence sentence;

	bool range = true;

	if(theMinimum == theMaximum)
	  range = false;
	else if(theMaximum - theMinimum >= theMinInterval)
	  range = true;
	else if(theMinimum <= 0 && theMaximum >= 0)
	  range = true;
	else
	  range = false;

	if(range)
	  {
		sentence << IntegerRange(theMinimum,theMaximum)
				 << *UnitFactory::create(DegreesCelsius);
	  }
	else
	  {
		sentence << "noin"
				 << Integer(theMean)
				 << *UnitFactory::create(DegreesCelsius);
	  }

	return sentence;
  }

} // namespace anonymous




namespace TextGen
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------
  
  TemperatureStory::~TemperatureStory()
  {
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theForecastTime The forecast time
   * \param theSources The analysis sources
   * \param theArea The area to be analyzed 
   * \param thePeriod The period to be analyzed
   * \param theVariable The associated configuration variable
  */
  // ----------------------------------------------------------------------
  
  TemperatureStory::TemperatureStory(const NFmiTime & theForecastTime,
									 const AnalysisSources & theSources,
									 const WeatherArea & theArea,
									 const WeatherPeriod & thePeriod,
									 const string & theVariable)
	: itsForecastTime(theForecastTime)
	, itsSources(theSources)
	, itsArea(theArea)
	, itsPeriod(thePeriod)
	, itsVar(theVariable)
  {
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Test whether the given story is known to this class
   *
   * \param theName The story name
   * \return True if this class can generate the story
   */
  // ----------------------------------------------------------------------
  
  bool TemperatureStory::hasStory(const string & theName)
  {
	if(theName == "temperature_mean")
	  return true;
	if(theName == "temperature_meanmax")
	  return true;
	if(theName == "temperature_meanmin")
	  return true;
	if(theName == "temperature_dailymax")
	  return true;
	if(theName == "temperature_nightlymin")
	  return true;
	if(theName == "temperature_weekly_minmax")
	  return true;
	if(theName == "temperature_weekly_averages")
	  return true;
	return false;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Generate the desired story
   *
   * Throws if the story name is not recognized.
   *
   * \param theName The story name
   * \return Paragraph containing the story
   */
  // ----------------------------------------------------------------------.
  
  Paragraph TemperatureStory::makeStory(const string & theName) const
  {
	if(theName == "temperature_mean")
	  return mean();
	if(theName == "temperature_meanmax")
	  return meanmax();
	if(theName == "temperature_meanmin")
	  return meanmin();
	if(theName == "temperature_dailymax")
	  return dailymax();
	if(theName == "temperature_nightlymin")
	  return nightlymin();
	if(theName == "temperature_weekly_minmax")
	  return weekly_minmax();
	if(theName == "temperature_weekly_averages")
	  return weekly_averages();

	throw TextGenError("TemperatureStory cannot make story "+theName);

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on mean temperature
   *
   * \return The generated paragraph
   *
   * \todo Is throwing the best way to handle missing results?
   *
   * \see page_temperature_mean
   */
  // ----------------------------------------------------------------------
  
  Paragraph TemperatureStory::mean() const
  {
	MessageLogger log("TemperatureStory::mean");

	Paragraph paragraph;
	Sentence sentence;

	GridForecaster forecaster;

	WeatherResult result = forecaster.analyze(itsVar+"::fake::mean",
											  itsSources,
											  Temperature,
											  Mean,
											  Mean,
											  itsArea,
											  itsPeriod);

	if(result.value() == kFloatMissing)
	  throw TextGenError("Mean temperature not available");

	log << "Temperature Mean(Mean) = " << result << endl;

	sentence << "keskilämpötila"
			 << Integer(FmiRound(result.value()))
			 << *UnitFactory::create(DegreesCelsius);
	
	paragraph << sentence;
	log << paragraph;
	return paragraph;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on mean maximum temperature
   *
   * \return The story
   *
   * \see page_temperature_meanmax
   */
  // ----------------------------------------------------------------------

  Paragraph TemperatureStory::meanmax() const
  {
	MessageLogger log("TemperatureStory::meanmax");

	Paragraph paragraph;
	Sentence sentence;

	GridForecaster forecaster;

	HourPeriodGenerator periods(itsPeriod,06,18,06,18);

	WeatherResult result = forecaster.analyze(itsVar+"::fake::mean",
											  itsSources,
											  Temperature,
											  Mean,
											  Mean,
											  Maximum,
											  itsArea,
											  periods);

	if(result.value() == kFloatMissing)
	  throw TextGenError("Mean daily maximum temperature not available");

	log << "Temperature Mean(Mean(Maximum())) = " << result << endl;

	sentence << "keskimääräinen ylin lämpötila"
			 << Integer(FmiRound(result.value()))
			 << *UnitFactory::create(DegreesCelsius);
	paragraph << sentence;
	log << paragraph;
	return paragraph;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on mean minimum temperature
   *
   * Throws if the weather period does not evenly divide into 24 hour
   * segments.
   *
   * \return The story
   *
   * \see page_temperature_meanmin
   */
  // ----------------------------------------------------------------------

  Paragraph TemperatureStory::meanmin() const
  {
	MessageLogger log("TemperatureStory::meanmin");

	Paragraph paragraph;
	Sentence sentence;

	GridForecaster forecaster;

	HourPeriodGenerator periods(itsPeriod,18,06,18,06);

	WeatherResult result = forecaster.analyze(itsVar+"::fake::mean",
											  itsSources,
											  Temperature,
											  Mean,
											  Mean,
											  Minimum,
											  itsArea,
											  periods);

	if(result.value() == kFloatMissing)
	  throw TextGenError("Mean daily minimum temperature not available");

	log << "Temperature Mean(Mean(Minimum())) = " << result << endl;

	sentence << "keskimääräinen alin lämpötila"
			 << Integer(FmiRound(result.value()))
			 << *UnitFactory::create(DegreesCelsius);
	paragraph << sentence;
	log << paragraph;
	return paragraph;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on daily maximum temperature
   *
   * \return The story
   *
   * \see page_temperature_dailymax
   */
  // ----------------------------------------------------------------------

  Paragraph TemperatureStory::dailymax() const
  {
	MessageLogger log("TemperatureStory::dailymax");

	using namespace Settings;
	using namespace WeatherPeriodTools;

	Paragraph paragraph;

	const int starthour    = require_hour(itsVar+"::starthour");
	const int endhour      = require_hour(itsVar+"::endhour");
	const int maxstarthour = optional_hour(itsVar+"::maxstarthour",starthour);
	const int minendhour   = optional_hour(itsVar+"::minendhour",endhour);

	const int mininterval = optional_int(itsVar+"::mininterval",2);
	const bool interval_zero = optional_bool(itsVar+"::always_interval_zero",false);

	const bool prefer_dayname = optional_bool(itsVar+"::prefer_dayname",false);


	const int days = countPeriods(itsPeriod,
								  starthour,
								  endhour,
								  maxstarthour,
								  minendhour);


	WeatherPeriod period = getPeriod(itsPeriod,
									 1,
									 starthour,
									 endhour,
									 maxstarthour,
									 minendhour);


	GridForecaster forecaster;

	WeatherResult minresult = forecaster.analyze(itsVar+"::fake::day1::minimum",
												 itsSources,
												 Temperature,
												 Minimum,
												 Maximum,
												 itsArea,
												 period);

	WeatherResult meanresult = forecaster.analyze(itsVar+"::fake::day1::mean",
												  itsSources,
												  Temperature,
												  Mean,
												  Maximum,
												  itsArea,
												  period);

	WeatherResult maxresult = forecaster.analyze(itsVar+"::fake::day1::maximum",
												 itsSources,
												 Temperature,
												 Maximum,
												 Maximum,
												 itsArea,
												 period);

	if(minresult.value() == kFloatMissing ||
	   maxresult.value() == kFloatMissing ||
	   meanresult.value() == kFloatMissing)
	  throw TextGenError("TemperatureStory: MaxTemperature is not available");

	log << "Temperature Minimum(Maximum) day 1 = " << minresult << endl;
	log << "Temperature Mean(Maximum) day 1 = " << meanresult << endl;
	log << "Temperature Maximum(Maximum) day 1 = " << maxresult << endl;

	const int min1 = FmiRound(minresult.value());
	const int max1 = FmiRound(maxresult.value());
	const int mean1 = FmiRound(meanresult.value());

	Sentence sentence;
	sentence << "päivän ylin lämpötila"
			 << "on"
			 << WeekdayTools::on_weekday(period.localStartTime())
			 << temperature_phrase(min1,mean1,max1,mininterval,interval_zero);

	// Remaining days

	for(int p=2; p<=days; p++)
	  {
		period = getPeriod(itsPeriod,
						   p,
						   starthour,
						   endhour,
						   maxstarthour,
						   minendhour);

		const string var = (itsVar
							+ "::fake::day"
							+ lexical_cast<string>(p));

		minresult = forecaster.analyze(var+"::minimum",
									   itsSources,
									   Temperature,
									   Minimum,
									   Maximum,
									   itsArea,
									   period);
		
		maxresult = forecaster.analyze(var+"::maximum",
									   itsSources,
									   Temperature,
									   Maximum,
									   Maximum,
									   itsArea,
									   period);

		meanresult = forecaster.analyze(var+"::mean",
										itsSources,
										Temperature,
										Mean,
										Maximum,
										itsArea,
										period);
		
		if(minresult.value() == kFloatMissing ||
		   maxresult.value() == kFloatMissing ||
		   meanresult.value() == kFloatMissing)
		  throw TextGenError("TemperatureStory: MaxTemperature is not available for day "+lexical_cast<string>(p));


		log << "Temperature Minimum(Maximum) day " << p << " = " << minresult << endl;
		log << "Temperature Mean(Maximum) day " << p << " = " << meanresult << endl;
		log << "Temperature Maximum(Maximum) day " << p << " = " << maxresult << endl;
		
		const int min2  = FmiRound(minresult.value());
		const int max2  = FmiRound(maxresult.value());
		const int mean2 = FmiRound(meanresult.value());
		
		// For second day:
		//
		// "seuraavana päivänä [komparatiivi]" tai
		// "[viikonpäivänä] [komparatiivi]"
		//
		// For third and so on
		//
		// "[viikonpäivänä] [noin x|x...y] astetta"
		
		sentence << Delimiter(",");
		
		if(p==2)
		  {
			if(prefer_dayname)
			  sentence << WeekdayTools::on_weekday(period.localStartTime());
			else
			  sentence << "seuraavana päivänä";
			sentence << temperature_comparison(mean1,mean2,itsVar);
		  }
		else
		  {
			sentence << WeekdayTools::on_weekday(period.localStartTime())
					 << temperature_phrase(min2,mean2,max2,mininterval,interval_zero);

		  }
		
	  }

	paragraph << sentence;
	log << paragraph;
	return paragraph;

  }


  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on nightly minimum temperature
   *
   * \return The story
   *
   * \see page_temperature_nightlymin
   */
  // ----------------------------------------------------------------------

  Paragraph TemperatureStory::nightlymin() const
  {
	MessageLogger log("TemperatureStory::nightlymin");

	using namespace Settings;
	using namespace WeatherPeriodTools;

	Paragraph paragraph;

	const int starthour    = require_hour(itsVar+"::starthour");
	const int endhour      = require_hour(itsVar+"::endhour");
	const int maxstarthour = optional_hour(itsVar+"::maxstarthour",starthour);
	const int minendhour   = optional_hour(itsVar+"::minendhour",endhour);

	const int mininterval = optional_int(itsVar+"::mininterval",2);
	const bool interval_zero = optional_bool(itsVar+"::always_interval_zero",false);

	const bool prefer_dayname = optional_bool(itsVar+"::prefer_dayname",false);


	const int nights = countPeriods(itsPeriod,
									starthour,
									endhour,
									maxstarthour,
									minendhour);

	WeatherPeriod period = getPeriod(itsPeriod,
									 1,
									 starthour,
									 endhour,
									 maxstarthour,
									 minendhour);


	GridForecaster forecaster;

	WeatherResult minresult = forecaster.analyze(itsVar+"::fake::night1::minimum",
												 itsSources,
												 Temperature,
												 Minimum,
												 Minimum,
												 itsArea,
												 period);

	WeatherResult meanresult = forecaster.analyze(itsVar+"::fake::night1::mean",
												  itsSources,
												  Temperature,
												  Mean,
												  Minimum,
												  itsArea,
												  period);

	WeatherResult maxresult = forecaster.analyze(itsVar+"::fake::night1::maximum",
												 itsSources,
												 Temperature,
												 Maximum,
												 Minimum,
												 itsArea,
												 period);

	if(minresult.value() == kFloatMissing ||
	   maxresult.value() == kFloatMissing ||
	   meanresult.value() == kFloatMissing)
	  throw TextGenError("TemperatureStory: MinTemperature is not available");

	log << "Temperature Minimum(Minimum) night 1 = " << minresult << endl;
	log << "Temperature Mean(Minimum) night 1 = " << meanresult << endl;
	log << "Temperature Maximum(Minimum) night 1 = " << maxresult << endl;
	
	const int min1 = FmiRound(minresult.value());
	const int max1 = FmiRound(maxresult.value());
	const int mean1 = FmiRound(meanresult.value());

	Sentence sentence;
	sentence << "yön alin lämpötila"
			 << "on"
			 << WeekdayTools::night_against_weekday(period.localEndTime())
			 << temperature_phrase(min1,mean1,max1,mininterval,interval_zero);

	// Remaining nights

	for(int p=2; p<=nights; p++)
	  {
		period = getPeriod(itsPeriod,
						   p,
						   starthour,
						   endhour,
						   maxstarthour,
						   minendhour);
		
		const string var = (itsVar
							+ "::fake::night"
							+ lexical_cast<string>(p));

		minresult = forecaster.analyze(var+"::minimum",
									   itsSources,
									   Temperature,
									   Minimum,
									   Minimum,
									   itsArea,
									   period);
		
		maxresult = forecaster.analyze(var+"::maximum",
									   itsSources,
									   Temperature,
									   Maximum,
									   Minimum,
									   itsArea,
									   period);

		meanresult = forecaster.analyze(var+"::mean",
										itsSources,
										Temperature,
										Mean,
										Minimum,
										itsArea,
										period);
		
		if(minresult.value() == kFloatMissing ||
		   maxresult.value() == kFloatMissing ||
		   meanresult.value() == kFloatMissing)
		  throw TextGenError("TemperatureStory: MinTemperature is not available for night "+lexical_cast<string>(p));
		
		log << "Temperature Minimum(Minimum) night " << p << " = " << minresult << endl;
		log << "Temperature Mean(Minimum) night " << p << " = " << meanresult << endl;
		log << "Temperature Maximum(Minimum) night " << p << " = " << maxresult << endl;

		const int min2  = FmiRound(minresult.value());
		const int max2  = FmiRound(maxresult.value());
		const int mean2 = FmiRound(meanresult.value());
		
		// For second night:
		//
		// "seuraavana yönä [komparatiivi]" tai
		// "[viikonpäivän vastaisena yönä] [komparatiivi]"
		//
		// For third and so on
		//
		// "[viikonpäivän vastaisena yönä] [noin x|x...y] astetta"
		
		sentence << Delimiter(",");
		
		if(p==2)
		  {
			if(prefer_dayname)
			  sentence << WeekdayTools::night_against_weekday(period.localEndTime());
			else
			  sentence << "seuraavana yönä";
			sentence << temperature_comparison(mean1,mean2,itsVar);
		  }
		else
		  {
			sentence << WeekdayTools::night_against_weekday(period.localEndTime())
					 << temperature_phrase(min2,mean2,max2,mininterval,interval_zero);

		  }
		
	  }

	paragraph << sentence;
	log << paragraph;
	return paragraph;

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on several day minmax temperatures
   *
   * \return The story
   *
   * \see page_temperature_weekly_minmax
   *
   */
  // ----------------------------------------------------------------------

  Paragraph TemperatureStory::weekly_minmax() const
  {
	MessageLogger log("TemperatureStory::weekly_minmax");

	using namespace Settings;

	Paragraph paragraph;

	const int daymininterval = optional_int(itsVar+"::day::mininterval",2);
	const int nightmininterval = optional_int(itsVar+"::night::mininterval",2);
	const bool day_interval_zero = optional_bool(itsVar+"::day::always_interval_zero",false);
	const bool night_interval_zero = optional_bool(itsVar+"::night::always_interval_zero",false);
	const bool emphasize_night_minimum = optional_bool(itsVar+"::emphasize_night_minimum",false);

	const HourPeriodGenerator days(itsPeriod,itsVar+"::day");
	const HourPeriodGenerator nights(itsPeriod,itsVar+"::night");

	GridForecaster forecaster;

	const WeatherResult dayminresult
	  = forecaster.analyze(itsVar+"::fake::day::minimum",
						   itsSources,
						   Temperature,
						   Minimum,
						   Mean,
						   Maximum,
						   itsArea,
						   days);

	const WeatherResult daymaxresult
	  = forecaster.analyze(itsVar+"::fake::day::maximum",
						   itsSources,
						   Temperature,
						   Maximum,
						   Mean,
						   Maximum,
						   itsArea,
						   days);

	const WeatherResult daymeanresult
	  = forecaster.analyze(itsVar+"::fake::day::mean",
						   itsSources,
						   Temperature,
						   Mean,
						   Mean,
						   Maximum,
						   itsArea,
						   days);

	const WeatherResult nightminresult
	  = forecaster.analyze(itsVar+"::fake::night::minimum",
						   itsSources,
						   Temperature,
						   Minimum,
						   Mean,
						   Minimum,
						   itsArea,
						   nights);

	const WeatherResult nightmaxresult
	  = forecaster.analyze(itsVar+"::fake::night::maximum",
						   itsSources,
						   Temperature,
						   Maximum,
						   Mean,
						   Minimum,
						   itsArea,
						   nights);

	const WeatherResult nightmeanresult
	  = forecaster.analyze(itsVar+"::fake::night::mean",
						   itsSources,
						   Temperature,
						   Mean,
						   Mean,
						   Minimum,
						   itsArea,
						   nights);


	if(dayminresult.value() == kFloatMissing ||
	   daymaxresult.value() == kFloatMissing ||
	   daymeanresult.value() == kFloatMissing ||
	   nightminresult.value() == kFloatMissing ||
	   nightmaxresult.value() == kFloatMissing ||
	   nightmeanresult.value() == kFloatMissing)
	  throw TextGenError("Daily maximum and nightly minimum not available");

	log << "Temperature Minimum(Mean(Maximum)) = " << dayminresult << endl;
	log << "Temperature Mean(Mean(Maximum)) = " << daymeanresult << endl;
	log << "Temperature Maximum(Mean(Maximum)) = " << daymaxresult << endl;

	log << "Temperature Minimum(Mean(Minimum)) = " << nightminresult << endl;
	log << "Temperature Mean(Mean(Minimum)) = " << nightmeanresult << endl;
	log << "Temperature Maximum(Mean(Minimum)) = " << nightmaxresult << endl;

	const int daymin  = FmiRound(dayminresult.value());
	const int daymax  = FmiRound(daymaxresult.value());
	const int daymean = FmiRound(daymeanresult.value());
	const int nightmin  = FmiRound(nightminresult.value());
	const int nightmax  = FmiRound(nightmaxresult.value());
	const int nightmean = FmiRound(nightmeanresult.value());

	Sentence sentence;
	sentence << "päivien ylin lämpötila"
			 << "on"
			 << temperature_phrase(daymin,daymean,daymax,
								   daymininterval,day_interval_zero)
			 << Delimiter(",")
			 << "öiden alin lämpötila";
	if(emphasize_night_minimum)
	  {
		sentence << "noin"
				 << Integer(nightmin)
				 << *UnitFactory::create(DegreesCelsius);
	  }
	else
	  {
		sentence << temperature_phrase(nightmin,nightmean,nightmax,
									   nightmininterval,night_interval_zero);
	  }

	paragraph << sentence;
	log << paragraph;
	return paragraph;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on several day mean day/night temperatures
   *
   * \return The story
   *
   * \see page_temperature_weekly_averages
   *
   */
  // ----------------------------------------------------------------------

  Paragraph TemperatureStory::weekly_averages() const
  {
	MessageLogger log("TemperatureStory::weekly_averages");

	using namespace Settings;

	Paragraph paragraph;

	const int daymininterval = optional_int(itsVar+"::day::mininterval",2);
	const int nightmininterval = optional_int(itsVar+"::night::mininterval",2);
	const bool day_interval_zero = optional_bool(itsVar+"::day::always_interval_zero",false);
	const bool night_interval_zero = optional_bool(itsVar+"::night::always_interval_zero",false);

	const HourPeriodGenerator days(itsPeriod,itsVar+"::day");
	const HourPeriodGenerator nights(itsPeriod,itsVar+"::night");

	GridForecaster forecaster;

	const WeatherResult dayminresult
	  = forecaster.analyze(itsVar+"::fake::day::minimum",
						   itsSources,
						   Temperature,
						   Minimum,
						   Mean,
						   Mean,
						   itsArea,
						   days);

	const WeatherResult daymaxresult
	  = forecaster.analyze(itsVar+"::fake::day::maximum",
						   itsSources,
						   Temperature,
						   Maximum,
						   Mean,
						   Mean,
						   itsArea,
						   days);

	const WeatherResult daymeanresult
	  = forecaster.analyze(itsVar+"::fake::day::mean",
						   itsSources,
						   Temperature,
						   Mean,
						   Mean,
						   Mean,
						   itsArea,
						   days);

	const WeatherResult nightminresult
	  = forecaster.analyze(itsVar+"::fake::night::minimum",
						   itsSources,
						   Temperature,
						   Minimum,
						   Mean,
						   Mean,
						   itsArea,
						   nights);

	const WeatherResult nightmaxresult
	  = forecaster.analyze(itsVar+"::fake::night::maximum",
						   itsSources,
						   Temperature,
						   Maximum,
						   Mean,
						   Mean,
						   itsArea,
						   nights);

	const WeatherResult nightmeanresult
	  = forecaster.analyze(itsVar+"::fake::night::mean",
						   itsSources,
						   Temperature,
						   Mean,
						   Mean,
						   Mean,
						   itsArea,
						   nights);


	if(dayminresult.value() == kFloatMissing ||
	   daymaxresult.value() == kFloatMissing ||
	   daymeanresult.value() == kFloatMissing ||
	   nightminresult.value() == kFloatMissing ||
	   nightmaxresult.value() == kFloatMissing ||
	   nightmeanresult.value() == kFloatMissing)
	  throw TextGenError("Daily maximum and nightly minimum not available");

	log << "Temperature Minimum(Mean(Maximum)) = " << dayminresult << endl;
	log << "Temperature Mean(Mean(Maximum)) = " << daymeanresult << endl;
	log << "Temperature Maximum(Mean(Maximum)) = " << daymaxresult << endl;

	log << "Temperature Minimum(Mean(Minimum)) = " << nightminresult << endl;
	log << "Temperature Mean(Mean(Minimum)) = " << nightmeanresult << endl;
	log << "Temperature Maximum(Mean(Minimum)) = " << nightmaxresult << endl;

	const int daymin  = FmiRound(dayminresult.value());
	const int daymax  = FmiRound(daymaxresult.value());
	const int daymean = FmiRound(daymeanresult.value());
	const int nightmin  = FmiRound(nightminresult.value());
	const int nightmax  = FmiRound(nightmaxresult.value());
	const int nightmean = FmiRound(nightmeanresult.value());

	Sentence sentence;
	sentence << "päivälämpötila"
			 << "on"
			 << temperature_phrase(daymin,daymean,daymax,
								   daymininterval,day_interval_zero)
			 << Delimiter(",")
			 << "yölämpötila"
			 << temperature_phrase(nightmin,nightmean,nightmax,
								   nightmininterval,night_interval_zero);

	paragraph << sentence;
	log << paragraph;
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
