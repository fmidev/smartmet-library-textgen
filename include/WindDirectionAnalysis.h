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

class NFmiPoint;

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  namespace WindDirectionAnalysis
  {
	WeatherResult forecast(const AnalysisSources & theSources,
						   const WeatherFunction & theFunction,
						   const WeatherLimits & theLimits,
						   const WeatherPeriod & thePeriod,
						   const WeatherArea & theArea);
  }
}

#endif // WEATHERANALYSIS_WINDDIRECTIONANALYSIS_H

// ======================================================================
