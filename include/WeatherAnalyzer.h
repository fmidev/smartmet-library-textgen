// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class WeatherAnalysis::WeatherAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WeatherAnalyzer
 *
 * \brief Provides analysis services to clients
 *
 * The WeatherAnalyzer class provides an uniform interface for
 * weather analysis services. The only thing required by the
 * API is the ability to calculate a single result either for
 * an area or for a single point. All the remaining work is in
 * initializing the class implementing the abstract interface
 * with query data, map information and so on.
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERANALYZER_H
#define WEATHERANALYSIS_WEATHERANALYZER_H

#include "WeatherDataType.h"
#include "WeatherParameter.h"
#include "WeatherFunction.h"
#include "NullAcceptor.h"
#include "DefaultAcceptor.h"

class NFmiPoint;

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherPeriod;
  class WeatherResult;

  class WeatherAnalyzer
  {
  public:

	virtual ~WeatherAnalyzer() { }

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherParameter & theParameter,
								  const WeatherFunction & theAreaFunction,
								  const WeatherFunction & theTimeFunction,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const Acceptor & theAreaAcceptor = DefaultAcceptor(),
								  const Acceptor & theTimeAcceptor = DefaultAcceptor(),
								  const Acceptor & theTester = NullAcceptor()) const = 0;

  }; // class WeatherAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERANALYZER_H

// ======================================================================
