// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::PlainTextFormatter
 */
// ======================================================================
/*!
 * \class TextGen::PlainTextFormatter
 *
 * \brief Glyph visitor generating normal ASCII output
 */
// ======================================================================

#include "PlainTextFormatter.h"
#include "Dictionary.h"
#include "Document.h"
#include "Glyph.h"
#include "Header.h"
#include "Integer.h"
#include "IntegerRange.h"
#include "Paragraph.h"
#include "PositiveRange.h"
#include "Real.h"
#include "SectionTag.h"
#include "Sentence.h"
#include "StoryTag.h"
#include "TextFormatterTools.h"
#include "TimePeriod.h"
#include "TimePhrase.h"
#include "WeatherTime.h"
#include <calculator/Settings.h>
#include <macgyver/Exception.h>

using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Set the dictionary to be used while formatting
 *
 * \param theDict The dictionary (shared)
 */
// ----------------------------------------------------------------------

void PlainTextFormatter::dictionary(const std::shared_ptr<Dictionary>& theDict)
{
  try
  {
    itsDictionary = theDict;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Format a glyph
 *
 * \param theGlyph The glyph
 */
// ----------------------------------------------------------------------

string PlainTextFormatter::format(const Glyph& theGlyph) const
{
  try
  {
    return theGlyph.realize(*this);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Visit a glyph
 *
 * \param theGlyph The glyph
 */
// ----------------------------------------------------------------------

string PlainTextFormatter::visit(const Glyph& theGlyph) const
{
  try
  {
    return theGlyph.realize(*itsDictionary);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Visit an integer number
 */
// ----------------------------------------------------------------------

string PlainTextFormatter::visit(const Integer& theInteger) const
{
  try
  {
    return theInteger.realize(*itsDictionary);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Visit a float
 */
// ----------------------------------------------------------------------

string PlainTextFormatter::visit(const Real& theReal) const
{
  try
  {
    return theReal.realize(*itsDictionary);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Visit an integer range
 */
// ----------------------------------------------------------------------

string PlainTextFormatter::visit(const IntegerRange& theRange) const
{
  try
  {
    return theRange.realize(*itsDictionary);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Visit a positive integer range
 */
// ----------------------------------------------------------------------

string PlainTextFormatter::visit(const PositiveRange& theRange) const
{
  try
  {
    return theRange.realize(*itsDictionary);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Visit a time phrase
 */
// ----------------------------------------------------------------------

string PlainTextFormatter::visit(const TimePhrase& theTime) const
{
  try
  {
    const string sep = TextFormatterTools::wordSeparator(itsDictionary.get());
    string ret = TextFormatterTools::realize(theTime.begin(), theTime.end(), *this, sep, "");
    return ret;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Visit a sentence
 */
// ----------------------------------------------------------------------

string PlainTextFormatter::visit(const Sentence& theSentence) const
{
  try
  {
    const string sep = TextFormatterTools::wordSeparator(itsDictionary.get());
    string ret =
        TextFormatterTools::realize(theSentence.begin(), theSentence.end(), *this, sep, "");
    ret = TextFormatterTools::capitalize(ret);
    if (!ret.empty())
      ret += TextFormatterTools::sentenceEnd(itsDictionary.get());

    return ret;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Visit a paragraph
 */
// ----------------------------------------------------------------------

string PlainTextFormatter::visit(const Paragraph& theParagraph) const
{
  try
  {
    const string sep = TextFormatterTools::wordSeparator(itsDictionary.get());
    string ret =
        TextFormatterTools::realize(theParagraph.begin(), theParagraph.end(), *this, sep, "");
    return ret;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Visit a header
 */
// ----------------------------------------------------------------------

string PlainTextFormatter::visit(const Header& theHeader) const
{
  try
  {
    const bool colon = Settings::optional_bool(itsSectionVar + "::header::colon", false);

    const string sep = TextFormatterTools::wordSeparator(itsDictionary.get());
    string ret = TextFormatterTools::realize(theHeader.begin(), theHeader.end(), *this, sep, "");
    ret = TextFormatterTools::capitalize(ret);
    if (!ret.empty() && colon)
      ret += ':';

    return ret;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Visit a document
 */
// ----------------------------------------------------------------------

string PlainTextFormatter::visit(const Document& theDocument) const
{
  try
  {
    string ret =
        TextFormatterTools::realize(theDocument.begin(), theDocument.end(), *this, "\n\n", "");
    ret += '\n';
    return ret;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Visit a section tag
 */
// ----------------------------------------------------------------------

string PlainTextFormatter::visit(const SectionTag& theSection) const
{
  try
  {
    itsSectionVar = theSection.realize(*itsDictionary);
    return "";
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Visit a story tag
 */
// ----------------------------------------------------------------------

string PlainTextFormatter::visit(const StoryTag& theStory) const
{
  try
  {
    itsStoryVar = theStory.realize(*itsDictionary);

    if (theStory.isPrefixTag())
    {
      return TextFormatterTools::get_story_value_param(itsStoryVar, itsProductName);
    }

    return "";
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Visit Time
 */
// ----------------------------------------------------------------------

string PlainTextFormatter::visit(const WeatherTime& theTime) const
{
  try
  {
    return TextFormatterTools::format_time(theTime.time(), itsStoryVar, "plain");
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Visit TimePeriod
 */
// ----------------------------------------------------------------------

string PlainTextFormatter::visit(const TimePeriod& thePeriod) const
{
  try
  {
    return TextFormatterTools::format_time(thePeriod.weatherPeriod(), itsStoryVar, "plain");
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

}  // namespace TextGen

// ======================================================================
