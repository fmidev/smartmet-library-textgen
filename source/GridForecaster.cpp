// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::GridForecaster
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::GridForecaster
 *
 * \brief Provides forecast analysis services to clients
 *
 * The GridForecaster class provides weather forecast analysis
 * services. All analysis is performed by analyzing the main
 * grid forecast. Probabilities are calculated by analyzing the
 * gridded forecasts.
 *
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
#include "PrecipitationFormAnalyzer.h"
#include "PrecipitationProbabilityAnalyzer.h"
#include "PrecipitationTypeAnalyzer.h"
#include "RelativeHumidityAnalyzer.h"
#include "SevereFrostAnalyzer.h"
#include "TemperatureAnalyzer.h"
#include "ThunderAnalyzer.h"
#include "WindDirectionAnalyzer.h"
#include "WindSpeedAnalyzer.h"

#include "boost/shared_ptr.hpp"

using namespace boost;

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
	typedef shared_ptr<ParameterAnalyzer> Analyzer;

	Analyzer analyzer;

	switch(theParameter)
	  {
	  case Temperature:
		{
		  analyzer = Analyzer(new TemperatureAnalyzer);
		  break;
		}
	  case Precipitation:
		{
		  analyzer = Analyzer(new PrecipitationAnalyzer);
		  break;
		}
	  case Cloudiness:
		{
		  analyzer = Analyzer(new CloudinessAnalyzer);
		  break;
		}
	  case Frost:
		{
		  analyzer = Analyzer(new FrostAnalyzer);
		  break;
		}
	  case SevereFrost:
		{
		  analyzer = Analyzer(new SevereFrostAnalyzer);
		  break;
		}
	  case RelativeHumidity:
		{
		  analyzer = Analyzer(new RelativeHumidityAnalyzer);
		  break;
		}
	  case WindSpeed:
		{
		  analyzer = Analyzer(new WindSpeedAnalyzer);
		  break;
		}
	  case WindDirection:
		{
		  analyzer = Analyzer(new WindDirectionAnalyzer);
		  break;
		}
	  case Thunder:
		{
		  analyzer = Analyzer(new ThunderAnalyzer);
		  break;
		}
	  case PrecipitationType:
		{
		  analyzer = Analyzer(new PrecipitationTypeAnalyzer);
		  break;
		}
	  case PrecipitationForm:
		{
		  analyzer = Analyzer(new PrecipitationFormAnalyzer);
		  break;
		}
	  case PrecipitationProbability:
		{
		  analyzer = Analyzer(new PrecipitationProbabilityAnalyzer);
		  break;
		}

	  }

	if(analyzer.get() != 0)
	  return analyzer->analyze(theSources,
							   Forecast,
							   theAreaFunction,
							   theTimeFunction,
							   theSubTimeFunction,
							   theArea,
							   thePeriods,
							   theAreaAcceptor,
							   theTimeAcceptor,
							   theTester);

	// should never reach this place
	return WeatherResult(kFloatMissing,0);
  }


} // namespace WeatherAnalysis

// ======================================================================
