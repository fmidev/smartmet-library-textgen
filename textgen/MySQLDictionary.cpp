// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::MySQLDictionary
 */
// ======================================================================
/*!
 * \class TextGen::MySQLDictionary
 *
 */
// ----------------------------------------------------------------------
#include "MySQLDictionary.h"
#include <calculator/Settings.h>
#include <calculator/TextGenError.h>

#include <boost/lexical_cast.hpp>

#include <mysql++/mysql++.h>

using namespace std;

namespace TextGen
{
void MySQLDictionary::getDataFromDB(const std::string& theLanguage,
                                    std::map<std::string, std::string>& theDataStorage)
{
  try
  {
    // Establish the settings for TextGen

    std::string host = Settings::require_string("textgen::host");
    std::string user = Settings::require_string("textgen::user");
    std::string passwd = Settings::require_string("textgen::passwd");
    std::string database = Settings::require_string("textgen::database");

    // Establish the connection
    mysqlpp::Connection mysql;

    if (!mysql.connect(database.c_str(), host.c_str(), user.c_str(), passwd.c_str()))
      throw runtime_error("Failed to connect to database" + database);

    mysqlpp::Query query = mysql.query();

    // set UTF8 charset
    query << "SET NAMES 'utf8'";
    query.exec();

    // reset the query
    query.reset();

    // select the right translation table
    std::string query_str = "select translationtable, active from languages";
    query_str += " where isocode = '";
    query_str += theLanguage;
    query_str += "'";

    query << query_str;

    mysqlpp::StoreQueryResult result1 = query.store();

    if (!result1)
      throw TextGenError(std::string("Error: Error occurred while querying languages table:\n") +
                         query.str());

    unsigned int num_rows = result1.num_rows();

    if (num_rows == 0)
      throw TextGenError("Error: Language " + theLanguage +
                         " is not among the supported languages");

    if (num_rows != 1)
      throw TextGenError("Error: Obtained multiple matches for language " + theLanguage);

    std::string translationtable = result1.at(0).at(0).c_str();
    std::string active = result1.at(0).at(1).c_str();

    if (active != "1")
      throw TextGenError("Error: Language " + theLanguage + " is not active");
    if (translationtable.empty())
      throw TextGenError("Error: Language " + theLanguage + " has no translationtable");

    query.reset();

    query << "select keyword, translation from " + translationtable;
    ;

    mysqlpp::StoreQueryResult result2 = query.store();

    if (!result2)
      throw TextGenError("Error: Error occurred while querying " + translationtable + " table");

    for (size_t i = 0; i < result2.num_rows(); ++i)
    {
      std::string keyword = result2.at(i).at(0).c_str();
      std::string translation = result2.at(i).at(1).c_str();

      if (!keyword.empty())
        theDataStorage.insert(std::make_pair(keyword, translation));
    }
  }
  catch (const mysqlpp::BadQuery& er)
  {
    // Handle any query errors
    throw TextGenError("Query error: " + string(er.what()));
  }
  catch (const mysqlpp::BadConversion& er)
  {
    // Handle bad conversions
    throw TextGenError("Conversion error: " + string(er.what()) +
                       ";\tretrieved data size: " + boost::lexical_cast<string>(er.retrieved) +
                       ", actual size: " + boost::lexical_cast<string>(er.actual_size));
  }
  catch (const mysqlpp::Exception& er)
  {
    // Catch-all for any other MySQL++ exceptions
    throw TextGenError("Error: " + string(er.what()));
  }
  catch (...)
  {
    throw;
  }
}

}  // namespace TextGen

// ======================================================================
