// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::TextFormatterFactory
 */
// ======================================================================

#include "TextFormatterFactory.h"
#include "PlainTextFormatter.h"
#include "HtmlTextFormatter.h"
#include "TextGenError.h"

using namespace boost;

namespace TextGen
{
  namespace TextFormatterFactory
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Create a text formatter of given type
	 *
	 * The available formatters are
	 *
	 *    - plain for plain text
	 *    - html for html
	 *
	 * \param theType The type of the text formatter
	 * \return An uninitialized text formatter
	 */
	// ----------------------------------------------------------------------
	
	shared_ptr<TextFormatter> create(const std::string & theType)
	{
	  if(theType == "plain")
		return shared_ptr<TextFormatter>(new PlainTextFormatter());
	  if(theType == "html")
		return shared_ptr<TextFormatter>(new HtmlTextFormatter());
	  throw TextGenError("Error: Unknown text formatter type "+theType);
	}

  } // namespace TextFormatterFactory
} // namespace TextGen

// ======================================================================
