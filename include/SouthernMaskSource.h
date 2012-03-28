// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::SouthernMaskSource
 */
// ======================================================================

#ifndef WEATHERANALYSIS_SOUTHERNMASKSOURCE_H
#define WEATHERANALYSIS_SOUTHERNMASKSOURCE_H

#include "MaskSource.h"

namespace WeatherAnalysis
{
  class SouthernMaskSource : public MaskSource
  {
  public:
	
	typedef MaskSource::mask_type mask_type;
	typedef MaskSource::masks_type masks_type;
	
	SouthernMaskSource(const WeatherArea & theArea);

	virtual mask_type mask(const WeatherArea & theArea,
						   const std::string & theData,
						   const WeatherSource & theWeatherSource) const;

	virtual masks_type masks(const WeatherArea & theArea,
							 const std::string & theData,
							 const WeatherSource & theWeatherSource) const;

  private:

	SouthernMaskSource();

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

  }; // class SouthernMaskSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_SOUTHERNMASKSOURCE_H

// ======================================================================
