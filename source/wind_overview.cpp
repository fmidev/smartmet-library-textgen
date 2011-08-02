#include "WindStory.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "WeatherPeriod.h"
#include "WeatherResult.h"
#include "WindStoryTools.h"
#include "GridForecaster.h"
#include "RegularFunctionAnalyzer.h"
#include "ParameterAnalyzer.h"
#include "QueryDataIntegrator.h"
#include "CalculatorFactory.h"
#include "SumCalculator.h"
#include "MaximumCalculator.h"
#include "MinimumCalculator.h"
#include "MeanCalculator.h"
#include "PositiveValueAcceptor.h"
#include <boost/lexical_cast.hpp>
#include <fstream>

#include "SubMaskExtractor.h"
#include "MaskSource.h"
#include "Settings.h"
#include "WeatherAnalysisError.h"
#include "WeatherResult.h"
#include "WeatherSource.h"
#include "QueryDataTools.h"
#include "StandardDeviationCalculator.h"

#include <newbase/NFmiStreamQueryData.h>
#include <newbase/NFmiNearTree.h>
#include <newbase/NFmiEnumConverter.h>
#include <newbase/NFmiFastQueryInfo.h>
#include <newbase/NFmiQueryData.h>
#include <newbase/NFmiFastQueryInfo.h>
#include <newbase/NFmiIndexMask.h>
#include <newbase/NFmiIndexMaskSource.h>
#include <newbase/NFmiTime.h>
#include <newbase/NFmiMetMath.h>
#include <newbase/NFmiSvgTools.h>
#include <cassert>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <sstream>

using namespace boost;
using namespace WeatherAnalysis;
using namespace TextGen::WindStoryTools;
using namespace WeatherAnalysis::QueryDataIntegrator;
using namespace std;

namespace TextGen
{
  struct WindDataItem
  {
	WindDataItem(const WeatherPeriod& period, 
				 const WeatherResult& windspeedMin, 
				 const WeatherResult& windspeedMax, 
				 const WeatherResult& windspeedMean,
				 const WeatherResult& windDirection,
				 const WindDirectionAccuracy& directionAccuracy) 
	  : thePeriod(period),
		theWindspeedMin(windspeedMin),
		theWindspeedMax(windspeedMax),
		theWindspeedMean(windspeedMean),
		theWindDirection(windDirection),
		theDirectionAccuracy(directionAccuracy)
	{}
	
	WeatherPeriod thePeriod;
	WeatherResult theWindspeedMin;
	WeatherResult theWindspeedMax;
	WeatherResult theWindspeedMean;
	WeatherResult theWindDirection;
	WindDirectionAccuracy theDirectionAccuracy;
  };

  std::ostream& operator<<(std::ostream & theOutput,
						   const WindDataItem& theWindDataItem)
  {
	/*
	const WeatherResult windspeedMax(theWindDataItem.windspeedMax);
	const WeatherResult windspeedMin(theWindDataItem.windspeedMin);
	const WeatherResult windspeedMean(theWindDataItem.windspeedMean);
	*/
	theOutput << theWindDataItem.thePeriod.localStartTime()
			  << " ... "
			  << theWindDataItem.thePeriod.localEndTime()
			  << ": ("
			  << theWindDataItem.theWindspeedMin.value()
			  << ", "
			  << theWindDataItem.theWindspeedMin.error()
			  << ")"
			  << "("
			  << theWindDataItem.theWindspeedMax.value()
			  << ", "
			  << theWindDataItem.theWindspeedMax.error()
			  << ")"
			  << "("
			  << theWindDataItem.theWindspeedMean.value()
			  << ", "
			  << theWindDataItem.theWindspeedMean.error()
			  << ")"
			  << endl;
	/*
	theOutput << theWindDataItem.period.localStartTime()
			  << " ... "
			  << theWindDataItem.period.localEndTime()
			  << ": "
			  << theWindDataItem.windspeedMin.value()
			  << ','
			  << theWindDataItem.windspeedMin.error()
			  << ')'
			  << endl;
	*/

	return theOutput;
  }

  typedef vector<WindDataItem*> wind_data_item_vector;


void allocate_data_structures(const WeatherPeriod& thePeriod, 
							  wind_data_item_vector& theResultVector)
{
	NFmiTime periodStartTime = thePeriod.localStartTime();

	while(periodStartTime.IsLessThan(thePeriod.localEndTime()))
	  {
		NFmiTime periodEndTime = periodStartTime;
		periodEndTime.ChangeByHours(1);
		WeatherPeriod weatherPeriod(periodEndTime, periodEndTime);
		WeatherResult minWind(kFloatMissing, kFloatMissing);
		WeatherResult maxWind(kFloatMissing, kFloatMissing);
		WeatherResult meanWind(kFloatMissing, kFloatMissing);
		WeatherResult windDirection(kFloatMissing, kFloatMissing);

		theResultVector.push_back(new WindDataItem(weatherPeriod,
												   minWind,
												   maxWind,
												   meanWind,
												   windDirection,
												   bad_accuracy));

		periodStartTime.ChangeByHours(1);		
	  }
}

  void deallocate_data_structure(wind_data_item_vector& theResultVector)
  {
	/*
	for(unsigned int i = 0; i < theResultVector.size(); i++)
	  {
		delete theResultVector[i];
	  }
	*/
	theResultVector.clear();
  }

void populate_wind_time_series(const string& theVar, 
							   const AnalysisSources& theSources,
							   const WeatherArea& theArea,	
							   wind_data_item_vector& theResultVector)
{
  GridForecaster forecaster;

  for(unsigned int i = 0; i < theResultVector.size(); i++)
	{
		const WeatherResult minspeed =
		  forecaster.analyze(theVar,//itsVar+"::fake::"+daystr+"::speed::minimum",
							 theSources,
							 WindSpeed,
							 Minimum,
							 Mean,
							 theArea,
							 theResultVector[i]->thePeriod);

		const WeatherResult maxspeed =
		  forecaster.analyze(theVar,//itsVar+"::fake::"+daystr+"::speed::maximum",
							 theSources,
							 WindSpeed,
							 Maximum,
							 Mean,
							 theArea,
							 theResultVector[i]->thePeriod);

		const WeatherResult meanspeed =
		  forecaster.analyze(theVar,//itsVar+"::fake::"+daystr+"::speed::mean",
							 theSources,
							 WindSpeed,
							 Mean,
							 Mean,
							 theArea,
							 theResultVector[i]->thePeriod);

		const WeatherResult direction =
		  forecaster.analyze(theVar,//itsVar+"::fake::"+daystr+"::direction::mean",
							 theSources,
							 WindDirection,
							 Mean,
							 Mean,
							 theArea,
							 theResultVector[i]->thePeriod);

		theResultVector[i]->theWindspeedMin = minspeed;
		theResultVector[i]->theWindspeedMax = maxspeed;
		theResultVector[i]->theWindspeedMean = meanspeed;
		theResultVector[i]->theWindDirection = direction;
		theResultVector[i]->theDirectionAccuracy = direction_accuracy(direction.error(),theVar);
	}
}

float calculate_standard_deviation(const WeatherParameter& theParameter,
								  const WeatherPeriod& thePeriod,
								  const AnalysisSources& theSources,
								  const WeatherArea& theArea)
{
	std::string parameterName;
	std::string dataName;

	ParameterAnalyzer::getParameterStrings(theParameter, parameterName, dataName);
	const string default_forecast = Settings::optional_string("textgen::default_forecast","");
	const string datavar = dataName + "_forecast";
	const string dataname = Settings::optional_string(datavar, default_forecast);

	// Get the data into use
	  
	shared_ptr<WeatherSource> wsource = theSources.getWeatherSource();
	shared_ptr<NFmiStreamQueryData> qd = wsource->data(dataname);
	//	const NFmiGrid* theGrid = qd->QueryInfoIter()->Grid();

	NFmiFastQueryInfo* q = qd->QueryInfoIter();
	NFmiIndexMask areaMask;
	PositiveValueAcceptor acceptor;
	ExtractMask(theSources,
				WindSpeed,
				theArea,
				thePeriod,
				acceptor,
				areaMask);

	StandardDeviationCalculator spaceCalc;
	StandardDeviationCalculator timeCalc;

	float retval = Integrate(*q, 
					 areaMask, 
					 spaceCalc,
					 thePeriod.localStartTime(), 
					 thePeriod.localEndTime(), 
					 timeCalc);

	//	cout << spaceCalc() << endl;
	//	cout << timeCalc() << endl;

	return retval;
}


  const Paragraph WindStory::overview() const
  {
	MessageLogger log("WeatherStory::overview");

	std::string areaName("");
	if(itsArea.isNamed())
	  {
		areaName = itsArea.name();
		log << "** " << areaName << " **" << endl;
	  }

    // Generate the story
    //
	Paragraph paragraph;

	wind_data_item_vector theResultVector;
	
	allocate_data_structures(itsPeriod, 
							 theResultVector);


	populate_wind_time_series(itsVar, 
							  itsSources,
							  itsArea,	
							  theResultVector);

	for(unsigned int i = 0; i < theResultVector.size(); i++)
	  {
		log << *(theResultVector[i]);
	  }
	log << "standard deviation: " 
		<< calculate_standard_deviation(WindSpeed,
										itsPeriod,
										itsSources,
										itsArea)
		<< endl;
	
	deallocate_data_structure(theResultVector);

	log << paragraph;
	return paragraph;
  }
} // namespace TextGen
  
// ======================================================================







#ifdef OLD_STUFF

// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::WindStory::overview
 *
 * AKa 20-Apr-2009: Taking over this; going to implement logic received
 *      from Mika H. as email (Excel sheet).
 */
// ======================================================================

#include "WindStory.h"
#include "MessageLogger.h"
#include "Paragraph.h"


using namespace WeatherAnalysis;
using namespace std;

#if 0

/*
* Function 
*/
Sentence f_navakka_tyyni( TuulenSuunta alku, TuulenSuunta loppu ) {
    Sentence s;

    (void)alku; (void)loppu;
    
    // "Navakka xxxpuoleinen tuuli heikkenee"
    s << "Navakka" << alku << "tuuli" << "heikkenee";

    return s;
}

/*
* Taulukko, jonka perusteella teksti syntyy.
*/
    // [aluksi][lopuksi]
table= {
    ["tyyntä"]["tyyntä"]= f_tuuli_on_heikkoa,
    ["tyyntä"]["heikkoa"]= f_tuuli_on_heikkoa,
    ["heikkoa"]["tyyntä"]= f_tuuli_on_heikkoa,
    ...
}

#endif

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Return wind overview story
   *
   * \return Paragraph containing the story
   */
  // ----------------------------------------------------------------------

  const Paragraph WindStory::overview() const
  {
	MessageLogger log("WeatherStory::overview");

    // Generate the story
    //
	Paragraph paragraph;

#if 0
	GridForecaster forecaster;

    // TBD: Jotain forecasterista
    
    TuulenSuunta ts_alku, ts_loppu;
    TuulenVoima tv_alku, tv_loppu;

    Sentence sent;

    if (ts_alku == ts_loppu) {
        sent= Tuuli_ei_kaanny( ts_alku, tv_alku, tv_loppu );
    } else {
        sent= Tuuli_kaantyy( ts_alku, ts_loppu, tv_alku, tv_loppu );
    }
    
	// log << "WindSpeed Minimum(Mean)  = " << minresult << endl;

	//Sentence sentence;
	//sentence << WindStoryTools::directed_speed_sentence(minresult,
	//													maxresult,
	//													meanresult,
	//													dirresult,
	//													itsVar);
	//paragraph << sentence;

#endif
	log << paragraph;
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================

#endif
