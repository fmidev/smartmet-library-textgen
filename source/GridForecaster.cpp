// ======================================================================
/*!
 * \file
 * \brief Implementation of class GridForecaster
 */
// ======================================================================

#include "GridForecaster.h"
#include "WeatherArea.h"
#include "WeatherLimits.h"
#include "WeatherPeriod.h"
#include "WeatherResult.h"

#include "PressureAnalysis.h"
#include "TemperatureAnalysis.h"
#include "WindDirectionAnalysis.h"
#include "WindSpeedAnalysis.h"

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Analyze weather forecast for area
   *
   * \param theParameter The weather phenomenon to analyze
   * \param theFunction The function to calculate
   * \param theLimits The associated limits
   * \param thePeriod The time period to analyze
   * \param theArea The name of the area to analyze
   * \return The result of the analysis
   */
  // ----------------------------------------------------------------------

  WeatherResult GridForecaster::analyze(const WeatherParameter & theParameter,
										const WeatherFunction & theFunction,
										const WeatherLimits & theLimits,
										const WeatherPeriod & thePeriod,
										const WeatherArea & theArea) const
  {
	switch(theParameter)
	  {
	  case Temperature:
		return TemperatureAnalysis::forecast(theFunction,theLimits,thePeriod,theArea);
	  case Pressure:
		return PressureAnalysis::forecast(theFunction,theLimits,thePeriod,theArea);
	  case WindSpeed:
		return WindSpeedAnalysis::forecast(theFunction,theLimits,thePeriod,theArea);
	  case WindDirection:
		return WindDirectionAnalysis::forecast(theFunction,theLimits,thePeriod,theArea);
	  }
	// should never reach this place
	return WeatherResult(kFloatMissing,0);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Analyze weather forecast for given location
   *
   * \param theParameter The weather phenomenon to analyze
   * \param theFunction The function to calculate
   * \param theLimits The associated limits
   * \param thePeriod The time period to analyze
   * \param theArea The name of the area to analyze
   * \return The result of the analysis
   */
  // ----------------------------------------------------------------------

  WeatherResult GridForecaster::analyze(const WeatherParameter & theParameter,
										const WeatherFunction & theFunction,
										const WeatherLimits & theLimits,
										const WeatherPeriod & thePeriod,
										const NFmiPoint & thePoint) const
  {
	switch(theParameter)
	  {
	  case Temperature:
		return TemperatureAnalysis::forecast(theFunction,theLimits,thePeriod,thePoint);
	  case Pressure:
		return PressureAnalysis::forecast(theFunction,theLimits,thePeriod,thePoint);
	  case WindSpeed:
		return WindSpeedAnalysis::forecast(theFunction,theLimits,thePeriod,thePoint);
	  case WindDirection:
		return WindDirectionAnalysis::forecast(theFunction,theLimits,thePeriod,thePoint);
	  }
	// should never reach this place
	return WeatherResult(kFloatMissing,0);
  }


} // namespace WeatherAnalysis

// ======================================================================
