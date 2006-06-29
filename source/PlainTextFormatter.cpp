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
#include "Real.h"
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
  
  const string PlainTextFormatter::format(const Glyph & theGlyph) const
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
  
  const string PlainTextFormatter::visit(const Glyph & theGlyph) const
  {
	return theGlyph.realize(*itsDictionary);
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit an integer number
   */
  // ----------------------------------------------------------------------

  const string PlainTextFormatter::visit(const Integer & theInteger) const
  {
	return theInteger.realize(*itsDictionary);
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a float
   */
  // ----------------------------------------------------------------------

  const string PlainTextFormatter::visit(const Real & theReal) const
  {
	return theReal.realize(*itsDictionary);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Visit an integer range
   */
  // ----------------------------------------------------------------------

  const string PlainTextFormatter::visit(const IntegerRange & theRange) const
  {
	return theRange.realize(*itsDictionary);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a sentence
   */
  // ----------------------------------------------------------------------
  
  const string PlainTextFormatter::visit(const Sentence & theSentence) const
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
  
  const string PlainTextFormatter::visit(const Paragraph & theParagraph) const
  {
	string ret = TextFormatterTools::realize(theParagraph.begin(),
											 theParagraph.end(),
											 *this,
											 " ","");
	return ret;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a header
   */
  // ----------------------------------------------------------------------
  
  const string PlainTextFormatter::visit(const Header & theHeader) const
  {
	const bool colon = Settings::optional_bool(itsSectionVar+"::header::colon",false);

    string ret = TextFormatterTools::realize(theHeader.begin(),
											 theHeader.end(),
											 *this,
											 " ","");
	TextFormatterTools::capitalize(ret);
	if(!ret.empty() && colon)
	  ret += ':';
	
	return ret;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a document
   */
  // ----------------------------------------------------------------------
  
  const string PlainTextFormatter::visit(const Document & theDocument) const
  {
	string ret = TextFormatterTools::realize(theDocument.begin(),
											 theDocument.end(),
											 *this,
											 "\n\n",
											 "");
	ret += '\n';
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a section tag
   */
  // ----------------------------------------------------------------------

  const string PlainTextFormatter::visit(const SectionTag & theSection) const
  {
	itsSectionVar = theSection.realize(*itsDictionary);
	return "";
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a story tag
   */
  // ----------------------------------------------------------------------

  const string PlainTextFormatter::visit(const StoryTag & theStory) const
  {
	itsStoryVar = theStory.realize(*itsDictionary);
	return "";
  }

} // namespace TextGen
  
// ======================================================================
  
  
