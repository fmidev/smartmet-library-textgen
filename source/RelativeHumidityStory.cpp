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

} // namespace anonymous

namespace
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Determine the number of acceptable days in the period
   *
   * \param thePeriod The time period
   * \param theMaxStartHour The maximum start hour
   * \param theMinEndHour The minimum end hour
   * \return The number of days
   */
  // ----------------------------------------------------------------------

  int day_count(const WeatherPeriod & thePeriod,
				int theMaxStartHour,
				int theMinEndHour)
  {
	NFmiTime t(thePeriod.localStartTime());
	int count = 0;

	if(t.GetHour() <= theMaxStartHour)
	  ++count;
	t.ChangeByHours(24);
	t.SetHour(theMinEndHour);

	while(thePeriod.localEndTime().IsLessThan(t))
	  {
		++count;
		t.ChangeByHours(24);
	  }
	return count;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Determine the first weather period for lowest()
   *
   * \param thePeriod The period
   * \param theFlag True, if the period should start tomorrow
   * \return The period
   */
  // ----------------------------------------------------------------------

  WeatherPeriod first_period(const WeatherPeriod & thePeriod,
							 bool theFlag)
  {
	NFmiTime starttime(thePeriod.localStartTime());
	if(theFlag)
	  {
		starttime.ChangeByDays(1);
		starttime.SetHour(0);
	  }
	NFmiTime endtime(starttime);
	endtime.ChangeByDays(1);
	endtime.SetHour(0);

	if(thePeriod.localStartTime().IsLessThan(endtime))
	  endtime = thePeriod.localStartTime();

	return WeatherPeriod(starttime,endtime);

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Determine the second weather period for lowest()
   *
   * \param thePeriod The full period from which to extract
   * \param theFirstPeriod The previously extracted period
   * \return The second period
   */
  // ----------------------------------------------------------------------

  WeatherPeriod second_period(const WeatherPeriod & thePeriod,
							  const WeatherPeriod & theFirstPeriod)
  {
	NFmiTime starttime(theFirstPeriod.localStartTime());
	NFmiTime endtime(starttime);
	endtime.ChangeByDays(1);

	if(thePeriod.localEndTime().IsLessThan(endtime))
	  endtime = thePeriod.localEndTime();

	return WeatherPeriod(starttime,endtime);
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

	const string var1 = itsVariable + "::maxstarthour";
	const string var2 = itsVariable + "::minendhour";
	const string var3 = itsVariable + "::precision";

	const int maxstarthour = Settings::require_hour(var1);
	const int minendhour   = Settings::require_hour(var2);
	const int precision    = Settings::require_percentage(var3);

	const int limit_greater = Settings::require_percentage(itsVariable+"::greater");
	const int limit_smaller = Settings::require_percentage(itsVariable+"::smaller");
	const int limit_somewhat_greater = Settings::require_percentage(itsVariable+"::somewhat_greater");
	const int limit_somewhat_smaller = Settings::require_percentage(itsVariable+"::somewhat_smaller");

	const int days = day_count(itsPeriod, maxstarthour, minendhour);

	if(days>2)
	  throw TextGenError("relativehumidity_lowest cannot handle "+lexical_cast<string>(days)+" days");

	const bool start_tomorrow = (itsPeriod.localStartTime().GetHour() < maxstarthour);

	WeatherPeriod firstperiod = first_period(itsPeriod,start_tomorrow);

	GridForecaster forecaster;

	WeatherResult result = forecaster.analyze(itsSources,
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
			 << Number<int>(humidity1)
			 << Delimiter("%");

	if(days==2)
	  {
		WeatherPeriod secondperiod = second_period(itsPeriod,firstperiod);

		WeatherResult result2 = forecaster.analyze(itsSources,
												   RelativeHumidity,
												   Minimum,
												   Minimum,
												   secondperiod,
												   itsArea);
		
		if(result2.value() == kFloatMissing)
		  throw TextGenError("RelativeHumidity not available");

		const int humidity2 = round_to_precision(result.value(),precision);

		if(humidity2 - humidity1 >= limit_greater)
		  sentence << "suurempi";
		else if(humidity2 - humidity1 >= limit_somewhat_greater)
		  sentence << "hieman suurempi";
		else if(humidity1 - humidity2 >= limit_smaller)
		  sentence << "pienempi";
		else if(humidity1 - humidity2 >= limit_somewhat_smaller)
		  sentence << "hieman_pienempi";
		else
		  sentence << "sama";

	  }

	paragraph << sentence;
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
  
