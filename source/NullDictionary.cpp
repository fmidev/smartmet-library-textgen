// ======================================================================
/*!
 * \file
 * \brief Implementation of class NullDictionary
 */
// ======================================================================

#include "NullDictionary.h"
#include "TextGenError.h"

namespace textgen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the phrase for the given key.
   *
   * This will always throw a TextGenError.
   *
   * \param theKey The key of the phrase
   * \return Nothing, always throws
   */
  // ----------------------------------------------------------------------

  const std::string & NullDictionary::find(const std::string & theKey) const
  {
	throw TextGenError("NullDictionary find is disabled");
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Insert a new phrase into the dictionary
   *
   * Always throws.
   *
   * \param theKey The key of the phrase
   * \param thePhrase The phrase
   */
  // ----------------------------------------------------------------------

  void NullDictionary::insert(const std::string & theKey,
						  const std::string & thePhrase)
  {
	throw TextGenError("NullDictionary insert is disabled");
  }

} // namespace textgen

// ======================================================================
