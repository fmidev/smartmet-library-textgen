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

#include "NFmiDataIntegrator.h"
#include "NFmiDataModifierMax.h"
#include "NFmiDataModifierStandardDeviation.h"
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

		// Result

		NFmiDataModifierMax spacemodifier1;
		NFmiDataModifierMax timemodifier1;

		float result = NFmiDataIntegrator::Integrate(qi,
													 *mask,
													 spacemodifier1,
													 thePeriod.startTime(),
													 thePeriod.endTime(),
													 timemodifier1);

		if(result == kFloatMissing)
		  return WeatherResult(kFloatMissing,0);

		NFmiDataModifierMax timemodifier2;
		NFmiDataModifierStandardDeviation spacemodifier2;

		float sdev = NFmiDataIntegrator::Integrate(qi,
												   thePeriod.startTime(),
												   thePeriod.endTime(),
												   timemodifier2,
												   *mask,
												   spacemodifier2);

		if(sdev==kFloatMissing)
		  return WeatherResult(result,1);

		if(sdev<=1)
		  return WeatherResult(result,1);

		return WeatherResult(result,1/sdev);

	  }
	else
	  {
		if(!(qi.Location(theArea.point())))
		  throw WeatherAnalysisError("Could not set desired coordinate in "+dataname);

		NFmiDataModifierMax timemodifier;
		float result = NFmiDataIntegrator::Integrate(qi,
													 thePeriod.startTime(),
													 thePeriod.endTime(),
													 timemodifier);

		return WeatherResult(result,1);

	  }

	return WeatherResult(kFloatMissing,0);
  }


} // namespace WeatherAnalysis

// ======================================================================
