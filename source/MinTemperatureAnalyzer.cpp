// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::MinTemperatureAnalyzer
 */
// ======================================================================

#include "MinTemperatureAnalyzer.h"
#include "AnalysisSources.h"
#include "MinimumCalculator.h"
#include "RegularFunctionAnalyzer.h"
#include "WeatherResult.h"

using namespace std;
using namespace boost;

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Analyze daily minimum temperature in an area
   *
   * \param theSources Analysis sources
   * \param theAreaFunction The area function to analyze
   * \param theTimeFunction The time function to analyze
   * \param thePeriod The time period
   * \param theArea The area
   * \param theAcceptor The acceptor for data
   */
  // ----------------------------------------------------------------------
  
  WeatherResult
  MinTemperatureAnalyzer::analyze(const AnalysisSources & theSources,
								  const WeatherFunction & theAreaFunction,
								  const WeatherFunction & theTimeFunction,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const Acceptor & theAcceptor) const
  {
	const string varname = "textgen::temperature_forecast";
	const string parname = "Temperature";

	auto_ptr<FunctionAnalyzer> analyzer(new RegularFunctionAnalyzer(theAreaFunction,theTimeFunction));

	MinimumCalculator mincalculator;
	const int one_day = 24;

	return analyzer->analyze(theSources,thePeriod,theArea,theAcceptor,
							 varname,parname,
							 one_day,mincalculator);
  }

} // namespace WeatherAnalysis

// ======================================================================
