// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::Paragraph
 */
// ======================================================================

#include "Paragraph.h"
#include "Dictionary.h"
#include "TextGenError.h"

using namespace std;
using namespace boost;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Return a clone
   */
  // ----------------------------------------------------------------------

  shared_ptr<Glyph> Paragraph::clone() const
  {
	shared_ptr<Glyph> ret(new Paragraph(*this));
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the text for the paragraph
   *
   * \param theDictionary The dictionary to be used
   * \return The text
   */
  // ----------------------------------------------------------------------

  std::string Paragraph::realize(const Dictionary & theDictionary) const
  {
	throw TextGenError("Paragraph::realize should not be called");
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the prefix for paragraphs
   *
   * The prefix for paragraphs is always an empty string
   *
   * \return An empty string
   */
  // ----------------------------------------------------------------------
  
  std::string Paragraph::prefix() const
  {
	return "";
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the suffix for paragraphs
   *
   * The suffix for paragraphs is always an empty string
   *
   * \return An empty string
   */
  // ----------------------------------------------------------------------
  
  std::string Paragraph::suffix() const
  {
	return "";
  }


  // ----------------------------------------------------------------------
  /*!
   * \brief Adding a glyph to a paragraph
   *
   * \param theGlyph The glyph to be added
   * \result The paragraph added to
   */
  // ----------------------------------------------------------------------

  Paragraph & Paragraph::operator<<(const Glyph & theGlyph)
  {
	itsData.push_back(theGlyph.clone());
	return *this;
  }

} // namespace TextGen

// ======================================================================
