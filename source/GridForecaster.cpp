// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::GridForecaster
 */
// ======================================================================

#include "GridForecaster.h"
#include "AnalysisSources.h"
#include "WeatherArea.h"
#include "WeatherLimits.h"
#include "WeatherPeriod.h"
#include "WeatherResult.h"

#include "TemperatureAnalyzer.h"
#include "MaxTemperatureAnalyzer.h"
#include "MinTemperatureAnalyzer.h"

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Analyze weather forecast for area
   *
   * \param theSources Analysis sources
   * \param theParameter The weather phenomenon to analyze
   * \param theFunction The function to calculate
   * \param theLimits The associated limits
   * \param thePeriod The time period to analyze
   * \param theArea The name of the area to analyze
   * \return The result of the analysis
   */
  // ----------------------------------------------------------------------

  WeatherResult GridForecaster::analyze(const AnalysisSources & theSources,
										const WeatherParameter & theParameter,
										const WeatherFunction & theFunction,
										const WeatherLimits & theLimits,
										const WeatherPeriod & thePeriod,
										const WeatherArea & theArea) const
  {
	switch(theParameter)
	  {
	  case Temperature:
		{
		  TemperatureAnalyzer analyzer;
		  return analyzer.analyze(theSources,theFunction,theLimits,thePeriod,theArea);
		}
	  case MaxTemperature:
		{
		  MaxTemperatureAnalyzer analyzer;
		  return analyzer.analyze(theSources,theFunction,theLimits,thePeriod,theArea);
		}
	  case MinTemperature:
		{
		  MinTemperatureAnalyzer analyzer;
		  return analyzer.analyze(theSources,theFunction,theLimits,thePeriod,theArea);
		}
	  case Pressure:
	  case WindSpeed:
	  case WindDirection:
		break;
	  }
	// should never reach this place
	return WeatherResult(kFloatMissing,0);
  }

} // namespace WeatherAnalysis

// ======================================================================
