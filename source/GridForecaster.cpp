// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::GridForecaster
 */
// ======================================================================

#include "GridForecaster.h"
#include "Acceptor.h"
#include "AnalysisSources.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"
#include "WeatherResult.h"

#include "PrecipitationAnalyzer.h"
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
   * \param theAreaFunction The area function
   * \param theTimeFunction The time function
   * \param theAcceptor The weather data acceptor
   * \param thePeriod The time period to analyze
   * \param theArea The name of the area to analyze
   * \return The result of the analysis
   */
  // ----------------------------------------------------------------------

  WeatherResult
  GridForecaster::analyze(const AnalysisSources & theSources,
						  const WeatherParameter & theParameter,
						  const WeatherFunction & theAreaFunction,
						  const WeatherFunction & theTimeFunction,
						  const WeatherPeriod & thePeriod,
						  const WeatherArea & theArea,
						  const Acceptor & theAcceptor) const
  {
	switch(theParameter)
	  {
	  case Temperature:
		{
		  TemperatureAnalyzer analyzer;
		  return analyzer.analyze(theSources,
								  theAreaFunction,
								  theTimeFunction,
								  thePeriod,
								  theArea,
								  theAcceptor);
		}
	  case MaxTemperature:
		{
		  MaxTemperatureAnalyzer analyzer;
		  return analyzer.analyze(theSources,
								  theAreaFunction,
								  theTimeFunction,
								  thePeriod,
								  theArea,
								  theAcceptor);
		}
	  case MinTemperature:
		{
		  MinTemperatureAnalyzer analyzer;
		  return analyzer.analyze(theSources,
								  theAreaFunction,
								  theTimeFunction,
								  thePeriod,
								  theArea,
								  theAcceptor);
		}
	  case Precipitation:
		{
		  PrecipitationAnalyzer analyzer;
		  return analyzer.analyze(theSources,
								  theAreaFunction,
								  theTimeFunction,
								  thePeriod,
								  theArea,
								  theAcceptor);
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
