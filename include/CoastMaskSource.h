// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::CoastMaskSource
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::CoastMaskSource
 *
 * \brief Provides mask services to clients (masked to coastal areas)
 *
 * The CoastMaskSource class provides access to masks calculated from
 * named SVG paths which represent geographic areas. This class is
 * differentiated from RegularMaskSource by the fact that any mask
 * is restricted to remain close to the coast. The acceptable coast
 * area is specified by giving the name of the coastal line
 * and the acceptable distance in kilometers from the coastal line.
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_COASTMASKSOURCE_H
#define WEATHERANALYSIS_COASTMASKSOURCE_H

#include "boost/shared_ptr.hpp"

namespace WeatherAnalysis
{
  class CoastMaskSource : public MaskSource
  {
  public:

	typedef MaskSource::mask_type mask_type;
	typedef MaskSource::masks_type masks_type;

	CoastMaskSource(const boost::shared_ptr<MapSource> theMapSource,
					const std::string & theCoastMapName,
					double theCoastDistance,
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

	void coast(const std::string & theCoastName, double theDistance) const;

  private:

	CoastMaskSource();

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

  }; // class CoastMaskSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_COASTMASKSOURCE_H

// ======================================================================
