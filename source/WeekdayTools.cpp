// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::WeekdayTools
 */
// ======================================================================

#include "WeekdayTools.h"
#include "NFmiTime.h"
#include "boost/lexical_cast.hpp"

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

  } // namespace WeekdayTools
} // namespace TextGen

// ======================================================================
