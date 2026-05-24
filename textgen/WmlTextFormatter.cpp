// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::WmlTextFormatter
 */
// ======================================================================
/*!
 * \class TextGen::WmlTextFormatter
 *
 * \brief Glyph visitor generating WML output
 */
// ======================================================================

#include "WmlTextFormatter.h"
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

#include <boost/lexical_cast.hpp>

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

void WmlTextFormatter::dictionary(const std::shared_ptr<Dictionary>& theDict)
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

string WmlTextFormatter::format(const Glyph& theGlyph) const
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

string WmlTextFormatter::visit(const Glyph& theGlyph) const
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

string WmlTextFormatter::visit(const Integer& theInteger) const
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

string WmlTextFormatter::visit(const Real& theReal) const
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

string WmlTextFormatter::visit(const IntegerRange& theRange) const
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

string WmlTextFormatter::visit(const PositiveRange& theRange) const
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

string WmlTextFormatter::visit(const TimePhrase& theTime) const
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

string WmlTextFormatter::visit(const Sentence& theSentence) const
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

string WmlTextFormatter::visit(const Paragraph& theParagraph) const
{
  try
  {
    const string tags = Settings::optional_string(itsSectionVar + "::paragraph::wml::tags", "");
    const string sep = TextFormatterTools::wordSeparator(itsDictionary.get());

    string tmp =
        TextFormatterTools::realize(theParagraph.begin(), theParagraph.end(), *this, sep, "");
    ostringstream out;
    if (!tmp.empty())
    {
      out << "<p";
      if (!tags.empty())
        out << ' ' << tags;
      out << '>' << tmp << "<br/></p>";
    }
    return out.str();
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

string WmlTextFormatter::visit(const Header& theHeader) const
{
  try
  {
    const bool colon = Settings::optional_bool(itsSectionVar + "::header::colon", false);
    const int level = Settings::optional_int(itsSectionVar + "::header::wml::level", 1);

    const string sep = TextFormatterTools::wordSeparator(itsDictionary.get());
    string text = TextFormatterTools::realize(theHeader.begin(), theHeader.end(), *this, sep, "");
    text = TextFormatterTools::capitalize(text);

    if (text.empty())
      return "";

    ostringstream out;

    if (level == 1)
      out << "<p><b>" << text << (colon ? ":" : "") << "</b><br/></p>";
    else if (level == 2)
      out << "<p>" << text << (colon ? ":" : "") << "</p>";

    return out.str();
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

string WmlTextFormatter::visit(const Document& theDocument) const
{
  try
  {
    return TextFormatterTools::realize(theDocument.begin(), theDocument.end(), *this, "\n\n", "");
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

string WmlTextFormatter::visit(const SectionTag& theSection) const
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

string WmlTextFormatter::visit(const StoryTag& theStory) const
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

string WmlTextFormatter::visit(const WeatherTime& theTime) const
{
  try
  {
    return TextFormatterTools::format_time(theTime.time(), itsStoryVar, "wml");
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

string WmlTextFormatter::visit(const TimePeriod& thePeriod) const
{
  try
  {
    return TextFormatterTools::format_time(thePeriod.weatherPeriod(), itsStoryVar, "wml");
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

}  // namespace TextGen

// ======================================================================
