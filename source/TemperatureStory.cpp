// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::TemperatureStory
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
#include "PeriodPhraseFactory.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "TemperatureStoryTools.h"
#include "UnitFactory.h"
#include "WeatherFunction.h"
#include "WeatherParameter.h"
#include "WeatherPeriodTools.h"
#include "WeatherResult.h"
#include "WeekdayTools.h"

#include "boost/lexical_cast.hpp"

using namespace WeatherAnalysis;
using namespace TextGen::TemperatureStoryTools;
using namespace boost;
using namespace std;

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

	const int starthour    = require_hour(itsVar+"::day::starthour");
	const int endhour      = require_hour(itsVar+"::day::endhour");
	const int maxstarthour = optional_hour(itsVar+"::day::maxstarthour",starthour);
	const int minendhour   = optional_hour(itsVar+"::day::minendhour",endhour);

	const int mininterval = optional_int(itsVar+"::mininterval",2);
	const bool interval_zero = optional_bool(itsVar+"::always_interval_zero",false);

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
			 << PeriodPhraseFactory::create("today",
											itsVar,
											itsForecastTime,
											period)
			 << temperature_sentence(min1,mean1,max1,mininterval,interval_zero);

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
			sentence << PeriodPhraseFactory::create("next_day",
													itsVar,
													itsForecastTime,
													period);
			sentence << temperature_comparison_phrase(mean1,mean2,itsVar);
		  }
		else
		  {
			sentence << WeekdayTools::on_weekday(period.localStartTime())
					 << temperature_sentence(min2,mean2,max2,mininterval,interval_zero);

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

	const int starthour    = require_hour(itsVar+"::night::starthour");
	const int endhour      = require_hour(itsVar+"::night::endhour");
	const int maxstarthour = optional_hour(itsVar+"::night::maxstarthour",starthour);
	const int minendhour   = optional_hour(itsVar+"::night::minendhour",endhour);

	const int mininterval = optional_int(itsVar+"::mininterval",2);
	const bool interval_zero = optional_bool(itsVar+"::always_interval_zero",false);

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
			 << PeriodPhraseFactory::create("tonight",
											itsVar,
											itsForecastTime,
											period)
			 << temperature_sentence(min1,mean1,max1,mininterval,interval_zero);

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
			sentence << PeriodPhraseFactory::create("next_night",
													itsVar,
													itsForecastTime,
													period);
			sentence << temperature_comparison_phrase(mean1,mean2,itsVar);
		  }
		else
		  {
			sentence << WeekdayTools::night_against_weekday(period.localEndTime())
					 << temperature_sentence(min2,mean2,max2,mininterval,interval_zero);

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
			 << temperature_sentence(daymin,daymean,daymax,
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
		sentence << temperature_sentence(nightmin,nightmean,nightmax,
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
			 << temperature_sentence(daymin,daymean,daymax,
									 daymininterval,day_interval_zero)
			 << Delimiter(",")
			 << "yölämpötila"
			 << temperature_sentence(nightmin,nightmean,nightmax,
									 nightmininterval,night_interval_zero);

	paragraph << sentence;
	log << paragraph;
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
