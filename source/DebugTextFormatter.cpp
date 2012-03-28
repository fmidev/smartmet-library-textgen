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
#include "Real.h"
#include "Glyph.h"
#include "Header.h"
#include "Integer.h"
#include "IntegerRange.h"
#include "Paragraph.h"
#include "SectionTag.h"
#include "Sentence.h"
#include "StoryTag.h"
#include "TextFormatterTools.h"

#include <sstream>
#include <string>

using namespace std;
using namespace boost;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Format a glyph
   *
   * \param theGlyph The glyph
   */
  // ----------------------------------------------------------------------
  
  const string DebugTextFormatter::format(const Glyph & theGlyph) const
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
  
  const string DebugTextFormatter::visit(const Glyph & theGlyph) const
  {
	return theGlyph.realize(itsDictionary);
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit an integer number
   */
  // ----------------------------------------------------------------------

  const string DebugTextFormatter::visit(const Integer & theInteger) const
  {
	return theInteger.realize(itsDictionary);
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a float
   */
  // ----------------------------------------------------------------------

  const string DebugTextFormatter::visit(const Real & theReal) const
  {
	return theReal.realize(itsDictionary);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Visit an integer range
   */
  // ----------------------------------------------------------------------

  const string DebugTextFormatter::visit(const IntegerRange & theRange) const
  {
	return theRange.realize(itsDictionary);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a sentence
   */
  // ----------------------------------------------------------------------
  
  const string DebugTextFormatter::visit(const Sentence & theSentence) const
  {
	string ret = TextFormatterTools::realize(theSentence.begin(),
											 theSentence.end(),
											 *this,
											 "",
											 "\n");
	ret = TextFormatterTools::capitalize(ret);
	TextFormatterTools::punctuate(ret);
	
	return ret;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a paragraph
   */
  // ----------------------------------------------------------------------
  
  const string DebugTextFormatter::visit(const Paragraph & theParagraph) const
  {
	string ret = TextFormatterTools::realize(theParagraph.begin(),
											 theParagraph.end(),
											 *this,
											 "","\n");
	return ret;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a header
   */
  // ----------------------------------------------------------------------
  
  const string DebugTextFormatter::visit(const Header & theHeader) const
  {
    string ret = TextFormatterTools::realize(theHeader.begin(),
											 theHeader.end(),
											 *this,
											 "","\n");
	ret = TextFormatterTools::capitalize(ret);
	if(!ret.empty())
	  ret += ':';
	
	return ret;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a document
   */
  // ----------------------------------------------------------------------
  
  const string DebugTextFormatter::visit(const Document & theDocument) const
  {
	string ret = TextFormatterTools::realize(theDocument.begin(),
											 theDocument.end(),
											 *this,
											 " ",
											 "");
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a section tag
   */
  // ----------------------------------------------------------------------

  const string DebugTextFormatter::visit(const SectionTag & theSection) const
  {
	itsSectionVar = theSection.realize(itsDictionary);
	return "";
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a story tag
   */
  // ----------------------------------------------------------------------

  const string DebugTextFormatter::visit(const StoryTag & theStory) const
  {
	itsStoryVar = theStory.realize(itsDictionary);
	return "";
  }

} // namespace TextGen
  
// ======================================================================
