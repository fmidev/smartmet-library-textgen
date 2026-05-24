// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::SpeechTextFormatter
 */
// ======================================================================
/*!
 * \class TextGen::SpeechTextFormatter
 *
 * \brief Glyph visitor generating input for BitLips speech generator
 */
// ======================================================================

#include "SpeechTextFormatter.h"
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

void SpeechTextFormatter::dictionary(const std::shared_ptr<Dictionary>& theDict)
{
  itsDictionary = theDict;
}

// ----------------------------------------------------------------------
/*!
 * \brief Format a glyph
 *
 * \param theGlyph The glyph
 */
// ----------------------------------------------------------------------

string SpeechTextFormatter::format(const Glyph& theGlyph) const
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

string SpeechTextFormatter::visit(const Glyph& theGlyph) const
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

string SpeechTextFormatter::visit(const Integer& theInteger) const
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
 *
 * Note: The generator understands only numbers of the form "2,8"
 */
// ----------------------------------------------------------------------

string SpeechTextFormatter::visit(const Real& theReal) const
{
  try
  {
    Real dummy(theReal.value(), theReal.precision(), false);

    return dummy.realize(*itsDictionary);
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

string SpeechTextFormatter::visit(const IntegerRange& theRange) const
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

string SpeechTextFormatter::visit(const PositiveRange& theRange) const
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

string SpeechTextFormatter::visit(const TimePhrase& theTime) const
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

string SpeechTextFormatter::visit(const Sentence& theSentence) const
{
  try
  {
    const string sep = TextFormatterTools::wordSeparator(itsDictionary.get());
    string ret = TextFormatterTools::realize(theSentence.begin(), theSentence.end(), *this, sep, "");
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

string SpeechTextFormatter::visit(const Paragraph& theParagraph) const
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

string SpeechTextFormatter::visit(const Header& theHeader) const
{
  try
  {
    const bool colon = Settings::optional_bool(itsSectionVar + "::header::colon", false);

    const string sep = TextFormatterTools::wordSeparator(itsDictionary.get());
    string ret = TextFormatterTools::realize(theHeader.begin(), theHeader.end(), *this, sep, "");
    ret = TextFormatterTools::capitalize(ret);
    if (!ret.empty())
    {
      if (colon)
        ret += ':';
      else
        ret += TextFormatterTools::sentenceEnd(itsDictionary.get());
    }

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

string SpeechTextFormatter::visit(const Document& theDocument) const
{
  try
  {
    const string sep = TextFormatterTools::wordSeparator(itsDictionary.get());
    string ret = TextFormatterTools::realize(theDocument.begin(), theDocument.end(), *this, sep, "");
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

string SpeechTextFormatter::visit(const SectionTag& theSection) const
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

string SpeechTextFormatter::visit(const StoryTag& theStory) const
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

string SpeechTextFormatter::visit(const WeatherTime& theTime) const
{
  try
  {
    return TextFormatterTools::format_time(theTime.time(), itsStoryVar, "speech");
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

string SpeechTextFormatter::visit(const TimePeriod& thePeriod) const
{
  try
  {
    return TextFormatterTools::format_time(thePeriod.weatherPeriod(), itsStoryVar, "speech");
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
}  // namespace TextGen

// ======================================================================
