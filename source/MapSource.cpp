// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::MapSource
 */
// ======================================================================

#include "MapSource.h"
#include "Settings.h"
#include "WeatherAnalysisError.h"

#include "newbase/NFmiFileSystem.h"
#include "newbase/NFmiSvgPath.h"

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
	string itsSearchPath;
  };

  // ----------------------------------------------------------------------
  /*!
   * \brief The pimple constructor
   */
  // ----------------------------------------------------------------------

  MapSource::Pimple::Pimple()
	: itsData()
	, itsSearchPath(".")
  {
	if(Settings::isset("textgen::mappath"))
	  {
		itsSearchPath += ':';
		itsSearchPath += Settings::require_string("textgen::mappath");
	  }
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

	const string filename = FileComplete(theName+".svg",itsPimple->itsSearchPath);

	if(!FileExists(filename))
	  throw WeatherAnalysisError("Map "+theName+" has no respective SVG file");
	
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

} // namespace WeatherAnalysis

// ======================================================================
