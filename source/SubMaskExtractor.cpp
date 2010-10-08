#include "RegularFunctionAnalyzer.h"

#include "ParameterAnalyzer.h"
#include "Acceptor.h"
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
#include "WeatherParameter.h"
#include "QueryDataTools.h"

#include <newbase/NFmiEnumConverter.h>
#include <newbase/NFmiFastQueryInfo.h>
#include <newbase/NFmiQueryData.h>
#include <newbase/NFmiFastQueryInfo.h>
#include <newbase/NFmiIndexMaskSource.h>
#include <newbase/NFmiTime.h>
#include <newbase/NFmiMetMath.h>
#include <newbase/NFmiGrid.h>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <sstream>

using namespace std;
using namespace boost;

	// ----------------------------------------------------------------------
	/*!
	 * \brief Integrate over space
	 *
	 * Integrate over space with current time, parameter and level.
	 *
	 * It is assumed that the index mask was generated from a grid
	 * equivalent to the one in the query data.
	 *
	 * \param theQI The query info
	 * \param theIndexMask The mask containing the space indices
	 * \param theSpaceCalculator The modifier for accumulating the result
	 * \return The accumulation result
	 */
	// ----------------------------------------------------------------------
	
namespace
{
  NFmiEnumConverter itsConverter;

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



  double GetLocationCoordinates(const AnalysisSources& theSources,
								const WeatherParameter& theParameter,
								const WeatherArea& theArea,
								const WeatherPeriod& thePeriod,
								const Acceptor& theAcceptor,
								vector<NFmiPoint*>& theResultData)
  {
 	double retval = 0.0;

	std::string parameterName;
	std::string dataName;

	ParameterAnalyzer::getParameterStrings(theParameter, parameterName, dataName);
	const string default_forecast = Settings::optional_string("textgen::default_forecast","");
	const string datavar = dataName + '_' + data_type_name(Forecast);
	const string dataname = Settings::optional_string(datavar, default_forecast);

	// Get the data into use
	  
	shared_ptr<WeatherSource> wsource = theSources.getWeatherSource();
	shared_ptr<NFmiStreamQueryData> qd = wsource->data(dataname);
	NFmiFastQueryInfo* theQI = qd->QueryInfoIter();

	FmiParameterName param = FmiParameterName(itsConverter.ToEnum(parameterName));
	if(param == kFmiBadParameter)
	  throw WeatherAnalysisError("Parameter "+parameterName+" is not defined in newbase");
  
	if(!theQI->Param(param))
	  throw WeatherAnalysisError(parameterName+" is not available in "+dataname);

	if(!theArea.isPoint())
	  {
		MaskSource::mask_type theIndexMask;
		
		switch(theArea.type())
		  {
		  case WeatherArea::Full:
			theIndexMask = theSources.getMaskSource()->mask(theArea,dataname,*wsource);
			break;
		  case WeatherArea::Land:
			theIndexMask = theSources.getLandMaskSource()->mask(theArea,dataname,*wsource);
			break;
		  case WeatherArea::Coast:
			theIndexMask = theSources.getCoastMaskSource()->mask(theArea,dataname,*wsource);
			break;
		  case WeatherArea::Inland:
			theIndexMask = theSources.getInlandMaskSource()->mask(theArea,dataname,*wsource);
			break;
		  }

		if(theIndexMask->empty())
		  return 0;
	  
		unsigned long startindex, endindex;
		
		if(!QueryDataTools::findIndices(*theQI,
										thePeriod.utcStartTime(),
										thePeriod.utcEndTime(),
										startindex,
										endindex))
		  {
			return 0;
		  }

		for(NFmiIndexMask::const_iterator it = theIndexMask->begin();
			it != theIndexMask->end();
			++it)
		  {
		  
			theQI->TimeIndex(startindex);

			do
			  {
				// possible -1 is handled by IndexFloatValue
				const unsigned long idx = theQI->Index(theQI->ParamIndex(),
													   *it,
													   theQI->LevelIndex(),
													   theQI->TimeIndex());
				const float tmp = theQI->GetFloatValue(idx);
			
				if(theAcceptor.accept(tmp))
				  {
					theResultData.push_back(new NFmiPoint(theQI->LatLon(*it)));
					retval += tmp;
				  }
			  }
			while(theQI->NextTime() && theQI->TimeIndex() < endindex);
		  }
	  }
	
	return retval;
 }

  double ExtractMask(const AnalysisSources& theSources,
					 const WeatherParameter& theParameter,
					 const WeatherArea& theArea,
					 const WeatherPeriod& thePeriod,
					 const Acceptor& theAcceptor,
					 NFmiIndexMask& theResultIndexMask)
  {
 	double retval = 0.0;

	std::string parameterName;
	std::string dataName;

	ParameterAnalyzer::getParameterStrings(theParameter, parameterName, dataName);
	const string default_forecast = Settings::optional_string("textgen::default_forecast","");
	const string datavar = dataName + '_' + data_type_name(Forecast);
	const string dataname = Settings::optional_string(datavar, default_forecast);

	// Get the data into use
	  
	shared_ptr<WeatherSource> wsource = theSources.getWeatherSource();
	shared_ptr<NFmiStreamQueryData> qd = wsource->data(dataname);
	NFmiFastQueryInfo* theQI = qd->QueryInfoIter();

	FmiParameterName param = FmiParameterName(itsConverter.ToEnum(parameterName));
	if(param == kFmiBadParameter)
	  throw WeatherAnalysisError("Parameter "+parameterName+" is not defined in newbase");
  
	if(!theQI->Param(param))
	  throw WeatherAnalysisError(parameterName+" is not available in "+dataname);

	if(!theArea.isPoint())
	  {
		MaskSource::mask_type theIndexMask;
		
		switch(theArea.type())
		  {
		  case WeatherArea::Full:
			theIndexMask = theSources.getMaskSource()->mask(theArea,dataname,*wsource);
			break;
		  case WeatherArea::Land:
			theIndexMask = theSources.getLandMaskSource()->mask(theArea,dataname,*wsource);
			break;
		  case WeatherArea::Coast:
			theIndexMask = theSources.getCoastMaskSource()->mask(theArea,dataname,*wsource);
			break;
		  case WeatherArea::Inland:
			theIndexMask = theSources.getInlandMaskSource()->mask(theArea,dataname,*wsource);
			break;
		  }

		if(theIndexMask->empty())
		  return 0;
	  
		unsigned long startindex, endindex;
		
		if(!QueryDataTools::findIndices(*theQI,
										thePeriod.utcStartTime(),
										thePeriod.utcEndTime(),
										startindex,
										endindex))
		  return 0;
		
		for(NFmiIndexMask::const_iterator it = theIndexMask->begin();
			it != theIndexMask->end();
			++it)
		  {
			
			theQI->TimeIndex(startindex);
			
			do
			  {
				// possible -1 is handled by IndexFloatValue
				const unsigned long idx = theQI->Index(theQI->ParamIndex(),
													   *it,
													   theQI->LevelIndex(),
													   theQI->TimeIndex());
				const float tmp = theQI->GetFloatValue(idx);
				
				if(theAcceptor.accept(tmp))
				  {
					theResultIndexMask.insert(*it);
					retval += tmp;
				  }
			  }
			while(theQI->NextTime() && theQI->TimeIndex() < endindex);
		  }
	  }

	return retval;
  }

  void PrintLatLon(const AnalysisSources& theSources,
				   const WeatherParameter& theParameter,
				   const NFmiIndexMask& theIndexMask)
  {
	std::string parameterName;
	std::string dataName;

	ParameterAnalyzer::getParameterStrings(theParameter, parameterName, dataName);
	const string default_forecast = Settings::optional_string("textgen::default_forecast","");
	const string datavar = dataName + '_' + data_type_name(Forecast);
	const string dataname = Settings::optional_string(datavar, default_forecast);

	shared_ptr<WeatherSource> wsource = theSources.getWeatherSource();
	shared_ptr<NFmiStreamQueryData> qd = wsource->data(dataname);
	NFmiFastQueryInfo* theQI = qd->QueryInfoIter();

	for(NFmiIndexMask::const_iterator it = theIndexMask.begin();
		it != theIndexMask.end();
		++it)
	  {
		cout << theQI->LatLon(*it);
	  }
  }

} // namespace
