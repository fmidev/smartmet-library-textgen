// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::PrecipitationAnalyzer
 */
// ======================================================================

#include "PrecipitationAnalyzer.h"
#include "AnalysisSources.h"
#include "FunctionAnalyzerFactory.h"
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
   * \param theFunction The function to analyze
   * \param theLimits The optional limits for the function
   * \param thePeriod The time period
   * \param theArea The area
   */
  // ----------------------------------------------------------------------
  
  WeatherResult
  PrecipitationAnalyzer::analyze(const AnalysisSources & theSources,
								 const WeatherFunction & theFunction,
								 const WeatherLimits & theLimits,
								 const WeatherPeriod & thePeriod,
								 const WeatherArea & theArea) const
  {
	const string varname = "textgen::precipitation_forecast";
	const string parname = "Precipitation1h";

	shared_ptr<FunctionAnalyzer> analyzer = FunctionAnalyzerFactory::create(theFunction);

	return analyzer->analyze(theSources,theLimits,thePeriod,theArea,
							 varname,parname);

  }

} // namespace WeatherAnalysis

// ======================================================================
