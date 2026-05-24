// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::DebugTextFormatter
 */
// ======================================================================
/*!
 * \class TextGen::DebugTextFormatter
 *
 * \brief Glyph visitor generating output for debug logs
 */
// ======================================================================

#include "DebugTextFormatter.h"
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

#include <sstream>
#include <string>

using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Format a glyph
 *
 * \param theGlyph The glyph
 */
// ----------------------------------------------------------------------

string DebugTextFormatter::format(const Glyph& theGlyph) const
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

string DebugTextFormatter::visit(const Glyph& theGlyph) const
{
  try
  {
    return theGlyph.realize(itsDictionary);
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

string DebugTextFormatter::visit(const Integer& theInteger) const
{
  try
  {
    return theInteger.realize(itsDictionary);
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

string DebugTextFormatter::visit(const Real& theReal) const
{
  try
  {
    return theReal.realize(itsDictionary);
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

string DebugTextFormatter::visit(const IntegerRange& theRange) const
{
  try
  {
    return theRange.realize(itsDictionary);
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

string DebugTextFormatter::visit(const PositiveRange& theRange) const
{
  try
  {
    return theRange.realize(itsDictionary);
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

string DebugTextFormatter::visit(const TimePhrase& theTime) const
{
  try
  {
    string ret = TextFormatterTools::realize(theTime.begin(), theTime.end(), *this, " ", "");
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

string DebugTextFormatter::visit(const Sentence& theSentence) const
{
  try
  {
    string ret =
        TextFormatterTools::realize(theSentence.begin(), theSentence.end(), *this, "", "\n");
    ret = TextFormatterTools::capitalize(ret);
    TextFormatterTools::punctuate(ret);

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

string DebugTextFormatter::visit(const Paragraph& theParagraph) const
{
  try
  {
    string ret =
        TextFormatterTools::realize(theParagraph.begin(), theParagraph.end(), *this, "", "\n");
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

string DebugTextFormatter::visit(const Header& theHeader) const
{
  try
  {
    string ret = TextFormatterTools::realize(theHeader.begin(), theHeader.end(), *this, "", "\n");
    ret = TextFormatterTools::capitalize(ret);
    if (!ret.empty())
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

string DebugTextFormatter::visit(const Document& theDocument) const
{
  try
  {
    string ret =
        TextFormatterTools::realize(theDocument.begin(), theDocument.end(), *this, " ", "");
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

string DebugTextFormatter::visit(const SectionTag& theSection) const
{
  try
  {
    itsSectionVar = theSection.realize(itsDictionary);
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

string DebugTextFormatter::visit(const StoryTag& theStory) const
{
  try
  {
    itsStoryVar = theStory.realize(itsDictionary);

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

string DebugTextFormatter::visit(const WeatherTime& theTime) const
{
  try
  {
    return TextFormatterTools::format_time(theTime.time(), itsStoryVar, "debug");
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

string DebugTextFormatter::visit(const TimePeriod& thePeriod) const
{
  try
  {
    return TextFormatterTools::format_time(thePeriod.weatherPeriod(), itsStoryVar, "debug");
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
}  // namespace TextGen

// ======================================================================
