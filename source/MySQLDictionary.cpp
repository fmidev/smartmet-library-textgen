// ======================================================================
/*!
 * \file
 * \brief Implementation of class MySQLDictionary
 */
// ======================================================================

#include "MySQLDictionary.h"
#include "TextGenError.h"

#include <map>
#include <sqlplus.hh>

namespace textgen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Implementation hiding pimple
   *
   */
  // ----------------------------------------------------------------------

  class MySQLDictionaryPimple
  {
  public:

	MySQLDictionaryPimple()
	  : itsInitialized(false)
	  , itsLanguage()
	  , itsData()
	{ }

	typedef std::map<std::string,std::string> StorageType;
	typedef StorageType::value_type value_type;

	bool itsInitialized;
	std::string itsLanguage;
	StorageType itsData;

  }; // class MySQLDictionaryPimple

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
  {
	itsPimple.reset(new MySQLDictionaryPimple());
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Copy constructor
   *
   * \param theDict The dictionary to copy
   */
  // ----------------------------------------------------------------------

  MySQLDictionary::MySQLDictionary(const MySQLDictionary & theDict)
	: Dictionary()
  {
	itsPimple.reset(new MySQLDictionaryPimple(*theDict.itsPimple));
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Assignment operator
   *
   * \param theDict The dictionary to copy
   * \return The dictionary assigned to
   */
  // ----------------------------------------------------------------------

  MySQLDictionary & MySQLDictionary::operator=(const MySQLDictionary & theDict)
  {
	if(this != &theDict)
	  itsPimple.reset(new MySQLDictionaryPimple(*theDict.itsPimple));
	return *this;
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

	// Establish the settings for textgen

	std::string host = "mimir.weatherproof.fi";
	std::string user = "mheiskan";
	std::string passwd = "hp48sx";
	std::string database = "textgen";

	// Establish the connection

	Connection con(use_exceptions);
	if(!con.connect(database.c_str(), host.c_str(), user.c_str(), passwd.c_str()))
	  throw TextGenError("Error: Could not connect to language database");

	// Fetch the languages

	Query query = con.query();
	query << "select translationtable, active from languages"
		  << " where isocode = '" << theLanguage << "'";
	
	Result res = query.store();

	if(res.size()!=1)
	  throw TextGenError("Error: Language "+theLanguage+" is not in the languages database");
	
	Row row(*res.begin());

	if(!static_cast<int>(row["active"]))
	  throw TextGenError("Error: Language "+theLanguage+" is not active");

	std::string translationtable(row["translationtable"]);

	// Fetch the translations

	query = con.query();
	query << "select keyword,translation from " << translationtable;

	res = query.store();

	Result::const_iterator it;
	for(it=res.begin(); it!=res.end(); ++it)
	  {
		row = *it;
		const char * const keyword = row[0];
		const char * const translation = row[1];
		itsPimple->itsData.insert(MySQLDictionaryPimple::value_type(keyword,translation));
	  }

	itsPimple->itsInitialized = true;

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test if the given phrase is in the dictionary
   *
   * Throws TextGenError if the dictionary has not been initialized.
   *
   * \param theKey The key of the phrase
   * \return True if the phrase is in the dictionary.
   */
  // ----------------------------------------------------------------------

  bool MySQLDictionary::contains(const std::string & theKey) const
  {
	if(!itsPimple->itsInitialized)
	  throw TextGenError("Error: MySQLDictionary::contains() called before init()");
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
	MySQLDictionaryPimple::StorageType::const_iterator
	  it = itsPimple->itsData.find(theKey);

	if(it!=itsPimple->itsData.end())
	  return it->second;
	throw TextGenError("Error: MySQLDictionary::find("+theKey+") failed");
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

} // namespace textgen

// ======================================================================
