// ======================================================================
/*!
 * \file
 * \brief Interface of the TheDictionary class
 */
// ======================================================================
/*!
 * \class TextGen::TheDictionary
 *
 * \brief Provides global dictionary services
 *
 * The responsibility of the TheDictionary class is to provide global
 * access to dictionary services. In essence, TheDictionary is a singleton
 * as implied by the "The" prefix.
 *
 * TheDictionary has a state variable which is the actual dictionary
 * implementation. The state can be changed by using the dictionary
 * method. The default dictionary is NullDictionary, meaning the
 * singleton is unusable before the state has been changed.
 *
 * Sample usage:
 * \code
 * TheDictionary::instance().dictionary(DictionaryFactory::create("mysql"));
 * cout << TheDictionary::instance().find("keyword");
 * \endcode
 *
 */
// ----------------------------------------------------------------------

#ifndef TEXTGEN_THEDICTIONARY_H
#define TEXTGEN_THEDICTIONARY_H

#include "Dictionary.h"
#include <memory>
#include <string>

namespace TextGen
{
  class TheDictionary
  {
  public:

	typedef Dictionary::size_type size_type;

	static TheDictionary & instance();

	void dictionary(std::auto_ptr<Dictionary> theDict);

	void init(const std::string & theLanguage);
	bool contains(const std::string & theKey) const;
	const std::string & find(const std::string & theKey) const;
	void insert(const std::string & theKey, const std::string & thePhrase);

	size_type size(void) const;
	bool empty(void) const;

  private:

	~TheDictionary();
	TheDictionary();
	TheDictionary(const Dictionary & theDict);
	TheDictionary & operator=(const TheDictionary & theDict);

	std::auto_ptr<Dictionary> itsDictionary;

  }; // class Dictionary

} // namespace TextGen

#endif // TEXTGEN_DICTIONARY_H

// ======================================================================
