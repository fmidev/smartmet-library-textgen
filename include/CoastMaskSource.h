// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::CoastMaskSource
 */
// ======================================================================

#ifndef WEATHERANALYSIS_COASTMASKSOURCE_H
#define WEATHERANALYSIS_COASTMASKSOURCE_H

#include "MaskSource.h"

namespace WeatherAnalysis
{
  class CoastMaskSource : public MaskSource
  {
  public:

	typedef MaskSource::mask_type mask_type;
	typedef MaskSource::masks_type masks_type;

	CoastMaskSource(const boost::shared_ptr<MapSource> theMapSource,
					const std::string & theCoastMapName,
					double theCoastDistance,
					const std::string & theLandMapName,
					double theExpansionDistance = 0.0);

	virtual mask_type mask(const WeatherArea & theArea,
						   const std::string & theData,
						   const WeatherSource & theWeatherSource) const;

	virtual masks_type masks(const WeatherArea & theArea,
							 const std::string & theData,
							 const WeatherSource & theWeatherSource) const;

  private:

	CoastMaskSource();

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

  }; // class CoastMaskSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_COASTMASKSOURCE_H

// ======================================================================
