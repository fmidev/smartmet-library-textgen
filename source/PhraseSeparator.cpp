// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::PhraseSeparator
 */
// ======================================================================

#include "PhraseSeparator.h"

using namespace std;

namespace TextGen
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------

  PhraseSeparator::~PhraseSeparator()
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Clone
   *
   * \return The clone
   */
  // ----------------------------------------------------------------------
  
  std::auto_ptr<Phrase> PhraseSeparator::clone() const
  {
	Phrase * tmp = new PhraseSeparator(*this);
	return auto_ptr<Phrase>(tmp);
  }
 
  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theSeparator The word
   */
  // ----------------------------------------------------------------------

  PhraseSeparator::PhraseSeparator(const std::string & theSeparator)
	: itsSeparator(theSeparator)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the word separator
   *
   * \return An empty string
   */
  // ----------------------------------------------------------------------

  std::string PhraseSeparator::wordseparator() const
  {
	return("");
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Realize using global dictionary
   *
   * Note that the dictionary is not needed in the implementation,
   * the returned string is always the one given in the constructor.
   *
   * \return The realized string
   */
  // ----------------------------------------------------------------------

  std::string PhraseSeparator::realize() const
  {
	return itsSeparator;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Realize using the given dictionary
   *
   * Note that the dictionary is not needed in the implementation,
   * the returned string is always the one given in the constructor.
   *
   * \param theDictionary The dictionary to realize with
   * \return The realized string
   */
  // ----------------------------------------------------------------------

  std::string PhraseSeparator::realize(const Dictionary & theDictionary) const
  {
	return itsSeparator;
  }

} // namespace TextGen

// ======================================================================
