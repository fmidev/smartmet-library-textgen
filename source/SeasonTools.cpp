// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace SeasonTools
 */
// ======================================================================
/*!
 * \namespace WeatherAnalysis::SeasonTools
 *
 * \brief Utilities to determine the season of the given date
 *
 *
 */
// ======================================================================

#include "SeasonTools.h"
#include "Settings.h"
#include "TextGenError.h"
#include "PositiveValueAcceptor.h"
#include "WeatherPeriod.h"
#include "WeatherArea.h"
#include "WeatherResult.h"
#include "GridForecaster.h"
#include "RangeAcceptor.h"
#include <newbase/NFmiTime.h>
#include <newbase/NFmiStringTools.h>

#include <ctime>

using namespace Settings;
using namespace TextGen;

namespace WeatherAnalysis
{
  namespace SeasonTools
  {

	template <class T>
	bool from_string(T& t, 
					 const std::string& s, 
					 std::ios_base& (*f)(std::ios_base&))
	{
	  std::istringstream iss(s);
	  return !(iss >> f >> t).fail();
	}

	bool is_parameter_valid(int theMonth, int theDay)
	{
	  if(theMonth < 1 || theMonth > 12)
		return false;

	  bool retval = true;

	  switch(theMonth)
		{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
		  retval = theDay >=1 && theDay <= 31;
		  break;
		case 2:
		  retval = theDay >=1 && theDay <= 29;
		  break;
		case 4:
		case 6:
		case 9:
		case 11:
		  retval = theDay >=1 && theDay <= 31;
		  break;
		}

	  return retval;
	}

	void read_date_variable(const std::string& theVariableName, 
							const std::string& theDefaultValue, 
							int& theMonth, int& theDay)
	{
	  // mmdd
	  string date = optional_string(theVariableName, theDefaultValue);

	  if(!from_string(theMonth, date.substr(0, 2), std::dec) ||
		 !from_string(theDay, date.substr(2, 2), std::dec) ||
		 !is_parameter_valid(theMonth, theDay))
		throw TextGenError("Variable " + theVariableName + "is not of correct type (mmdd): " + date);

	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Test if the given date is in wintertime
	 *
	 * \param theDate The date
	 * \return True if the date belongs to winter season, false otherwise
	 */
	// ----------------------------------------------------------------------
	
	bool isWinter(const NFmiTime& theDate, const string& theVar)
	{
	  int winterStartMonth = -1, winterStartDay = -1, winterEndMonth = -1, winterEndDay = -1;

	  // by default dec-feb
	  read_date_variable(theVar+"::winter::startdate", "1201", winterStartMonth, winterStartDay);
	  read_date_variable(theVar+"::winter::enddate", "0229", winterEndMonth, winterEndDay);

	  int compareDate  = theDate.GetMonth()*100 + theDate.GetDay();
	  int winterStartDate  = winterStartMonth*100 + winterStartDay;
	  int winterEndDate  = winterEndMonth*100 + winterEndDay;

	  return(compareDate >= winterStartDate && compareDate <= winterEndDate);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Test if the given date is in springtime
	 *
	 * \param theDate The date
	 * \return True if the date belongs to spring season, false otherwise
	 */
	// ----------------------------------------------------------------------
	
	bool isSpring(const NFmiTime& theDate, const string& theVar)
	{
	  int springStartMonth = -1, springStartDay = -1, springEndMonth = -1, springEndDay = -1;

	  // by default mar-may
	  read_date_variable(theVar+"::spring::startdate", "0301", springStartMonth, springStartDay);
	  read_date_variable(theVar+"::spring::enddate", "0531", springEndMonth, springEndDay);

	  int compareDate  = theDate.GetMonth()*100 + theDate.GetDay();
	  int springStartDate  = springStartMonth*100 + springStartDay;
	  int springEndDate  = springEndMonth*100 + springEndDay;

	  return(compareDate >= springStartDate && compareDate <= springEndDate);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Test if the given date is in summer
	 *
	 * \param theDate The date
	 * \return True if the date belongs to summer season, false otherwise
	 */
	// ----------------------------------------------------------------------
	
	bool isSummer(const NFmiTime& theDate, const string& theVar)
	{
	  int summerStartMonth = -1, summerStartDay = -1, summerEndMonth = -1, summerEndDay = -1;

	  // by default jun-aug
	  read_date_variable(theVar+"::summer::startdate", "0601", summerStartMonth, summerStartDay);
	  read_date_variable(theVar+"::summer::enddate", "0831", summerEndMonth, summerEndDay);

	  int compareDate  = theDate.GetMonth()*100 + theDate.GetDay();
	  int summerStartDate  = summerStartMonth*100 + summerStartDay;
	  int summerEndDate  = summerEndMonth*100 + summerEndDay;

	  return(compareDate >= summerStartDate && compareDate <= summerEndDate);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Test if the given date is in autumntime
	 *
	 * \param theDate The date
	 * \return True if the date belongs to autumn season, false otherwise
	 */
	// ----------------------------------------------------------------------
	
	bool isAutumn(const NFmiTime& theDate, const string& theVar)
	{
	  int autumnStartMonth = -1, autumnStartDay = -1, autumnEndMonth = -1, autumnEndDay = -1;

	  // by default sep-nov
	  read_date_variable(theVar+"::autumn::startdate", "0901", autumnStartMonth, autumnStartDay);
	  read_date_variable(theVar+"::autumn::enddate", "1130", autumnEndMonth, autumnEndDay);

	  int compareDate  = theDate.GetMonth()*100 + theDate.GetDay();
	  int autumnStartDate  = autumnStartMonth*100 + autumnStartDay;
	  int autumnEndDate  = autumnEndMonth*100 + autumnEndDay;

	  return(compareDate >= autumnStartDate && compareDate <= autumnEndDate);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Test if the given date is in summertime
	 *
	 * \param theDate The date
	 * \return True if the date belongs to summer part of the year, false otherwise
	 */
	// ----------------------------------------------------------------------
	
	bool isSummerHalf(const NFmiTime& theDate, const string& theVar)
	{
	  int summerStartMonth = -1, summerStartDay = -1, summerEndMonth = -1, summerEndDay = -1;

	  read_date_variable(theVar+"::summertime::startdate", "0401", summerStartMonth, summerStartDay);
	  read_date_variable(theVar+"::summertime::enddate", "0930", summerEndMonth, summerEndDay);

	  int compareDate  = theDate.GetMonth()*100 + theDate.GetDay();
	  int summerStartDate  = summerStartMonth*100 + summerStartDay;
	  int summerEndDate  = summerEndMonth*100 + summerEndDay;

	  return(compareDate >= summerStartDate && compareDate <= summerEndDate);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Test if the given date is in winter
	 *
	 * \param theDate The date
	 * \return True if the date belongs to winter part of the year, false otherwise
	 */
	// ----------------------------------------------------------------------
	
	bool isWinterHalf(const NFmiTime& theDate, const string& theVar)
	{
	  return !isSummerHalf(theDate, theVar);
	}


	float get_GrowthPeriodOnOff_percentage(const WeatherArea& theArea, 
										   const AnalysisSources& theSources,
										   const WeatherPeriod& thePeriod,
										   const std::string& theVariable)
	{
	  if(Settings::isset("textgen::effectivetemperaturesum_forecast"))
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
	  GridForecaster forecaster;
	  // 5 days average temperature
	  NFmiTime startTime(thePeriod.localStartTime());
	  NFmiTime endTime(thePeriod.localStartTime());
	  endTime.ChangeByDays(5);
	  WeatherPeriod period(startTime, endTime);
	  
	  RangeAcceptor temperatureAcceptor;
	  temperatureAcceptor.lowerLimit(5.0); // temperatures > 5 degrees
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
	  
	  return  meanTemperaturePercentage.value();	  
	}

	float growing_season_percentage(const WeatherArea& theArea, 
									const AnalysisSources& theSources,
									const WeatherPeriod& thePeriod,
									const std::string& theVariable)
	{
	  float growthPeriodOnOffPercentage = get_GrowthPeriodOnOff_percentage(theArea, 
																		   theSources,
																		   thePeriod,
																		   theVariable);

	  if(growthPeriodOnOffPercentage != -1.0)
		return growthPeriodOnOffPercentage;


	  float overFiveDegreesPercentage = get_OverFiveDegrees_percentage(theArea, 
																	   theSources,
																	   thePeriod,
																	   theVariable);
	  
	  return overFiveDegreesPercentage;
	}

#ifdef OLD_IMPL
	bool growing_season_going_on(const WeatherArea& theArea,
								 const AnalysisSources& theSources,
								 const WeatherPeriod& thePeriod,
								 const std::string theVariable)
	{
	  bool retval(false);
	  
	  std::string parameter_name(theVariable+"::required_growing_season_percentage::default");
	  if(theArea.isNamed() && (Settings::isset(theVariable+"::required_growing_season_percentage::"+theArea.name())))
		parameter_name = theVariable+"::required_growing_season_percentage::"+theArea.name();
	  
	  const double required_growing_season_percentage = Settings::optional_double(parameter_name, 33.33);

	  float growingSeasonPercentage = growing_season_percentage(theArea, 
																theSources,
																thePeriod,
															    theVariable);

	  if(theArea.isPoint())
		{
		  retval = growingSeasonPercentage != kFloatMissing && 
			growingSeasonPercentage > 0;
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
								 const std::string theVariable)
	{
	  if(isset(theVariable+"::fake::growing_season_on"))
		{
		  return Settings::require_bool(theVariable+"::fake::growing_season_on");
		}

	  std::string parameter_name(theVariable+"::required_growing_season_percentage::default");
	  if(theArea.isNamed() && (Settings::isset(theVariable+"::required_growing_season_percentage::"+theArea.name())))
		parameter_name = theVariable+"::required_growing_season_percentage::"+theArea.name();

	  const double required_growing_season_percentage = Settings::optional_double(parameter_name, 33.33);

	  float growthPeriodOnOffPercentage = get_GrowthPeriodOnOff_percentage(theArea, 
																		   theSources,
																		   thePeriod,
																		   theVariable);

	  float overFiveDegreesPercentage = get_OverFiveDegrees_percentage(theArea, 
																	   theSources,
																	   thePeriod,
																	   theVariable);
	  
	  if(growthPeriodOnOffPercentage >= 0)
		{
		  return(growthPeriodOnOffPercentage != kFloatMissing &&
				 growthPeriodOnOffPercentage >= required_growing_season_percentage &&
				 overFiveDegreesPercentage != kFloatMissing &&
				 overFiveDegreesPercentage >= required_growing_season_percentage);
		}
	  else
		{
		  return(overFiveDegreesPercentage != kFloatMissing &&
				 overFiveDegreesPercentage >= required_growing_season_percentage);
		  
		}
	}


	forecast_season_id get_forecast_season(const WeatherArea& theArea,
										   const AnalysisSources& theSources,
										   const WeatherPeriod& thePeriod,
										   const std::string theVariable)
	{
	  bool growingSeasonGoingOn =  growing_season_going_on(theArea,
														   theSources,
														   thePeriod,
														   theVariable);

	  bool isSummer = SeasonTools::isSummerHalf(thePeriod.localStartTime(), theVariable);

	  return ((isSummer || growingSeasonGoingOn) ? SUMMER_SEASON : WINTER_SEASON);
	}


  } // namespace SeasonTools
} // namespace WeatherAnalysis
// ======================================================================
