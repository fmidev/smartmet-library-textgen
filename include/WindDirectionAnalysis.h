// ======================================================================
/*!
 * \file
 * \brief Interface of class WindDirectionAnalysis
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WindDirectionAnalysis
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
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class WindDirectionAnalysis
  {
  public:

	static WeatherResult forecast(const AnalysisSources & theSources,
								  const WeatherFunction & theFunction,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea);
  };
}

#endif // WEATHERANALYSIS_WINDDIRECTIONANALYSIS_H

// ======================================================================
