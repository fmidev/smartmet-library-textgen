// ======================================================================
/*!
 * \file
 * \brief Implementation of class TheDictionary
 */
// ======================================================================

#include "TheDictionary.h"

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief The void constructor is used privately by instance()
   */
  // ----------------------------------------------------------------------

  TheDictionary::TheDictionary()
	: itsDictionary()
  { }

  // ----------------------------------------------------------------------
  /*!
   * \brief The destructor
   */
  // ----------------------------------------------------------------------

  TheDictionary::~TheDictionary()
  { }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return an instance of TheDictionary
   *
   * The instance method is the de factor standard means of getting
   * access to a singleton.
   *
   * \return A reference to the singleton
   */
  // ----------------------------------------------------------------------

  TheDictionary & TheDictionary::instance()
  {
	static TheDictionary theDict;
	return theDict;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set the dictionary state
   *
   * Setting the dictionary is effectively setting the dictionary type to
   * be used. Normally one would use this in association with
   * DictionaryFactory::create.
   *
   * \param theDict The dictionary to take into use
   */
  // ----------------------------------------------------------------------

  void TheDictionary::dictionary(std::auto_ptr<Dictionary> theDict)
  {
	itsDictionary = theDict;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Initialize with given language
   *
   * \param theLanguage The ISO-code of the language
   */
  // ----------------------------------------------------------------------

  void TheDictionary::init(const std::string & theLanguage)
  {
	itsDictionary->init(theLanguage);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test if the given phrase is in the dictionary
   *
   * \param theKey The key of the phrase
   * \return True if the phrase is in the dictionary.
   */
  // ----------------------------------------------------------------------

  bool TheDictionary::contains(const std::string & theKey) const
  {
	return itsDictionary->contains(theKey);
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

  const std::string & TheDictionary::find(const std::string & theKey) const
  {
	return itsDictionary->find(theKey);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Insert a new phrase into the dictionary
   *
   * \param theKey The key of the phrase
   * \param thePhrase The phrase
   */
  // ----------------------------------------------------------------------

  void TheDictionary::insert(const std::string & theKey,
						  const std::string & thePhrase)
  {
	itsDictionary->insert(theKey,thePhrase);
  }

  // ----------------------------------------------------------------------
  /*!
   * Return the size of the dictionary
   *
   * \return The size of the dictionary
   */
  // ----------------------------------------------------------------------

  TheDictionary::size_type TheDictionary::size(void) const
  {
	return itsDictionary->size();
  }

  // ----------------------------------------------------------------------
  /*!
   * Test if the dictionary is empty
   *
   * \return True if the dictionary is empty
   */
  // ----------------------------------------------------------------------

  bool TheDictionary::empty(void) const
  {
	return itsDictionary->empty();
  }

} // namespace TextGen

// ----------------------------------------------------------------------
