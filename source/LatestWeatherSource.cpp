// ======================================================================
/*!
 * \file
 * \brief Implementation of class LatestWeatherSource
 */
// ======================================================================

#include "LatestWeatherSource.h"
#include "NFmiFileSystem.h"
#include "NFmiQueryData.h"
#include "NFmiSettings.h"
#include <cassert>
#include <map>
#include <stdexcept>

using namespace std;

namespace
{

  // ----------------------------------------------------------------------
  /*!
   * \brief ID number generator
   *
   * The assumption here is that we never really need so many IDs
   * that long would not be sufficient to guarantee uniqueness.
   *
   * \return The next free ID
   */
  // ----------------------------------------------------------------------

  long id_generator()
	{
	  static long id;
	  return ++id;
	}

  // ----------------------------------------------------------------------
  /*!
   * \brief Cached query data container
   */
  // ----------------------------------------------------------------------

  struct WeatherDataStruct
  {
	time_t itsTime;
	string itsFilename;
	long itsId;
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
	if(theName.empty())
	  throw runtime_error("Trying to search unnamed querydata");

	string queryname;

	const string varname = "textgen::"+theName;
	if(NFmiSettings::instance().isset(varname))
	  queryname = NFmiSettings::instance().value(varname);
	else
	  queryname = theName;

	if(queryname.empty())
	  throw runtime_error("The setting "+varname+" has no value");

	if(FileExists(queryname))
	  return queryname;

	if(!DirectoryExists(queryname))
	  throw runtime_error("No directory named "+queryname+" containing querydata found");

	string newestfile = NewestFile(queryname);
	if(newestfile.empty())
	  throw runtime_error("Directory "+queryname+" does not contain any querydata");

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

	boost::shared_ptr<NFmiQueryData> data(new NFmiQueryData);
	ifstream in(filename.c_str(), ios::in|ios::binary);
	if(!in)
	  throw runtime_error("Failed to open "+filename+" for reading");
	in >> *data;
	in.close();

	WeatherDataStruct newdata;
	newdata.itsId = id_generator();
	newdata.itsTime = modtime;
	newdata.itsFilename = filename;
	newdata.itsData = data;

	typedef Pimple::container_type::value_type value_type;

	itsPimple->itsData.insert(value_type(theName,newdata));

	return data;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Get id of desired data
   *
   * \param theName The name of the data
   * \return The ID
   */
  // ----------------------------------------------------------------------

  long LatestWeatherSource::id(const std::string & theName) const
  {
	// See if we have a stored result
	typedef Pimple::container_type::const_iterator const_iterator;
	const_iterator it = itsPimple->itsData.find(theName);
	if(it == itsPimple->itsData.end())
	  throw runtime_error("No data named "+theName+" stored in LatestWeatherSource");

	return it->second.itsId;
  }

} // namespace WeatherAnalysis

// ======================================================================