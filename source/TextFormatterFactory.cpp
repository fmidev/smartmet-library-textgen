// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::TextFormatterFactory
 */
// ======================================================================
/*!
 * \namespace TextGen::TextFormatterFactory
 *
 * \brief TextFormatter creation services
 *
 * The responsibility of the TextFormatterFactory namespace is to provide
 * TextFormatter creation services.
 *
 * For example,
 * \code
 * boost::shared_ptr<TextFormatter> form1 = TextFormatterFactory::create("plain");
 * boost::shared_ptr<TextFormatter> form2 = TextFormatterFactory::create("html");
 * \endcode
 *
 */
// ======================================================================

#include "TextFormatterFactory.h"
#include "DebugTextFormatter.h"
#include "PlainTextFormatter.h"
#include "PlainLinesTextFormatter.h"
#include "HtmlTextFormatter.h"
#include "WmlTextFormatter.h"
#include "SoneraTextFormatter.h"
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
	  if(theType == "plainlines")
		return shared_ptr<TextFormatter>(new PlainLinesTextFormatter());
	  if(theType == "html")
		return shared_ptr<TextFormatter>(new HtmlTextFormatter());
	  if(theType == "wml")
		return shared_ptr<TextFormatter>(new WmlTextFormatter());
	  if(theType == "sonera")
		return shared_ptr<TextFormatter>(new SoneraTextFormatter());
	  if(theType == "debug")
		return shared_ptr<TextFormatter>(new DebugTextFormatter());
	  throw TextGenError("Error: Unknown text formatter type "+theType);
	}

  } // namespace TextFormatterFactory
} // namespace TextGen

// ======================================================================
