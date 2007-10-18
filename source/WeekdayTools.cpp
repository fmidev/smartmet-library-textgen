// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::WeekdayTools
 */
// ======================================================================
/*!
 * \namespace TextGen::WeekdayTools
 *
 * \brief Tools for generating weekday phrases
 *
 */
// ----------------------------------------------------------------------

#include <boost/lexical_cast.hpp> // boost included laitettava ennen newbase:n NFmiGlobals-includea, muuten MSVC:ss� min max m��rittelyt jo tehty

#include "WeekdayTools.h"
#include "TextGenError.h"
#include <newbase/NFmiTime.h>

using namespace std;
using namespace boost;

namespace TextGen
{
  namespace WeekdayTools
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-na" description based on the given time
	 *
	 * \param theTime The time of the day
	 * \return The "N-na" phrase
	 */
	// ----------------------------------------------------------------------

	const string on_weekday(const NFmiTime & theTime)
	{
	  const string out = (lexical_cast<string>(theTime.GetWeekday())
						  +"-na");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-na kello" description based on the given time
	 *
	 * \param theTime The time of the day
	 * \return The "N-na kello" phrase
	 */
	// ----------------------------------------------------------------------

	const string on_weekday_time(const NFmiTime & theTime)
	{
	  const string out = (lexical_cast<string>(theTime.GetWeekday())
						  +"-na kello");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-vastaisena y�n�" phrase
	 *
	 * \param theTime The time
	 * \return The "N-vastaisena y�n�" phrase
	 */
	// ----------------------------------------------------------------------

	const string night_against_weekday(const NFmiTime & theTime)
	{
	  const string out = (lexical_cast<string>(theTime.GetWeekday())
						  +"-vastaisena y�n�");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-aamuun" phrase
	 *
	 * \param theTime The time
	 * \return The "N-aamuun" phrase
	 */
	// ----------------------------------------------------------------------

	const string until_weekday_morning(const NFmiTime & theTime)
	{
	  const string out = (lexical_cast<string>(theTime.GetWeekday())
						  +"-aamuun");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-iltaan" phrase
	 *
	 * \param theTime The time
	 * \return The "N-iltaan" phrase
	 */
	// ----------------------------------------------------------------------

	const string until_weekday_evening(const NFmiTime & theTime)
	{
	  const string out = (lexical_cast<string>(theTime.GetWeekday())
						  +"-iltaan");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-aamuun" or "N-iltaan" phrase
	 *
	 * \param theTime The time
	 * \return The phrase
	 */
	// ----------------------------------------------------------------------

	const string until_weekday_time(const NFmiTime & theTime)
	{
	  const int hour = theTime.GetHour();
	  if(hour == 6)
		return until_weekday_morning(theTime);
	  else if(hour == 18)
		return until_weekday_evening(theTime);
	  else
		{
		  const string msg = "WeekdayTools::until_weekday_time: Cannot generate -aamuun or -iltaan phrase for hour "+lexical_cast<int>(hour);
		  throw TextGenError(msg);
		}
	}


	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-alkaen" phrase
	 *
	 * \param theTime The time
	 * \return The "N-alkaen" phrase
	 */
	// ----------------------------------------------------------------------

	const string from_weekday(const NFmiTime & theTime)
	{
	  const string out = (lexical_cast<string>(theTime.GetWeekday())
						  +"-alkaen");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-aamusta" phrase
	 *
	 * \param theTime The time
	 * \return The "N-aamusta" phrase
	 */
	// ----------------------------------------------------------------------

	const string from_weekday_morning(const NFmiTime & theTime)
	{
	  const string out = (lexical_cast<string>(theTime.GetWeekday())
						  +"-aamusta");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-illasta" phrase
	 *
	 * \param theTime The time
	 * \return The "N-illasta" phrase
	 */
	// ----------------------------------------------------------------------

	const string from_weekday_evening(const NFmiTime & theTime)
	{
	  const string out = (lexical_cast<string>(theTime.GetWeekday())
						  +"-illasta");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-aamusta" or "N-illasta" phrase
	 *
	 * \param theTime The time
	 * \return The phrase
	 */
	// ----------------------------------------------------------------------

	const string from_weekday_time(const NFmiTime & theTime)
	{
	  const int hour = theTime.GetHour();
	  if(hour == 6)
		return from_weekday_morning(theTime);
	  else if(hour == 18)
		return from_weekday_evening(theTime);
	  else
		{
		  const string msg = "WeekdayTools::from_weekday_time: Cannot generate -aamusta or -illasta phrase for hour "+lexical_cast<int>(hour);
		  throw TextGenError(msg);
		}
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-aamulla" phrase
	 *
	 * \param theTime The time
	 * \return The "N-aamulla" phrase
	 */
	// ----------------------------------------------------------------------

	const string on_weekday_morning(const NFmiTime & theTime)
	{
	  const string out = (lexical_cast<string>(theTime.GetWeekday())
						  +"-aamulla");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-aamup�iv�ll�" phrase
	 *
	 * \param theTime The time
	 * \return The "N-aamup�iv�ll�" phrase
	 */
	// ----------------------------------------------------------------------

	const string on_weekday_forenoon(const NFmiTime & theTime)
	{
	  const string out = (lexical_cast<string>(theTime.GetWeekday())
						  +"-aamup�iv�ll�");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-iltap�iv�ll�" phrase
	 *
	 * \param theTime The time
	 * \return The "N-iltap�iv�ll�" phrase
	 */
	// ----------------------------------------------------------------------

	const string on_weekday_afternoon(const NFmiTime & theTime)
	{
	  const string out = (lexical_cast<string>(theTime.GetWeekday())
						  +"-iltap�iv�ll�");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-illalla" phrase
	 *
	 * \param theTime The time
	 * \return The "N-illalla" phrase
	 */
	// ----------------------------------------------------------------------

	const string on_weekday_evening(const NFmiTime & theTime)
	{
	  const string out = (lexical_cast<string>(theTime.GetWeekday())
						  +"-illalla");
	  return out;
	}


  } // namespace WeekdayTools
} // namespace TextGen

// ======================================================================
