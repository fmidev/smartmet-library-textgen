// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::MeanAnalyzer
 */
// ======================================================================

#include "MeanAnalyzer.h"
#include "AnalysisSources.h"
#include "MaskSource.h"
#include "WeatherAnalysisError.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"
#include "WeatherResult.h"
#include "WeatherSource.h"

#include "NFmiDataIntegrator.h"
#include "NFmiDataModifierAvg.h"
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
   * \brief Analyze parameter area mean of time means
   *
   * \param theSources Analysis sources
   * \param theLimits Analysis limits, not used
   * \param thePeriod Analysis period
   * \param theArea Analysis area
   * \param theDataName The name of the data file
   * \param theParameterName The name of the parameter
   * \return The analysis result
   *
   * \todo Returning quality = 1 is not correct
   */
  // ----------------------------------------------------------------------

  WeatherResult
  MeanAnalyzer::analyze(const AnalysisSources & theSources,
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

		// Result

		NFmiDataModifierAvg spacemodifier;
		NFmiDataModifierAvg timemodifier;

		float result = NFmiDataIntegrator::Integrate(qi,
													 thePeriod.utcStartTime(),
													 thePeriod.utcEndTime(),
													 timemodifier,
													 *mask,
													 spacemodifier);

		if(result == kFloatMissing)
		  return WeatherResult(kFloatMissing,0);

		return WeatherResult(result,1);

	  }
	else
	  {
		if(!(qi.Location(theArea.point())))
		  throw WeatherAnalysisError("Could not set desired coordinate in "+dataname);

		NFmiDataModifierAvg timemodifier;
		float result = NFmiDataIntegrator::Integrate(qi,
													 thePeriod.utcStartTime(),
													 thePeriod.utcEndTime(),
													 timemodifier);

		return WeatherResult(result,1);

	  }

	return WeatherResult(kFloatMissing,0);
  }


} // namespace WeatherAnalysis

// ======================================================================
