// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::MySQLDictionary
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

#include "MySQLDictionary.h"
#include "Settings.h"
#include "TextGenError.h"

#include <map>
#include <mysql.h>

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Implementation hiding pimple
   *
   */
  // ----------------------------------------------------------------------

  class MySQLDictionary::Pimple
  {
  public:

	Pimple()
	  : itsInitialized(false)
	  , itsLanguage()
	  , itsData()
	{ }

	typedef std::map<std::string,std::string> StorageType;
	typedef StorageType::value_type value_type;

	bool itsInitialized;
	std::string itsLanguage;
	StorageType itsData;

  }; // class Pimple

  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   *
   * The destructor does nothing special.
   */
  // ----------------------------------------------------------------------

  MySQLDictionary::~MySQLDictionary()
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * The constructor does nothing special.
   */
  // ----------------------------------------------------------------------

  MySQLDictionary::MySQLDictionary()
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

  const std::string & MySQLDictionary::language(void) const
  {
	return itsPimple->itsLanguage;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Initialize with given language
   *
   * Initializing the MySQLDictionary involves connecting to the MySQL
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

  void MySQLDictionary::init(const std::string & theLanguage)
  {
	// clear possible earlier language
	itsPimple->itsLanguage = theLanguage;
	itsPimple->itsInitialized = false;
	itsPimple->itsData.clear();

	// Establish the settings for TextGen

	std::string host     = Settings::require_string("textgen::host");
	std::string user     = Settings::require_string("textgen::user");
	std::string passwd   = Settings::require_string("textgen::passwd");
	std::string database = Settings::require_string("textgen::database");

	// Establish the connection

	MYSQL mysql;
	mysql_init(&mysql);
	if(!mysql_real_connect(&mysql,
						   host.c_str(),
						   user.c_str(),
						   passwd.c_str(),
						   database.c_str(),
						   0,NULL,0))
	  throw TextGenError("Error: Failed to connect to "+database+" database");

	MYSQL_RES * result=0;
	MYSQL_ROW row;
	try
	  {
		std::string query = "select translationtable, active from languages";
		query += " where isocode = '";
		query += theLanguage;
		query += "'";

		mysql_query(&mysql,query.c_str());

		if(mysql_errno(&mysql))
			throw TextGenError(std::string("Error: Error occurred while querying languages table:\n") + query);

		result = mysql_store_result(&mysql);
		if(result==NULL)
		  throw TextGenError("Error: Error occurred while reading languages table");

		unsigned int num_rows = mysql_num_rows(result);
		if(num_rows == 0)
		  throw TextGenError("Error: Language " + theLanguage + " is not among the supported languages");

		if(num_rows != 1)
		  throw TextGenError("Error: Obtained multiple matches for language " + theLanguage);

		row = mysql_fetch_row(result);
		if(row == NULL)
		  throw TextGenError("Error: Error occurred while reading information on language " + theLanguage);

		unsigned long * lengths = mysql_fetch_lengths(result);
		std::string translationtable(row[0],lengths[0]);
		std::string active(row[1],lengths[1]);

		if(active != "1")
		  throw TextGenError("Error: Language "+theLanguage+" is not active");
		if(translationtable.empty())
		  throw TextGenError("Error: Language "+theLanguage+" has no translationtable");

		// Prepare for reading translationtable

		mysql_free_result(result);
		result = 0;

		query = "select keyword, translation from " + translationtable;

		mysql_query(&mysql,query.c_str());

		if(mysql_errno(&mysql))
		  throw TextGenError("Error: Error occurred while querying " + translationtable + " table");

		result = mysql_store_result(&mysql);
		if(result==NULL)
		  throw TextGenError("Error: Error occurred while reading " + translationtable + " table");

		while(row=mysql_fetch_row(result))
		  {
			unsigned long * lengths = mysql_fetch_lengths(result);
			std::string keyword(row[0],lengths[0]);
			std::string translation(row[1],lengths[1]);

			if(!keyword.empty())
			  itsPimple->itsData.insert(Pimple::value_type(keyword,translation));
		  }
	  }

	catch(...)
	  {
		if(result) mysql_free_result(result);
		mysql_close(&mysql);
		throw;
	  }

	if(result) mysql_free_result(result);
	mysql_close(&mysql);

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

  bool MySQLDictionary::contains(const std::string & theKey) const
  {
	return (itsPimple->itsData.find(theKey) != itsPimple->itsData.end());
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

  const std::string & MySQLDictionary::find(const std::string & theKey) const
  {
	if(!itsPimple->itsInitialized)
	  throw TextGenError("Error: MySQLDictionary::find() called before init()");
	Pimple::StorageType::const_iterator
	  it = itsPimple->itsData.find(theKey);

	if(it!=itsPimple->itsData.end())
	  return it->second;
	throw TextGenError("Error: MySQLDictionary::find("+theKey+") failed in language "+itsPimple->itsLanguage);
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

  void MySQLDictionary::insert(const std::string & theKey,
							   const std::string & thePhrase)
  {
	throw TextGenError("Error: MySQLDictionary::insert() is not allowed");
  }

  // ----------------------------------------------------------------------
  /*!
   * Return the size of the dictionary
   *
   * \return The size of the dictionary
   */
  // ----------------------------------------------------------------------

  MySQLDictionary::size_type MySQLDictionary::size(void) const
  {
	return itsPimple->itsData.size();
  }

  // ----------------------------------------------------------------------
  /*!
   * Test if the dictionary is empty
   *
   * \return True if the dictionary is empty
   */
  // ----------------------------------------------------------------------

  bool MySQLDictionary::empty(void) const
  {
	return itsPimple->itsData.empty();
  }

} // namespace TextGen

// ======================================================================
