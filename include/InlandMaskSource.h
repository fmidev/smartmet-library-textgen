// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::InlandMaskSource
 */
// ======================================================================

#ifndef WEATHERANALYSIS_INLANDMASKSOURCE_H
#define WEATHERANALYSIS_INLANDMASKSOURCE_H

#include "MaskSource.h"

namespace WeatherAnalysis
{
  class InlandMaskSource : public MaskSource
  {
  public:

	typedef MaskSource::mask_type mask_type;
	typedef MaskSource::masks_type masks_type;

	InlandMaskSource(const WeatherArea & theCoast);

	virtual mask_type mask(const WeatherArea & theArea,
						   const std::string & theData,
						   const WeatherSource & theWeatherSource) const;

	virtual masks_type masks(const WeatherArea & theArea,
							 const std::string & theData,
							 const WeatherSource & theWeatherSource) const;

  private:

	InlandMaskSource();

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

  }; // class InlandMaskSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_INLANDMASKSOURCE_H

// ======================================================================
