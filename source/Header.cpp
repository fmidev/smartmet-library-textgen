// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::Header
 */
// ======================================================================

#include "Header.h"

#include "Dictionary.h"
#include "PlainTextFormatter.h"
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

  shared_ptr<Glyph> Header::clone() const
  {
	shared_ptr<Glyph> ret(new Header(*this));
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the text for the header
   *
   * \param theDictionary The dictionary to be used
   * \return The text
   */
  // ----------------------------------------------------------------------

  std::string Header::realize(const Dictionary & theDictionary) const
  {
	throw TextGenError("Header::realize(Dictionary) should not be called");
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Adding a glyph to a header
   *
   * \param theGlyph The glyph to be added
   * \result The header added to
   */
  // ----------------------------------------------------------------------

  Header & Header::operator<<(const Glyph & theGlyph)
  {
	itsData.push_back(theGlyph.clone());
	return *this;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the prefix for headers
   *
   * The prefix for headers is always an empty string
   *
   * \return An empty string
   */
  // ----------------------------------------------------------------------
  
  std::string Header::prefix() const
  {
	return "";
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the suffix for headers
   *
   * The suffix for headers is always an empty string
   *
   * \return An empty string
   */
  // ----------------------------------------------------------------------
  
  std::string Header::suffix() const
  {
	return "";
  }

} // namespace TextGen

// ======================================================================
