// ======================================================================
/*!
 * \file
 * \brief Implementation of class TemperatureAnalyzer
 */
// ======================================================================

#include "TemperatureAnalyzer.h"
#include "AnalysisSources.h"
#include "FunctionAnalyzerFactory.h"
#include "MaskSource.h"
#include "WeatherAnalysisError.h"
#include "WeatherArea.h"
#include "WeatherLimits.h"
#include "WeatherPeriod.h"
#include "WeatherResult.h"
#include "WeatherSource.h"

#include "NFmiDataIntegrator.h"
#include "NFmiDataModifierMax.h"
#include "NFmiSettings.h"
#include "NFmiFastQueryInfo.h"

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
	using namespace WeatherAnalysis;

	const string varname = "textgen::temperature_forecast";
	if(!NFmiSettings::instance().isset(varname))
	  throw WeatherAnalysisError("No temperature source defined in "+varname);

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
  TemperatureAnalyzer::analyze(const AnalysisSources & theSources,
							   const WeatherFunction & theFunction,
							   const WeatherLimits & theLimits,
							   const WeatherPeriod & thePeriod,
							   const WeatherArea & theArea) const
  {
	const string varname = "textgen::temperature_forecast";
	const string parname = "Temperature";


	const string dataname = temperature_forecast_source();

	shared_ptr<WeatherSource> wsource = theSources.getWeatherSource();
	shared_ptr<NFmiQueryData> qd = wsource->data(dataname);
	NFmiFastQueryInfo qi(qd.get());

	if(!qi.Param(kFmiTemperature))
	  throw WeatherAnalysisError("Temperature not available in dataname");

	MaskSource::mask_type mask;
	if(theArea.isNamed())
	  {
		shared_ptr<MaskSource> msource = theSources.getMaskSource();
		mask = msource->mask(theArea,dataname,*wsource);
	  }

	std::auto_ptr<FunctionAnalyzer> analyzer = FunctionAnalyzerFactory::create(theFunction);

	return analyzer->analyze(theSources,theLimits,thePeriod,theArea,
							 varname,parname);

  }

} // namespace WeatherAnalysis

// ======================================================================
