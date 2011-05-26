#include "WindStory.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "WeatherPeriod.h"
#include "WeatherResult.h"
#include "WindStoryTools.h"
#include "GridForecaster.h"

using namespace WeatherAnalysis;
using namespace TextGen::WindStoryTools;
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

  const Paragraph WindStory::overview() const
  {
	MessageLogger log("WeatherStory::overview");

    // Generate the story
    //
	Paragraph paragraph;

	wind_data_item_vector resultVector;
  

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
