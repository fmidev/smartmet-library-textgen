// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class WeatherAnalysis::ParameterAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::ParameterAnalyzer
 *
 * \brief Parameter analysis functions
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_PARAMETERANALYZER_H
#define WEATHERANALYSIS_PARAMETERANALYZER_H

#include "NullAcceptor.h"
#include "WeatherDataType.h"
#include "WeatherFunction.h"

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherPeriod;
  class WeatherResult;

  class ParameterAnalyzer
  {
  public:

	virtual ~ParameterAnalyzer() { }

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherDataType & theDataType,
								  const WeatherFunction & theAreaFunction,
								  const WeatherFunction & theTimeFunction,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const Acceptor & theAreaAcceptor,
								  const Acceptor & theTimeAcceptor,
								  const Acceptor & theTester = NullAcceptor()) const = 0;
  }; // class ParameterAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_PARAMETERANALYZER_H

// ======================================================================
