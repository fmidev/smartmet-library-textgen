// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::DatabaseDictionaries
 */
// ======================================================================
/*!
 * \class TextGen::DatabaseDictionaries
 *
 * \brief Provides dictionary services
 *
 * The responsibility of the DatabaseDictionaries class is to provide natural
 * language text for the given keyword.
 *
 * The dictionary has an initialization method, which fetches the specified
 * language from the Database server, unless it has been fetched already.
 * In a sence this class is a version of DatabaseDictionary which remembers
 * all used languages. The language in use is changed by using the init command.
 *
 * Sample usage:
 * \code
 * using namespace TextGen;
 *
 * DatabaseDictionaries dict;
 * dict.init("fi");
 * dict.init("en");
 *
 * cout << dict.find("good morning") << '\n';
 * cout << dict.find("good night") << '\n';
 *
 * if(dict.contains("good night"))
 *    cout << dict.find("good night") << '\n';
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

#include "DatabaseDictionaries.h"
#ifdef UNIX
#include "MySQLDictionary.h"
#include "PostgreSQLDictionary.h"
#include <macgyver/Exception.h>
#include <map>
#include <memory>

using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Implementation hiding pimple
 *
 */
// ----------------------------------------------------------------------

class DatabaseDictionaries::Pimple
{
 public:
  using storage_type = map<string, std::shared_ptr<DatabaseDictionary>>;

  storage_type itsData;
  string itsLanguage;
  bool itsInitialized;

  storage_type::const_iterator itsCurrentDictionary;

};  // class Pimple

// ----------------------------------------------------------------------
/*!
 * \brief Destructor
 *
 * The destructor does nothing special.
 */
// ----------------------------------------------------------------------

DatabaseDictionaries::~DatabaseDictionaries() = default;
// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 *
 * The constructor does nothing special.
 */
// ----------------------------------------------------------------------

DatabaseDictionaries::DatabaseDictionaries(const std::string& theDictionaryId)
    : itsPimple(new Pimple())
{
  itsDictionaryId = theDictionaryId;
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the language
 *
 * An empty string is returned if no language has been initialized.
 *
 * \return The Language
 */
// ----------------------------------------------------------------------

const std::string& DatabaseDictionaries::language() const
{
  return itsPimple->itsLanguage;
}
// ----------------------------------------------------------------------
/*!
 * \brief Initialize with given language
 *
 * Initializing the DatabaseDictionaries involves connecting to the Database
 * database containing the dictionary for the given language, and
 * reading all words defined in the dictionary to the internal containers.
 *
 * If any errors occur during the initialization, a TextGenError is thrown.
 *
 * Any language initialized earlier will be remembered for later use.
 * Initializing the same language twice merely reactives the dictionary
 * initialized earlier.
 *
 * \param theLanguage The ISO-code of the language to initialize
 */
// ----------------------------------------------------------------------

void DatabaseDictionaries::init(const std::string& theLanguage)
{
  // Done if language is already active

  if (theLanguage == itsPimple->itsLanguage)
    return;

  itsPimple->itsLanguage = theLanguage;

  // Activate old language if possible

  itsPimple->itsCurrentDictionary = itsPimple->itsData.find(theLanguage);
  if (itsPimple->itsCurrentDictionary != itsPimple->itsData.end())
    return;

  // Load new language

  std::shared_ptr<DatabaseDictionary> dict;
  if (itsDictionaryId == "mysql")
    dict.reset(new MySQLDictionary);
  else if (itsDictionaryId == "postgresql")
    dict.reset(new PostgreSQLDictionary);
  if (dict.get() == nullptr)
    throw Fmi::Exception(BCP, "Failed to allocate a new DatabaseDictionary");

  dict->init(theLanguage);

  itsPimple->itsData.insert(Pimple::storage_type::value_type(theLanguage, dict));
  itsPimple->itsCurrentDictionary = itsPimple->itsData.find(theLanguage);

  itsPimple->itsInitialized = true;
}

// ----------------------------------------------------------------------
/*!
 * \brief Test if the given phrase is in the dictionary
 *
 * \param theKey The key of the phrase
 * \return True if the phrase is in the dictionary.
 */
// ----------------------------------------------------------------------

bool DatabaseDictionaries::contains(const std::string& theKey) const
{
  if (!itsPimple->itsInitialized)
    throw Fmi::Exception(BCP, "Error: DatabaseDictionaries::contains() called before init()");

  return (itsPimple->itsCurrentDictionary->second->contains(theKey));
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the phrase for the given key.
 *
 * This will throw a TextGenError if the phrase is not in
 * the dictionary, or if the dictionary has not been initialized.
 *
 * \param theKey The key of the phrase
 * \return The phrase
 */
// ----------------------------------------------------------------------

std::string DatabaseDictionaries::find(const std::string& theKey) const
{
  if (!itsPimple->itsInitialized)
    throw Fmi::Exception(BCP, "Error: DatabaseDictionaries::find() called before init()");

  return itsPimple->itsCurrentDictionary->second->find(theKey);
}

// ----------------------------------------------------------------------
/*!
 * \brief Inserting a new phrase into the dictionary is disabled
 *
 * We do not wish to manage the dictionaries via C++, it is far too
 * risky. Therefore this method will always throw.
 *
 * \param theKey The key of the phrase
 * \param thePhrase The phrase
 */
// ----------------------------------------------------------------------

void DatabaseDictionaries::insert(const std::string& /*theKey*/, const std::string& /*thePhrase*/)
{
  throw Fmi::Exception(BCP, "Error: DatabaseDictionaries::insert() is not allowed");
}

// ----------------------------------------------------------------------
/*!
 * Return the size of the dictionary
 *
 * \return The size of the dictionary
 */
// ----------------------------------------------------------------------

DatabaseDictionaries::size_type DatabaseDictionaries::size() const
{
  if (!itsPimple->itsInitialized)
    throw Fmi::Exception(BCP, "Error: DatabaseDictionaries::size() called before init()");
  return itsPimple->itsCurrentDictionary->second->size();
}

// ----------------------------------------------------------------------
/*!
 * Test if the dictionary is empty
 *
 * \return True if the dictionary is empty
 */
// ----------------------------------------------------------------------

bool DatabaseDictionaries::empty() const
{
  if (!itsPimple->itsInitialized)
    throw Fmi::Exception(BCP, "Error: DatabaseDictionaries::empty() called before init()");

  return itsPimple->itsCurrentDictionary->second->empty();
}

// ----------------------------------------------------------------------
/*!
 * Change dictonary language
 *
 */
// ----------------------------------------------------------------------

void DatabaseDictionaries::changeLanguage(const std::string& theLanguage)
{
  if (itsPimple->itsData.find(theLanguage) == itsPimple->itsData.end())
    throw Fmi::Exception(BCP, "Error: The requested language not supported: " + theLanguage);

  itsPimple->itsLanguage = theLanguage;
  itsPimple->itsCurrentDictionary = itsPimple->itsData.find(theLanguage);
}

}  // namespace TextGen

// ======================================================================
#endif  // UNIX
