// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::LandMaskSource
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::LandMaskSource
 *
 * \brief Provides mask services to clients (masked to remain on land)
 *
 * The LandMaskSource class provides access to masks calculated from
 * named SVG paths which represent geographic areas. This class is
 * differentiated from RegularMaskSource by the fact that any mask
 * is restricted from expanding into the sea. The acceptable land
 * area is specified by giving the name of the land area mask
 * \em and the distance in kilometers by which it may be expanded.
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_LANDMASKSOURCE_H
#define WEATHERANALYSIS_LANDMASKSOURCE_H

#include "boost/shared_ptr.hpp"

namespace WeatherAnalysis
{
  class LandMaskSource : public MaskSource
  {
  public:

	typedef MaskSource::mask_type mask_type;
	typedef MaskSource::masks_type masks_type;

	LandMaskSource(const boost::shared_ptr<MapSource> theMapSource,
				   const std::string & theLandMapName,
				   double theExpansionDistance = 0.0);

	virtual mask_type mask(const WeatherArea & theArea,
						   const std::string & theData,
						   const WeatherSource & theWeatherSource) const;

	virtual masks_type masks(const WeatherArea & theArea,
							 const std::string & theData,
							 const WeatherSource & theWeatherSource) const;

	virtual masks_type masks(const NFmiPoint & thePoint,
							 const std::string & theData,
							 const WeatherSource & theWeatherSource) const;

	// Not available in base class:

	void land(const std::string & theLandName, double theDistance) const;

  private:

	LandMaskSource();

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

  }; // class LandMaskSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_LANDMASKSOURCE_H

// ======================================================================
