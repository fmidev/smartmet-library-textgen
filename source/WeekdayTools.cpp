// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::WeekdayTools
 */
// ======================================================================

#include "boost/lexical_cast.hpp" // boost included laitettava ennen newbase:n NFmiGlobals-includea, muuten MSVC:ssä min max määrittelyt jo tehty

#include "WeekdayTools.h"
#include "TextGenError.h"
#include "NFmiTime.h"

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

	string on_weekday(const NFmiTime & theTime)
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

	string on_weekday_time(const NFmiTime & theTime)
	{
	  const string out = (lexical_cast<string>(theTime.GetWeekday())
						  +"-na kello");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-vastaisena yönä" phrase
	 *
	 * \param theTime The time
	 * \return The "N-vastaisena yönä" phrase
	 */
	// ----------------------------------------------------------------------

	string night_against_weekday(const NFmiTime & theTime)
	{
	  const string out = (lexical_cast<string>(theTime.GetWeekday())
						  +"-vastaisena yönä");
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

	string until_weekday_morning(const NFmiTime & theTime)
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

	string until_weekday_evening(const NFmiTime & theTime)
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

	string until_weekday_time(const NFmiTime & theTime)
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
	 * \brief Return the "N-aamusta" phrase
	 *
	 * \param theTime The time
	 * \return The "N-aamusta" phrase
	 */
	// ----------------------------------------------------------------------

	string from_weekday_morning(const NFmiTime & theTime)
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

	string from_weekday_evening(const NFmiTime & theTime)
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

	string from_weekday_time(const NFmiTime & theTime)
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


  } // namespace WeekdayTools
} // namespace TextGen

// ======================================================================
