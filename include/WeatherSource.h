// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class WeatherSource
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WeatherSource
 *
 * \brief Weather data source
 *
 * The WeatherSource class manages shared instances of weather data.
 * This class is abstract so that we may define different kinds
 * of weather sources. For example, a server would use one that
 * would automatically check that any requested data is the latest
 * possible (LatestWeatherSource), while regression tests would use
 * one that allows one to specify the data in more detail.
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERSOURCE_H
#define WEATHERANALYSIS_WEATHERSOURCE_H

#include "boost/shared_ptr.hpp"

class NFmiQueryData;

namespace WeatherAnalysis
{
  typedef long WeatherId;

  class WeatherSource
  {
  public:

	virtual ~WeatherSource() { }
	virtual boost::shared_ptr<NFmiQueryData> data(const std::string & theName) const = 0;
	virtual WeatherId id(const std::string & theName) const = 0;

  }; // class WeatherSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERSOURCE_H

// ======================================================================

