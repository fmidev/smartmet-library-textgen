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
	  const string out = lexical_cast<string>(theTime.GetWeekday())+"-na";
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-vastaisena yönä" description
	 *
	 * \param theTime The time
	 * \return The "N-vastaisena yönä" phrase
	 */
	// ----------------------------------------------------------------------

	string night_against_weekday(const NFmiTime & theTime)
	{
	  const string out = lexical_cast<string>(theTime.GetWeekday())+"-vastaisena yönä";
	  return out;
	}

  } // namespace WeekdayTools
} // namespace TextGen

// ======================================================================
