// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::PrecipitationAnalyzer
 */
// ======================================================================

#include "PrecipitationAnalyzer.h"
#include "AnalysisSources.h"
#include "RegularFunctionAnalyzer.h"
#include "WeatherResult.h"

using namespace std;
using namespace boost;

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Analyze precipitation in an area
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
  PrecipitationAnalyzer::analyze(const AnalysisSources & theSources,
								 const WeatherFunction & theAreaFunction,
								 const WeatherFunction & theTimeFunction,
								 const WeatherPeriod & thePeriod,
								 const WeatherArea & theArea,
								 const Acceptor & theAcceptor,
								 const Acceptor & theTester) const
  {
	const string varname = "textgen::precipitation_forecast";
	const string parname = "Precipitation1h";

	auto_ptr<FunctionAnalyzer> analyzer(new RegularFunctionAnalyzer(theAreaFunction,theTimeFunction));

	return analyzer->analyze(theSources,thePeriod,theArea,
							 theAcceptor,theTester,
							 varname,parname);

  }

} // namespace WeatherAnalysis

// ======================================================================
