// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::WesternMaskSource
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WESTERNMASKSOURCE_H
#define WEATHERANALYSIS_WESTERNMASKSOURCE_H

#include "MaskSource.h"

namespace WeatherAnalysis
{
  class WesternMaskSource : public MaskSource
  {
  public:
	
	typedef MaskSource::mask_type mask_type;
	typedef MaskSource::masks_type masks_type;
	
	WesternMaskSource(const WeatherArea & theArea);

	virtual mask_type mask(const WeatherArea & theArea,
								 const std::string & theData,
								 const WeatherSource & theWeatherSource) const;

	virtual masks_type masks(const WeatherArea & theArea,
								   const std::string & theData,
								   const WeatherSource & theWeatherSource) const;

  private:

	WesternMaskSource();

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

  }; // class WesternMaskSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WESTERNMASKSOURCE_H

// ======================================================================
