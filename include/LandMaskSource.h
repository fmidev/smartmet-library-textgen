// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::LandMaskSource
 */
// ======================================================================

#ifndef WEATHERANALYSIS_LANDMASKSOURCE_H
#define WEATHERANALYSIS_LANDMASKSOURCE_H

#include "MaskSource.h"

namespace WeatherAnalysis
{
  class LandMaskSource : public MaskSource
  {
  public:

	typedef MaskSource::mask_type mask_type;
	typedef MaskSource::masks_type masks_type;

	LandMaskSource(const std::string & theLandMapName);

	virtual const mask_type mask(const WeatherArea & theArea,
								 const std::string & theData,
								 const WeatherSource & theWeatherSource) const;
	
	virtual const masks_type masks(const WeatherArea & theArea,
								   const std::string & theData,
								   const WeatherSource & theWeatherSource) const;

  private:

	LandMaskSource();

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

  }; // class LandMaskSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_LANDMASKSOURCE_H

// ======================================================================
