// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::LatestWeatherSource
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::LatestWeatherSource
 *
 * \brief Weather data source for the latest forecasts
 *
 * The LatestWeatherSource class manages shared instances of weather data.
 * The class makes sure each requested forecast is the latest possible.
 *
 */
// ======================================================================

#ifdef _MSC_VER
#pragma warning(disable : 4786) // poistaa n kpl VC++ kääntäjän varoitusta (liian pitkä nimi >255 merkkiä joka johtuu 'puretuista' STL-template nimistä)
#endif

#include "LatestWeatherSource.h"
#include "IdGenerator.h"
#include "Settings.h"
#include "WeatherAnalysisError.h"

#include "NFmiFileSystem.h"
#include "NFmiQueryData.h"

#include <cassert>
#include <map>

using namespace std;

namespace
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Cached query data container
   */
  // ----------------------------------------------------------------------

  struct WeatherDataStruct
  {
	time_t itsTime;
	string itsFilename;
	WeatherAnalysis::WeatherId itsId;
	boost::shared_ptr<NFmiQueryData> itsData;
  };

  // ----------------------------------------------------------------------
  /*!
   * \brief Complete the query data name into filename
   *
   * A valid name is for example "/data/pal/querydata/pal/skandinavia/pinta/"
   *
   * \param theName The name
   * \return The filename
   */
  // ----------------------------------------------------------------------

  string complete_filename(const string & theName)
  {
	using namespace WeatherAnalysis;

	if(theName.empty())
	  throw WeatherAnalysisError("Trying to search unnamed querydata");

	const string varname = "textgen::"+theName;
	const string queryname = Settings::optional_string(varname,theName);

	if(FileExists(queryname))
	  return queryname;

	if(!DirectoryExists(queryname))
	  throw WeatherAnalysisError("No directory named "+queryname+" containing querydata found");

	string newestfile = NewestFile(queryname);
	if(newestfile.empty())
	  throw WeatherAnalysisError("Directory "+queryname+" does not contain any querydata");

	string fullname = queryname;
	const char lastchar = fullname[fullname.size()-1];
	if(lastchar != '/' && lastchar != '\\')
	  fullname += '/';
	fullname += newestfile;

	return fullname;

  }

} // namespace anonymous

namespace WeatherAnalysis
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Implementation hiding details for LatestWeatherSource
   */
  // ----------------------------------------------------------------------

  class LatestWeatherSource::Pimple
  {
  public:
	typedef map<string,WeatherDataStruct> container_type;
	container_type itsData;

  }; // class LatestWeatherSource::Pimple

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * The constructor makes sure the internal shared data is initialized.
   */
  // ----------------------------------------------------------------------

  LatestWeatherSource::LatestWeatherSource()
	: itsPimple(new Pimple)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Get the desired data
   *
   * \param theName The name of the data
   * \return Shared pointer to the desired data
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<NFmiQueryData> LatestWeatherSource::data(const std::string & theName) const
  {
	// The constructor should guarantee valid pimple
	assert(itsPimple.get() != 0);

	// Associated filename
	const string filename = complete_filename(theName);
	const time_t modtime = FileModificationTime(filename);

	// See if we have a cached result
	typedef Pimple::container_type::iterator iterator;
	iterator it = itsPimple->itsData.find(theName);
	if(it != itsPimple->itsData.end())
	  {
		// See if the cached data is outdated
		if(it->second.itsFilename != filename ||  it->second.itsTime < modtime)
		  itsPimple->itsData.erase(it);
		else
		  return it->second.itsData;
	  }

	// Read the new data

	boost::shared_ptr<NFmiQueryData> qdata(new NFmiQueryData);
	ifstream in(filename.c_str(), ios::in|ios::binary);
	if(!in)
	  throw WeatherAnalysisError("Failed to open "+filename+" for reading");
	in >> *qdata;
	in.close();

	WeatherDataStruct newdata;
	newdata.itsId = IdGenerator::generate();
	newdata.itsTime = modtime;
	newdata.itsFilename = filename;
	newdata.itsData = qdata;

	typedef Pimple::container_type::value_type value_type;

	itsPimple->itsData.insert(value_type(theName,newdata));

	return qdata;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Get id of desired data
   *
   * \param theName The name of the data
   * \return The ID
   */
  // ----------------------------------------------------------------------

  WeatherId LatestWeatherSource::id(const std::string & theName) const
  {
	// See if we have a stored result
	typedef Pimple::container_type::const_iterator const_iterator;
	const_iterator it = itsPimple->itsData.find(theName);
	if(it == itsPimple->itsData.end())
	  throw WeatherAnalysisError("No data named "+theName+" stored in LatestWeatherSource");

	return it->second.itsId;
  }

} // namespace WeatherAnalysis

// ======================================================================
