// ======================================================================
/*!
 * \file
 * \brief Interface of singleton LocationSource
 */
// ======================================================================

#ifndef WEATHERANALYSIS_LOCATIONSOURCE_H
#define WEATHERANALYSIS_LOCATIONSOURCE_H

#include "boost/shared_ptr.hpp"
#include <string>

class NFmiPoint;

namespace WeatherAnalysis
{
  class LocationSource
  {
  public:

	static LocationSource & instance();

	bool hasCoordinates(const std::string & theLocation) const;
	const NFmiPoint coordinates(const std::string & theLocation) const;

  private:

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

	// Singleton protection
	~LocationSource();
	LocationSource();
	LocationSource(const LocationSource & theSrc);
	LocationSource & operator=(const LocationSource & theSrc);

  }; // class LocationSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_LOCATIONSOURCE_H

// ======================================================================
