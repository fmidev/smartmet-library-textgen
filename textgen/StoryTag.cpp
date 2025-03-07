// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::StoryTag
 */
// ======================================================================
/*!
 * \class TextGen::StoryTag
 * \brief Represents a non-realizable story name tag for formatter
 */
// ======================================================================

#include "StoryTag.h"
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

StoryTag::~StoryTag() = default;
// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 */
// ----------------------------------------------------------------------

StoryTag::StoryTag(std::string theName, const bool& prefixTag /*= true*/)
    : itsName(std::move(theName)), itsPrefixTag(prefixTag)
{
}

// ----------------------------------------------------------------------
/*!
 * \brief Return a clone
 */
// ----------------------------------------------------------------------

std::shared_ptr<Glyph> StoryTag::clone() const
{
  std::shared_ptr<Glyph> ret(new StoryTag(*this));
  return ret;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the text for the StoryTag
 *
 * \param theDictionary The dictionary to be used
 * \return The text
 */
// ----------------------------------------------------------------------

std::string StoryTag::realize(const Dictionary& /*theDictionary*/) const
{
  return itsName;
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the text for the number
 *
 * \param theFormatter The formatter
 * \return The text
 */
// ----------------------------------------------------------------------

std::string StoryTag::realize(const TextFormatter& theFormatter) const
{
  return theFormatter.visit(*this);
}

// ----------------------------------------------------------------------
/*!
 * \brief Returns false since StoryTag is not a separator
 */
// ----------------------------------------------------------------------

bool StoryTag::isDelimiter() const
{
  return false;
}
// ----------------------------------------------------------------------
/*!
 * \brief Returns true if tag is locatated before the story, false if after the story
 */
// ----------------------------------------------------------------------

bool StoryTag::isPrefixTag() const
{
  return itsPrefixTag;
}
}  // namespace TextGen

// ======================================================================
