// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::RegularFunctionAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::RegularFunctionAnalyzer
 *
 * \brief Regular function analysis
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_REGULARFUNCTIONANALYZER_H
#define WEATHERANALYSIS_REGULARFUNCTIONANALYZER_H

#include "FunctionAnalyzer.h"
#include "WeatherFunction.h"

#include <string>

class NFmiDataModifier;

namespace WeatherAnalysis
{
  class AnalysisSources;
  class Calculator;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class RegularFunctionAnalyzer : public FunctionAnalyzer
  {
  public:

	virtual ~RegularFunctionAnalyzer() { }

	RegularFunctionAnalyzer(const WeatherFunction & theAreaFunction,
							const WeatherFunction & theTimeFunction);

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const Acceptor & theAreaAcceptor,
								  const Acceptor & theTimeAcceptor,
								  const Acceptor & theTester,
								  const std::string & theDataName,
								  const std::string & theParameterName,
								  int theInterval,
								  Calculator & theSubCalculator) const;
  private:

	RegularFunctionAnalyzer();
	WeatherFunction itsAreaFunction;
	WeatherFunction itsTimeFunction;

  }; // class RegularFunctionAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_REGULARFUNCTIONANALYZER_H

// ======================================================================
