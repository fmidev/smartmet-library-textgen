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
 * weather analysis services. The only method required by the
 * API is the ability to calculate a single result. All the
 * remaining work is in initializing the class implementing
 * the abstract interface with query data, map information
 * and so on.
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERANALYZER_H
#define WEATHERANALYSIS_WEATHERANALYZER_H

#include "WeatherParameter.h"
#include "WeatherFunction.h"

namespace WeatherAnalysis
{
  class WeatherResult;
  class WeatherPeriod;
  class WeatherArea;

  class WeatherAnalyzer
  {
  public:
	virtual WeatherResult analyze(const WeatherParameter & theParameter,
								  const WeatherFunction & theFunction,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea) const = 0;
  }; // class WeatherAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERANALYZER_H

// ======================================================================
