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
   * \brief Set the variable to be used while formatting
   *
   * The variable and its subvariables may be used to control the
   * details of the text formatter.
   *
   * The variable can be accessed with NFmiSettings::IsSet()
   * and other methods.
   *
   * \param theVariable The variable name
   */
  // ----------------------------------------------------------------------
  
  void HtmlTextFormatter::variable(const string & theVariable)
  {
	itsVar = theVariable;
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
  
  
