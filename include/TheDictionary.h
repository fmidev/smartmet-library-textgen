// ======================================================================
/*!
 * \file
 * \brief Interface of singleton class TextGen::TheDictionary
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
#include "boost/shared_ptr.hpp"
#include <memory>
#include <string>

namespace TextGen
{
  class TheDictionary : public Dictionary
  {
  public:

	typedef Dictionary::size_type size_type;

	static TheDictionary & instance();

	void dictionary(boost::shared_ptr<Dictionary> theDict);

	virtual void init(const std::string & theLanguage);
	virtual const std::string & language(void) const;
	virtual bool contains(const std::string & theKey) const;
	virtual const std::string & find(const std::string & theKey) const;
	virtual void insert(const std::string & theKey, const std::string & thePhrase);

	virtual size_type size(void) const;
	virtual bool empty(void) const;

#ifdef _MSC_VER
	virtual ~TheDictionary(); // pakko oli siirt‰‰ public:iksi kun MSVC ei muuten k‰‰nn‰ (MSVC bugi?)
  private:
#else
  private:
	virtual ~TheDictionary();
#endif


	TheDictionary();
	TheDictionary(const Dictionary & theDict);
	TheDictionary & operator=(const TheDictionary & theDict);

	boost::shared_ptr<Dictionary> itsDictionary;

  }; // class Dictionary

} // namespace TextGen

#endif // TEXTGEN_DICTIONARY_H

// ======================================================================
