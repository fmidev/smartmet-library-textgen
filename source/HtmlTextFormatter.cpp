// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::HtmlTextFormatter
 */
// ======================================================================
/*!
 * \class TextGen::HtmlTextFormatter
 *
 * \brief Glyph visitor generating HTML output
 */
// ======================================================================

#include "HtmlTextFormatter.h"
#include "Dictionary.h"
#include "Document.h"
#include "Glyph.h"
#include "Header.h"
#include "Integer.h"
#include "IntegerRange.h"
#include "Paragraph.h"
#include "SectionTag.h"
#include "Sentence.h"
#include "Settings.h"
#include "StoryTag.h"
#include "TextFormatterTools.h"

#include "boost/lexical_cast.hpp"

using namespace std;
using namespace boost;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Set the dictionary to be used while formatting
   *
   * \param theDict The dictionary (shared)
   */
  // ----------------------------------------------------------------------
  
  void HtmlTextFormatter::dictionary(const shared_ptr<Dictionary> & theDict)
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
  
  string HtmlTextFormatter::format(const Glyph & theGlyph) const
  {
	return theGlyph.realize(*this);
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a glyph
   *
   * \param theGlyph The glyph
   */
  // ----------------------------------------------------------------------
  
  string HtmlTextFormatter::visit(const Glyph & theGlyph) const
  {
	return theGlyph.realize(*itsDictionary);
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit an integer number
   */
  // ----------------------------------------------------------------------

  string HtmlTextFormatter::visit(const Integer & theInteger) const
  {
	return theInteger.realize(*itsDictionary);
  }
  

  // ----------------------------------------------------------------------
  /*!
   * \brief Visit an integer range
   */
  // ----------------------------------------------------------------------

  string HtmlTextFormatter::visit(const IntegerRange & theRange) const
  {
	return theRange.realize(*itsDictionary);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a sentence
   */
  // ----------------------------------------------------------------------
  
  string HtmlTextFormatter::visit(const Sentence & theSentence) const
  {
	string ret = TextFormatterTools::realize(theSentence.begin(),
											 theSentence.end(),
											 *this,
											 " ",
											 "");
	TextFormatterTools::capitalize(ret);
	TextFormatterTools::punctuate(ret);
	
	return ret;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a paragraph
   */
  // ----------------------------------------------------------------------
  
  string HtmlTextFormatter::visit(const Paragraph & theParagraph) const
  {
	string ret = "<p>";
	ret += TextFormatterTools::realize(theParagraph.begin(),
									   theParagraph.end(),
									   *this,
									   " ",
									   "");
	ret += "</p>";
	
	return ret;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a header
   */
  // ----------------------------------------------------------------------
  
  string HtmlTextFormatter::visit(const Header & theHeader) const
  {
	const bool colon = Settings::optional_bool(itsSectionVar+"::header::colon",false);
	const int level = Settings::optional_int(itsSectionVar+"::header::html::level",1);

	string text = TextFormatterTools::realize(theHeader.begin(),
											  theHeader.end(),
											  *this,
											  " ",
											  "");
	TextFormatterTools::capitalize(text);

	if(text.empty())
	  return "";

	ostringstream out;
	out << "<h" << level << '>'
		<< text
		<< (colon ? ":" : "")
		<< "</h" << level << '>';
	return out.str();
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a document
   */
  // ----------------------------------------------------------------------
  
  string HtmlTextFormatter::visit(const Document & theDocument) const
  {
	string ret = "<div>";
	ret += TextFormatterTools::realize(theDocument.begin(),
									   theDocument.end(),
									   *this,
									   "\n\n",
									   "");
	ret += "</div>";
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a section tag
   */
  // ----------------------------------------------------------------------

  string HtmlTextFormatter::visit(const SectionTag & theSection) const
  {
	itsSectionVar = theSection.realize(*itsDictionary);
	return "";
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a story tag
   */
  // ----------------------------------------------------------------------

  string HtmlTextFormatter::visit(const StoryTag & theStory) const
  {
	itsStoryVar = theStory.realize(*itsDictionary);
	return "";
  }

} // namespace TextGen
  
// ======================================================================
