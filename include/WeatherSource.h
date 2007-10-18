// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class WeatherAnalysis::WeatherSource
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERSOURCE_H
#define WEATHERANALYSIS_WEATHERSOURCE_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <newbase/NFmiStreamQueryData.h>

class NFmiStreamQueryData;

namespace WeatherAnalysis
{
  typedef long WeatherId;

  class WeatherSource
  {
  public:

	virtual ~WeatherSource() { }
	virtual boost::shared_ptr<NFmiStreamQueryData> data(const std::string & theName) const = 0;
	virtual WeatherId id(const std::string & theName) const = 0;

  }; // class WeatherSource

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERSOURCE_H

// ======================================================================

