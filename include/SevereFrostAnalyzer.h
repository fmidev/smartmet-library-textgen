// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::SevereFrostAnalyzer
 */
// ======================================================================

#ifndef WEATHERANALYZER_SEVEREFROSTANALYZER_H
#define WEATHERANALYZER_SEVEREFROSTANALYZER_H

#include "WeatherFunction.h"
#include "ParameterAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalyzerSources;
  class WeatherArea;
  class WeatherPeriodGenerator;
  class WeatherResult;

  class SevereFrostAnalyzer : public ParameterAnalyzer
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

#endif // WEATHERANALYSIS_SEVEREFROSTANALYZER_H

// ======================================================================
