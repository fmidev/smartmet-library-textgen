// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::NullMaskSource
 */
// ======================================================================

#ifndef WEATHERANALYSIS_NULLMASKSOURCE_H
#define WEATHERANALYSIS_NULLMASKSOURCE_H

#include "MaskSource.h"

namespace WeatherAnalysis
{
  class NullMaskSource : public MaskSource
  {
  public:

	typedef MaskSource::mask_type mask_type;
	typedef MaskSource::masks_type masks_type;

	NullMaskSource();

	virtual const mask_type mask(const WeatherArea & theArea,
								 const std::string & theData,
								 const WeatherSource & theWeatherSource) const;
	
	virtual const masks_type masks(const WeatherArea & theArea,
								   const std::string & theData,
								   const WeatherSource & theWeatherSource) const;

  private:

	boost::shared_ptr<NFmiIndexMask> itsData;

  }; // class NullMaskSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_NULLMASKSOURCE_H

// ======================================================================
