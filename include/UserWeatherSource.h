// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::UserWeatherSource
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
	virtual boost::shared_ptr<NFmiStreamQueryData> data(const std::string & theName) const;
	virtual WeatherId id(const std::string & theName) const;

	void insert(const std::string & theName,
				boost::shared_ptr<NFmiStreamQueryData> theData) const;

  private:

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

  }; // class UserWeatherSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_USERWEATHERSOURCE_H

// ======================================================================

