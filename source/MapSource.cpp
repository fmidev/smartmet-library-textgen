// ======================================================================
/*!
 * \file
 * \brief Implementation of class MapSource
 */
// ======================================================================

#include "MapSource.h"
#include "NFmiFileSystem.h"
#include "NFmiSettings.h"
#include "NFmiSvgPath.h"
#include <cassert>
#include <fstream>
#include <map>
#include <stdexcept>

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
	typedef map<string,NFmiSvgPath> storage_type;
	storage_type itsData;
  };

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
   * textgen::mappath in fmi.conf used by NFmiSettings.
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

	const string varname = "textgen::mappath";

	if(!NFmiSettings::instance().isset(varname))
	  throw runtime_error("textgen::mappath is not set in fmi.conf");

	const string path = NFmiSettings::instance().value(varname);

	const string filename = FileComplete(theName+".svg",path);

	if(!FileExists(filename))
	  throw runtime_error("Map "+theName+" has no respective SVG file");
	
	ifstream in(filename.c_str(), ios::in);
	if(!in)
	  throw runtime_error("Could not open "+filename+" for reading");
	NFmiSvgPath svg;
	in >> svg;
	if(svg.empty())
	  throw runtime_error("Failed to read SVG data in "+filename);

	typedef Pimple::storage_type::value_type value_type;

	return itsPimple->itsData.insert(value_type(theName,svg)).first->second;

  }

} // namespace WeatherAnalysis

// ======================================================================
