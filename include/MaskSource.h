// ======================================================================
/*!
 * \file
 * \brief Interface of class MaskSource
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::MaskSource
 *
 * \brief Provides mask services to clients
 *
 * The MaskSource class provides access to masks calculated from
 * named SVG paths which represent geographic areas.
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MASKSOURCE_H
#define WEATHERANALYSIS_MASKSOURCE_H

#include "boost/shared_ptr.hpp"
#include <string>

class NFmiIndexMask;
class NFmiIndexMaskSource;
class NFmiPoint;

namespace WeatherAnalysis
{
  class MapSource;
  class WeatherSource;

  class MaskSource
  {
  public:

	typedef boost::shared_ptr<NFmiIndexMask> mask_type;
	typedef boost::shared_ptr<NFmiIndexMaskSource> masks_type;

	virtual ~MaskSource();

	virtual mask_type mask(const std::string & theArea,
						   const std::string & theData,
						   const WeatherSource & theWeatherSource) const = 0;

	virtual masks_type masks(const std::string & theArea,
							 const std::string & theData,
							 const WeatherSource & theWeatherSource) const = 0;

	virtual masks_type masks(const NFmiPoint & thePoint,
							 const std::string & theData,
							 const WeatherSource & theWeatherSource) const = 0;

  }; // class MaskSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MASKSOURCE_H

// ======================================================================
