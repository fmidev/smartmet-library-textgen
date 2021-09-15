// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::PostgreSQLDictionary
 */
// ======================================================================
/*!
 * \class TextGen::PostgreSQLDictionary
 *
 */
// ----------------------------------------------------------------------

#include "PostgreSQLDictionary.h"
#include <calculator/Settings.h>
#include <calculator/TextGenError.h>
#include <macgyver/StringConversion.h>
#include <macgyver/PostgreSQLConnection.h>

#include <thread>

#include <map>

using namespace std;

namespace
{
inline double as_double(const pqxx::field& obj)
{
#if PQXX_VERSION_MAJOR > 5
  return obj.as<double>();
#else
  return Fmi::stod(obj.as<std::string>());
#endif
}

inline int as_int(const pqxx::field& obj)
{
#if PQXX_VERSION_MAJOR > 5
  return obj.as<int>();
#else
  return Fmi::stoi(obj.as<std::string>());
#endif
}
}

namespace TextGen
{


void PostgreSQLDictionary::getDataFromDB(const std::string& theLanguage,  std::map<std::string, std::string>& theDataStorage)
{
  try
  {
	// Establish the settings for TextGen

	Fmi::Database::PostgreSQLConnectionOptions connectionOptions;
    connectionOptions.host = Settings::require_string("textgen::host");
    connectionOptions.port = Settings::require_int("textgen::port");
    connectionOptions.database = Settings::require_string("textgen::database");
    connectionOptions.username = Settings::require_string("textgen::user");
    connectionOptions.password = Settings::require_string("textgen::passwd");
    connectionOptions.encoding = Settings::require_string("textgen::encoding");
    connectionOptions.connect_timeout = Settings::require_int("textgen::connect_timeout");

	Fmi::Database::PostgreSQLConnection dbConnection;
	try
	  {	
		dbConnection.open(connectionOptions);

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));		
	  }
	catch (const std::exception &e)
	  {
		throw TextGenError("PostgreSQL error: " + std::string(e.what()));
	  }
	catch (...)
	  {
		throw TextGenError("SmartMet::Textgen::PostgreSQLDictionary: Creating database connection failed: " + std::string(connectionOptions));
	  }
	
    // select the right translation table
    std::string sqlStmt = "select translationtable, active from textgen.languages";
    sqlStmt += " where isocode = '";
    sqlStmt += theLanguage;
    sqlStmt += "'";

    pqxx::result result_set = dbConnection.executeNonTransaction(sqlStmt);

	if (result_set.empty())
	  {
		throw TextGenError("Error: Error occurred while querying languages table:\n" + sqlStmt);
	  }

	if (result_set.size() != 1)
	  {
		throw TextGenError("Error: Obtained multiple matches for language " + theLanguage);
	  }

    std::string translationtable = result_set.at(0).at(0).as<std::string>();
    int active = as_int(result_set.at(0).at(1));

	if (active != 1) throw TextGenError("Error: Language " + theLanguage + " is not active");
    if (translationtable.empty())
      throw TextGenError("Error: Language " + theLanguage + " has no translationtable");

    sqlStmt = ("select keyword, translation from textgen." + translationtable);

    result_set = dbConnection.executeNonTransaction(sqlStmt);

    for (auto row : result_set)
    {
      std::string keyword = row.at(0).as<std::string>();
      std::string translation = row.at(1).as<std::string>();
      if (!keyword.empty()) theDataStorage.insert(std::make_pair(keyword, translation));
	}

	dbConnection.close();
  }
  catch (...)
  {
    // Handle any query errors
    throw TextGenError("Query error: ");
  }
}

}  // namespace TextGen

// ======================================================================
