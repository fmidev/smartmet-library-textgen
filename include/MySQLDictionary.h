// ======================================================================
/*!
 * \file
 * \brief Interface of the TextGen::MySQLDictionary class
 */
// ======================================================================
/*!
 * \class TextGen::MySQLDictionary
 *
 * \brief Provides dictionary services
 *
 * The responsibility of the MySQLDictionary class is to provide natural
 * language text for the given keyword. Inserting new keyword-text pairs.
 *
 * The dictionary has an initialization method, which fetches the specified
 * language from the MySQL server.
 *
 * Sample usage:
 * \code
 * using namespace TextGen;
 *
 * MySQLDictionary finnish;
 * finnish.init("fi");
 *
 * cout << finnish.find("good morning") << endl;
 * cout << finnish.find("good night") << endl;
 *
 * if(finnish.contains("good night"))
 *    cout << finnish.find("good night") << endl;
 * \endcode
 *
 * Note that find throws if the given keyword does not exist.
 *
 * The database address, table name, user name and password
 * are all specified externally in fmi.conf used by newbase
 * NFmiSettings class.
 *
 * The dictionary can be initialized multiple times. Each init
 * erases the language initialized earlier.
 */
// ----------------------------------------------------------------------

#ifndef TEXTGEN_MYSQLDICTIONARY_H
#define TEXTGEN_MYSQLDICTIONARY_H

#include "Dictionary.h"

#include <memory>
#include <string>

namespace TextGen
{
  class MySQLDictionary : public Dictionary
  {
  public:

	typedef Dictionary::size_type size_type;

	virtual ~MySQLDictionary();
	MySQLDictionary();
	MySQLDictionary(const MySQLDictionary & theDict);
	MySQLDictionary & operator=(const MySQLDictionary & theDict);

	virtual void init(const std::string & theLanguage);
	virtual const std::string & language(void) const;
	virtual bool contains(const std::string & theKey) const;
	virtual const std::string & find(const std::string & theKey) const;
	virtual void insert(const std::string & theKey, const std::string & thePhrase);

	virtual size_type size(void) const;
	virtual bool empty(void) const;

  private:

	class Pimple;
	std::auto_ptr<Pimple> itsPimple;

  }; // class BasicDictionary

} // namespace TextGen

#endif // TEXTGEN_BASICDICTIONARY_H

// ======================================================================
