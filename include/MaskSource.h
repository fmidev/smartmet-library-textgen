// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::MaskSource
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
  class WeatherArea;
  class WeatherSource;

  class MaskSource
  {
  public:

	typedef boost::shared_ptr<NFmiIndexMask> mask_type;
	typedef boost::shared_ptr<NFmiIndexMaskSource> masks_type;

	virtual ~MaskSource() {}

	virtual const mask_type mask(const WeatherArea & theArea,
								 const std::string & theData,
								 const WeatherSource & theWeatherSource) const = 0;
	
	virtual const masks_type masks(const WeatherArea & theArea,
								   const std::string & theData,
								   const WeatherSource & theWeatherSource) const = 0;

  }; // class MaskSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MASKSOURCE_H

// ======================================================================
