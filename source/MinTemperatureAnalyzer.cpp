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
   * \param theDataType The source type to be used
   * \param theAreaFunction The area function to analyze
   * \param theTimeFunction The time function to analyze
   * \param thePeriod The time period
   * \param theArea The area
   * \param theAreaAcceptor The acceptor for data in area integrals
   * \param theTimeAcceptor The acceptor for data in time integrals
   * \param theTester The optional acceptor for Percentage calculations
   */
  // ----------------------------------------------------------------------
  
  WeatherResult
  MinTemperatureAnalyzer::analyze(const AnalysisSources & theSources,
								  const WeatherDataType & theDataType,
								  const WeatherFunction & theAreaFunction,
								  const WeatherFunction & theTimeFunction,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const Acceptor & theAreaAcceptor,
								  const Acceptor & theTimeAcceptor,
								  const Acceptor & theTester) const
  {
	const string varname = "textgen::temperature";
	const string parname = "Temperature";

	auto_ptr<FunctionAnalyzer> analyzer(new RegularFunctionAnalyzer(theAreaFunction,theTimeFunction));

	MinimumCalculator mincalculator;
	const int one_day = 24;

	return analyzer->analyze(theSources,theDataType,
							 thePeriod,theArea,
							 theAreaAcceptor,theTimeAcceptor,theTester,
							 varname,parname,
							 one_day,mincalculator);
  }

} // namespace WeatherAnalysis

// ======================================================================
