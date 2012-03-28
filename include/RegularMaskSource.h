// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::RegularMaskSource
 */
// ======================================================================

#ifndef WEATHERANALYSIS_REGULARMASKSOURCE_H
#define WEATHERANALYSIS_REGULARMASKSOURCE_H

#include "MaskSource.h"

namespace WeatherAnalysis
{
  class RegularMaskSource : public MaskSource
  {
  public:

	typedef MaskSource::mask_type mask_type;
	typedef MaskSource::masks_type masks_type;

	RegularMaskSource();

	virtual mask_type mask(const WeatherArea & theArea,
						   const std::string & theData,
						   const WeatherSource & theWeatherSource) const;

	virtual masks_type masks(const WeatherArea & theArea,
							 const std::string & theData,
							 const WeatherSource & theWeatherSource) const;

  private:

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

  }; // class RegularMaskSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_REGULARMASKSOURCE_H

// ======================================================================
