#include "WindStory.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "WindStoryTools.h"
#include "GridForecaster.h"
#include "ParameterAnalyzer.h"
#include "PositiveValueAcceptor.h"
#include "WeatherResult.h"
#include "Settings.h"
#include <iomanip>
#include <fstream>

using namespace boost;
using namespace WeatherAnalysis;
using namespace TextGen::WindStoryTools;
using namespace std;

namespace TextGen
{
  enum wind_strength_id
	{
	  tyyni,        // ...0.5
	  heikko,       // 0.5...3.5
	  kohtalainen,  // 3.5...7.5
	  navakka,      // 7.5...13.5
	  kova,         // 13.5...20.5
	  myrsky,       // 20.5...32.5
	  hirmumyrsky   // 32.5...
	};

  enum wind_direction_id
	{
	  pohjoinen,
	  koillinen,
	  ita,
	  kaakko,
	  etela,
	  lounas,
	  lansi,
	  luode
	};

  struct WindDataItem
  {
	WindDataItem(const WeatherPeriod& period, 
				 const WeatherResult& windspeedMin, 
				 const WeatherResult& windspeedMax, 
				 const WeatherResult& windspeedMean,
				 const WeatherResult& windDirection)
	  : thePeriod(period),
		theWindspeedMin(windspeedMin),
		theWindspeedMax(windspeedMax),
		theWindspeedMean(windspeedMean),
		theWindDirection(windDirection)
	{}
	
	WeatherPeriod thePeriod;
	WeatherResult theWindspeedMin;
	WeatherResult theWindspeedMax;
	WeatherResult theWindspeedMean;
	WeatherResult theWindDirection;
  };

  struct WindStrengthDataItem
  {
	WindStrengthDataItem(const WeatherPeriod& period,
						 const wind_strength_id& windstrength)
	  : thePeriod(period),
		theWindStrength(windstrength)
	{}
	WeatherPeriod thePeriod;
	wind_strength_id theWindStrength;
  };

  struct WindDirectionDataItem
  {
	WindDirectionDataItem(const WeatherPeriod& period,
						  const wind_direction_id& winddirection)
	  : thePeriod(period),
		theWindDirection(winddirection)
	{}
	WeatherPeriod thePeriod;
	wind_direction_id theWindDirection;
  };

  typedef vector<WindDataItem*> wind_data_item_vector;
  typedef vector<WindStrengthDataItem*> wind_strength_data_item_vector;
  typedef vector<WindDirectionDataItem*> wind_direction_data_item_vector;

  std::string get_wind_strength_string(const wind_strength_id& theWindStrengthId)
  {
	std::string retval;

	switch(theWindStrengthId)
	  {
	  case tyyni:
		retval = "tyyni";
		break;
	  case heikko:
		retval = "heikko";
		break;
	  case kohtalainen:
		retval = "kohtalainen";
		break;
	  case navakka:
		retval = "navakka";
		break;
	  case kova:
		retval = "kova";
		break;
	  case myrsky:
		retval = "myrsky";
		break;
	  case hirmumyrsky:
		retval = "hirmumyrsky";
		break;
	  }

	return retval;
  }

   std::string get_wind_direction_string(const wind_direction_id& theWindDirectionId)
  {
	std::string retval;

	switch(theWindDirectionId)
	  {
	  case pohjoinen:
		retval = "pohjoinen";
		break;
	  case koillinen:
		retval = "koillinen";
		break;
	  case ita:
		retval = "itä";
		break;
	  case kaakko:
		retval = "kaakko";
		break;
	  case etela:
		retval = "etelä";
		break;
	  case lounas:
		retval = "lounas";
		break;
	  case lansi:
		retval = "länsi";
		break;
	  case luode:
		retval = "luode";
		break;
	  }

	return retval;
  }

 std::ostream& operator<<(std::ostream & theOutput,
						   const WindDataItem& theWindDataItem)
  {
	theOutput << theWindDataItem.thePeriod.localStartTime()
			  << " ... "
			  << theWindDataItem.thePeriod.localEndTime()
			  << ": min: "
			  << fixed << setprecision(1) << theWindDataItem.theWindspeedMin.value()
			  << "; max: "
			  << fixed << setprecision(1) << theWindDataItem.theWindspeedMax.value()
			  << "; ka,k-hajonta: ("
			  << fixed << setprecision(1) << theWindDataItem.theWindspeedMean.value()
			  << ", "
			  << fixed << setprecision(1) << theWindDataItem.theWindspeedMean.error()
			  << ")"
			  << "; suunta "
			  << fixed << setprecision(1) << theWindDataItem.theWindDirection.value()
			  << "; suunnan k-hajonta: "
			  << fixed << setprecision(1) << theWindDataItem.theWindDirection.error();

	return theOutput;
  }

  std::ostream& operator<<(std::ostream & theOutput,
						   const WindStrengthDataItem& theWindStrengthDataItem)
  {
	theOutput << theWindStrengthDataItem.thePeriod.localStartTime()
			  << " ... "
			  << theWindStrengthDataItem.thePeriod.localEndTime()
			  << ": "
			  << get_wind_strength_string(theWindStrengthDataItem.theWindStrength)
			  << endl;

	return theOutput;
  }


  std::ostream& operator<<(std::ostream & theOutput,
						   const WindDirectionDataItem& theWindDirectionDataItem)
  {
	theOutput << theWindDirectionDataItem.thePeriod.localStartTime()
			  << " ... "
			  << theWindDirectionDataItem.thePeriod.localEndTime()
			  << ": "
			  << get_wind_direction_string(theWindDirectionDataItem.theWindDirection)
			  << endl;

	return theOutput;
  }

  void print_wiki_table(const std::string areaName,
						const string& theVar,
						const wind_data_item_vector& theResultVector)
  {
	if(areaName.empty())
	  return;

	std::string filename("./"+areaName+".txt");

	ofstream output_file(filename.c_str(), ios::out);

	if(output_file.fail())
	  {
		throw std::runtime_error("wind_overview failed to open '" + filename + "' for writing");
	  }
	
	output_file << "|| aika || min || max || ka, k-hajonta || suunta || suunnan k-hajonta || fraasi ||" << endl;

	for(unsigned int i = 0; i < theResultVector.size(); i++)
	  {
		const WindDataItem& theWindDataItem = *(theResultVector[i]);
		output_file << "| "
					<< theWindDataItem.thePeriod.localEndTime()
					<< " | "
					<< fixed << setprecision(1) << theWindDataItem.theWindspeedMin.value()
					<< " | "
					<< fixed << setprecision(1) << theWindDataItem.theWindspeedMax.value()
					<< " | ("
					<< fixed << setprecision(1) << theWindDataItem.theWindspeedMean.value()
					<< ", "
					<< fixed << setprecision(1) << theWindDataItem.theWindspeedMean.error()
					<< ")"
					<< " | "
					<< fixed << setprecision(1) << theWindDataItem.theWindDirection.value()
					<< " | "
					<< fixed << setprecision(1) << theWindDataItem.theWindDirection.error()
					<< " | ";
		
		output_file << directed_speed_string(theResultVector[i]->theWindspeedMean, 
											 theResultVector[i]->theWindDirection, 
											 theVar)
					<< " |" << endl;
	  }
  }

  void print_csv_table(const std::string areaName,
					   const string& theVar,
					   const wind_data_item_vector& theResultVector)
  {
	if(areaName.empty())
	  return;

	std::string filename("./"+areaName+".csv");

	ofstream output_file(filename.c_str(), ios::out);

	if(output_file.fail())
	  {
		throw std::runtime_error("wind_overview failed to open '" + filename + "' for writing");
	  }
	
	output_file << "aika, min, max, ka, nopeuden k-hajonta, suunta, suunnan k-hajonta, fraasi" << endl;

	for(unsigned int i = 0; i < theResultVector.size(); i++)
	  {
		const WindDataItem& theWindDataItem = *(theResultVector[i]);
		output_file << theWindDataItem.thePeriod.localEndTime()
					<< ", "
					<< fixed << setprecision(1) << theWindDataItem.theWindspeedMin.value()
					<< ", "
					<< fixed << setprecision(1) << theWindDataItem.theWindspeedMax.value()
					<< ", "
					<< fixed << setprecision(1) << theWindDataItem.theWindspeedMean.value()
					<< ", "
					<< fixed << setprecision(1) << theWindDataItem.theWindspeedMean.error()
					<< ", "
					<< fixed << setprecision(1) << theWindDataItem.theWindDirection.value()
					<< ", "
					<< fixed << setprecision(1) << theWindDataItem.theWindDirection.error()
					<< ", ";
		
		output_file << directed_speed_string(theResultVector[i]->theWindspeedMean, 
											 theResultVector[i]->theWindDirection, 
											 theVar)
					<< endl;
	  }
  }
  
  void log_raw_data_vector(MessageLogger& logger,
						   const std::string areaName,
						   const string& theVar,
						   const wind_data_item_vector& theResultVector)
  {
	/*
	  print_wiki_table(areaName,
	  theVar,
	  theResultVector);
	  print_csv_table(areaName,
	  theVar,
	  theResultVector);
	*/
	
	for(unsigned int i = 0; i < theResultVector.size(); i++)
	  {
		logger << *(theResultVector[i]);
		logger << "; fraasi: "<< directed_speed_string(theResultVector[i]->theWindspeedMean, 
													   theResultVector[i]->theWindDirection, 
													   theVar)
			   << endl;
	  }
  }

  void log_wind_strength_data_vector(MessageLogger& logger,
									 const std::string areaName,
									 const string& theVar,
									 const wind_strength_data_item_vector& theWindStrengthVector)
  {
	logger << "*********** WIND STRENGTH PERIODS ***********" << endl;;
	for(unsigned int i = 0; i < theWindStrengthVector.size(); i++)
	  {
		logger << *(theWindStrengthVector[i]);
	  }
  }

  void log_wind_direction_data_vector(MessageLogger& logger,
									  const std::string areaName,
									  const string& theVar,
									  const wind_direction_data_item_vector& theWindDirectionVector)
  {
	logger << "*********** WIND DIRECTION PERIODS ***********" << endl;;
	for(unsigned int i = 0; i < theWindDirectionVector.size(); i++)
	  {
		logger << *(theWindDirectionVector[i]);
	  }
  }

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
												   windDirection));

		periodStartTime.ChangeByHours(1);		
	  }
  }

  void deallocate_data_structure(wind_data_item_vector& theResultVector,
								 wind_strength_data_item_vector& theWindStrengthVector,
								 wind_direction_data_item_vector& theWindDirectionVector)
  {
	for(unsigned int i = 0; i < theResultVector.size(); i++)
	  {
		delete theResultVector[i];
	  }	
	theResultVector.clear();

	for(unsigned int i = 0; i < theWindStrengthVector.size(); i++)
	  {
		delete theWindStrengthVector[i];
	  }	
	theWindStrengthVector.clear();

	for(unsigned int i = 0; i < theWindDirectionVector.size(); i++)
	  {
		delete theWindDirectionVector[i];
	  }	
	theWindDirectionVector.clear();

  }

  void populate_wind_time_series(const string& theVar, 
								 const AnalysisSources& theSources,
								 const WeatherArea& theArea,	
								 wind_data_item_vector& theResultVector)
  {
	GridForecaster forecaster;

	for(unsigned int i = 0; i < theResultVector.size(); i++)
	  {
		theResultVector[i]->theWindspeedMin =
		  forecaster.analyze(theVar,//itsVar+"::fake::"+daystr+"::speed::minimum",
							 theSources,
							 WindSpeed,
							 Minimum,
							 Mean,
							 theArea,
							 theResultVector[i]->thePeriod);

		theResultVector[i]->theWindspeedMax =
		  forecaster.analyze(theVar,//itsVar+"::fake::"+daystr+"::speed::maximum",
							 theSources,
							 WindSpeed,
							 Maximum,
							 Mean,
							 theArea,
							 theResultVector[i]->thePeriod);

		theResultVector[i]->theWindspeedMean =
		  forecaster.analyze(theVar,//itsVar+"::fake::"+daystr+"::speed::mean",
							 theSources,
							 WindSpeed,
							 Mean,
							 Mean,
							 theArea,
							 theResultVector[i]->thePeriod);

		theResultVector[i]->theWindDirection =
		  forecaster.analyze(theVar,//itsVar+"::fake::"+daystr+"::direction::mean",
							 theSources,
							 WindDirection,
							 Mean,
							 Mean,
							 theArea,
							 theResultVector[i]->thePeriod);

	  }
  }
    
  wind_strength_id get_wind_strength_id(const WeatherResult& windStrength)
  {
	if(windStrength.value() < 0.5)
	  return tyyni;
	else if(windStrength.value() >= 0.5 && windStrength.value() < 3.5)
	  return heikko;
	else if(windStrength.value() >= 3.5 && windStrength.value() < 7.5)
	  return kohtalainen;
	else if(windStrength.value() >= 7.5 && windStrength.value() < 13.5)
	  return navakka;
	else if(windStrength.value() >= 13.5 && windStrength.value() < 20.5)
	  return kova;
	else if(windStrength.value() >= 20.5 && windStrength.value() < 32.5)
	  return myrsky;
	else
	  return hirmumyrsky;
  }

  wind_direction_id get_wind_direction_id(const WeatherResult& windDirection)
  {
	/*
	  pohjoinen,
	  koillinen,
	  ita,
	  kaakko,
	  etela,
	  lounas,
	  lansi,
	  luode

	 */
	if(windDirection.value() >= 337.5 || windDirection.value() < 22.5)
	  return pohjoinen;
	else if(windDirection.value() >= 22.5 || windDirection.value() < 67.5)
	  return koillinen;
	else if(windDirection.value() >= 67.5 || windDirection.value() < 112.5)
	  return ita;
	else if(windDirection.value() >= 112.5 || windDirection.value() < 157.5)
	  return kaakko;
	else if(windDirection.value() >= 157.5 || windDirection.value() < 202.5)
	  return etela;
	else if(windDirection.value() >= 202.5 || windDirection.value() < 247.5)
	  return lounas;
	else if(windDirection.value() >= 247.5 || windDirection.value() < 292.5)
	  return lansi;
	else
	  return luode;

  }
  void find_out_wind_strength_periods(const wind_data_item_vector& theRawDataVector,
									  wind_strength_data_item_vector& theWindStrengthVector)
  {
	if(theRawDataVector.size() == 0)
	  return;
	else if(theRawDataVector.size() == 1)
	  {
		theWindStrengthVector.push_back(new WindStrengthDataItem(theRawDataVector[0]->thePeriod,
																 get_wind_strength_id(theRawDataVector[0]->theWindspeedMean)));
		return;
	  }

	unsigned int periodStartIndex = 0;
	wind_strength_id previous_wind_strength_id(get_wind_strength_id(theRawDataVector[periodStartIndex]->theWindspeedMean));	  
	for(unsigned int i = 1; i < theRawDataVector.size(); i++)
	  {
		wind_strength_id current_wind_strength_id(get_wind_strength_id(theRawDataVector[i]->theWindspeedMean));
		
		if(current_wind_strength_id != previous_wind_strength_id)
		  {
			WeatherPeriod windStrengthPeriod(theRawDataVector[periodStartIndex]->thePeriod.localStartTime(),
											 theRawDataVector[i-1]->thePeriod.localEndTime());
			theWindStrengthVector.push_back(new WindStrengthDataItem(windStrengthPeriod, previous_wind_strength_id));
			periodStartIndex = i;
			previous_wind_strength_id = current_wind_strength_id;
		  }
	  }
	WeatherPeriod windStrengthPeriod(theRawDataVector[periodStartIndex]->thePeriod.localStartTime(),
									 theRawDataVector[theRawDataVector.size() - 1]->thePeriod.localEndTime());

	theWindStrengthVector.push_back(new WindStrengthDataItem(windStrengthPeriod, previous_wind_strength_id));
  }
  
  void find_out_wind_direction_periods(const wind_data_item_vector& theRawDataVector,
									   wind_direction_data_item_vector& theWindDirectionVector)
  {
	if(theRawDataVector.size() == 0)
	  return;
	else if(theRawDataVector.size() == 1)
	  {
		theWindDirectionVector.push_back(new WindDirectionDataItem(theRawDataVector[0]->thePeriod,
																   get_wind_direction_id(theRawDataVector[0]->theWindDirection)));
		return;
	  }

	unsigned int periodStartIndex = 0;
	wind_direction_id previous_wind_direction_id(get_wind_direction_id(theRawDataVector[periodStartIndex]->theWindDirection));	  
	for(unsigned int i = 1; i < theRawDataVector.size(); i++)
	  {
		wind_direction_id current_wind_direction_id(get_wind_direction_id(theRawDataVector[i]->theWindDirection));
		
		if(current_wind_direction_id != previous_wind_direction_id)
		  {
			WeatherPeriod windDirectionPeriod(theRawDataVector[periodStartIndex]->thePeriod.localStartTime(),
											  theRawDataVector[i-1]->thePeriod.localEndTime());
			theWindDirectionVector.push_back(new WindDirectionDataItem(windDirectionPeriod, previous_wind_direction_id));
			periodStartIndex = i;
			previous_wind_direction_id = current_wind_direction_id;
		  }
	  }
	WeatherPeriod windDirectionPeriod(theRawDataVector[periodStartIndex]->thePeriod.localStartTime(),
									  theRawDataVector[theRawDataVector.size() - 1]->thePeriod.localEndTime());

	theWindDirectionVector.push_back(new WindDirectionDataItem(windDirectionPeriod, previous_wind_direction_id));
  }

  const Paragraph WindStory::overview() const
  {
	MessageLogger logger("WeatherStory::overview");

	std::string areaName("");
	if(itsArea.isNamed())
	  {
		areaName = itsArea.name();
		logger << "** " << areaName << " **" << endl;
	  }

    // Generate the story
    //
	Paragraph paragraph;

	wind_data_item_vector theRawDataVector;
	wind_strength_data_item_vector theWindStrengthVector;
	wind_direction_data_item_vector theWindDirectionVector;
	
	allocate_data_structures(itsPeriod, 
							 theRawDataVector);


	populate_wind_time_series(itsVar, 
							  itsSources,
							  itsArea,	
							  theRawDataVector);
	
	find_out_wind_strength_periods(theRawDataVector,
								   theWindStrengthVector);
	find_out_wind_direction_periods(theRawDataVector,
									theWindDirectionVector);

	log_raw_data_vector(logger,
						areaName,
						itsVar,
						theRawDataVector);

	log_wind_strength_data_vector(logger,
								  areaName,
								  itsVar,
								  theWindStrengthVector);
	log_wind_direction_data_vector(logger,
								   areaName,
								   itsVar,
								   theWindDirectionVector);

	deallocate_data_structure(theRawDataVector,
							  theWindStrengthVector,
							  theWindDirectionVector);

	logger << paragraph;

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
