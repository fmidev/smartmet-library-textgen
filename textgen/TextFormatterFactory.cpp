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
 * std::shared_ptr<TextFormatter> form1(TextFormatterFactory::create("plain"));
 * std::shared_ptr<TextFormatter> form2(TextFormatterFactory::create("html"));
 * \endcode
 *
 */
// ======================================================================

#include "TextFormatterFactory.h"
#include "CssTextFormatter.h"
#include "DebugTextFormatter.h"
#include "ExtendedDebugTextFormatter.h"
#include "HtmlTextFormatter.h"
#include "PlainLinesTextFormatter.h"
#include "PlainTextFormatter.h"
#include "SingleLineTextFormatter.h"
#include "SoneraTextFormatter.h"
#include "SpeechTextFormatter.h"
#include "WmlTextFormatter.h"
#include <macgyver/Exception.h>

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

TextFormatter* create(const std::string& theType)
{
  if (theType == "singleline")
    return new SingleLineTextFormatter();
  if (theType == "plain")
    return new PlainTextFormatter();
  if (theType == "plainlines")
    return new PlainLinesTextFormatter();
  if (theType == "html")
    return new HtmlTextFormatter();
  if (theType == "css")
    return new CssTextFormatter();
  if (theType == "speechtext")
    return new SpeechTextFormatter();
  if (theType == "wml")
    return new WmlTextFormatter();
  if (theType == "sonera")
    return new SoneraTextFormatter();
  if (theType == "debug")
    return new DebugTextFormatter();
  if (theType == "extended-debug")
    return new ExtendedDebugTextFormatter();
  throw Fmi::Exception(BCP, "Error: Unknown text formatter type " + theType);
}

}  // namespace TextFormatterFactory
}  // namespace TextGen

// ======================================================================
