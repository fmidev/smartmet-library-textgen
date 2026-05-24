// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::ExtendedDebugTextFormatter
 */
// ======================================================================
/*!
 * \class TextGen::ExtendedDebugTextFormatter
 *
 * \brief Glyph visitor generating output for debug logs
 */
// ======================================================================

#include "ExtendedDebugTextFormatter.h"
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

string ExtendedDebugTextFormatter::format(const Glyph& theGlyph) const
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

string ExtendedDebugTextFormatter::visit(const Glyph& theGlyph) const
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

string ExtendedDebugTextFormatter::visit(const Integer& theInteger) const
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

string ExtendedDebugTextFormatter::visit(const Real& theReal) const
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

string ExtendedDebugTextFormatter::visit(const IntegerRange& theRange) const
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

string ExtendedDebugTextFormatter::visit(const PositiveRange& theRange) const
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

string ExtendedDebugTextFormatter::visit(const TimePhrase& theTime) const
{
  try
  {
    string txt = TextFormatterTools::realize(theTime.begin(), theTime.end(), *this, " ", "");
    return "<timephrase var=\"" + itsSectionVar + "\">\n" + txt + "</timephrase>";
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

string ExtendedDebugTextFormatter::visit(const Sentence& theSentence) const
{
  try
  {
    string txt =
        TextFormatterTools::realize(theSentence.begin(), theSentence.end(), *this, "", "\n");
    txt = TextFormatterTools::capitalize(txt);
    TextFormatterTools::punctuate(txt);

    return "<sentence var=\"" + itsSectionVar + "\">\n" + txt + "\n</sentence>\n";
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

string ExtendedDebugTextFormatter::visit(const Paragraph& theParagraph) const
{
  try
  {
    string txt =
        TextFormatterTools::realize(theParagraph.begin(), theParagraph.end(), *this, "", "\n");
    return "<paragraph var=\"" + itsSectionVar + "\">\n" + txt + "</paragraph>\n";
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

string ExtendedDebugTextFormatter::visit(const Header& theHeader) const
{
  try
  {
    string txt = TextFormatterTools::realize(theHeader.begin(), theHeader.end(), *this, "", "\n");
    txt = TextFormatterTools::capitalize(txt);
    if (!txt.empty())
      txt += ':';

    return "<header var=\"" + itsSectionVar + "\">" + txt + "</header>\n";
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

string ExtendedDebugTextFormatter::visit(const Document& theDocument) const
{
  try
  {
    string txt =
        TextFormatterTools::realize(theDocument.begin(), theDocument.end(), *this, " ", "");
    return "<document var=\"" + itsSectionVar + "\">\n" + txt + "</document>\n";
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

string ExtendedDebugTextFormatter::visit(const SectionTag& theSection) const
{
  try
  {
    itsSectionVar = theSection.realize(itsDictionary);
    return "<sectiontag var=\"" + itsSectionVar + "\"/>\n";
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

string ExtendedDebugTextFormatter::visit(const StoryTag& theStory) const
{
  try
  {
    itsStoryVar = theStory.realize(itsDictionary);

    if (theStory.isPrefixTag())
      return TextFormatterTools::get_story_value_param(itsStoryVar, itsProductName);

    return "<storytag var=\"" + itsSectionVar + "\" story=\"" + itsStoryVar + "\"/>\n";
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

string ExtendedDebugTextFormatter::visit(const WeatherTime& theTime) const
{
  try
  {
    auto txt = TextFormatterTools::format_time(theTime.time(), itsStoryVar, "debug");
    return "<time var=\"" + txt + "\"</time>";
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

string ExtendedDebugTextFormatter::visit(const TimePeriod& thePeriod) const
{
  try
  {
    auto txt = TextFormatterTools::format_time(thePeriod.weatherPeriod(), itsStoryVar, "debug");
    return "<timeperiod var=\"" + txt + "\"</timeperiod>";
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
}  // namespace TextGen

// ======================================================================
