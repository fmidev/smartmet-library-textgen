// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::LatestWeatherSource
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::LatestWeatherSource
 *
 * \brief Weather data source for the latest forecasts
 *
 * The LatestWeatherSource class manages shared instances of weather data.
 * The class makes sure each requested forecast is the latest possible.
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_LATESTWEATHERSOURCE_H
#define WEATHERANALYSIS_LATESTWEATHERSOURCE_H

#include "WeatherSource.h"
#include "boost/shared_ptr.hpp"

namespace WeatherAnalysis
{
  class LatestWeatherSource : public WeatherSource
  {
  public:

	LatestWeatherSource();
	virtual boost::shared_ptr<NFmiQueryData> data(const std::string & theName) const;
	virtual WeatherId id(const std::string & theName) const;

  private:

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

  }; // class LatestWeatherSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_LATESTWEATHERSOURCE_H

// ======================================================================

