// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextFormatterTools
 */
// ======================================================================

#include "TextFormatterTools.h"
#include "TextFormatter.h"
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
	  if(!theString.empty())
		{
		  setlocale(LC_ALL, "Finnish");
		  theString[0] = ::toupper(theString[0]);
		  setlocale(LC_ALL, "C");
		}
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
