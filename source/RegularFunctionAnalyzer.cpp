// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::RegularFunctionAnalyzer
 */
// ======================================================================

#include "RegularFunctionAnalyzer.h"

#include "AnalysisSources.h"
#include "DataModifierFactory.h"
#include "MaskSource.h"
#include "WeatherAnalysisError.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"
#include "WeatherResult.h"
#include "WeatherSource.h"

#include "NFmiDataIntegrator.h"
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
   * \brief Constructor
   *
   * \param theAreaFunction The area function
   * \param theTimeFunction The time function
   */
  // ----------------------------------------------------------------------

  RegularFunctionAnalyzer::RegularFunctionAnalyzer(const WeatherFunction & theAreaFunction,
												   const WeatherFunction & theTimeFunction)
	: itsAreaFunction(theAreaFunction)
	, itsTimeFunction(theTimeFunction)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Analyze area and time functions
   *
   * If theInterval is < 0, an invalid result is returned.
   * If theInterval is 0, no subintervals are created and
   * theSubModifier is ignored.
   *
   * \param theSources Analysis sources
   * \param theLimits Analysis limits, not used
   * \param thePeriod Analysis period
   * \param theArea Analysis area
   * \param theDataName The name of the data file
   * \param theParameterName The name of the parameter
   * \param theInterval The sub interval in hours
   * \param theSubModifier The modifier for the sub interval
   * \return The analysis result
   */
  // ----------------------------------------------------------------------

  WeatherResult
  RegularFunctionAnalyzer::analyze(const AnalysisSources & theSources,
								   const WeatherLimits & theLimits,
								   const WeatherPeriod & thePeriod,
								   const WeatherArea & theArea,
								   const std::string & theDataName,
								   const std::string & theParameterName,
								   int theInterval,
								   NFmiDataModifier & theSubModifier) const
  {
	// Safety against bad loop
	if(theInterval<0)
	  return WeatherResult(kFloatMissing,0);

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

		shared_ptr<NFmiDataModifier> spacemod = DataModifierFactory::create(itsAreaFunction);
		shared_ptr<NFmiDataModifier> timemod = DataModifierFactory::create(itsTimeFunction);

		float result = NFmiDataIntegrator::Integrate(qi,
													 thePeriod.utcStartTime(),
													 thePeriod.utcEndTime(),
													 theInterval,
													 theSubModifier,
													 *timemod,
													 *mask,
													 *spacemod);

		if(result == kFloatMissing)
		  return WeatherResult(kFloatMissing,0);

		return WeatherResult(result,1);

	  }
	else
	  {
		if(!(qi.Location(theArea.point())))
		  throw WeatherAnalysisError("Could not set desired coordinate in "+dataname);

		shared_ptr<NFmiDataModifier> timemod = DataModifierFactory::create(itsTimeFunction);

		float result = NFmiDataIntegrator::Integrate(qi,
													 thePeriod.utcStartTime(),
													 thePeriod.utcEndTime(),
													 theInterval,
													 theSubModifier,
													 *timemod);

		return WeatherResult(result,1);

	  }

	return WeatherResult(kFloatMissing,0);
  }


} // namespace WeatherAnalysis

// ======================================================================