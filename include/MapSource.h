// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::MapSource
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MAPSOURCE_H
#define WEATHERANALYSIS_MAPSOURCE_H

#include "boost/shared_ptr.hpp"
#include <string>

class NFmiSvgPath;
class NFmiPoint;

namespace WeatherAnalysis
{
  class MapSource
  {
  public:
	MapSource();
	const NFmiSvgPath & getMap(const std::string & theName) const;
	std::string findMapName(const NFmiPoint & theLatlon) const;

  private:

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

  }; // class Mapsource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MAPSOURCE_H

// ======================================================================
