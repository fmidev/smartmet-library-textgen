// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::Text
 */
// ======================================================================
/*!
 * \class TextGen::Text
 * \brief Representation of a generic text component
 */
// ======================================================================

#include "Text.h"
#include "Dictionary.h"
#include "TextFormatter.h"
#include <macgyver/Exception.h>

#include <memory>

#include <sstream>
#include <utility>

using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Destructor
 */
// ----------------------------------------------------------------------

Text::~Text() = default;
// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 */
// ----------------------------------------------------------------------

Text::Text(std::string theText) : itsText(std::move(theText)) {}
// ----------------------------------------------------------------------
/*!
 * \brief Return a clone
 */
// ----------------------------------------------------------------------

std::shared_ptr<Glyph> Text::clone() const
{
  std::shared_ptr<Glyph> ret(new Text(*this));
  return ret;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the text for the Text
 *
 * \param theDictionary The dictionary to be used
 * \return The text
 */
// ----------------------------------------------------------------------

std::string Text::realize(const Dictionary& /*theDictionary*/) const
{
  return itsText;
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the text for the number
 *
 * \param theFormatter The formatter
 * \return The text
 */
// ----------------------------------------------------------------------

std::string Text::realize(const TextFormatter& /*theFormatter*/) const
{
  return itsText;
}
// ----------------------------------------------------------------------
/*!
 * \brief Returns false since Text is not a separator
 */
// ----------------------------------------------------------------------

bool Text::isDelimiter() const
{
  return false;
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the value of the text
 *
 * \return The text
 */
// ----------------------------------------------------------------------

const std::string& Text::value() const
{
  return itsText;
}
}  // namespace TextGen

// ======================================================================
