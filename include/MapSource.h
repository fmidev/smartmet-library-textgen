// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::MapSource
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
  class MapSource
  {
  public:
	MapSource();
	const NFmiSvgPath & getMap(const std::string & theName) const;

  private:

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

  }; // class Mapsource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MAPSOURCE_H

// ======================================================================
