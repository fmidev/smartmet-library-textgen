// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::LatestWeatherSource
 */
// ======================================================================

#ifndef WEATHERANALYSIS_LATESTWEATHERSOURCE_H
#define WEATHERANALYSIS_LATESTWEATHERSOURCE_H

#include "WeatherSource.h"
#include <boost/shared_ptr.hpp>

namespace WeatherAnalysis
{
  class LatestWeatherSource : public WeatherSource
  {
  public:

	LatestWeatherSource();
	virtual boost::shared_ptr<NFmiStreamQueryData> data(const std::string & theName) const;
	virtual WeatherId id(const std::string & theName) const;

  private:

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

  }; // class LatestWeatherSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_LATESTWEATHERSOURCE_H

// ======================================================================

