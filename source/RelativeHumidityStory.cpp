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
#include "Number.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "WeatherFunction.h"
#include "WeatherParameter.h"
#include "WeatherPeriodTools.h"
#include "WeatherResult.h"

#include "boost/lexical_cast.hpp"

using namespace WeatherAnalysis;
using namespace boost;
using namespace std;

namespace
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Round the input value to the given integer precision
   *
   * \param theValue The floating value
   * \param thePrecision The precision in range 0-100, should divide 100 evenly
   * \return The rounded value
   */
  // ----------------------------------------------------------------------

  int round_to_precision(float theValue, int thePrecision)
  {
        if(thePrecision <=0 || thePrecision > 100)
          return FmiRound(theValue);
        const int value = FmiRound(theValue/thePrecision)*thePrecision;
        return value;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the "1-na" weekday description for the given period
   *
   * \param thePeriod The weather period
   * \return The phrase
   */
  // ----------------------------------------------------------------------

  string on_weekday(const WeatherPeriod & thePeriod)
  {
	const int wd = thePeriod.localStartTime().GetWeekday();
	const string out = lexical_cast<string>(wd)+"-na";
	return out;
  }

}


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
   * \param theSources The analysis sources
   * \param theArea The area to be analyzed 
   * \param thePeriod The period to be analyzed
   * \param theVariable The associated configuration variable
  */
  // ----------------------------------------------------------------------
  
  RelativeHumidityStory::RelativeHumidityStory(const AnalysisSources & theSources,
											   const WeatherArea & theArea,
											   const WeatherPeriod & thePeriod,
											   const string & theVariable)
	: itsSources(theSources)
	, itsArea(theArea)
	, itsPeriod(thePeriod)
	, itsVariable(theVariable)
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
	Paragraph paragraph;

	const int starthour = Settings::optional_hour(itsVariable+"::starthour",0);
	const int endhour = Settings::optional_hour(itsVariable+"::endhour",0);

	const int maxstarthour = Settings::optional_hour(itsVariable+"::maxstarthour",starthour);
	const int minendhour   = Settings::optional_hour(itsVariable+"::minendhour",endhour);

	const int precision    = Settings::require_percentage(itsVariable+"::precision");

	const int limit_significantly_greater = Settings::require_percentage(itsVariable+"::significantly_greater");
	const int limit_significantly_smaller = Settings::require_percentage(itsVariable+"::significantly_smaller");
	const int limit_greater = Settings::require_percentage(itsVariable+"::greater");
	const int limit_smaller = Settings::require_percentage(itsVariable+"::smaller");
	const int limit_somewhat_greater = Settings::require_percentage(itsVariable+"::somewhat_greater");
	const int limit_somewhat_smaller = Settings::require_percentage(itsVariable+"::somewhat_smaller");

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

	WeatherResult result = forecaster.analyze(itsVariable+"::fake::day1::minimum",
											  itsSources,
											  RelativeHumidity,
											  Minimum,
											  Minimum,
											  firstperiod,
											  itsArea);
	
	if(result.value() == kFloatMissing)
	  throw TextGenError("RelativeHumidity not available");

	const int humidity1 = round_to_precision(result.value(),precision);

	Sentence sentence;

	sentence << "alin suhteellinen kosteus"
			 << "on"
			 << on_weekday(firstperiod)
			 << Number<int>(humidity1)
			 << Delimiter("%");

	if(days==2)
	  {
		WeatherPeriod secondperiod = WeatherPeriodTools::getPeriod(itsPeriod,
																   2,
																   starthour,
																   endhour,
																   maxstarthour,
																   minendhour);

		WeatherResult result2 = forecaster.analyze(itsVariable+"::fake::day2::minimum",
												   itsSources,
												   RelativeHumidity,
												   Minimum,
												   Minimum,
												   secondperiod,
												   itsArea);
		
		if(result2.value() == kFloatMissing)
		  throw TextGenError("RelativeHumidity not available");

		const int humidity2 = round_to_precision(result2.value(),precision);

		sentence << Delimiter(",")
				 << on_weekday(secondperiod);
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
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
  
