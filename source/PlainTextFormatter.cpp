// ======================================================================
/*!
 * \file
 * \brief Implementation of class PlainTextFormatter
 */
// ======================================================================

#include "PlainTextFormatter.h"
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
  
  void PlainTextFormatter::dictionary(const shared_ptr<Dictionary> & theDict)
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
  
  string PlainTextFormatter::format(const Glyph & theGlyph) const
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
  
  string PlainTextFormatter::visit(const Glyph & theGlyph) const
  {
	return theGlyph.realize(*itsDictionary);
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a sentence
   */
  // ----------------------------------------------------------------------
  
  string PlainTextFormatter::visit(const Sentence & theSentence) const
  {
	string ret = TextFormatterTools::realize(theSentence.begin(),
											 theSentence.end(),
											 " ",
											 *this);
	TextFormatterTools::capitalize(ret);
	TextFormatterTools::punctuate(ret);
	
	return ret;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a paragraph
   */
  // ----------------------------------------------------------------------
  
  string PlainTextFormatter::visit(const Paragraph & theParagraph) const
  {
	string ret = TextFormatterTools::realize(theParagraph.begin(),
											 theParagraph.end(),
											 " ",
											 *this);
	
	return ret;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a header
   */
  // ----------------------------------------------------------------------
  
  string PlainTextFormatter::visit(const Header & theHeader) const
  {
	string ret = TextFormatterTools::realize(theHeader.begin(),
											 theHeader.end(),
											 " ",
											 *this);
	if(!ret.empty())
	  ret += ':';
	
	return ret;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a document
   */
  // ----------------------------------------------------------------------
  
  string PlainTextFormatter::visit(const Document & theDocument) const
  {
	string ret = TextFormatterTools::realize(theDocument.begin(),
											 theDocument.end(),
											 "\n\n",
											 *this);
	return ret;
  }

} // namespace TextGen
  
// ======================================================================
  
  