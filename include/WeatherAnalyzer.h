// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class WeatherAnalysis::WeatherAnalyzer
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERANALYZER_H
#define WEATHERANALYSIS_WEATHERANALYZER_H

#include "WeatherDataType.h"
#include "WeatherParameter.h"
#include "WeatherFunction.h"
#include "NullAcceptor.h"
#include "DefaultAcceptor.h"

#include <string>

class NFmiPoint;

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherPeriod;
  class WeatherPeriodGenerator;
  class WeatherResult;

  class WeatherAnalyzer
  {
  public:

	virtual ~WeatherAnalyzer() { }

	// derivatives override
	virtual const WeatherResult
	analyze(const AnalysisSources & theSources,
			const WeatherParameter & theParameter,
			const WeatherFunction & theAreaFunction,
			const WeatherFunction & theTimeFunction,
			const WeatherFunction & theSubTimeFunction,
			const WeatherArea & theArea,
			const WeatherPeriodGenerator & thePeriods,
			const Acceptor & theAreaAcceptor = DefaultAcceptor(),
			const Acceptor & theTimeAcceptor = DefaultAcceptor(),
			const Acceptor & theTester = NullAcceptor()) const = 0;

	// has default implementation
	virtual const WeatherResult
	analyze(const AnalysisSources & theSources,
			const WeatherParameter & theParameter,
			const WeatherFunction & theAreaFunction,
			const WeatherFunction & theTimeFunction,
			const WeatherArea & theArea,
			const WeatherPeriod & thePeriod,
			const Acceptor & theAreaAcceptor = DefaultAcceptor(),
			const Acceptor & theTimeAcceptor = DefaultAcceptor(),
			const Acceptor & theTester = NullAcceptor()) const;

	// has default implementation
	virtual const WeatherResult
	analyze(const std::string & theFakeVariable,
			const AnalysisSources & theSources,
			const WeatherParameter & theParameter,
			const WeatherFunction & theAreaFunction,
			const WeatherFunction & theTimeFunction,
			const WeatherFunction & theSubTimeFunction,
			const WeatherArea & theArea,
			const WeatherPeriodGenerator & thePeriods,
			const Acceptor & theAreaAcceptor = DefaultAcceptor(),
			const Acceptor & theTimeAcceptor = DefaultAcceptor(),
			const Acceptor & theTester = NullAcceptor()) const;

	// has default implementation
	virtual const WeatherResult
	analyze(const std::string & theFakeVariable,
			const AnalysisSources & theSources,
			const WeatherParameter & theParameter,
			const WeatherFunction & theAreaFunction,
			const WeatherFunction & theTimeFunction,
			const WeatherArea & theArea,
			const WeatherPeriod & thePeriod,
			const Acceptor & theAreaAcceptor = DefaultAcceptor(),
			const Acceptor & theTimeAcceptor = DefaultAcceptor(),
			const Acceptor & theTester = NullAcceptor()) const;

	
  }; // class WeatherAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERANALYZER_H

// ======================================================================
