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
#include "Number.h"
#include "NumberRange.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
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
		sentence << NumberRange<Number<int> >(theMinimum,theMaximum)
				 << "astetta";
	  }
	else
	  {
		sentence << "noin"
				 << Number<int>(theMean)
				 << "astetta";
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
   * \param theSources The analysis sources
   * \param theArea The area to be analyzed 
   * \param thePeriod The period to be analyzed
   * \param theVariable The associated configuration variable
  */
  // ----------------------------------------------------------------------
  
  TemperatureStory::TemperatureStory(const AnalysisSources & theSources,
									 const WeatherArea & theArea,
									 const WeatherPeriod & thePeriod,
									 const string & theVariable)
	: itsSources(theSources)
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
	Paragraph paragraph;
	Sentence sentence;

	GridForecaster forecaster;

	WeatherResult result = forecaster.analyze(itsVar+"::fake::mean",
											  itsSources,
											  Temperature,
											  Mean,
											  Mean,
											  itsPeriod,
											  itsArea);

	if(result.value() == kFloatMissing)
	  throw TextGenError("Mean temperature not available");

	Number<int> num = FmiRound(result.value());

	sentence << "keskilämpötila"
			 << num
			 << "astetta";
	
	paragraph << sentence;
	return paragraph;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on mean maximum temperature
   *
   * Throws if the weather period does not evenly divide into 24 hour
   * segments.
   *
   * \return The story
   *
   * \see page_temperature_meanmax
   */
  // ----------------------------------------------------------------------

  Paragraph TemperatureStory::meanmax() const
  {
	Paragraph paragraph;
	Sentence sentence;

	GridForecaster forecaster;

	WeatherResult result = forecaster.analyze(itsVar+"::fake::mean",
											  itsSources,
											  MaxTemperature,
											  Mean,
											  Mean,
											  itsPeriod,
											  itsArea);

	if(result.value() == kFloatMissing)
	  throw TextGenError("Mean daily maximum temperature not available");

	Number<int> num = FmiRound(result.value());

	sentence << "keskimääräinen ylin lämpötila"
			 << num
			 << "astetta";
	paragraph << sentence;
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
	Paragraph paragraph;
	Sentence sentence;

	GridForecaster forecaster;

	WeatherResult result = forecaster.analyze(itsVar+"::fake::mean",
											  itsSources,
											  MinTemperature,
											  Mean,
											  Mean,
											  itsPeriod,
											  itsArea);

	if(result.value() == kFloatMissing)
	  throw TextGenError("Mean daily minimum temperature not available");

	Number<int> num = FmiRound(result.value());

	sentence << "keskimääräinen alin lämpötila"
			 << num
			 << "astetta";
	paragraph << sentence;
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
												 MaxTemperature,
												 Minimum,
												 Maximum,
												 period,
												 itsArea);

	WeatherResult meanresult = forecaster.analyze(itsVar+"::fake::day1::mean",
												  itsSources,
												  MaxTemperature,
												  Mean,
												  Maximum,
												  period,
												  itsArea);

	WeatherResult maxresult = forecaster.analyze(itsVar+"::fake::day1::maximum",
												 itsSources,
												 MaxTemperature,
												 Maximum,
												 Maximum,
												 period,
												 itsArea);

	if(minresult.value() == kFloatMissing ||
	   maxresult.value() == kFloatMissing ||
	   meanresult.value() == kFloatMissing)
	  throw TextGenError("TemperatureStory: MaxTemperature is not available");

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
									   MaxTemperature,
									   Minimum,
									   Maximum,
									   period,
									   itsArea);
		
		maxresult = forecaster.analyze(var+"::maximum",
									   itsSources,
									   MaxTemperature,
									   Maximum,
									   Maximum,
									   period,
									   itsArea);

		meanresult = forecaster.analyze(var+"::mean",
										itsSources,
										MaxTemperature,
										Mean,
										Maximum,
										period,
										itsArea);
		
		if(minresult.value() == kFloatMissing ||
		   maxresult.value() == kFloatMissing ||
		   meanresult.value() == kFloatMissing)
		  throw TextGenError("TemperatureStory: MaxTemperature is not available for day "+lexical_cast<string>(p));
		
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
	using namespace Settings;
	using namespace WeatherPeriodTools;

	Paragraph paragraph;

#if 0
	const int starthour    = require_hour(itsVar+"::starthour");
	const int endhour      = require_hour(itsVar+"::endhour");
	const int maxstarthour = optional_hour(itsVar+"::maxstarthour",starthour);
	const int minendhour   = optional_hour(itsVar+"::minendhour",endhour);

	const int mininterval  = optional_int(itsVar+"::mininterval",1);
	const bool interval_zero = optional_bool(itsVar+"::always_interval_zero",false);

	const bool prefer_dayname = optional_bool(itsVar+"::prefer_dayname",false);

	const int days = countPeriods(itsPeriod,
								  starthour,
								  endhour,
								  maxstarthour,
								  minendhour);


	WeatherPeriod period = getperiod(itsPeriod,
									 1,
									 starthour,
									 endhour,
									 maxstarthour,
									 minendhour);

								  

	paragraph << sentence;
#endif
	return paragraph;

  }

} // namespace TextGen
  
// ======================================================================
  
