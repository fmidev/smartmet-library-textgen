// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TimeTools
 */
// ======================================================================
/*!
 * \namespace TextGen::TimeTools
 *
 * \brief Various NFmiTime related utilities
 *
 * This namespace contains functions which depend on NFmiTime but
 * do not depend on any other textgen library classes or namespaces.
 *
 * That is, miscellaneous time utilities which could have been
 * in newbase too.
 *
 */
// ======================================================================

#include "TimeTools.h"
#include "newbase/NFmiTime.h"

namespace TextGen
{
  namespace TimeTools
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Test if the dates are the same
	 *
	 * \param theDate1 The first date
	 * \param theDate2 The second date
	 * \return True if the dates are the same
	 */
	// ----------------------------------------------------------------------
	
	bool isSameDay(const NFmiTime & theDate1,
				   const NFmiTime & theDate2)
	{
	  return(theDate1.GetDay() == theDate2.GetDay() &&
			 theDate1.GetMonth() == theDate2.GetMonth() &&
			 theDate1.GetYear() == theDate2.GetYear());
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Test if the second day is the day after the first
	 *
	 * \param theDate1 The first date
	 * \param theDate2 The second date
	 * \return True if date1+1==date2
	 */
	// ----------------------------------------------------------------------
	
	bool isNextDay(const NFmiTime & theDate1,
				   const NFmiTime & theDate2)
	{
	  NFmiTime date1(theDate1);
	  date1.ChangeByDays(1);
	  
	  return(date1.GetDay() == theDate2.GetDay() &&
			 date1.GetMonth() == theDate2.GetMonth() &&
			 date1.GetYear() == theDate2.GetYear());
	}
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief Test if the period covers several days
	 *
	 * Note that a period that ends at 00:00 the next day is not considered
	 * to span several days, the end hour must be atleast 01:00 if the period
	 * ends the day after the start date.
	 *
	 * \param theDate1 The first date
	 * \param theDate2 The second date
	 * \return True if the period covers several days
	 */
	// ----------------------------------------------------------------------
	
	bool isSeveralDays(const NFmiTime & theDate1,
					   const NFmiTime & theDate2)
  {
	if(isSameDay(theDate1,theDate2))
	  return false;
	if(isNextDay(theDate1,theDate2) && theDate2.GetHour()==0)
	  return false;
	return true;
  }

  } // namespace TimeTools
} // namespace TextGen

// ======================================================================
