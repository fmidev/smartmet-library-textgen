// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::WindChillFunctionAnalyzer
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WINDCHILLFUNCTIONALYZER_H
#define WEATHERANALYSIS_WINDCHILLFUNCTIONANALYZER_H

#include "FunctionAnalyzer.h"
#include "WeatherFunction.h"
#include "WindChillQueryInfo.h"

#include <string>

class NFmiDataModifier;

namespace WeatherAnalysis
{
  class AnalysisSources;
  class Calculator;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherPeriodGenerator;
  class WeatherResult;

  class WindChillFunctionAnalyzer : public FunctionAnalyzer
  {
  public:

	virtual ~WindChillFunctionAnalyzer() { }

	WindChillFunctionAnalyzer(const WeatherFunction & theAreaFunction,
							const WeatherFunction & theTimeFunction,
							const WeatherFunction & theSubTimeFunction);

	virtual const WeatherResult
	analyze(const AnalysisSources & theSources,
			const WeatherDataType & theDataType,
			const WeatherArea & theArea,
			const WeatherPeriodGenerator & thePeriods,
			const Acceptor & theAreaAcceptor,
			const Acceptor & theTimeAcceptor,
			const Acceptor & theTester,
			const std::string & theDataName,
			const std::string & theParameterName) const;

	void modulo(int theModulo);

  private:

	WindChillFunctionAnalyzer();

	const WeatherFunction itsAreaFunction;
	const WeatherFunction itsTimeFunction;
	const WeatherFunction itsSubTimeFunction;

	bool itIsModulo;
	int itsModulo;

  }; // class WindChillFunctionAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WINDCHILLFUNCTIONANALYZER_H

// ======================================================================
