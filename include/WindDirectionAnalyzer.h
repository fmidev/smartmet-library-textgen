// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::WindDirectionAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WindDirectionAnalyzer
 *
 * \brief WindDirection analysis functions
 *
 */
// ======================================================================

#ifndef WEATHERANALYZER_WINDDIRECTIONANALYZER_H
#define WEATHERANALYZER_WINDDIRECTIONANALYZER_H

#include "ParameterAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalyzerSources;
  class WeatherArea;
  class WeatherPeriodGenerator;
  class WeatherResult;

  class WindDirectionAnalyzer : public ParameterAnalyzer
  {
  public:

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherDataType & theDataType,
								  const WeatherFunction & theAreaFunction,
								  const WeatherFunction & theTimeFunction,
								  const WeatherFunction & theSubTimeFunction,
								  const WeatherArea & theArea,
								  const WeatherPeriodGenerator & thePeriods,
								  const Acceptor & theAreaAcceptor,
								  const Acceptor & theTimeAcceptor,
								  const Acceptor & theTester = NullAcceptor()) const;

  };
}

#endif // WEATHERANALYSIS_WINDDIRECTIONANALYZER_H

// ======================================================================
