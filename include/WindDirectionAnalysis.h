// ======================================================================
/*!
 * \file
 * \brief Interface of namespace WindDirectionAnalysis
 */
// ======================================================================
/*!
 * \namespace WeatherAnalysis::WindDirectionAnalysis
 *
 * \brief WindDirection analysis functions
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WINDDIRECTIONANALYSIS_H
#define WEATHERANALYSIS_WINDDIRECTIONANALYSIS_H

#include "WeatherFunction.h"

namespace WeatherAnalysis
{
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  namespace WindDirectionAnalysis
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

#endif // WEATHERANALYSIS_WINDDIRECTIONANALYSIS_H

// ======================================================================
