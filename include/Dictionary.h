// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class TextGen::Dictionary
 */
// ======================================================================

#ifndef TEXTGEN_DICTIONARY_H
#define TEXTGEN_DICTIONARY_H

#include <map>
#include <string>

namespace TextGen
{
  class Dictionary
  {
  public:

	typedef unsigned long size_type;

	// Compiler generated:
	//
	// Dictionary();
	// Dictionary(const Dictionary & theDict);
	// Dictionary & operator=(const Dictionary & theDict);

	virtual ~Dictionary() { }
	virtual void init(const std::string & theLanguage) = 0;
	virtual const std::string & language(void) const = 0;
	virtual bool contains(const std::string & theKey) const = 0;
	virtual const std::string & find(const std::string & theKey) const = 0;
	virtual void insert(const std::string & theKey, const std::string & thePhrase) = 0;

	virtual size_type size(void) const = 0;
	virtual bool empty(void) const = 0;

  }; // class Dictionary

} // namespace TextGen

#endif // TEXTGEN_DICTIONARY_H

// ======================================================================
