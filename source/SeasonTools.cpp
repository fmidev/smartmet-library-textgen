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
#include <newbase/NFmiTime.h>

#include <ctime>

namespace WeatherAnalysis
{
  namespace SeasonTools
  {

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
	
	bool isSummerHalf(const NFmiTime& theDate)
	{
	  // may-apr
	  return(theDate.GetMonth() >= 5 && theDate.GetMonth() <= 10);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Test if the given date is in winter
	 *
	 * \param theDate The date
	 * \return True if the date belongs to winter part of the year, false otherwise
	 */
	// ----------------------------------------------------------------------
	
	bool isWinterHalf(const NFmiTime& theDate)
	{
	  // nov-apr
	  return(theDate.GetMonth() <= 4 || theDate.GetMonth() >= 11);
	}


  } // namespace SeasonTools
} // namespace WeatherAnalysis
// ======================================================================
