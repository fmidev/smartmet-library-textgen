// ======================================================================
/*!
 * \file
 * \brief Implementation of class RelativeHumidityStory
 */
// ======================================================================

#include "RelativeHumidityStory.h"
#include "DefaultAcceptor.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "Integer.h"
#include "MathTools.h"
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
#include "PeriodPhraseFactory.h"

#include "boost/lexical_cast.hpp"

using namespace WeatherAnalysis;
using namespace boost;
using namespace std;

namespace TextGen
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------
  
  RelativeHumidityStory::~RelativeHumidityStory()
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
  
  RelativeHumidityStory::RelativeHumidityStory(const NFmiTime & theForecastTime,
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
  
  bool RelativeHumidityStory::hasStory(const string & theName)
  {
	if(theName == "relativehumidity_lowest")
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
  
  Paragraph RelativeHumidityStory::makeStory(const string & theName) const
  {
	if(theName == "relativehumidity_lowest")
	  return lowest();

	throw TextGenError("RelativeHumidityStory cannot make story "+theName);

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on lowest relative humidity
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  
  Paragraph RelativeHumidityStory::lowest() const
  {
	MessageLogger log("RelativeHumidityStory::lowest");

	using MathTools::to_precision;

	Paragraph paragraph;

	const int starthour = Settings::optional_hour(itsVar+"::starthour",0);
	const int endhour = Settings::optional_hour(itsVar+"::endhour",0);

	const int maxstarthour = Settings::optional_hour(itsVar+"::maxstarthour",starthour);
	const int minendhour   = Settings::optional_hour(itsVar+"::minendhour",endhour);

	const int precision    = Settings::require_percentage(itsVar+"::precision");

	const int limit_significantly_greater = Settings::require_percentage(itsVar+"::significantly_greater");
	const int limit_significantly_smaller = Settings::require_percentage(itsVar+"::significantly_smaller");
	const int limit_greater = Settings::require_percentage(itsVar+"::greater");
	const int limit_smaller = Settings::require_percentage(itsVar+"::smaller");
	const int limit_somewhat_greater = Settings::require_percentage(itsVar+"::somewhat_greater");
	const int limit_somewhat_smaller = Settings::require_percentage(itsVar+"::somewhat_smaller");

	const int days = WeatherPeriodTools::countPeriods(itsPeriod,
													  starthour,
													  endhour,
													  maxstarthour,
													  minendhour);

	WeatherPeriod firstperiod = WeatherPeriodTools::getPeriod(itsPeriod,
															  1,
															  starthour,
															  endhour,
															  maxstarthour,
															  minendhour);

	GridForecaster forecaster;

	WeatherResult result = forecaster.analyze(itsVar+"::fake::day1::minimum",
											  itsSources,
											  RelativeHumidity,
											  Minimum,
											  Minimum,
											  itsArea,
											  firstperiod);
	
	if(result.value() == kFloatMissing)
	  throw TextGenError("RelativeHumidity not available");

	log << "RelativeHumidity Minimum(Minimum) for day 1 " << result << endl;

	const int humidity1 = to_precision(result.value(),precision);

	Sentence sentence;

	sentence << "alin suhteellinen kosteus"
			 << "on"
			 << PeriodPhraseFactory::create("today",
											itsVar+"::day1",
											itsForecastTime,
											firstperiod)
			 << Integer(humidity1)
			 << *UnitFactory::create(Percent);

	if(days==2)
	  {
		WeatherPeriod secondperiod = WeatherPeriodTools::getPeriod(itsPeriod,
																   2,
																   starthour,
																   endhour,
																   maxstarthour,
																   minendhour);

		WeatherResult result2 = forecaster.analyze(itsVar+"::fake::day2::minimum",
												   itsSources,
												   RelativeHumidity,
												   Minimum,
												   Minimum,
												   itsArea,
												   secondperiod);
		
		if(result2.value() == kFloatMissing)
		  throw TextGenError("RelativeHumidity not available");

		log << "RelativeHumidity Minimum(Minimum) for day 2 " << result2 << endl;

		const int humidity2 = to_precision(result2.value(),precision);

		sentence << Delimiter(",")
				 << PeriodPhraseFactory::create("next_day",
												itsVar+"::day2",
												itsForecastTime,
												secondperiod);
		if(humidity2 - humidity1 >= limit_significantly_greater)
		  sentence << "huomattavasti suurempi";
		else if(humidity2 - humidity1 >= limit_greater)
		  sentence << "suurempi";
		else if(humidity2 - humidity1 >= limit_somewhat_greater)
		  sentence << "hieman suurempi";
		else if(humidity1 - humidity2 >= limit_significantly_smaller)
		  sentence << "huomattavasti pienempi";
		else if(humidity1 - humidity2 >= limit_smaller)
		  sentence << "pienempi";
		else if(humidity1 - humidity2 >= limit_somewhat_smaller)
		  sentence << "hieman pienempi";
		else
		  sentence << "sama";

	  }

	paragraph << sentence;
	log << paragraph;
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
  
