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

	// ----------------------------------------------------------------------
	/*!
	 * \brief Test if the given date is in wintertime
	 *
	 * \param theDate The date
	 * \return True if the date belongs to winter season, false otherwise
	 */
	// ----------------------------------------------------------------------
	
	bool isWinter(const NFmiTime& theDate)
	{
	  // dec-feb
	  return(theDate.GetMonth() == 12 || theDate.GetMonth() <= 2);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Test if the given date is in springtime
	 *
	 * \param theDate The date
	 * \return True if the date belongs to spring season, false otherwise
	 */
	// ----------------------------------------------------------------------
	
	bool isSpring(const NFmiTime& theDate)
	{
	  // mar-may
	  return(theDate.GetMonth() >= 3 && theDate.GetMonth() <= 5);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Test if the given date is in summertime
	 *
	 * \param theDate The date
	 * \return True if the date belongs to summer season, false otherwise
	 */
	// ----------------------------------------------------------------------
	
	bool isSummer(const NFmiTime& theDate)
	{
	  // jun-aug
	  return(theDate.GetMonth() >= 6 && theDate.GetMonth() <= 8);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Test if the given date is in autumntime
	 *
	 * \param theDate The date
	 * \return True if the date belongs to autumn season, false otherwise
	 */
	// ----------------------------------------------------------------------
	
	bool isAutumn(const NFmiTime& theDate)
	{
	  // sep-nov
	  return(theDate.GetMonth() >= 9 && theDate.GetMonth() <= 11);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Test if the given date is in summer
	 *
	 * \param theDate The date
	 * \return True if the date belongs to summer part of the year, false otherwise
	 */
	// ----------------------------------------------------------------------
	
	bool isSummerHalf(const NFmiTime& theDate, const string& theVar)
	{
	  // mmdd
	  string startDateVar(theVar+"::summer::startdate");
	  string endDateVar(theVar+"::summer::enddate");
	  string summerStartDate = optional_string(startDateVar, "0501");
	  string summerEndDate   = optional_string(endDateVar, "1031");
using namespace Settings;

	  int summerStartMonth = -1, summerStartDay = -1, summerEndMonth = -1, summerEndDay = -1;

	  if(!from_string(summerStartMonth, summerStartDate.substr(0, 2), std::dec) ||
		 !from_string(summerStartDay, summerStartDate.substr(2, 2), std::dec))
		throw TextGenError("Variable " + startDateVar + "is not of correct type (mmdd): " + summerStartDate);

	  
	  if(!from_string(summerEndMonth, summerEndDate.substr(0, 2), std::dec) ||
		 !from_string(summerEndDay, summerEndDate.substr(2, 2), std::dec))
		throw TextGenError("Variable " + endDateVar + "is not of correct type (mmdd): " + summerEndDate);

	  int compareDate  = theDate.GetMonth()*100 + theDate.GetDay();
	  int compareSummerStartDate  = summerStartMonth*100 + summerStartDay;
	  int compareSummerEndDate  = summerEndMonth*100 + summerEndDay;

	  return(compareDate >= compareSummerStartDate && compareDate <= compareSummerEndDate);

	  //	  return(theDate.GetMonth() >= 5 && theDate.GetMonth() <= 10);
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

	  //return(theDate.GetMonth() <= 4 || theDate.GetMonth() >= 11);
	}


  } // namespace SeasonTools
} // namespace WeatherAnalysis
// ======================================================================
