// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::Sentence
 */
// ======================================================================

#include "Sentence.h"
#include "Dictionary.h"
#include "Number.h"
#include "Phrase.h"
#include "TextGenError.h"
#include <algorithm>

using namespace std;
using namespace boost;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Return a clone
   */
  // ----------------------------------------------------------------------

  shared_ptr<Glyph> Sentence::clone() const
  {
	shared_ptr<Glyph> ret(new Sentence(*this));
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the text for the sentence
   *
   * \param theDictionary The dictionary to be used
   * \return The text
   */
  // ----------------------------------------------------------------------

  std::string Sentence::realize(const Dictionary & theDictionary) const
  {
	throw TextGenError("Sentence::realize should not be called");
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the prefix for sentences
   *
   * The prefix for sentences is always the space character
   *
   * \return The space character
   */
  // ----------------------------------------------------------------------
  
  std::string Sentence::prefix() const
  {
	return " ";
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the suffix for sentences
   *
   * The suffix for sentences is always an empty string
   *
   * \return An empty string
   */
  // ----------------------------------------------------------------------
  
  std::string Sentence::suffix() const
  {
	return "";
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Add a sentence to the end of this sentence
   *
   * \param theSentence The sentence to be added
   * \result The sentence added to
   */
  // ----------------------------------------------------------------------

  Sentence & Sentence::operator<<(const Sentence & theSentence)
  {
	if(this != &theSentence)
	  copy(itsData.begin(), itsData.end(), back_inserter(itsData));
	else
	  {
		storage_type tmp(itsData);
		copy(tmp.begin(),tmp.end(), back_inserter(itsData));
	  }
	return *this;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Add a glyph to a sentence
   *
   * \param theGlyph The glyph to be added
   * \result The sentence added to
   */
  // ----------------------------------------------------------------------

  Sentence & Sentence::operator<<(const Glyph & theGlyph)
  {
	itsData.push_back(theGlyph.clone());
	return *this;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Add a phrase to a sentence with automatic conversion
   *
   * \param thePhrase The string initializer for the phrase
   * \return The sentence added to
   */
  // ----------------------------------------------------------------------

  Sentence & Sentence::operator<<(const string & thePhrase)
  {
	shared_ptr<Phrase> phrase(new Phrase(thePhrase));
	itsData.push_back(phrase);
	return *this;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Add a number to a sentence with automatic conversion
   *
   * \param thePhrase The integer initializer for the number
   * \return The sentence added to
   */
  // ----------------------------------------------------------------------

  Sentence & Sentence::operator<<(int theNumber)
  {
	shared_ptr<Number<int> > number(new Number<int>(theNumber));
	itsData.push_back(number);
	return *this;
  }

} // namespace TextGen

// ======================================================================
