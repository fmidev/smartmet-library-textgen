// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::MaxTemperatureAnalyzer
 */
// ======================================================================

#include "MaxTemperatureAnalyzer.h"
#include "AnalysisSources.h"
#include "FunctionAnalyzerFactory.h"
#include "WeatherResult.h"

#include "NFmiDataModifierMax.h"

using namespace std;
using namespace boost;

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Analyze daily maximum temperature in an area
   *
   * \param theSources Analysis sources
   * \param theFunction The function to analyze
   * \param theLimits The optional limits for the function
   * \param thePeriod The time period
   * \param theArea The area
   */
  // ----------------------------------------------------------------------
  
  WeatherResult
  MaxTemperatureAnalyzer::analyze(const AnalysisSources & theSources,
								  const WeatherFunction & theFunction,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea) const
  {
	const string varname = "textgen::temperature_forecast";
	const string parname = "Temperature";

	shared_ptr<FunctionAnalyzer> analyzer = FunctionAnalyzerFactory::create(theFunction);

	NFmiDataModifierMax modifier;

	const int one_day = 24;
	return analyzer->analyze(theSources,theLimits,thePeriod,theArea,
							 varname,parname,
							 one_day,modifier);

  }

} // namespace WeatherAnalysis

// ======================================================================