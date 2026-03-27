// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::DatabaseDictionary
 */
// ======================================================================
/*!
 * \class TextGen::DatabaseDictionary
 *
 * \brief Provides dictionary services
 *
 * The responsibility of the DatabaseDictionary class is to provide natural
 * language text for the given keyword. Inserting new keyword-text pairs.
 *
 * The dictionary has an initialization method, which fetches the specified
 * language from the database server.
 *
 * Sample usage:
 * \code
 * using namespace TextGen;
 *
 * DatabaseDictionary finnish;
 * finnish.init("fi");
 *
 * cout << finnish.find("good morning") << '\n';
 * cout << finnish.find("good night") << '\n';
 *
 * if(finnish.contains("good night"))
 *    cout << finnish.find("good night") << '\n';
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
#include "DatabaseDictionary.h"
#include <macgyver/Exception.h>

#include <map>

using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Implementation hiding pimple
 *
 */
// ----------------------------------------------------------------------

class DatabaseDictionary::Pimple
{
 public:
  Pimple() = default;
  using StorageType = std::map<std::string, std::string>;
  using value_type = StorageType::value_type;

  bool itsInitialized{false};
  std::string itsLanguage;
  StorageType itsData;

};  // class Pimple

// ----------------------------------------------------------------------
/*!
 * \brief Destructor
 *
 * The destructor does nothing special.
 */
// ----------------------------------------------------------------------

DatabaseDictionary::~DatabaseDictionary() = default;
// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 *
 * The constructor does nothing special.
 */
// ----------------------------------------------------------------------

DatabaseDictionary::DatabaseDictionary() : itsPimple(new Pimple())
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
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

const std::string& DatabaseDictionary::language() const
{
  try
  {
    return itsPimple->itsLanguage;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Initialize with given language
 *
 * Initializing the DatabaseDictionary involves connecting to the MySQL
 * database containing the dictionary for the given language, and
 * reading all words defined in the dictionary to the internal containers.
 *
 * If any errors occur during the initialization, a TextGenError is thrown.
 *
 * Any language initialized earlier will be erased. Initializing the
 * same language twice is essentially a reload from the MySQL database.
 *
 * \param theLanguage The ISO-code of the language to initialize
 */
// ----------------------------------------------------------------------

void DatabaseDictionary::init(const std::string& theLanguage)
{
  try
  {
    // clear possible earlier language
    itsPimple->itsLanguage = theLanguage;
    itsPimple->itsInitialized = false;
    itsPimple->itsData.clear();

    getDataFromDB(theLanguage, itsPimple->itsData);

    itsPimple->itsInitialized = true;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed").addParameter("database", itsDictionaryId);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Test if the given phrase is in the dictionary
 *
 * \param theKey The key of the phrase
 * \return True if the phrase is in the dictionary.
 */
// ----------------------------------------------------------------------

bool DatabaseDictionary::contains(const std::string& theKey) const
{
  try
  {
    return (itsPimple->itsData.find(theKey) != itsPimple->itsData.end());
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed").addParameter("key", theKey);
  }
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

std::string DatabaseDictionary::find(const std::string& theKey) const
{
  try
  {
    if (!itsPimple->itsInitialized)
      throw Fmi::Exception(BCP, "Error: DatabaseDictionary::find() called before init()");

    auto it = itsPimple->itsData.find(theKey);

    if (it != itsPimple->itsData.end())
      return it->second;
    throw Fmi::Exception(BCP,
                         "Error: DatabaseDictionary::find(" + theKey + ") failed in language " +
                             itsPimple->itsLanguage);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed").addParameter("key", theKey);
  }
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

void DatabaseDictionary::insert(const std::string& /*theKey*/, const std::string& /*thePhrase*/)
{
  try
  {
    throw Fmi::Exception(BCP, "Error: DatabaseDictionary::insert() is not allowed");
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * Return the size of the dictionary
 *
 * \return The size of the dictionary
 */
// ----------------------------------------------------------------------

DatabaseDictionary::size_type DatabaseDictionary::size() const
{
  try
  {
    return itsPimple->itsData.size();
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * Test if the dictionary is empty
 *
 * \return True if the dictionary is empty
 */
// ----------------------------------------------------------------------

bool DatabaseDictionary::empty() const
{
  try
  {
    return itsPimple->itsData.empty();
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * Change dictonary language
 *
 */
// ----------------------------------------------------------------------

void DatabaseDictionary::changeLanguage(const std::string& theLanguage)
{
  try
  {
    init(theLanguage);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed").addParameter("language", theLanguage);
  }
}

}  // namespace TextGen

// ======================================================================
