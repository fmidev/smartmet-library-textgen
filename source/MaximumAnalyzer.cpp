// ======================================================================
/*!
 * \file
 * \brief Implementation of class MaximumAnalyzer
 */
// ======================================================================

#include "MaximumAnalyzer.h"
#include "AnalysisSources.h"
#include "MaskSource.h"
#include "WeatherAnalysisError.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"
#include "WeatherResult.h"
#include "WeatherSource.h"

#include "NFmiEnumConverter.h"
#include "NFmiFastQueryInfo.h"
#include "NFmiQueryData.h"
#include "NFmiSettings.h"

#include "boost/shared_ptr.hpp"
#include <string>

using namespace std;
using namespace boost;

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Analyze parameter maximum value
   *
   * \param theSources Analysis sources
   * \param theLimits Analysis limits, not used
   * \param thePeriod Analysis period
   * \param theArea Analysis area
   * \param theDataName The name of the data file
   * \param theParameterName The name of the parameter
   * \return The analysis result
   */
  // ----------------------------------------------------------------------

  WeatherResult
  MaximumAnalyzer::analyze(const AnalysisSources & theSources,
						   const WeatherLimits & theLimits,
						   const WeatherPeriod & thePeriod,
						   const WeatherArea & theArea,
						   const std::string & theDataName,
						   const std::string & theParameterName) const
  {

	// Establish the data

	if(!NFmiSettings::instance().isset(theDataName))
	  throw WeatherAnalysisError("No "+theDataName+" defined in settings");
	string dataname = NFmiSettings::instance().value(theDataName);

	// Get the data into use

	shared_ptr<WeatherSource> wsource = theSources.getWeatherSource();
	shared_ptr<NFmiQueryData> qd = wsource->data(dataname);
	NFmiFastQueryInfo qi(qd.get());

	// Try activating the parameter

	NFmiEnumConverter converter;
	FmiParameterName param = FmiParameterName(converter.ToEnum(theParameterName));
	if(param == kFmiBadParameter)
	  throw WeatherAnalysisError("Parameter "+theParameterName+" is not defined in newbase");

	if(!qi.Param(param))
	  throw WeatherAnalysisError(theParameterName+" is not available in "+dataname);
	
	// Handle points and areas separately

	if(theArea.isNamed())
	  {
		shared_ptr<MaskSource> msource = theSources.getMaskSource();
		MaskSource::mask_type mask = msource->mask(theArea,dataname,*wsource);

	  }
	else
	  {
	  }
	


	return WeatherResult(kFloatMissing,0);
  }


} // namespace WeatherAnalysis

// ======================================================================
