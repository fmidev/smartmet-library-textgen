// ======================================================================
/*!
 * \file
 * \brief Interface of class UserWeatherSource
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::UserWeatherSource
 *
 * \brief Weather data source
 *
 * In this version of the WeatherSource interface the user
 * is expected to manually insert all name-data pairs into
 * the class. This facilitates for example regression testing
 * with specific data.
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_USERWEATHERSOURCE_H
#define WEATHERANALYSIS_USERWEATHERSOURCE_H

#include "WeatherSource.h"
#include "boost/shared_ptr.hpp"

class NFmiQueryData;

namespace WeatherAnalysis
{
  class UserWeatherSource : public WeatherSource
  {
  public:

	UserWeatherSource();
	virtual boost::shared_ptr<NFmiQueryData> getData(const std::string & theName) const;

	void insert(const std::string & theName,
				boost::shared_ptr<NFmiQueryData> theData) const;

  private:

	class Pimple;
	static boost::shared_ptr<Pimple> itsPimple;

  }; // class UserWeatherSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_USERWEATHERSOURCE_H

// ======================================================================

