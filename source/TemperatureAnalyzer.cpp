// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::TemperatureAnalyzer
 */
// ======================================================================

#include "TemperatureAnalyzer.h"
#include "AnalysisSources.h"
#include "RegularFunctionAnalyzer.h"
#include "WeatherResult.h"

using namespace std;
using namespace boost;

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Analyze temperature in an area
   *
   * \param theSources Analysis sources
   * \param theAreaFunction The area function to analyze
   * \param theTimeFunction The time function to analyze
   * \param theLimits The optional limits for the function
   * \param thePeriod The time period
   * \param theArea The area
   */
  // ----------------------------------------------------------------------
  
  WeatherResult
  TemperatureAnalyzer::analyze(const AnalysisSources & theSources,
							   const WeatherFunction & theAreaFunction,
							   const WeatherFunction & theTimeFunction,
							   const WeatherLimits & theLimits,
							   const WeatherPeriod & thePeriod,
							   const WeatherArea & theArea) const
  {
	const string varname = "textgen::temperature_forecast";
	const string parname = "Temperature";

	auto_ptr<FunctionAnalyzer> analyzer(new RegularFunctionAnalyzer(theAreaFunction,theTimeFunction));

	return analyzer->analyze(theSources,theLimits,thePeriod,theArea,
							 varname,parname);

  }

} // namespace WeatherAnalysis

// ======================================================================
