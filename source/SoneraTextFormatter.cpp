// ======================================================================
/*!
 * \file
 * \brief Implementation of class SoneraTextFormatter
 */
// ======================================================================

#include "SoneraTextFormatter.h"
#include "Dictionary.h"
#include "Document.h"
#include "Glyph.h"
#include "Header.h"
#include "Paragraph.h"
#include "Sentence.h"

using namespace std;
using namespace boost;

namespace
{
  template <typename Iterator>
  void sonera_realize(Iterator it,
					  Iterator end,
					  const TextGen::TextFormatter & theFormatter,
					  list<string> & theList)
  {
	for( ; it!=end; ++it)
	  {
		if( !((*it)->isDelimiter()))
		  theFormatter.format(**it);
	  }
  }
}

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  // ----------------------------------------------------------------------

  SoneraTextFormatter::SoneraTextFormatter()
	: TextFormatter()
	, itsParts()
	, itsDepth(0)
	, itsDictionary()
	, itsVar()
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set the dictionary to be used while formatting
   *
   * \param theDict The dictionary (shared)
   */
  // ----------------------------------------------------------------------
  
  void SoneraTextFormatter::dictionary(const shared_ptr<Dictionary> & theDict)
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
   * The variable can be accessed with Settings::isset()
   * and other methods.
   *
   * \param theVariable The variable name
   */
  // ----------------------------------------------------------------------
  
  void SoneraTextFormatter::variable(const string & theVariable)
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
  
  string SoneraTextFormatter::format(const Glyph & theGlyph) const
  {
	static string dummy("dummy");

	++itsDepth;
	theGlyph.realize(*this);
	--itsDepth;

	if(itsDepth>0)
	  return dummy;

	string ret;
	for(container_type::const_iterator it = itsParts.begin();
		it != itsParts.end();
		++it)
	  {
		if(!it->empty())
		  {
			if(!ret.empty())
			  ret += ",";
			ret += *it;
		  }
	  }
	return ret;

  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a glyph
   *
   * \param theGlyph The glyph
   */
  // ----------------------------------------------------------------------
  
  string SoneraTextFormatter::visit(const Glyph & theGlyph) const
  {
	static string dummy("glyph");
	string tmp = theGlyph.realize(*itsDictionary);
	itsParts.push_back(tmp);
	return dummy;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a sentence
   */
  // ----------------------------------------------------------------------
  
  string SoneraTextFormatter::visit(const Sentence & theSentence) const
  {
	static string dummy("sentence");
	sonera_realize(theSentence.begin(), theSentence.end(), *this, itsParts);
	return dummy;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a paragraph
   */
  // ----------------------------------------------------------------------
  
  string SoneraTextFormatter::visit(const Paragraph & theParagraph) const
  {
	static string dummy("paragraph");
	sonera_realize(theParagraph.begin(), theParagraph.end(), *this, itsParts);
	return dummy;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a header
   */
  // ----------------------------------------------------------------------
  
  string SoneraTextFormatter::visit(const Header & theHeader) const
  {
	static string dummy("header");
	sonera_realize(theHeader.begin(), theHeader.end(), *this, itsParts);
	return dummy;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Visit a document
   */
  // ----------------------------------------------------------------------
  
  string SoneraTextFormatter::visit(const Document & theDocument) const
  {
	static string dummy("document");
	sonera_realize(theDocument.begin(), theDocument.end(), *this, itsParts);
	return dummy;
  }

} // namespace TextGen
  
// ======================================================================
  
  
