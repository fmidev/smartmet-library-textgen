// ======================================================================
/*!
 * \file
 * \brief Interface of class PressureAnalysis
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::PressureAnalysis
 *
 * \brief Pressure analysis functions
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_PRESSUREANALYSIS_H
#define WEATHERANALYSIS_PRESSUREANALYSIS_H

#include "WeatherFunction.h"

class NFmiPoint;

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class PressureAnalysis
  {
  public:

	static WeatherResult forecast(const AnalysisSources & theSources,
								  const WeatherFunction & theFunction,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea);
  };
}

#endif // WEATHERANALYSIS_PRESSUREANALYSIS_H

// ======================================================================
