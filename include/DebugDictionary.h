// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::DebugDictionary
 */
// ======================================================================
/*!
 * \class TextGen::DebugDictionary
 *
 * \brief Provides a dummy dictionary service for debugging
 *
 * The debug dictionary returns all keys back phrases - no actual
 * translation is performed.
 *
 * The dictionary is mostly useful for debugging logs.
 */
// ----------------------------------------------------------------------

#ifndef TEXTGEN_DEBUGDICTIONARY_H
#define TEXTGEN_DEBUGDICTIONARY_H

#include "Dictionary.h"

#include <string>

namespace TextGen
{
  class DebugDictionary : public Dictionary
  {
  public:

	typedef Dictionary::size_type size_type;

	// Compiler generated:
	//
	// DebugDictionary();
	// DebugDictionary(const DebugDictionary & theDict);
	// DebugDictionary & operator=(const DebugDictionary & theDict);

	virtual ~DebugDictionary()
	{ } 

	virtual void init(const std::string & theLanguage)
	{ itsLanguage = theLanguage; }

	virtual const std::string & language(void) const
	{ return itsLanguage; }

	virtual bool contains(const std::string & theKey) const
	{ return true; }

	virtual const std::string & find(const std::string & theKey) const
	{ return theKey; }

	virtual void insert(const std::string & theKey, const std::string & thePhrase)
	{ }

	virtual size_type size(void) const
	{ return 0; }

	virtual bool empty(void) const
	{ return false; }

  private:

	std::string itsLanguage;

  }; // class DebugDictionary

} // namespace TextGen

#endif // TEXTGEN_DEBUGDICTIONARY_H

// ======================================================================
