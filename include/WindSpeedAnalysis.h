// ======================================================================
/*!
 * \file
 * \brief Interface of namespace WindSpeedAnalysis
 */
// ======================================================================
/*!
 * \namespace WeatherAnalysis::WindSpeedAnalysis
 *
 * \brief WindSpeed analysis functions
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WINDSPEEDANALYSIS_H
#define WEATHERANALYSIS_WINDSPEEDANALYSIS_H

#include "WeatherFunction.h"

class NFmiPoint;

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  namespace WindSpeedAnalysis
  {
	WeatherResult forecast(const AnalysisSources & theSources,
						   const WeatherFunction & theFunction,
						   const WeatherLimits & theLimits,
						   const WeatherPeriod & thePeriod,
						   const WeatherArea & theArea);
  }
}

#endif // WEATHERANALYSIS_WINDSPEEDANALYSIS_H

// ======================================================================
