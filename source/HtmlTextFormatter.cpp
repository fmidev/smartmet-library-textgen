// ======================================================================
/*!
 * \file
 * \brief Implementation of class HtmlTextFormatter
 */
// ======================================================================

#include "HtmlTextFormatter.h"
#include "Dictionary.h"
#include "Document.h"
#include "Glyph.h"
#include "Header.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "TextFormatterTools.h"

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
	string ret = "<h1>";
	string text = TextFormatterTools::realize(theHeader.begin(),
											  theHeader.end(),
											  *this,
											  " ",
											  "");
	TextFormatterTools::capitalize(text);
	ret += text;
	ret += "</h1>";
	return ret;
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

} // namespace TextGen
  
// ======================================================================
  
  
