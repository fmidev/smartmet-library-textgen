// ======================================================================
/*!
 * \file
 * \brief Interface of namespace PressureAnalysis
 */
// ======================================================================
/*!
 * \namespace WeatherAnalysis::PressureAnalysis
 *
 * \brief Pressure analysis functions
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_PRESSUREANALYSIS_H
#define WEATHERANALYSIS_PRESSUREANALYSIS_H

#include "WeatherFunction.h"

namespace WeatherAnalysis
{
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  namespace PressureAnalysis
  {
	WeatherResult forecast(const WeatherFunction & theFunction,
						   const WeatherLimits & theLimits,
						   const WeatherPeriod & thePeriod,
						   const WeatherArea & theArea);

	WeatherResult forecast(const WeatherFunction & theFunction,
						   const WeatherLimits & theLimits,
						   const WeatherPeriod & thePeriod,
						   const NFmiPoint & thePoint);
  }
}

#endif // WEATHERANALYSIS_PRESSUREANALYSIS_H

// ======================================================================
