// ======================================================================
/*!
 * \file
 * \brief Implementation of class BasicDictionary
 */
// ======================================================================

#include "BasicDictionary.h"
#include "TextGenError.h"

namespace textgen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test if the given phrase is in the dictionary
   *
   * \param theKey The key of the phrase
   * \return True if the phrase is in the dictionary.
   */
  // ----------------------------------------------------------------------

  bool BasicDictionary::contains(const std::string & theKey) const
  {
	return (itsData.find(theKey) != itsData.end());
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the phrase for the given key.
   *
   * This will throw a TextGenError if the phrase is not in
   * the dictionary.
   *
   * \param theKey The key of the phrase
   * \return The phrase
   */
  // ----------------------------------------------------------------------

  const std::string & BasicDictionary::find(const std::string & theKey) const
  {
	StorageType::const_iterator it = itsData.find(theKey);
	if(it!=itsData.end())
	  return it->second;
	throw TextGenError("Dictionary does not contain phrase '"+theKey+"'");
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Insert a new phrase into the dictionary
   *
   * Note that nothing will occur if the phrase key already exists in
   * the dictionary. The user is expected to use contains before
   * insert if such checking is desired.
   *
   * \param theKey The key of the phrase
   * \param thePhrase The phrase
   */
  // ----------------------------------------------------------------------

  void BasicDictionary::insert(const std::string & theKey,
						  const std::string & thePhrase)
  {
	itsData.insert(StorageType::value_type(theKey,thePhrase));
  }


} // namespace textgen

// ======================================================================
