// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class WeatherAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WeatherAnalyzer
 *
 * \brief Provides analysis services to clients
 *
 * The WeatherAnalyzer class provides an uniform interface for
 * weather analysis services. The only thing required by the
 * API is the ability to calculate a single result either for
 * an area or for a single point. All the remaining work is in
 * initializing the class implementing the abstract interface
 * with query data, map information and so on.
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERANALYZER_H
#define WEATHERANALYSIS_WEATHERANALYZER_H

#include "WeatherParameter.h"
#include "WeatherFunction.h"

class NFmiPoint;

namespace WeatherAnalysis
{
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class WeatherAnalyzer
  {
  public:

	virtual WeatherResult analyze(const WeatherParameter & theParameter,
								  const WeatherFunction & theFunction,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea) const = 0;

	virtual WeatherResult analyze(const WeatherParameter & theParameter,
								  const WeatherFunction & theFunction,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const NFmiPoint & thePoint) const = 0;
  }; // class WeatherAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERANALYZER_H

// ======================================================================
