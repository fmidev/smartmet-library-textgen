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
#include "WeatherPeriodGenerator.h"
#include "WeatherResult.h"

#include "CloudinessAnalyzer.h"
#include "FrostAnalyzer.h"
#include "PrecipitationAnalyzer.h"
#include "RelativeHumidityAnalyzer.h"
#include "SevereFrostAnalyzer.h"
#include "TemperatureAnalyzer.h"
#include "WindSpeedAnalyzer.h"

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
   * \param theSubTimeFunction The time function for subperiods
   * \param theArea The name of the area to analyze
   * \param thePeriods The time period to analyze
   * \param theAreaAcceptor The weather data acceptor in area integration
   * \param theTimeAcceptor The weather data acceptor in time integration
   * \param theTester The acceptor for Percentage calculations
   * \return The result of the analysis
   */
  // ----------------------------------------------------------------------

  WeatherResult
  GridForecaster::analyze(const AnalysisSources & theSources,
						  const WeatherParameter & theParameter,
						  const WeatherFunction & theAreaFunction,
						  const WeatherFunction & theTimeFunction,
						  const WeatherFunction & theSubTimeFunction,
						  const WeatherArea & theArea,
						  const WeatherPeriodGenerator & thePeriods,
						  const Acceptor & theAreaAcceptor,
						  const Acceptor & theTimeAcceptor,
						  const Acceptor & theTester) const
  {
	switch(theParameter)
	  {
	  case Temperature:
		{
		  TemperatureAnalyzer analyzer;
		  return analyzer.analyze(theSources,
								  Forecast,
								  theAreaFunction,
								  theTimeFunction,
								  theSubTimeFunction,
								  theArea,
								  thePeriods,
								  theAreaAcceptor,
								  theTimeAcceptor,
								  theTester);
		}
	  case Precipitation:
		{
		  PrecipitationAnalyzer analyzer;
		  return analyzer.analyze(theSources,
								  Forecast,
								  theAreaFunction,
								  theTimeFunction,
								  theSubTimeFunction,
								  theArea,
								  thePeriods,
								  theAreaAcceptor,
								  theTimeAcceptor,
								  theTester);
		}
	  case Cloudiness:
		{
		  CloudinessAnalyzer analyzer;
		  return analyzer.analyze(theSources,
								  Forecast,
								  theAreaFunction,
								  theTimeFunction,
								  theSubTimeFunction,
								  theArea,
								  thePeriods,
								  theAreaAcceptor,
								  theTimeAcceptor,
								  theTester);
		}
	  case Frost:
		{
		  FrostAnalyzer analyzer;
		  return analyzer.analyze(theSources,
								  Forecast,
								  theAreaFunction,
								  theTimeFunction,
								  theSubTimeFunction,
								  theArea,
								  thePeriods,
								  theAreaAcceptor,
								  theTimeAcceptor,
								  theTester);
		}
	  case SevereFrost:
		{
		  SevereFrostAnalyzer analyzer;
		  return analyzer.analyze(theSources,
								  Forecast,
								  theAreaFunction,
								  theTimeFunction,
								  theSubTimeFunction,
								  theArea,
								  thePeriods,
								  theAreaAcceptor,
								  theTimeAcceptor,
								  theTester);
		}
	  case RelativeHumidity:
		{
		  RelativeHumidityAnalyzer analyzer;
		  return analyzer.analyze(theSources,
								  Forecast,
								  theAreaFunction,
								  theTimeFunction,
								  theSubTimeFunction,
								  theArea,
								  thePeriods,
								  theAreaAcceptor,
								  theTimeAcceptor,
								  theTester);
		}
	  case WindSpeed:
		{
		  WindSpeedAnalyzer analyzer;
		  return analyzer.analyze(theSources,
								  Forecast,
								  theAreaFunction,
								  theTimeFunction,
								  theSubTimeFunction,
								  theArea,
								  thePeriods,
								  theAreaAcceptor,
								  theTimeAcceptor,
								  theTester);
		}
	  case Pressure:
	  case WindDirection:
		break;
	  }
	// should never reach this place
	return WeatherResult(kFloatMissing,0);
  }


} // namespace WeatherAnalysis

// ======================================================================
