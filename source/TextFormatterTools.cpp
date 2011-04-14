// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::TextFormatterTools
 */
// ======================================================================
/*!
 * \namespace TextGen::TextFormatterTools
 *
 * \brief Common utility functions for actual TextFormatter classes
 *
 */
// ======================================================================

#include "TextFormatterTools.h"
#include <newbase/NFmiStringTools.h>
#include <cctype>
#include <clocale>

using namespace std;

namespace TextGen
{
  namespace TextFormatterTools
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Capitalize the given string
	 *
	 * \todo Capitalize in global locale, we will assume that the user
	 *       has set some local on.
	 *
	 * \param theString The string to capitalize
	 */
	// ----------------------------------------------------------------------

	void capitalize(std::string & theString)
	{
	  NFmiStringTools::FirstCharToUpper(theString);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Punctuate the given string
	 *
	 * \param theString The string to punctuate
	 */
	// ----------------------------------------------------------------------

	void punctuate(std::string & theString)
	{
	  if(!theString.empty())
		theString += '.';
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Create pattern string of form [N]
	 *
	 * \param n The pattern number
	 */
	// ----------------------------------------------------------------------

	std::string make_needle(int n)
	{
	  return "[" + boost::lexical_cast<std::string>(n) + "]";
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Count patterns in order [1] ... [N]
	 *
	 * \param theString the string containing 0-N patterns
	 */
	// ----------------------------------------------------------------------

	int count_patterns(const std::string & theString)
	{
	  int n = 0;
	  std::string needle = "[1]";

	  while(true)
		{
		  if(!boost::algorithm::find_first(theString,needle))
			return n;
		  ++n;
		  needle = make_needle(n+1);
		}
	}

  } // namespace TextFormatterTools
} // namespace T
