// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TemperatureAnalysis
 */
// ======================================================================

#include "TemperatureAnalysis.h"
#include "AnalysisSources.h"
#include "WeatherArea.h"
#include "WeatherLimits.h"
#include "WeatherPeriod.h"
#include "WeatherResult.h"
#include "NFmiSettings.h"
#include <stdexcept>

using namespace std;

namespace
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Return the temperature forecast source
   */
  // ----------------------------------------------------------------------

  string temperature_forecast_source()
  {
	const string varname = "textgen::temperature_forecast";
	if(!NFmiSettings::instance().isset(varname))
	  throw runtime_error("No temperature source defined in "+varname);

	const string source = NFmiSettings::instance().value(varname);
	return source;
  }

}

namespace WeatherAnalysis
{
  namespace TemperatureAnalysis
  {

	// ----------------------------------------------------------------------
	/*!
	 * \param Analyze temperature in an area
	 *
	 * \param theSources Analysis sources
	 * \param theFunction The function to analyze
	 * \param theLimits The optional limits for the function
	 * \param thePeriod The time period
	 * \param theArea The area
	 */
	// ----------------------------------------------------------------------

	WeatherResult forecast(const AnalysisSources & theSources,
						   const WeatherFunction & theFunction,
						   const WeatherLimits & theLimits,
						   const WeatherPeriod & thePeriod,
						   const WeatherArea & theArea)
	{
	  const string source = temperature_forecast_source();

	  switch(theFunction)
		{
		case Maximum:
		  break;
		case Minimum:
		  break;
		case Mean:
		  break;
		case StandardDeviation:
		  break;
		case Probability:
		  break;
		case MaxMinimum:
		  break;
		case MinMaximum:
		  break;
		case MeanMinimum:
		  break;
		case MeanMaximum:
		  break;
		}
	  return WeatherResult(kFloatMissing,0);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \param Analyze temperature at given point
	 *
	 * \param theSources Analysis sources
	 * \param theFunction The function to analyze
	 * \param theLimits The optional limits for the function
	 * \param thePeriod The time period
	 * \param thePoint The point
	 */
	// ----------------------------------------------------------------------

	WeatherResult forecast(const AnalysisSources & theSources,
						   const WeatherFunction & theFunction,
						   const WeatherLimits & theLimits,
						   const WeatherPeriod & thePeriod,
						   const NFmiPoint & thePoint)
	{
	  const string source = temperature_forecast_source();

	  switch(theFunction)
		{
		case Maximum:
		  break;
		case Minimum:
		  break;
		case Mean:
		  break;
		case StandardDeviation:
		  break;
		case Probability:
		  break;
		case MaxMinimum:
		  break;
		case MinMaximum:
		  break;
		case MeanMinimum:
		  break;
		case MeanMaximum:
		  break;
		}
	  return WeatherResult(kFloatMissing,0);
	}

  } // namespace TemperatureAnalysis
} // namespace WeatherAnalysis

// ======================================================================
