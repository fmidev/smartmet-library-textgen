// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class WeatherAnalysis::WeatherSource
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERSOURCE_H
#define WEATHERANALYSIS_WEATHERSOURCE_H

#include <string>
#include "boost/shared_ptr.hpp"
#include "NFmiQueryData.h"

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

