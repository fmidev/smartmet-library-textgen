// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::PhraseWord
 */
// ======================================================================

#include "PhraseWord.h"
#include "Dictionary.h"
#include "TheDictionary.h"

using namespace std;

namespace TextGen
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------

  PhraseWord::~PhraseWord()
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Clone
   *
   * \return The clone
   */
  // ----------------------------------------------------------------------
  
  std::auto_ptr<Phrase> PhraseWord::clone() const
  {
	Phrase * tmp = new PhraseWord(*this);
	return auto_ptr<Phrase>(tmp);
  }
 
  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theWord The word
   */
  // ----------------------------------------------------------------------

  PhraseWord::PhraseWord(const std::string & theWord)
	: itsWord(theWord)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Realize using global dictionary
   *
   * \return The realized string
   */
  // ----------------------------------------------------------------------

  std::string PhraseWord::realize() const
  {
	return realize(TheDictionary::instance());
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Realize using the given dictionary
   *
   * \param theDictionary The dictionary to realize with
   * \return The realized string
   */
  // ----------------------------------------------------------------------

  std::string PhraseWord::realize(const Dictionary & theDictionary) const
  {
	return theDictionary.find(itsWord);
  }

} // namespace TextGen

// ======================================================================
