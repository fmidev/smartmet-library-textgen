// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::RegularMaskSource
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::RegularMaskSource
 *
 * \brief Provides mask services to clients
 *
 * The RegularMaskSource class provides access to masks calculated from
 * named SVG paths which represent geographic areas.
 *
 * The constructor takes as an optional argument the distance in kilometers
 * by which each area may be expanded for a grid point to be considered
 * to be representative for the area. Normally we might use a distance
 * half the forecast grid spacing.
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

	RegularMaskSource(const boost::shared_ptr<MapSource> theMapSource,
					  double theExpansionDistance = 0.0);

	virtual mask_type mask(const WeatherArea & theArea,
						   const std::string & theData,
						   const WeatherSource & theWeatherSource) const;

	virtual masks_type masks(const WeatherArea & theArea,
							 const std::string & theData,
							 const WeatherSource & theWeatherSource) const;

  private:

	RegularMaskSource();

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

  }; // class RegularMaskSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_REGULARMASKSOURCE_H

// ======================================================================
