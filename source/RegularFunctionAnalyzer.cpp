// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::RegularFunctionAnalyzer
 */
// ======================================================================

#include "RegularFunctionAnalyzer.h"

#include "AnalysisSources.h"
#include "QueryDataIntegrator.h"
#include "CalculatorFactory.h"
#include "MaskSource.h"
#include "Settings.h"
#include "WeatherAnalysisError.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"
#include "WeatherResult.h"
#include "WeatherSource.h"

#include "NFmiEnumConverter.h"
#include "NFmiFastQueryInfo.h"
#include "NFmiQueryData.h"

#include "boost/shared_ptr.hpp"
#include <string>

using namespace std;
using namespace boost;

namespace
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Return data type as string
   *
   * \param theDataType The data type
   * \return The string
   */
  // ----------------------------------------------------------------------

  const char * data_type_name(const WeatherAnalysis::WeatherDataType & theDataType)
  {
	using namespace WeatherAnalysis;

	switch(theDataType)
	  {
	  case Forecast:
		return "forecast";
	  case Observation:
		return "observation";
	  case Climatology:
		return "climatology";
	  }

	throw WeatherAnalysisError("Unrecognized WeatherDataType in RegularFunctionAnalyzer");
  }

} // namespace anonymous


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
												   const WeatherFunction & theTimeFunction,
												   const WeatherFunction & theSubTimeFunction)
	: itsAreaFunction(theAreaFunction)
	, itsTimeFunction(theTimeFunction)
	, itsSubTimeFunction(theSubTimeFunction)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Analyze area and time functions
   *
   * Note that theTester argument is associated to both itsAreaCalculator
   * and itsTimeCalculator. Naturally the association is real only
   * for functions which require the tester. At the moment Percentage
   * is the only such function.
   *
   * It is assumed that the space and time functions are never both
   * Percentage, hence atmost one Acceptor is needed for Percentage
   * calculations.
   *
   * \param theSources Analysis sources
   * \param theDataType Analysis data type
   * \param theArea Analysis area
   * \param thePeriod Analysis period
   * \param theGenerator Analysis subperiod generator
   * \param theAreaAcceptor The data acceptor in area integration
   * \param theTimeAcceptor The data acceptor in space integration
   * \param theTester The data tester for Percentage calculations
   * \param theDataName The name of the data file
   * \param theParameterName The name of the parameter
   * \return The analysis result
   */
  // ----------------------------------------------------------------------

  WeatherResult
  RegularFunctionAnalyzer::analyze(const AnalysisSources & theSources,
								   const WeatherDataType & theDataType,
								   const WeatherArea & theArea,
								   const WeatherPeriodGenerator & thePeriods,
								   const Acceptor & theAreaAcceptor,
								   const Acceptor & theTimeAcceptor,
								   const Acceptor & theTester,
								   const std::string & theDataName,
								   const std::string & theParameterName) const
  {

	// Establish the data

	const string datavar = theDataName + '_' + data_type_name(theDataType);
	const string dataname = Settings::require_string(datavar);

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

		shared_ptr<Calculator> spacemod = CalculatorFactory::create(itsAreaFunction,theTester);
		shared_ptr<Calculator> timemod = CalculatorFactory::create(itsTimeFunction,theTester);
		shared_ptr<Calculator> subtimemod = CalculatorFactory::create(itsSubTimeFunction,theTester);

		spacemod->acceptor(theAreaAcceptor);
		timemod->acceptor(theTimeAcceptor);
		subtimemod->acceptor(theTimeAcceptor);

		float result = QueryDataIntegrator::Integrate(qi,
													  thePeriods,
													  *subtimemod,
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

		shared_ptr<Calculator> timemod = CalculatorFactory::create(itsTimeFunction,theTester);
		shared_ptr<Calculator> subtimemod = CalculatorFactory::create(itsSubTimeFunction,theTester);
		timemod->acceptor(theTimeAcceptor);
		subtimemod->acceptor(theTimeAcceptor);

		float result = QueryDataIntegrator::Integrate(qi,
													  thePeriods,
													  *subtimemod,
													  *timemod);
		
		return WeatherResult(result,1);

	  }

	return WeatherResult(kFloatMissing,0);
  }


} // namespace WeatherAnalysis

// ======================================================================
