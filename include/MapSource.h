// ======================================================================
/*!
 * \file
 * \brief Interface of class MapSource
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

#ifndef WEATHERANALYSIS_MAPSOURCE_H
#define WEATHERANALYSIS_MAPSOURCE_H

#include "boost/shared_ptr.hpp"
#include <string>

class NFmiSvgPath;

namespace WeatherAnalysis
{
  class MapSourcePimple;

  class MapSource
  {
  public:
	MapSource();
	const NFmiSvgPath & getMap(const std::string & theName) const;

  private:

	boost::shared_ptr<MapSourcePimple> itsPimple;

  }; // class Mapsource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MAPSOURCE_H

// ======================================================================
