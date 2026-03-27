// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::SectionTag
 */
// ======================================================================
/*!
 * \class TextGen::SectionTag
 * \brief Represents a non-realizable section name tag for formatter
 */
// ======================================================================

#include "SectionTag.h"
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

SectionTag::~SectionTag() = default;
// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 */
// ----------------------------------------------------------------------

SectionTag::SectionTag(std::string theName, const bool& prefixTag /*= true*/)
    : itsName(std::move(theName)), itsPrefixTag(prefixTag)
{
}

// ----------------------------------------------------------------------
/*!
 * \brief Return a clone
 */
// ----------------------------------------------------------------------

std::shared_ptr<Glyph> SectionTag::clone() const
{
  try
  {
    std::shared_ptr<Glyph> ret(new SectionTag(*this));
    return ret;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the text for the SectionTag
 *
 * \param theDictionary The dictionary to be used
 * \return The text
 */
// ----------------------------------------------------------------------

std::string SectionTag::realize(const Dictionary& /*theDictionary*/) const
{
  try
  {
    return itsName;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the text for the number
 *
 * \param theFormatter The formatter
 * \return The text
 */
// ----------------------------------------------------------------------

std::string SectionTag::realize(const TextFormatter& theFormatter) const
{
  try
  {
    return theFormatter.visit(*this);
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Returns false since SectionTag is not a separator
 */
// ----------------------------------------------------------------------

bool SectionTag::isDelimiter() const
{
  try
  {
    return false;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Returns true if tag is locatated before the story, false if after the story
 */
// ----------------------------------------------------------------------

bool SectionTag::isPrefixTag() const
{
  try
  {
    return itsPrefixTag;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
}  // namespace TextGen

// ======================================================================
