// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::EasternMaskSource
 */
// ======================================================================

#ifndef WEATHERANALYSIS_EASTERNMASKSOURCE_H
#define WEATHERANALYSIS_EASTERNMASKSOURCE_H

#include "MaskSource.h"

namespace WeatherAnalysis
{
  class EasternMaskSource : public MaskSource
  {
  public:
	
	typedef MaskSource::mask_type mask_type;
	typedef MaskSource::masks_type masks_type;
	
	EasternMaskSource(const WeatherArea & theArea);

	virtual const mask_type mask(const WeatherArea & theArea,
								 const std::string & theData,
								 const WeatherSource & theWeatherSource) const;

	virtual const masks_type masks(const WeatherArea & theArea,
								   const std::string & theData,
								   const WeatherSource & theWeatherSource) const;

  private:

	EasternMaskSource();

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

  }; // class EasternMaskSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_EASTERNMASKSOURCE_H

// ======================================================================