// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TimeTools
 */
// ======================================================================
/*!
 * \namespace WeatherAnalysis::TimeTools
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
#include "NFmiTime.h"

#include <ctime>

namespace
{

  // ----------------------------------------------------------------------
  /*!
   * A local help subroutine to convert a UTC tm to UTC time_t
   *
   * The original C code is by C.A. Lademann and Richard Kettlewell.
   *
   * \param t The UTC time as a tm struct
   * \return The UTC time as a time_t
   * \bug This has not been verified to work in SGI/Windows
   */
  // ----------------------------------------------------------------------
  
#if !defined(UNIX)
  
  time_t my_timegm(struct ::tm * t)
  {
	const int MINUTE = 60;
	const int HOUR = 60*MINUTE;
	const int DAY = 24*HOUR;
	const int YEAR = 365*DAY;
	
	const int mon[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	
	if(t->tm_year < 70)
	  return(static_cast<time_t>(-1));
	
	int n = t->tm_year + 1900 - 1;
	time_t epoch = (t->tm_year - 70) * YEAR
	  + ((n/4-n/100+n/400)-(1969/4-1969/100+1969/400))*DAY;
	
	int y = t->tm_year + 1900;
	int m=0;
	for(int i=0; i<t->tm_mon; i++)
	  {
		epoch += mon[m]*DAY;
		if(m==1 && y%4==0 && (y%100!=0 || y%400==0))
		  epoch += DAY;
		if(++m>11)
		  {
			m=0;
			y++;
		  }
	  }
	
	epoch += (t->tm_mday-1)*DAY;
	epoch += t->tm_hour*HOUR;
	epoch += t->tm_min*MINUTE;
	epoch += t->tm_sec;
	
	return epoch;
	
  }
#endif

} // namespace anonymous


namespace WeatherAnalysis
{
  namespace TimeTools
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Convert local time to UTC time using current TZ
	 *
	 * \param theLocalTime The local time
	 * \return The UTC time
	 */
	// ----------------------------------------------------------------------
	
	const NFmiTime toUtcTime(const NFmiTime & theLocalTime)
	{
	  ::tm tlocal;
	  tlocal.tm_sec   = theLocalTime.GetSec();
	  tlocal.tm_min   = theLocalTime.GetMin();
	  tlocal.tm_hour  = theLocalTime.GetHour();
	  tlocal.tm_mday  = theLocalTime.GetDay();
	  tlocal.tm_mon   = theLocalTime.GetMonth()-1;
	  tlocal.tm_year  = theLocalTime.GetYear()-1900;
	  tlocal.tm_wday  = -1;
	  tlocal.tm_yday  = -1;
	  tlocal.tm_isdst = -1;
	  
	  ::time_t tsec = mktime(&tlocal);
	  
	  ::tm * tutc = ::gmtime(&tsec);
	  
	  NFmiTime out(tutc->tm_year + 1900,
				   tutc->tm_mon + 1,
				   tutc->tm_mday,
				   tutc->tm_hour,
				   tutc->tm_min,
				   tutc->tm_sec);
	  
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Convert UTC time to local time using current TZ
	 *
	 * \param theUtcTime The UTC time
	 * \return The local time
	 */
	// ----------------------------------------------------------------------
	
	const NFmiTime toLocalTime(const NFmiTime & theUtcTime)
	{
	  // The UTC time
	  struct ::tm utc;
	  utc.tm_sec = theUtcTime.GetSec();
	  utc.tm_min = theUtcTime.GetMin();
	  utc.tm_hour = theUtcTime.GetHour();
	  utc.tm_mday = theUtcTime.GetDay();
	  utc.tm_mon = theUtcTime.GetMonth()-1;        // tm months start from 0
	  utc.tm_year = theUtcTime.GetYear()-1900;     // tm years start from 1900
	  utc.tm_wday = -1;
	  utc.tm_yday = -1;
	  utc.tm_isdst = -1;

#if defined(UNIX)
	  ::time_t epochtime = ::timegm(&utc);  // timegm is a GNU extension
#else
	  ::time_t epochtime = my_timegm(&utc);
#endif

	  // As local time. Note that localtime owns the struct
	  // that it will create statically
	  
	  struct ::tm * local = ::localtime(&epochtime);
	  
	  // And build a NFmiTime from the result
	  
	  NFmiTime out(local->tm_year + 1900,
				   local->tm_mon + 1,
				   local->tm_mday,
				   local->tm_hour,
				   local->tm_min,
				   local->tm_sec);
	  
	  return out;
	}

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

	// ----------------------------------------------------------------------
	/*!
	 * \brief Round the given time down to the start of the day
	 *
	 * Note: In all cases the date part remains the same. That is,
	 * time 00:00::00 is not assumed to be midnight, but the
	 * actual desired day start.
	 *
	 * \param theDate The time to round
	 * \return The rounded date
	 */
	// ----------------------------------------------------------------------

	const NFmiTime dayStart(const NFmiTime & theDate)
	{
	  return NFmiTime(theDate.GetYear(),
					  theDate.GetMonth(),
					  theDate.GetDay());
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Round the given time up to the end of the day
	 *
	 * Note: DD.MM.YYYY 00:00:00 remains the same, in all other
	 * cases the date increases by one. That is, the time 00:00:00
	 * is assumed to be the desired midnight, not the start of the
	 * date.
	 *
	 * \param theDate The time to round
	 * \return The rounded date
	 */
	// ----------------------------------------------------------------------

	const NFmiTime dayEnd(const NFmiTime & theDate)
	{
	  NFmiTime tmp(dayStart(theDate));
	  if(!tmp.IsEqual(theDate))
		tmp.ChangeByDays(1);
	  return tmp;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Round the given time up to the next start of day
	 *
	 * Note: DD.MM.YYYY 00:00:00 does NOT remain the same, the date
	 * always increases by one.
	 *
	 * \param theDate The time to round
	 * \return The rounded date
	 */
	// ----------------------------------------------------------------------

	const NFmiTime nextDay(const NFmiTime & theDate)
	{
	  NFmiTime tmp(dayStart(theDate));
	  tmp.ChangeByDays(1);
	  return tmp;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Add the given number of hours to the given time
	 *
	 * \param theDate The date to add to
	 * \param theHours The number of hours to add
	 * \return The new date
	 */
	// ----------------------------------------------------------------------

	const NFmiTime addHours(const NFmiTime & theDate, int theHours)
	{
	  NFmiTime tmp(theDate);
	  tmp.ChangeByHours(theHours);
	  return tmp;
	}

  } // namespace TimeTools
} // namespace WeatherAnalysis

// ======================================================================
