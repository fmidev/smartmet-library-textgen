// ======================================================================
/*!
 * \file
 * \brief Interface of the NullDictionary class
 */
// ======================================================================
/*!
 * \class textgen::NullDictionary
 *
 * \brief Provides a dummy dictionary service
 *
 * The responsibility of the NullDictionary class is to serve as
 * a default null dictionary so that conditional testing can be
 * avoided.
 *
 * The intended use is withing a singleton providing global access
 * to dictionary services. Since the singleton must be initialized
 * with some dictionary implementation, we must either have a boolean
 * indicating a successful init, or a null dictionary serving as
 * the default dictionary.
 *
 * NullDictionary will throw a TextGenError when accessing data.
 *
 * For example, a singleton providing global dictionary services
 * could have an attribute
 * \code
 *   std::auto_ptr<Dictionary>
 * \endcode
 * and could have a constructor and methods like
 * \code
 * GlobalDictionary::GlobalDictionary()
 * {
 *    itsDictionary.reset(new NullDictionary());
 * }
 *
 * GlobalDictionary::find(const std::string & theKey) const
 * {
 *   return itsDictionary->find(theKey);
 * }
 * \endcode
 * instead of
 * \code
 * GlobalDictionary::GlobalDictionary()
 *   : itsDictionary()
 * {
 * }
 *
 * GlobalDictionary::find(const std::string & theKey) const
 * {
 *   if(itsDictionary.get())
 *      return itsDictionary->find(theKey);
 *   else
 *      throw TextGenError("Dictionary not initialized");
 * }
 * \endcode
 * That is, we avoid having to test the validity of the pointer
 * whenever we call any method in the stored Dictionary pointer.
 */
// ----------------------------------------------------------------------

#ifndef TEXTGEN_NULLDICTIONARY_H
#define TEXTGEN_NULLDICTIONARY_H

#include "Dictionary.h"

#include <string>

namespace textgen
{
  class NullDictionary : public Dictionary
  {
  public:

	typedef Dictionary::size_type size_type;

	// Compiler generated:
	//
	// NullDictionary();
	// NullDictionary(const NullDictionary & theDict);
	// NullDictionary & operator=(const NullDictionary & theDict);

	virtual ~NullDictionary() { } 
	virtual void init(const std::string & theLanguage) { }
	virtual bool contains(const std::string & theKey) const { return false; }
	virtual const std::string & find(const std::string & theKey) const;
	virtual void insert(const std::string & theKey, const std::string & thePhrase);

	virtual size_type size(void) const { return 0; }
	virtual bool empty(void) const { return true; }

  }; // class NullDictionary

} // namespace textgen

#endif // TEXTGEN_NULLDICTIONARY_H

// ======================================================================
