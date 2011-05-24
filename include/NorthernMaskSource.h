// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::NorthernMaskSource
 */
// ======================================================================

#ifndef WEATHERANALYSIS_NORTHERNMASKSOURCE_H
#define WEATHERANALYSIS_NORTHERNMASKSOURCE_H

#include "MaskSource.h"

namespace WeatherAnalysis
{
  class NorthernMaskSource : public MaskSource
  {
  public:
	
	typedef MaskSource::mask_type mask_type;
	typedef MaskSource::masks_type masks_type;
	
	NorthernMaskSource(const WeatherArea & theArea);

	virtual const mask_type mask(const WeatherArea & theArea,
								 const std::string & theData,
								 const WeatherSource & theWeatherSource) const;

	virtual const masks_type masks(const WeatherArea & theArea,
								   const std::string & theData,
								   const WeatherSource & theWeatherSource) const;

  private:

	NorthernMaskSource();

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

  }; // class NorthernMaskSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_NORTHERNMASKSOURCE_H

// ======================================================================
