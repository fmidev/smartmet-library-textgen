// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherSource
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WeatherSource
 *
 * \brief Weather data source
 *
 * The WeatherSource class manages shared instances of weather data.
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERSOURCE_H
#define WEATHERANALYSIS_WEATHERSOURCE_H

#include "boost/shared_ptr.hpp"

class NFmiQueryData;

namespace WeatherAnalysis
{
  class WeatherSource
  {
  public:

	WeatherSource();
	boost::shared_ptr<NFmiQueryData> getData(const std::string & theName) const;
  private:

	class Pimple;
	static boost::shared_ptr<Pimple> itsPimple;

  }; // class WeatherSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERSOURCE_H

// ======================================================================

