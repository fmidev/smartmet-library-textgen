// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace SeasonTools
 */
// ======================================================================
/*!
 * \namespace TextGen::SeasonTools
 *
 * \brief Utilities to determine the season of the given date
 *
 *
 */
// ======================================================================

#include "SeasonTools.h"
#include "PositiveValueAcceptor.h"
#include <calculator/GridForecaster.h>
#include <calculator/RangeAcceptor.h>
#include <calculator/Settings.h>
#include <calculator/TextGenPosixTime.h>
#include <calculator/WeatherArea.h>
#include <calculator/WeatherPeriod.h>
#include <calculator/WeatherResult.h>
#include <macgyver/Exception.h>
#include <newbase/NFmiStringTools.h>

#include <ctime>

using namespace std;
using namespace Settings;
using namespace TextGen;

namespace TextGen
{
namespace SeasonTools
{
template <class T>
bool from_string(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&))
{
  std::istringstream iss(s);
  return !(iss >> f >> t).fail();
}

bool is_parameter_valid(int theMonth, int theDay)
{
  if (theMonth < 1 || theMonth > 12)
    return false;
  if (theDay < 1)
    return false;

  if (theMonth == 2)
    return theDay <= 29;
  if (theMonth == 4 || theMonth == 6 || theMonth == 9 || theMonth == 11)
    return theDay <= 30;
  return theDay <= 31;
}

void read_date_variable(const std::string& theVariableName,
                        const std::string& theDefaultValue,
                        int& theMonth,
                        int& theDay)
{
  // mmdd
  string date = optional_string(theVariableName, theDefaultValue);

  if (!from_string(theMonth, date.substr(0, 2), std::dec) ||
      !from_string(theDay, date.substr(2, 2), std::dec) || !is_parameter_valid(theMonth, theDay))
    throw Fmi::Exception(BCP,
                         "Variable " + theVariableName + "is not of correct type (mmdd): " + date);
}

bool isBetweenDates(
    const TextGenPosixTime& theDate, int startMonth, int startDay, int endMonth, int endDay)
{
  int compareDate = theDate.GetMonth() * 100 + theDate.GetDay();
  int startDate = startMonth * 100 + startDay;
  int endDate = endMonth * 100 + endDay;

  if (endMonth < startMonth)  // year changes
  {
    if (theDate.GetMonth() >= startMonth && theDate.GetMonth() <= 12)
      return (compareDate >= startDate);
    return (compareDate <= endDate);
  }

  return (compareDate >= startDate && compareDate <= endDate);
}

// ----------------------------------------------------------------------
/*!
 * \brief Test if the given date is in wintertime
 *
 * \param theDate The date
 * \return True if the date belongs to winter season, false otherwise
 */
// ----------------------------------------------------------------------

bool isWinter(const TextGenPosixTime& theDate, const string& theVar)
{
  int winterStartMonth = -1;
  int winterStartDay = -1;
  int winterEndMonth = -1;
  int winterEndDay = -1;

  // by default dec-feb
  read_date_variable(theVar + "::winter::startdate", "1201", winterStartMonth, winterStartDay);
  read_date_variable(theVar + "::winter::enddate", "0229", winterEndMonth, winterEndDay);

  return isBetweenDates(theDate, winterStartMonth, winterStartDay, winterEndMonth, winterEndDay);
}

// ----------------------------------------------------------------------
/*!
 * \brief Test if the given date is in springtime
 *
 * \param theDate The date
 * \return True if the date belongs to spring season, false otherwise
 */
// ----------------------------------------------------------------------

bool isSpring(const TextGenPosixTime& theDate, const string& theVar)
{
  int springStartMonth = -1;
  int springStartDay = -1;
  int springEndMonth = -1;
  int springEndDay = -1;

  // by default mar-may
  read_date_variable(theVar + "::spring::startdate", "0301", springStartMonth, springStartDay);
  read_date_variable(theVar + "::spring::enddate", "0531", springEndMonth, springEndDay);

  return isBetweenDates(theDate, springStartMonth, springStartDay, springEndMonth, springEndDay);
}

// ----------------------------------------------------------------------
/*!
 * \brief Test if the given date is in summer
 *
 * \param theDate The date
 * \return True if the date belongs to summer season, false otherwise
 */
// ----------------------------------------------------------------------

bool isSummer(const TextGenPosixTime& theDate, const string& theVar)
{
  int summerStartMonth = -1;
  int summerStartDay = -1;
  int summerEndMonth = -1;
  int summerEndDay = -1;

  // by default jun-aug
  read_date_variable(theVar + "::summer::startdate", "0601", summerStartMonth, summerStartDay);
  read_date_variable(theVar + "::summer::enddate", "0831", summerEndMonth, summerEndDay);

  return isBetweenDates(theDate, summerStartMonth, summerStartDay, summerEndMonth, summerEndDay);
}

// ----------------------------------------------------------------------
/*!
 * \brief Test if the given date is in autumntime
 *
 * \param theDate The date
 * \return True if the date belongs to autumn season, false otherwise
 */
// ----------------------------------------------------------------------

bool isAutumn(const TextGenPosixTime& theDate, const string& theVar)
{
  int autumnStartMonth = -1;
  int autumnStartDay = -1;
  int autumnEndMonth = -1;
  int autumnEndDay = -1;

  // by default sep-nov
  read_date_variable(theVar + "::autumn::startdate", "0901", autumnStartMonth, autumnStartDay);
  read_date_variable(theVar + "::autumn::enddate", "1130", autumnEndMonth, autumnEndDay);

  return isBetweenDates(theDate, autumnStartMonth, autumnStartDay, autumnEndMonth, autumnEndDay);
}

// ----------------------------------------------------------------------
/*!
 * \brief Test if the given date is in summertime
 *
 * \param theDate The date
 * \return True if the date belongs to summer part of the year, false otherwise
 */
// ----------------------------------------------------------------------

bool isSummerHalf(const TextGenPosixTime& theDate, const string& theVar)
{
  int summerStartMonth = -1;
  int summerStartDay = -1;
  int summerEndMonth = -1;
  int summerEndDay = -1;

  read_date_variable(theVar + "::summertime::startdate", "0401", summerStartMonth, summerStartDay);
  read_date_variable(theVar + "::summertime::enddate", "0930", summerEndMonth, summerEndDay);

  return isBetweenDates(theDate, summerStartMonth, summerStartDay, summerEndMonth, summerEndDay);
}

// ----------------------------------------------------------------------
/*!
 * \brief Test if the given date is in winter
 *
 * \param theDate The date
 * \return True if the date belongs to winter part of the year, false otherwise
 */
// ----------------------------------------------------------------------

bool isWinterHalf(const TextGenPosixTime& theDate, const string& theVar)
{
  return !isSummerHalf(theDate, theVar);
}

float get_GrowthPeriodOnOff_percentage(const WeatherArea& theArea,
                                       const AnalysisSources& theSources,
                                       const WeatherPeriod& thePeriod,
                                       const std::string& theVariable)
{
  if (Settings::isset("textgen::effectivetemperaturesum_forecast"))
  {
    GridForecaster forecaster;
    PositiveValueAcceptor acceptor;
    WeatherResult growingSeasonPercentage = forecaster.analyze(theVariable,
                                                               theSources,
                                                               GrowthPeriodOnOff,
                                                               Percentage,
                                                               Mean,
                                                               theArea,
                                                               thePeriod,
                                                               DefaultAcceptor(),
                                                               DefaultAcceptor(),
                                                               acceptor);

    return growingSeasonPercentage.value();
  }
  return -1.0;
}

float get_OverFiveDegrees_percentage(const WeatherArea& theArea,
                                     const AnalysisSources& theSources,
                                     const WeatherPeriod& thePeriod,
                                     const std::string& theVariable)
{
  string fake_var("onenight::fake::growing_season_percentange");
  if (theArea.type() == WeatherArea::Inland)
    fake_var += "::inland";
  if (theArea.type() == WeatherArea::Coast)
    fake_var += "::coastal";

  if (Settings::isset(fake_var))
    return Settings::optional_double(fake_var, 0.0);

  GridForecaster forecaster;
  // 5 days average temperature
  const TextGenPosixTime& startTime = thePeriod.localStartTime();
  TextGenPosixTime endTime = thePeriod.localStartTime();
  endTime.ChangeByDays(5);
  WeatherPeriod period(startTime, endTime);

  RangeAcceptor temperatureAcceptor;
  temperatureAcceptor.lowerLimit(5.0);  // temperatures > 5 degrees
  WeatherResult meanTemperaturePercentage = forecaster.analyze(theVariable,
                                                               theSources,
                                                               Temperature,
                                                               Percentage,
                                                               Mean,
                                                               theArea,
                                                               period,
                                                               DefaultAcceptor(),
                                                               DefaultAcceptor(),
                                                               temperatureAcceptor);

  return meanTemperaturePercentage.value();
}

float growing_season_percentage(const WeatherArea& theArea,
                                const AnalysisSources& theSources,
                                const WeatherPeriod& thePeriod,
                                const std::string& theVariable)
{
  float growthPeriodOnOffPercentage =
      get_GrowthPeriodOnOff_percentage(theArea, theSources, thePeriod, theVariable);

  if (growthPeriodOnOffPercentage != -1.0)
    return growthPeriodOnOffPercentage;

  float overFiveDegreesPercentage =
      get_OverFiveDegrees_percentage(theArea, theSources, thePeriod, theVariable);

  return overFiveDegreesPercentage;
}

#ifdef OLD_IMPL
bool growing_season_going_on(const WeatherArea& theArea,
                             const AnalysisSources& theSources,
                             const WeatherPeriod& thePeriod,
                             const std::string theVariable)
{
  bool retval(false);

  std::string parameter_name(theVariable + "::required_growing_season_percentage::default");
  if (theArea.isNamed() &&
      (Settings::isset(theVariable + "::required_growing_season_percentage::" + theArea.name())))
    parameter_name = theVariable + "::required_growing_season_percentage::" + theArea.name();

  const double required_growing_season_percentage =
      Settings::optional_double(parameter_name, 33.33);

  float growingSeasonPercentage =
      growing_season_percentage(theArea, theSources, thePeriod, theVariable);

  if (theArea.isPoint())
  {
    retval = growingSeasonPercentage != kFloatMissing && growingSeasonPercentage > 0;
  }
  else
  {
    retval = growingSeasonPercentage != kFloatMissing &&
             growingSeasonPercentage >= required_growing_season_percentage;
  }

  return retval;
}
#endif

bool growing_season_going_on(const WeatherArea& theArea,
                             const AnalysisSources& theSources,
                             const WeatherPeriod& thePeriod,
                             const std::string& theVariable)
{
  if (isset(theVariable + "::fake::growing_season_on"))
  {
    return Settings::require_bool(theVariable + "::fake::growing_season_on");
  }

  std::string parameter_name(theVariable + "::required_growing_season_percentage::default");
  if (theArea.isNamed() &&
      (Settings::isset(theVariable + "::required_growing_season_percentage::" + theArea.name())))
    parameter_name = theVariable + "::required_growing_season_percentage::" + theArea.name();

  const double required_growing_season_percentage =
      Settings::optional_double(parameter_name, 33.33);

  float growthPeriodOnOffPercentage =
      get_GrowthPeriodOnOff_percentage(theArea, theSources, thePeriod, theVariable);

  float overFiveDegreesPercentage =
      get_OverFiveDegrees_percentage(theArea, theSources, thePeriod, theVariable);

  if (growthPeriodOnOffPercentage >= 0)
  {
    return (growthPeriodOnOffPercentage != kFloatMissing &&
            growthPeriodOnOffPercentage >= required_growing_season_percentage &&
            overFiveDegreesPercentage != kFloatMissing &&
            overFiveDegreesPercentage >= required_growing_season_percentage);
  }
  if (growthPeriodOnOffPercentage == -1.0)  // indicates that GrowthPeriodOnOff can not be used
  {
    return (overFiveDegreesPercentage != kFloatMissing &&
            overFiveDegreesPercentage >= required_growing_season_percentage);
  }

  return false;
}

forecast_season_id get_forecast_season(const WeatherArea& theArea,
                                       const AnalysisSources& theSources,
                                       const WeatherPeriod& thePeriod,
                                       const std::string& theVariable)
{
  bool growingSeasonGoingOn = growing_season_going_on(theArea, theSources, thePeriod, theVariable);

  bool isSummer = SeasonTools::isSummerHalf(thePeriod.localStartTime(), theVariable);

  return ((isSummer || growingSeasonGoingOn) ? SUMMER_SEASON : WINTER_SEASON);
}

}  // namespace SeasonTools
}  // namespace TextGen
// ======================================================================
