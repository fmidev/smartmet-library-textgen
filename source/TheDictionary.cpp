// ======================================================================
/*!
 * \file
 * \brief Implementation of class TheDictionary
 */
// ======================================================================

#include "TheDictionary.h"
#include "TextGenError.h"

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
	if(!itsDictionary.get())
	  throw TextGenError("Cannot use TheDictionary::init() before ::dictionary()");
	itsDictionary->init(theLanguage);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the language
   *
   * This will throw if no dictionary has been set.
   *
   * \return The language
   */
  // ----------------------------------------------------------------------

  const std::string & TheDictionary::language(void) const
  {
	if(!itsDictionary.get())
	  throw TextGenError("Cannot use TheDictionary::language() before ::dictionary()");
	return itsDictionary->language();
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
	if(!itsDictionary.get())
	  throw TextGenError("Cannot use TheDictionary::contains() before ::dictionary()");
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
	if(!itsDictionary.get())
	  throw TextGenError("Cannot use TheDictionary::find() before ::dictionary()");
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
	if(!itsDictionary.get())
	  throw TextGenError("Cannot use TheDictionary::insert() before ::dictionary()");
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
	if(!itsDictionary.get())
	  throw TextGenError("Cannot use TheDictionary::size() before ::dictionary()");
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
	if(!itsDictionary.get())
	  throw TextGenError("Cannot use TheDictionary::empty() before ::dictionary()");
	return itsDictionary->empty();
  }

} // namespace TextGen

// ----------------------------------------------------------------------
