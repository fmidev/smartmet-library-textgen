// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::MapSource
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::MapSource
 *
 * \brief Provides map services to clients
 *
 * The MapSource class provides access to named SVG paths which
 * represent geographic areas.
 *
 */
// ======================================================================

#include "MapSource.h"
#include "Settings.h"
#include "WeatherAnalysisError.h"

#include "NFmiFileSystem.h"
#include "NFmiSvgPath.h"
#include "NFmiSvgTools.h"

#include <cassert>
#include <fstream>
#include <map>

using namespace std;

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Implementation hiding pimple for MapSource
   */
  // ----------------------------------------------------------------------

  class MapSource::Pimple
  {
  public:
	Pimple();
	typedef map<string,NFmiSvgPath> storage_type;
	storage_type itsData;
  };

  // ----------------------------------------------------------------------
  /*!
   * \brief The pimple constructor
   */
  // ----------------------------------------------------------------------

  MapSource::Pimple::Pimple()
	: itsData()
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  // ----------------------------------------------------------------------

  MapSource::MapSource()
	: itsPimple(new Pimple)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the given named map
   *
   * The name is the subpath to the actual map data, apart from the
   * SVG suffix. The initial path is determined by the variable
   * textgen::mappath in fmi.conf used by NFmiSettings
   *
   * This will throw if the given map does not exist.
   *
   * \param theName The name of the map
   * \return The SVG path for the map
   */
  // ----------------------------------------------------------------------
  
  const NFmiSvgPath & MapSource::getMap(const std::string & theName) const
  {
	// Safety check, constructor should always initialize properly
	assert(itsPimple.get() != 0);

	// See if the result is cached first
	Pimple::storage_type::const_iterator it = itsPimple->itsData.find(theName);

	if(it != itsPimple->itsData.end())
	  return it->second;

	// Must read from file

	const string default_filename = Settings::require_string("textgen::mappath")+"/"+theName+".svg";
	const string filename = Settings::optional_string("textgen::areas::map::"+theName,default_filename);

	if(!NFmiFileSystem::FileExists(filename))
	  throw WeatherAnalysisError("The map for area "+theName+" is missing: "+filename);
	
	ifstream in(filename.c_str(), ios::in);
	if(!in)
	  throw WeatherAnalysisError("Could not open "+filename+" for reading");
	NFmiSvgPath svg;
	in >> svg;
	if(svg.empty())
	  throw WeatherAnalysisError("Failed to read SVG data in "+filename);

	typedef Pimple::storage_type::value_type value_type;

	return itsPimple->itsData.insert(value_type(theName,svg)).first->second;

  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Find map containing the desired point
   *
   * \bug This should also find the searchpath for all SVG files, not just
   *       the cached maps.
   *
   * \param theLatLon The point to be searched
   * \return The name of the area containing the point, or empty string
   */
  // ----------------------------------------------------------------------
   
  const std::string MapSource::findMapName(const NFmiPoint & theLatLon) const
  {
	// Safety check, constructor should always initialize properly
	assert(itsPimple.get() != 0);
	
	// Traverse all cached maps, return first one containing given point

	Pimple::storage_type::const_iterator it = itsPimple->itsData.begin();
	Pimple::storage_type::const_iterator endIter = itsPimple->itsData.end();
	for( ; it != endIter; ++it)
	  if(NFmiSvgTools::IsInside(it->second,theLatLon))
		return it->first;
	
	return "";
  }


} // namespace WeatherAnalysis

// ======================================================================
