// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::MySQLDictionaries
 */
// ======================================================================
/*!
 * \class TextGen::MySQLDictionaries
 *
 * \brief Provides dictionary services
 *
 * The responsibility of the MySQLDictionaries class is to provide natural
 * language text for the given keyword.
 *
 * The dictionary has an initialization method, which fetches the specified
 * language from the MySQL server, unless it has been fetched already.
 * In a sence this class is a version of MySQLDictionary which remembers
 * all used languages. The language in use is changed by using the init command.
 *
 * Sample usage:
 * \code
 * using namespace TextGen;
 *
 * MySQLDictionaries dict;
 * dict.init("fi");
 * dict.init("en");
 *
 * cout << dict.find("good morning") << endl;
 * cout << dict.find("good night") << endl;
 *
 * if(dict.contains("good night"))
 *    cout << dict.find("good night") << endl;
 * \endcode
 *
 * Note that find throws if the given keyword does not exist.
 *
 * The database address, table name, user name and password
 * are all specified externally in fmi.conf used by newbase
 * NFmiSettings class.
 *
 * The dictionary can be initialized multiple times. All
 * initializations are effectively cached for best possible
 * speed.
 */
// ----------------------------------------------------------------------

#ifndef TEXTGEN_MYSQLDICTIONARIES_H
#define TEXTGEN_MYSQLDICTIONARIES_H

#include "Dictionary.h"

#include "boost/shared_ptr.hpp"
#include <string>

namespace TextGen
{
  class MySQLDictionaries : public Dictionary
  {
  public:

	typedef Dictionary::size_type size_type;

	virtual ~MySQLDictionaries();
	MySQLDictionaries();
#ifdef NO_COMPILER_OPTIMIZE
	MySQLDictionaries(const MySQLDictionaries & theDict);
	MySQLDictionaries & operator=(const MySQLDictionaries & theDict);
#endif

	virtual void init(const std::string & theLanguage);
	virtual const std::string & language(void) const;
	virtual bool contains(const std::string & theKey) const;
	virtual const std::string & find(const std::string & theKey) const;
	virtual void insert(const std::string & theKey, const std::string & thePhrase);

	virtual size_type size(void) const;
	virtual bool empty(void) const;

  private:

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

  }; // class MySQLDictionaries

} // namespace TextGen

#endif // TEXTGEN_MYSQLDICTIONARIES_H

// ======================================================================
