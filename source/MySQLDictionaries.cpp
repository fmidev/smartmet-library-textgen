// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::MySQLDictionaries
 */
// ======================================================================

#include "MySQLDictionaries.h"
#include "MySQLDictionary.h"
#include "TextGenError.h"
#include "boost/shared_ptr.hpp"
#include <map>

using namespace std;
using namespace boost;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Implementation hiding pimple
   *
   */
  // ----------------------------------------------------------------------

  class MySQLDictionaries::Pimple
  {
  public:

	typedef map<string,shared_ptr<MySQLDictionary> > storage_type;

	storage_type itsData;
	string itsLanguage;
	bool itsInitialized;

	storage_type::const_iterator itsCurrentDictionary;

  }; // class Pimple

  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   *
   * The destructor does nothing special.
   */
  // ----------------------------------------------------------------------

  MySQLDictionaries::~MySQLDictionaries()
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * The constructor does nothing special.
   */
  // ----------------------------------------------------------------------

  MySQLDictionaries::MySQLDictionaries()
	: Dictionary()
	, itsPimple(new Pimple())
  {
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

  const std::string & MySQLDictionaries::language(void) const
  {
	return itsPimple->itsLanguage;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Initialize with given language
   *
   * Initializing the MySQLDictionaries involves connecting to the MySQL
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

  void MySQLDictionaries::init(const std::string & theLanguage)
  {
	// Done if language is already active

	if(theLanguage == itsPimple->itsLanguage)
	  return;

	itsPimple->itsLanguage = theLanguage;

	// Activate old language if possible

	itsPimple->itsCurrentDictionary = itsPimple->itsData.find(theLanguage);
	if(itsPimple->itsCurrentDictionary != itsPimple->itsData.end())
	  return;

	// Load new language

	shared_ptr<MySQLDictionary> dict(new MySQLDictionary);
	dict->init(theLanguage);

	itsPimple->itsData.insert(Pimple::storage_type::value_type(theLanguage,dict));
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

  bool MySQLDictionaries::contains(const std::string & theKey) const
  {
	if(!itsPimple->itsInitialized)
	  throw TextGenError("Error: MySQLDictionaries::contains() called before init()");

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

  const std::string & MySQLDictionaries::find(const std::string & theKey) const
  {
	if(!itsPimple->itsInitialized)
	  throw TextGenError("Error: MySQLDictionaries::find() called before init()");

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

  void MySQLDictionaries::insert(const std::string & theKey,
							   const std::string & thePhrase)
  {
	throw TextGenError("Error: MySQLDictionaries::insert() is not allowed");
  }

  // ----------------------------------------------------------------------
  /*!
   * Return the size of the dictionary
   *
   * \return The size of the dictionary
   */
  // ----------------------------------------------------------------------

  MySQLDictionaries::size_type MySQLDictionaries::size(void) const
  {
	if(!itsPimple->itsInitialized)
	  throw TextGenError("Error: MySQLDictionaries::size() called before init()");
	return itsPimple->itsCurrentDictionary->second->size();
  }

  // ----------------------------------------------------------------------
  /*!
   * Test if the dictionary is empty
   *
   * \return True if the dictionary is empty
   */
  // ----------------------------------------------------------------------

  bool MySQLDictionaries::empty(void) const
  {
	if(!itsPimple->itsInitialized)
	  throw TextGenError("Error: MySQLDictionaries::empty() called before init()");
	
	return itsPimple->itsCurrentDictionary->second->empty();
  }

} // namespace TextGen

// ======================================================================
