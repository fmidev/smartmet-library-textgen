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
#include "NFmiStringTools.h"
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

  } // namespace TextFormatterTools
} // namespace T
