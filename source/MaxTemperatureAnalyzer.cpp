// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::MaxTemperatureAnalyzer
 */
// ======================================================================

#include "MaxTemperatureAnalyzer.h"
#include "AnalysisSources.h"
#include "MaximumCalculator.h"
#include "RegularFunctionAnalyzer.h"
#include "WeatherResult.h"

using namespace std;
using namespace boost;

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Analyze daily maximum temperature in an area
   *
   * \param theSources Analysis sources
   * \param theAreaFunction The area function to analyze
   * \param theTimeFunction The time function to analyze
   * \param thePeriod The time period
   * \param theArea The area
   * \param theAcceptor The acceptor for data
   * \param theTester The optional acceptor for Percentage calculations
   */
  // ----------------------------------------------------------------------
  
  WeatherResult
  MaxTemperatureAnalyzer::analyze(const AnalysisSources & theSources,
								  const WeatherFunction & theAreaFunction,
								  const WeatherFunction & theTimeFunction,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const Acceptor & theAcceptor,
								  const Acceptor & theTester) const
  {
	const string varname = "textgen::temperature_forecast";
	const string parname = "Temperature";

	auto_ptr<FunctionAnalyzer> analyzer(new RegularFunctionAnalyzer(theAreaFunction,theTimeFunction));

	MaximumCalculator maxcalculator;
	const int one_day = 24;

	return analyzer->analyze(theSources,thePeriod,theArea,
							 theAcceptor,theTester,
							 varname,parname,
							 one_day,maxcalculator);
  }

} // namespace WeatherAnalysis

// ======================================================================
