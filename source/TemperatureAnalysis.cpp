// ======================================================================
/*!
 * \file
 * \brief Implementation of class TemperatureAnalysis
 */
// ======================================================================

#include "TemperatureAnalysis.h"
#include "AnalysisSources.h"
#include "MaskSource.h"
#include "WeatherArea.h"
#include "WeatherLimits.h"
#include "WeatherPeriod.h"
#include "WeatherResult.h"
#include "WeatherSource.h"

#include "NFmiDataIntegrator.h"
#include "NFmiDataModifierMax.h"
#include "NFmiSettings.h"
#include "NFmiFastQueryInfo.h"

#include <stdexcept>

using namespace std;
using namespace boost;

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

  // ----------------------------------------------------------------------
  /*!
   * \brief Analyze temperature in an area
   *
   * \param theSources Analysis sources
   * \param theFunction The function to analyze
   * \param theLimits The optional limits for the function
   * \param thePeriod The time period
   * \param theArea The area
   */
  // ----------------------------------------------------------------------
  
  WeatherResult
  TemperatureAnalysis::forecast(const AnalysisSources & theSources,
								const WeatherFunction & theFunction,
								const WeatherLimits & theLimits,
								const WeatherPeriod & thePeriod,
								const WeatherArea & theArea)
  {
	const string dataname = temperature_forecast_source();

	shared_ptr<WeatherSource> wsource = theSources.getWeatherSource();
	shared_ptr<NFmiQueryData> qd = wsource->data(dataname);
	// WeatherId id = wsource->id(dataname);

	MaskSource::mask_type mask;
	if(theArea.isNamed())
	  {
		shared_ptr<MaskSource> msource = theSources.getMaskSource();
		mask = msource->mask(theArea,dataname,*wsource);
	  }

	switch(theFunction)
	  {
	  case Maximum:
		break;
	  case Minimum:
		break;
	  case Mean:
		break;
	  case Probability:
		break;
	  }
	return WeatherResult(kFloatMissing,0);
  }

} // namespace WeatherAnalysis

// ======================================================================
