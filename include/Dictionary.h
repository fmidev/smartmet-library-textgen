// ======================================================================
/*!
 * \file
 * \brief Interface of the Dictionary class
 */
// ======================================================================
/*!
 * \class textgen::Dictionary
 *
 * \brief Provides dictionary services
 *
 * The responsibility of the Dictionary class is to provide natural
 * language text for the given keyword, and to accept inserting
 * new keyword-text pairs.
 *
 * Dictionary is an abstract interface. Implementations of the interface
 * are provided for example by the BasicDictionary class.
 *
 * Note that the design intentionally does not provide a read method,
 * those are to be implemented externally using the add method. Also,
 * no write method is provided, all dictionary management is to be
 * done externally using real GUI tools.
 *
 */
// ----------------------------------------------------------------------

#ifndef TEXTGEN_DICTIONARY_H
#define TEXTGEN_DICTIONARY_H

#include <map>
#include <string>

namespace textgen
{
  class Dictionary
  {
  public:

	// Compiler generated:
	//
	// Dictionary();
	// Dictionary(const Dictionary & theDict);
	// Dictionary & operator=(const Dictionary & theDict);

	virtual ~Dictionary() { }
	virtual void init(const std::string & theLanguage) = 0;
	virtual bool contains(const std::string & theKey) const = 0;
	virtual const std::string & find(const std::string & theKey) const = 0;
	virtual void insert(const std::string & theKey, const std::string & thePhrase) = 0;

  }; // class Dictionary

} // namespace textgen

#endif // TEXTGEN_DICTIONARY_H

// ======================================================================
