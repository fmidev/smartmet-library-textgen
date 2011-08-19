#include "WindStory.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "WindStoryTools.h"
#include "GridForecaster.h"
#include "ParameterAnalyzer.h"
#include "PositiveValueAcceptor.h"
#include "WeatherResult.h"
#include "Sentence.h"
#include "Settings.h"
#include "WeatherForecast.h"
#include "WindForecast.h"
#include <iomanip>
#include <fstream>
#include <cmath>

using namespace boost;
using namespace WeatherAnalysis;
using namespace TextGen::WindStoryTools;
using namespace std;

namespace TextGen
{

  std::string get_wind_speed_string(const wind_speed_id& theWindSpeedId)
  {
	std::string retval;

	switch(theWindSpeedId)
	  {
	  case TYYNI:
		retval = "tyyni";
		break;
	  case HEIKKO:
		retval = "heikko";
		break;
	  case KOHTALAINEN:
		retval = "kohtalainen";
		break;
	  case NAVAKKA:
		retval = "navakka";
		break;
	  case KOVA:
		retval = "kova";
		break;
	  case MYRSKY:
		retval = "myrsky";
		break;
	  case HIRMUMYRSKY:
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
	  case POHJOINEN:
		retval = "pohjoinen";
		break;
	  case POHJOISEN_PUOLEINEN:
		retval = "pohjoisen puoleinen";
		break;
	  case KOILLINEN:
		retval = "koillinen";
		break;
	  case KOILLISEN_PUOLEINEN:
		retval = "koillisen puoleinen";
		break;
	  case ITA:
		retval = "it‰";
		break;
	  case IDAN_PUOLEINEN:
		retval = "id‰n puoleinen";
		break;
	  case KAAKKO:
		retval = "kaakko";
		break;
	  case KAAKON_PUOLEINEN:
		retval = "kaakon puoleinen";
		break;
	  case ETELA:
		retval = "etel‰";
		break;
	  case ETELAN_PUOLEINEN:
		retval = "etel‰n puoleinen";
		break;
	  case LOUNAS:
		retval = "lounas";
		break;
	  case LOUNAAN_PUOLEINEN:
		retval = "lounaan puoleinen";
		break;
	  case LANSI:
		retval = "l‰nsi";
		break;
	  case LANNEN_PUOLEINEN:
		retval = "l‰nnen puoleinen";
		break;
	  case LUODE:
		retval = "luode";
		break;
	  case LUOTEEN_PUOLEINEN:
		retval = "luoteen puoleinen";
		break;
	  case VAIHTELEVA:
		retval = "vaihteleva";
		break;
	  }

	return retval;
  }

  std::string get_wind_direction_large_string(const wind_direction_large_id& theWindDirectionId)
  {
	std::string retval;

	switch(theWindDirectionId)
	  {
	  case POHJOINEN_:
		retval = "pohjoinen";
		break;
	  case POHJOINEN_KOILLINEN:
		retval = "pohjoisen ja koillisen v‰linen";
		break;
	  case KOILLINEN_:
		retval = "koillinen";
		break;
	  case KOILLINEN_ITA:
		retval = "koillisen ja id‰n v‰linen";
		break;
	  case ITA_:
		retval = "it‰";
		break;
	  case ITA_KAAKKO:
		retval = "id‰n ja kaakon v‰linen";
		break;
	  case KAAKKO_:
		retval = "kaakko";
		break;
	  case KAAKKO_ETELA:
		retval = "kaakon ja etel‰n v‰linen";
		break;
	  case ETELA_:
		retval = "etel‰";
		break;
	  case ETELA_LOUNAS:
		retval = "etel‰n ja lounaan v‰linen";
		break;
	  case LOUNAS_:
		retval = "lounas";
		break;
	  case LOUNAS_LANSI:
		retval = "lounaan ja l‰nnen v‰linen";
		break;
	  case LANSI_:
		retval = "l‰nsi";
		break;
	  case LANSI_LUODE:
		retval = "l‰nnen ja luoteen v‰linen";
		break;
	  case LUODE_:
		retval = "luode";
		break;
	  case LUODE_POHJOINEN:
		retval = "luoteen ja pohjoisen v‰linen";
		break;
	  case VAIHTELEVA_:
		retval = "vaihteleva";
		break;
	  }

	return retval;
  }

 std::string get_wind_event_string(const wind_event_id& theWindEventId)
  {
	std::string retval;

	switch(theWindEventId)
	  {
	  case TUULI_HEIKKENEE:
		retval = "tuuli heikkenee";
		break;
	  case TUULI_VOIMISTUU:
		retval = "tuuli voimistuu";
		break;
	  case TUULI_TYYNTYY:
		retval = "tuuli tyyntyy";
		break;
	  case TUULI_KAANTYY:
		retval = "tuuli k‰‰ntyy";
		break;
	  case TUULI_KAANTYY_JA_HEIKEKNEE:
		retval = "tuuli k‰‰ntyy ja heikkenee";
		break;
	  case TUULI_KAANTYY_JA_VOIMISTUU:
		retval = "tuuli k‰‰ntyy ja voimistuu";
		break;
	  case TUULI_KAANTYY_JA_TYYNTYY:
		retval = "tuuli k‰‰ntyy ja tyyntyy";
		break;
	  case TUULI_MUUTTUU_VAIHTELEVAKSI:
		retval = "tuuli muuttuu vaihtelevaksi";
		break;
	  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE:
		retval = "tuuli muuttuu vaihtelevaksi ja heikkenee";
		break;
	  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU:
		retval = "tuuli muuttuu vaihtelevaksi ja voimistuu";
		break;
	  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY:
		retval = "tuuli muuttuu vaihtelevaksi ja tyyntyy";
		break;
	  case MISSING_WIND_EVENT:
		retval = "missing wind event";
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
			  << fixed << setprecision(1) << theWindDataItem.theWindSpeedMin.value()
			  << "; max: "
			  << fixed << setprecision(1) << theWindDataItem.theWindSpeedMax.value()
			  << "; ka,k-hajonta: ("
			  << fixed << setprecision(1) << theWindDataItem.theWindSpeedMean.value()
			  << ", "
			  << fixed << setprecision(1) << theWindDataItem.theWindSpeedMean.error()
			  << ")"
			  << "; suunta "
			  << fixed << setprecision(1) << theWindDataItem.theWindDirection.value()
			  << "; suunnan k-hajonta: "
			  << fixed << setprecision(1) << theWindDataItem.theWindDirection.error();

	return theOutput;
  }

  std::ostream& operator<<(std::ostream & theOutput,
						   const WindSpeedPeriodDataItem& theWindSpeedPeriodDataItem)
  {
	theOutput << theWindSpeedPeriodDataItem.thePeriod.localStartTime()
			  << " ... "
			  << theWindSpeedPeriodDataItem.thePeriod.localEndTime()
			  << ": "
			  << get_wind_speed_string(theWindSpeedPeriodDataItem.theWindSpeedId)
			  << endl;

	return theOutput;
  }


  std::ostream& operator<<(std::ostream & theOutput,
						   const WindDirectionPeriodDataItem& theWindDirectionPeriodDataItem)
  {
	theOutput << theWindDirectionPeriodDataItem.thePeriod.localStartTime()
			  << " ... "
			  << theWindDirectionPeriodDataItem.thePeriod.localEndTime()
			  << ": "
			  << get_wind_direction_string(theWindDirectionPeriodDataItem.theWindDirection)
			  << endl;

	return theOutput;
  }

  std::ostream& operator<<(std::ostream & theOutput,
						   const WindDirectionLargePeriodDataItem& theWindDirectionPeriodDataItem)
  {
	theOutput << theWindDirectionPeriodDataItem.thePeriod.localStartTime()
			  << " ... "
			  << theWindDirectionPeriodDataItem.thePeriod.localEndTime()
			  << ": "
			  << get_wind_direction_large_string(theWindDirectionPeriodDataItem.theWindDirection)
			  << endl;

	return theOutput;
  }

  void print_wiki_table(const std::string areaName,
						const string& theVar,
						const wind_data_item_vector& theWindDataItemVector)
  {
	if(areaName.empty())
	  return;

	std::string filename("./"+areaName+"_rawdata.txt");

	ofstream output_file(filename.c_str(), ios::out);

	if(output_file.fail())
	  {
		throw std::runtime_error("wind_overview failed to open '" + filename + "' for writing");
	  }
	
	output_file << "|| aika || min || max || ka, k-hajonta || suunta || suunnan k-hajonta || fraasi ||" << endl;

	for(unsigned int i = 0; i < theWindDataItemVector.size(); i++)
	  {
		const WindDataItem& theWindDataItem = *(theWindDataItemVector[i]);
		output_file << "| "
					<< theWindDataItem.thePeriod.localEndTime()
					<< " | "
					<< fixed << setprecision(1) << theWindDataItem.theWindSpeedMin.value()
					<< " | "
					<< fixed << setprecision(1) << theWindDataItem.theWindSpeedMax.value()
					<< " | ("
					<< fixed << setprecision(1) << theWindDataItem.theWindSpeedMean.value()
					<< ", "
					<< fixed << setprecision(1) << theWindDataItem.theWindSpeedMean.error()
					<< ")"
					<< " | "
					<< fixed << setprecision(1) << theWindDataItem.theWindDirection.value()
					<< " | "
					<< fixed << setprecision(1) << theWindDataItem.theWindDirection.error()
					<< " | ";
		
		output_file << directed_speed_string(theWindDataItemVector[i]->theWindSpeedMean, 
											 theWindDataItemVector[i]->theWindDirection, 
											 theVar)
					<< " |" << endl;
	  }
  }

  void print_csv_table(const std::string& areaName,
					   const std::string& fileIdentifierString,
					   const std::string& theVar,
					   const wind_data_item_vector& theWindDataItemVector,
					   const vector<unsigned int>& theIndexVector)
  {
	if(areaName.empty())
	  return;

	std::string filename("./"+areaName+fileIdentifierString+".csv");

	ofstream output_file(filename.c_str(), ios::out);

	if(output_file.fail())
	  {
		throw std::runtime_error("wind_overview failed to open '" + filename + "' for writing");
	  }
	
	output_file << "aika, min, max, ka, nopeuden k-hajonta, suunta, suunnan k-hajonta, fraasi" << endl;

	for(unsigned int i = 0; i < theIndexVector.size(); i++)
	  {
		const unsigned int& index = theIndexVector[i];

		const WindDataItem& theWindDataItem = *(theWindDataItemVector[index]);
		output_file << theWindDataItem.thePeriod.localEndTime()
					<< ", "
					<< fixed << setprecision(1) << theWindDataItem.theWindSpeedMin.value()
					<< ", "
					<< fixed << setprecision(1) << theWindDataItem.theWindSpeedMax.value()
					<< ", "
					<< fixed << setprecision(1) << theWindDataItem.theWindSpeedMean.value()
					<< ", "
					<< fixed << setprecision(1) << theWindDataItem.theWindSpeedMean.error()
					<< ", "
					<< fixed << setprecision(1) << theWindDataItem.theWindDirection.value()
					<< ", "
					<< fixed << setprecision(1) << theWindDataItem.theWindDirection.error()
					<< ", ";
		
		output_file << directed_speed_string(theWindDataItemVector[index]->theWindSpeedMean, 
											 theWindDataItemVector[index]->theWindDirection, 
											 theVar)
					<< endl;
	  }
  }
  
  void log_raw_data_vector(wo_story_params& storyParams)
  {
	print_csv_table(storyParams.theAreaName,
					"_original",
					storyParams.theVar,
					storyParams.theRawDataVector,
					storyParams.theOriginalWindSpeedIndexes);

	print_csv_table(storyParams.theAreaName,
					"_equalized_windspeed",
					storyParams.theVar,
					storyParams.theRawDataVector,
					storyParams.theEqualizedWindSpeedIndexes);

	print_csv_table(storyParams.theAreaName,
					"_equalized_winddirection",
					storyParams.theVar,
					storyParams.theRawDataVector,
					storyParams.theEqualizedWindDirectionIndexes);

	print_wiki_table(storyParams.theAreaName,
					 storyParams.theVar,
					 storyParams.theRawDataVector);
	
	for(unsigned int i = 0; i < storyParams.theRawDataVector.size(); i++)
	  {
		storyParams.theLog << *(storyParams.theRawDataVector[i]);
		storyParams.theLog << "; fraasi: "<< directed_speed_string(storyParams.theRawDataVector[i]->theWindSpeedMean, 
																   storyParams.theRawDataVector[i]->theWindDirection, 
																   storyParams.theVar)
						   << endl;
	  }
  }

  void log_equalized_wind_speed_data_vector(wo_story_params& storyParams)
  {
	storyParams.theLog << "*********** EQUALIZED WIND SPEED DATA ***********" << endl;

	for(unsigned int i = 0; i < storyParams.theEqualizedWindSpeedIndexes.size(); i++)
	  {
		const unsigned int& index = storyParams.theEqualizedWindSpeedIndexes[i];
		storyParams.theLog << *(storyParams.theRawDataVector[index]) << endl;
	  }
  }

  void log_equalized_wind_direction_data_vector(wo_story_params& storyParams)
  {
	storyParams.theLog << "*********** EQUALIZED WIND DIRECTION DATA ***********" << endl;

	for(unsigned int i = 0; i < storyParams.theEqualizedWindDirectionIndexes.size(); i++)
	  {
		const unsigned int& index = storyParams.theEqualizedWindDirectionIndexes[i];
		storyParams.theLog << *(storyParams.theRawDataVector[index]) << endl;
	  }
  }


  void log_wind_speed_periods(wo_story_params& storyParams)
  {
	storyParams.theLog << "*********** WIND SPEED PERIODS ***********" << endl;
	for(unsigned int i = 0; i < storyParams.theWindSpeedVector.size(); i++)
	  {
		storyParams.theLog << *(storyParams.theWindSpeedVector[i]);
	  }
  }

  void log_wind_direction_periods(wo_story_params& storyParams)
  {
	storyParams.theLog << "*********** WIND DIRECTION PERIODS ***********" << endl;
	for(unsigned int i = 0; i < storyParams.theWindDirectionVector.size(); i++)
	  {
		storyParams.theLog << *(storyParams.theWindDirectionVector[i]);
	  }

	storyParams.theLog << "*********** WIND DIRECTION LARGE PERIODS ***********" << endl;
	for(unsigned int i = 0; i < storyParams.theWindDirectionLargeVector.size(); i++)
	  {
		storyParams.theLog << *(storyParams.theWindDirectionLargeVector[i]);
	  }

  }
  void log_wind_events(wo_story_params& storyParams)
  {
	storyParams.theLog << "*********** WIND EVENTS ***********" << endl;

	unsigned int windDirectionIndex = 0;
	unsigned int windSpeedIndex = 0;

	for(unsigned int i = 0; i < storyParams.theWindEventVector.size(); i++)
	  {
		wind_event_id windEventId = storyParams.theWindEventVector[i].second;

		storyParams.theLog << storyParams.theWindEventVector[i].first 
						   << " "
						   <<  get_wind_event_string(storyParams.theWindEventVector[i].second)
						   << " ";

		if(windEventId > 0x0 && windEventId <= TUULI_TYYNTYY ||
		   windEventId >= TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE && windEventId <= TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY)
		  {
			storyParams.theLog << "(";
			storyParams.theLog << get_wind_speed_string(storyParams.theWindSpeedVector[windSpeedIndex++]->theWindSpeedId);
			storyParams.theLog <<  " -> ";
			storyParams.theLog << get_wind_speed_string(storyParams.theWindSpeedVector[windSpeedIndex]->theWindSpeedId);
			storyParams.theLog << ")" << endl;
		  }
		else if(windEventId == TUULI_KAANTYY)
		  {
			storyParams.theLog << "(";
			storyParams.theLog << get_wind_direction_large_string(storyParams.theWindDirectionLargeVector[windDirectionIndex]->theWindDirection);
			windDirectionIndex++;
			storyParams.theLog <<  " -> ";
			storyParams.theLog <<  get_wind_direction_large_string(storyParams.theWindDirectionLargeVector[windDirectionIndex]->theWindDirection);			
			storyParams.theLog << ")" << endl;
		  }
		else if(windEventId >= TUULI_KAANTYY_JA_HEIKEKNEE && windEventId <= TUULI_KAANTYY_JA_TYYNTYY)
		  {
			storyParams.theLog << "(";
			storyParams.theLog << get_wind_direction_large_string(storyParams.theWindDirectionLargeVector[windDirectionIndex]->theWindDirection);
			windDirectionIndex++;
			storyParams.theLog <<  " -> ";
			storyParams.theLog << get_wind_direction_large_string(storyParams.theWindDirectionLargeVector[windDirectionIndex]->theWindDirection);
			storyParams.theLog << "; ";
			storyParams.theLog << get_wind_speed_string(storyParams.theWindSpeedVector[windSpeedIndex++]->theWindSpeedId);
			storyParams.theLog <<  " -> ";
			storyParams.theLog <<  get_wind_speed_string(storyParams.theWindSpeedVector[windSpeedIndex]->theWindSpeedId);
			storyParams.theLog << ")" << endl;
		  }
		else
		  {
			storyParams.theLog << "" << endl;
		  }

		/*
  enum wind_event_id 
	{
	  TUULI_HEIKKENEE  = 0x1,
	  TUULI_VOIMISTUU = 0x2,
	  TUULI_TYYNTYY = 0x4,
	  TUULI_KAANTYY = 0x8,
	  TUULI_MUUTTUU_VAIHTELEVAKSI = 0x10,
	  TUULI_KAANTYY_JA_HEIKEKNEE = 0x9,
	  TUULI_KAANTYY_JA_VOIMISTUU = 0xA,
	  TUULI_KAANTYY_JA_TYYNTYY = 0xC,
	  TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE = 0x11,
	  TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU = 0x12,
	  TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY = 0x14,
	  MISSING_WIND_EVENT = 0x0
	};
		*/
	  }
  }

  void allocate_data_structures(wo_story_params& storyParams)
  {
	NFmiTime periodStartTime = storyParams.theDataPeriod.localStartTime();
	
	while(periodStartTime.IsLessThan(storyParams.theDataPeriod.localEndTime()))
	  {
		NFmiTime periodEndTime = periodStartTime;
		periodEndTime.ChangeByHours(1);
		WeatherPeriod weatherPeriod(periodEndTime, periodEndTime);
		WeatherResult minWind(kFloatMissing, kFloatMissing);
		WeatherResult maxWind(kFloatMissing, kFloatMissing);
		WeatherResult meanWind(kFloatMissing, kFloatMissing);
		WeatherResult windDirection(kFloatMissing, kFloatMissing);

		storyParams.theRawDataVector.push_back(new WindDataItem(weatherPeriod,
																minWind,
																maxWind,
																meanWind,
																windDirection));

		periodStartTime.ChangeByHours(1);
	  }
  }

  void deallocate_data_structure(wo_story_params& storyParams)
  {
	for(unsigned int i = 0; i < storyParams.theRawDataVector.size(); i++)
	  {
		delete storyParams.theRawDataVector[i];
	  }	
	storyParams.theRawDataVector.clear();

	for(unsigned int i = 0; i < storyParams.theWindSpeedVector.size(); i++)
	  {
		delete storyParams.theWindSpeedVector[i];
	  }	
	storyParams.theWindSpeedVector.clear();

	for(unsigned int i = 0; i < storyParams.theWindDirectionVector.size(); i++)
	  {
		delete storyParams.theWindDirectionVector[i];
	  }	
	storyParams.theWindDirectionVector.clear();

  }

  void populate_wind_time_series(wo_story_params& storyParams)
  {
	GridForecaster forecaster;

	for(unsigned int i = 0; i < storyParams.theRawDataVector.size(); i++)
	  {
		storyParams.theRawDataVector[i]->theWindSpeedMin =
		  forecaster.analyze(storyParams.theVar,//itsVar+"::fake::"+daystr+"::speed::minimum",
							 storyParams.theSources,
							 WindSpeed,
							 Minimum,
							 Mean,
							 storyParams.theArea,
							 storyParams.theRawDataVector[i]->thePeriod);

		storyParams.theRawDataVector[i]->theWindSpeedMax =
		  forecaster.analyze(storyParams.theVar,//itsVar+"::fake::"+daystr+"::speed::maximum",
							 storyParams.theSources,
							 WindSpeed,
							 Maximum,
							 Mean,
							 storyParams.theArea,
							 storyParams.theRawDataVector[i]->thePeriod);

		storyParams.theRawDataVector[i]->theWindSpeedMean =
		  forecaster.analyze(storyParams.theVar,//itsVar+"::fake::"+daystr+"::speed::mean",
							 storyParams.theSources,
							 WindSpeed,
							 Mean,
							 Mean,
							 storyParams.theArea,
							 storyParams.theRawDataVector[i]->thePeriod);

		storyParams.theRawDataVector[i]->theWindDirection =
		  forecaster.analyze(storyParams.theVar,//itsVar+"::fake::"+daystr+"::direction::mean",
							 storyParams.theSources,
							 WindDirection,
							 Mean,
							 Mean,
							 storyParams.theArea,
							 storyParams.theRawDataVector[i]->thePeriod);

		storyParams.theOriginalWindSpeedIndexes.push_back(i);
		storyParams.theEqualizedWindSpeedIndexes.push_back(i);
		storyParams.theOriginalWindDirectionIndexes.push_back(i);
		storyParams.theEqualizedWindDirectionIndexes.push_back(i);
	  }

  }
    
  wind_speed_id get_wind_speed_id(const WeatherResult& windSpeed)
  {
	if(windSpeed.value() < 0.5)
	  return TYYNI;
	else if(windSpeed.value() >= 0.5 && windSpeed.value() < 3.5)
	  return HEIKKO;
	else if(windSpeed.value() >= 3.5 && windSpeed.value() < 7.5)
	  return KOHTALAINEN;
	else if(windSpeed.value() >= 7.5 && windSpeed.value() < 13.5)
	  return NAVAKKA;
	else if(windSpeed.value() >= 13.5 && windSpeed.value() < 20.5)
	  return KOVA;
	else if(windSpeed.value() >= 20.5 && windSpeed.value() < 32.5)
	  return MYRSKY;
	else
	  return HIRMUMYRSKY;
  }

  wind_direction_id get_wind_direction_id(const WeatherResult& windDirection, const string& var)
  {
	WindDirectionAccuracy accuracy(direction_accuracy(windDirection.error(), var));

	if(accuracy == bad_accuracy)
	  return VAIHTELEVA;

	if(windDirection.value() >= 337.5 && windDirection.value() < 22.5)
	  return (accuracy == good_accuracy ? POHJOINEN : POHJOISEN_PUOLEINEN);
	else if(windDirection.value() >= 22.5 && windDirection.value() < 67.5)
	  return (accuracy == good_accuracy ? KOILLINEN : KOILLISEN_PUOLEINEN);
	else if(windDirection.value() >= 67.5 && windDirection.value() < 112.5)
	  return (accuracy == good_accuracy ? ITA : IDAN_PUOLEINEN);
	else if(windDirection.value() >= 112.5 && windDirection.value() < 157.5)
	  return (accuracy == good_accuracy ? KAAKKO : KAAKON_PUOLEINEN);
	else if(windDirection.value() >= 157.5 && windDirection.value() < 202.5)
	  return (accuracy == good_accuracy ? ETELA : ETELAN_PUOLEINEN);
	else if(windDirection.value() >= 202.5 && windDirection.value() < 247.5)
	  return (accuracy == good_accuracy ? LOUNAS : LOUNAAN_PUOLEINEN);
	else if(windDirection.value() >= 247.5 && windDirection.value() < 292.5)
	  return (accuracy == good_accuracy ? LANSI : LANNEN_PUOLEINEN);
	else
	  return (accuracy == good_accuracy ? LUODE : LUOTEEN_PUOLEINEN);
  }


  wind_direction_large_id get_wind_direction_large_id(const WeatherResult& windDirection, const string& var)
  {
	WindDirectionAccuracy accuracy(direction_accuracy(windDirection.error(), var));

	if(accuracy == bad_accuracy)
	  return VAIHTELEVA_;

	if(windDirection.value() >= 348.75 && windDirection.value() < 11.25)
	  return POHJOINEN_;
	else if(windDirection.value() >= 11.25 && windDirection.value() < 33.75)
	  return POHJOINEN_KOILLINEN;
	else if(windDirection.value() >= 33.75 && windDirection.value() < 56.25)
	  return KOILLINEN_;
	else if(windDirection.value() >= 56.25 && windDirection.value() < 78.75)
	  return KOILLINEN_ITA;
	else if(windDirection.value() >= 78.75 && windDirection.value() < 101.25)
	  return ITA_;
	else if(windDirection.value() >= 101.25 && windDirection.value() < 123.75)
	  return ITA_KAAKKO;
	else if(windDirection.value() >= 123.75 && windDirection.value() < 146.25)
	  return KAAKKO_;
	else if(windDirection.value() >= 146.25 && windDirection.value() < 168.75)
	  return KAAKKO_ETELA;
	else if(windDirection.value() >= 168.75 && windDirection.value() < 191.25)
	  return ETELA_;
	else if(windDirection.value() >= 191.25 && windDirection.value() < 213.75)
	  return ETELA_LOUNAS;
	else if(windDirection.value() >= 213.75 && windDirection.value() < 236.25)
	  return LOUNAS_;
	else if(windDirection.value() >= 236.25 && windDirection.value() < 258.75)
	  return LOUNAS_LANSI;
	else if(windDirection.value() >= 258.75 && windDirection.value() < 281.25)
	  return LANSI_;
	else if(windDirection.value() >= 281.25 && windDirection.value() < 303.75)
	  return LANSI_LUODE;
	else if(windDirection.value() >= 303.75 && windDirection.value() < 326.25)
	  return LUODE_;
	else if(windDirection.value() >= 326.25 && windDirection.value() < 348.75)
	  return LUODE_POHJOINEN;
	else
	  return VAIHTELEVA_;
  }

  void find_out_wind_speed_periods(wo_story_params& storyParams)
  {
	unsigned int equalizedDataIndex;

	if(storyParams.theEqualizedWindSpeedIndexes.size() == 0)
	  return;
	else if(storyParams.theEqualizedWindSpeedIndexes.size() == 1)
	  {
		equalizedDataIndex = storyParams.theEqualizedWindSpeedIndexes[0];
		storyParams.theWindSpeedVector.push_back(new WindSpeedPeriodDataItem(storyParams.theRawDataVector[equalizedDataIndex]->thePeriod,
																				   get_wind_speed_id(storyParams.theRawDataVector[equalizedDataIndex]->theWindSpeedMean)));
		return;
	  }

	unsigned int periodStartEqualizedDataIndex = storyParams.theEqualizedWindSpeedIndexes[0];
	unsigned int periodEndEqualizedDataIndex = storyParams.theEqualizedWindSpeedIndexes[0];

	wind_speed_id previous_wind_speed_id(get_wind_speed_id(storyParams.theRawDataVector[periodStartEqualizedDataIndex]->theWindSpeedMean));	  


	for(unsigned int i = 1; i < storyParams.theEqualizedWindSpeedIndexes.size(); i++)
	  {
		equalizedDataIndex = storyParams.theEqualizedWindSpeedIndexes[i];

		wind_speed_id current_wind_speed_id(get_wind_speed_id(storyParams.theRawDataVector[equalizedDataIndex]->theWindSpeedMean));
		
		if(current_wind_speed_id != previous_wind_speed_id)
		  {
			periodEndEqualizedDataIndex = storyParams.theEqualizedWindSpeedIndexes[i-1];
			
			NFmiTime periodStartTime(storyParams.theRawDataVector[periodStartEqualizedDataIndex]->thePeriod.localStartTime());
			NFmiTime periodEndTime(storyParams.theRawDataVector[equalizedDataIndex]->thePeriod.localStartTime());
			periodEndTime.ChangeByHours(-1);

			WeatherPeriod windSpeedPeriod(periodStartTime, periodEndTime);

			storyParams.theWindSpeedVector.push_back(new WindSpeedPeriodDataItem(windSpeedPeriod, previous_wind_speed_id));
			periodStartEqualizedDataIndex = equalizedDataIndex;
			previous_wind_speed_id = current_wind_speed_id;
		  }
	  }
	periodEndEqualizedDataIndex = storyParams.theRawDataVector.size() - 1;

	WeatherPeriod windSpeedPeriod(storyParams.theRawDataVector[periodStartEqualizedDataIndex]->thePeriod.localStartTime(),
									 storyParams.theRawDataVector[periodEndEqualizedDataIndex]->thePeriod.localEndTime());

	storyParams.theWindSpeedVector.push_back(new WindSpeedPeriodDataItem(windSpeedPeriod, previous_wind_speed_id));

  }
  
  void find_out_wind_direction_periods(wo_story_params& storyParams)
  {
	if(storyParams.theRawDataVector.size() == 0)
	  return;
	else if(storyParams.theRawDataVector.size() == 1)
	  {
		WeatherPeriod period(storyParams.theRawDataVector[0]->thePeriod);
		WeatherResult directionResult(storyParams.theRawDataVector[0]->theWindDirection);
		wind_direction_id directionId(get_wind_direction_id(directionResult, storyParams.theVar));
		storyParams.theWindDirectionVector.push_back(new WindDirectionPeriodDataItem(period, directionId));
		return;
	  }

	unsigned int periodStartIndex = 0;

	WeatherResult previousDirectionResult(storyParams.theRawDataVector[periodStartIndex]->theWindDirection);
	wind_direction_id previousWindDirectionId(get_wind_direction_id(previousDirectionResult, storyParams.theVar));	  
	for(unsigned int i = 1; i < storyParams.theRawDataVector.size(); i++)
	  {
		WeatherResult currentDirectionResult(storyParams.theRawDataVector[i]->theWindDirection);
		wind_direction_id currentWindDirectionId(get_wind_direction_id(currentDirectionResult, storyParams.theVar));
		
		if(currentWindDirectionId != previousWindDirectionId)
		  {
			WeatherPeriod windDirectionPeriod(storyParams.theRawDataVector[periodStartIndex]->thePeriod.localStartTime(),
											  storyParams.theRawDataVector[i-1]->thePeriod.localEndTime());
			storyParams.theWindDirectionVector.push_back(new WindDirectionPeriodDataItem(windDirectionPeriod, previousWindDirectionId));
			periodStartIndex = i;
			previousWindDirectionId = currentWindDirectionId;
		  }
	  }
	WeatherPeriod windDirectionPeriod(storyParams.theRawDataVector[periodStartIndex]->thePeriod.localStartTime(),
									  storyParams.theRawDataVector[storyParams.theRawDataVector.size() - 1]->thePeriod.localEndTime());

	storyParams.theWindDirectionVector.push_back(new WindDirectionPeriodDataItem(windDirectionPeriod, previousWindDirectionId));
  }

  void find_out_wind_direction_large_periods(wo_story_params& storyParams)
  {
	if(storyParams.theRawDataVector.size() == 0)
	  return;
	else if(storyParams.theRawDataVector.size() == 1)
	  {
		WeatherPeriod period(storyParams.theRawDataVector[0]->thePeriod);
		WeatherResult directionResult(storyParams.theRawDataVector[0]->theWindDirection);
		wind_direction_large_id directionId(get_wind_direction_large_id(directionResult, storyParams.theVar));
		storyParams.theWindDirectionLargeVector.push_back(new WindDirectionLargePeriodDataItem(period, directionId));
		return;
	  }

	unsigned int periodStartIndex = 0;

	WeatherResult previousDirectionResult(storyParams.theRawDataVector[periodStartIndex]->theWindDirection);
	wind_direction_large_id previousWindDirectionId(get_wind_direction_large_id(previousDirectionResult, storyParams.theVar));	  
	for(unsigned int i = 1; i < storyParams.theRawDataVector.size(); i++)
	  {
		WeatherResult currentDirectionResult(storyParams.theRawDataVector[i]->theWindDirection);
		wind_direction_large_id currentWindDirectionId(get_wind_direction_large_id(currentDirectionResult, storyParams.theVar));
		
		if(currentWindDirectionId != previousWindDirectionId)
		  {
			WeatherPeriod windDirectionPeriod(storyParams.theRawDataVector[periodStartIndex]->thePeriod.localStartTime(),
											  storyParams.theRawDataVector[i-1]->thePeriod.localEndTime());
			storyParams.theWindDirectionLargeVector.push_back(new WindDirectionLargePeriodDataItem(windDirectionPeriod, previousWindDirectionId));
			periodStartIndex = i;
			previousWindDirectionId = currentWindDirectionId;
		  }
	  }
	WeatherPeriod windDirectionPeriod(storyParams.theRawDataVector[periodStartIndex]->thePeriod.localStartTime(),
									  storyParams.theRawDataVector[storyParams.theRawDataVector.size() - 1]->thePeriod.localEndTime());

	storyParams.theWindDirectionLargeVector.push_back(new WindDirectionLargePeriodDataItem(windDirectionPeriod, previousWindDirectionId));
  }

  wind_event_id merge_wind_events(const wind_event_id& speedEvent, const wind_event_id& directionEvent)
  {
	
	return static_cast<wind_event_id>(speedEvent + directionEvent);
  }


  bool wind_event_sort(const timestamp_wind_event_id_pair& first, const timestamp_wind_event_id_pair& second) 
  { 
	return (first.first < second.first);
  }

  void find_out_wind_events(wo_story_params& storyParams)
  {
	if(storyParams.theRawDataVector.size() == 0)
	  return;

	wind_event_id windEventId(MISSING_WIND_EVENT);

	for(unsigned int i = 1; i < storyParams.theWindSpeedVector.size(); i++)
	  {
		if(storyParams.theWindSpeedVector[i]->theWindSpeedId == TYYNI)
		  windEventId = TUULI_TYYNTYY;
		else if(storyParams.theWindSpeedVector[i]->theWindSpeedId > 
				storyParams.theWindSpeedVector[i-1]->theWindSpeedId)
		  windEventId = TUULI_VOIMISTUU;
		else
		  windEventId = TUULI_HEIKKENEE;

		  
		storyParams.theWindEventVector.push_back(make_pair(storyParams.theWindSpeedVector[i]->thePeriod.localStartTime(), 
														   windEventId));
	  }

	wind_direction_large_id wind_dir_id(VAIHTELEVA_);

	for(unsigned int i = 1; i < storyParams.theWindDirectionLargeVector.size(); i++)
	  {
		bool simultaneousEventsOccurred(false);
		wind_dir_id = storyParams.theWindDirectionLargeVector[i]->theWindDirection;

		// check if event with same timestamp exists
		for(unsigned int k = 0; k < storyParams.theWindEventVector.size(); k++)
		  {
			if(storyParams.theWindEventVector[k].first == 
			   storyParams.theWindDirectionLargeVector[i]->thePeriod.localStartTime())
			  {
				
				storyParams.theWindEventVector[k].second = static_cast<wind_event_id>(storyParams.theWindEventVector[k].second +
																	   (wind_dir_id == VAIHTELEVA_ ? TUULI_MUUTTUU_VAIHTELEVAKSI : TUULI_KAANTYY));
				simultaneousEventsOccurred = true;
				break;
			  }
		  }

		if(simultaneousEventsOccurred)
		  continue;

		storyParams.theWindEventVector.push_back(make_pair(storyParams.theWindDirectionLargeVector[i]->thePeriod.localStartTime(), 
														   wind_dir_id == VAIHTELEVA_ ? TUULI_MUUTTUU_VAIHTELEVAKSI : TUULI_KAANTYY));
	  }

	/*
  enum wind_event_id 
	{
	  TUULI_HEIKKENEE,
	  TUULI_VOIMISTUU,
	  TUULI_TYYNTYY,
	  TUULI_KAANTYY,
	  TUULI_KAANTYY_JA_HEIKEKNEE,
	  TUULI_KAANTYY_JA_VOIMISTUU,
	  TUULI_MUUTTUU_VAIHTELEVAKSI,
	  TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE,
	  TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU,
	  MISSING_WIND_EVENT
	};
	 */

	std::sort(storyParams.theWindEventVector.begin(), storyParams.theWindEventVector.end(), wind_event_sort);

  }

  double distance_from_line1(const NFmiPoint& point,  const NFmiPoint& lineBeg, const NFmiPoint& lineEnd)
  {
	double slope = (lineEnd.Y() - lineBeg.Y()) / (lineEnd.X() - lineBeg.X());
	double c = lineBeg.Y() - (slope * lineBeg.X());
	double numerator = fabs((slope * point.X())+(-1.0 * point.Y()) + c);
	double denominator = sqrt(pow(slope,2.0) + 1);
	double distance = numerator / denominator;

 	return distance;
  }
  
  double distance_from_line2(const NFmiPoint& point,  const NFmiPoint& lineBeg, const NFmiPoint& lineEnd)
  {
	double slope = (lineEnd.Y() - lineBeg.Y()) / (lineEnd.X() - lineBeg.X());
	double c = lineBeg.Y() - (slope * lineBeg.X());
	double missingX = (point.Y() - c) / slope;
	double missingY = (slope * point.X()) + c;
	double angle = atan(fabs(missingY - point.Y()) / fabs(point.X() - missingX));
	double distance = sin(angle) * fabs(( point.X() - missingX));

 	return distance;
  }

  double distance_from_line3(const NFmiPoint& point,  const NFmiPoint& lineBeg, const NFmiPoint& lineEnd)
  {
	double side1Len = sqrt(pow(fabs(point.X()-lineEnd.X()),2)+ pow(fabs(point.Y() - lineEnd.Y()),2));
	double side2Len = sqrt(pow(fabs(point.X()-lineBeg.X()),2)+ pow(fabs(point.Y() - lineBeg.Y()),2));
	double baseLen = sqrt(pow(fabs(lineEnd.X()-lineBeg.X()),2)+ pow(fabs(lineEnd.Y() - lineBeg.Y()),2));
	double s = (side1Len + side2Len + baseLen) / 2.0;
	double A = sqrt(s*(s-side1Len)*(s-side2Len)*(s-baseLen));
	double distance = A/(0.5*baseLen);

 	return distance;
  }

  double distance_from_line4(const NFmiPoint& point,  const NFmiPoint& lineBeg, const NFmiPoint& lineEnd)
  {
	double slopeOfLine1 = (lineEnd.Y() - lineBeg.Y()) / (lineEnd.X() - lineBeg.X());
	double constantOfLine1 = lineBeg.Y() - (slopeOfLine1 * lineBeg.X());
	double slopeOfLine2 = -1.0/slopeOfLine1;
	double constantOfLine2 = point.Y() - (slopeOfLine2 * point.X());

	double missingX = (constantOfLine2 - constantOfLine1) / (slopeOfLine1 - slopeOfLine2);
	double missingY = slopeOfLine1 * missingX + constantOfLine1;

	double distance = sqrt(pow(point.X() - missingX, 2.0) + pow(point.Y() - missingY, 2.0));

 	return distance;
  }

  double distance_from_line(const NFmiPoint& point,  const NFmiPoint& lineBeg, const NFmiPoint& lineEnd)
  {

	double distance = distance_from_line3(point, lineBeg, lineEnd);

	return distance;
  }

  void calculate_equalized_wind_speed_indexes(wo_story_params& storyParams)
  {
	unsigned int index1, index2, index3;
	while(1)
	  {
		double minError = UINT_MAX;
		unsigned int minErrorIndex = UINT_MAX;

		for(unsigned int i = 0; i < storyParams.theEqualizedWindSpeedIndexes.size() - 3; i++)
		  {	
			index1 = storyParams.theEqualizedWindSpeedIndexes[i];
			index2 = storyParams.theEqualizedWindSpeedIndexes[i+1];
			index3 = storyParams.theEqualizedWindSpeedIndexes[i+2];

			double lineBegX = index1;
			double lineBegY = storyParams.theRawDataVector[index1]->theWindSpeedMean.value();
			double lineEndX = index3;
			double lineEndY = storyParams.theRawDataVector[index3]->theWindSpeedMean.value();
			double pointX = index2;
			double pointY = storyParams.theRawDataVector[index2]->theWindSpeedMean.value();
			NFmiPoint point(pointX, pointY);
			NFmiPoint lineBegPoint(lineBegX, lineBegY);
			NFmiPoint lineEndPoint(lineEndX, lineEndY);
			double deviation_from_line = distance_from_line(point, lineBegPoint, lineEndPoint);
			if(deviation_from_line < minError)
			  {
				minError = deviation_from_line;
				minErrorIndex = i+1;
			  }
		  }

		if(minError > storyParams.theMaxError)
		  {
			return;
		  }
		
		storyParams.theEqualizedWindSpeedIndexes.erase(storyParams.theEqualizedWindSpeedIndexes.begin()+minErrorIndex);

	  }

	/*
	while(1)
	  {
		double minError = 1000000.0;
		
		list<unsigned int>::iterator minErrorIndex;	
		list<unsigned int>::iterator it = storyParams.theEqualizedWindSpeedIndexes.begin();
		list<unsigned int>::iterator it1 = storyParams.theEqualizedWindSpeedIndexes.begin();
		list<unsigned int>::iterator it2 = storyParams.theEqualizedWindSpeedIndexes.begin();
		it1++;
		it2++;
		it2++;

		for(it = storyParams.theEqualizedWindSpeedIndexes.begin();
			it != storyParams.theEqualizedWindSpeedIndexes.end() &&
			  it1 != storyParams.theEqualizedWindSpeedIndexes.end() &&
			  it2 != storyParams.theEqualizedWindSpeedIndexes.end(); 
			it++, it1++, it2++)
		  {
			double lineBegX = *it;
			double lineBegY = storyParams.theRawDataVector[*it]->theWindSpeedMean.value();
			double lineEndX = *(it2);
			double lineEndY = storyParams.theRawDataVector[*it2]->theWindSpeedMean.value();
			double pointX = *it1;
			double pointY = storyParams.theRawDataVector[*it1]->theWindSpeedMean.value();
			NFmiPoint point(pointX, pointY);
			NFmiPoint lineBegPoint(lineBegX, lineBegY);
			NFmiPoint lineEndPoint(lineEndX, lineEndY);
			double deviation_from_line = distance_from_line(point, lineBegPoint, lineEndPoint);
			if(deviation_from_line < minError)
			  {
				minError = deviation_from_line;
				minErrorIndex = it1;
			  }
		  }

		if(minError > storyParams.theMaxError)
		  {
			return;
		  }

		storyParams.theEqualizedWindSpeedIndexes.erase(minErrorIndex);
	  }
	*/
  }

  void calculate_equalized_wind_direction_indexes(wo_story_params& storyParams)
  {
	unsigned int index1, index2, index3;

	while(1)
	  {
		double minError = UINT_MAX;
		unsigned int minErrorIndex = UINT_MAX;

		for(unsigned int i = 0; i < storyParams.theEqualizedWindDirectionIndexes.size() - 3; i++)
		  {
			index1 = storyParams.theEqualizedWindSpeedIndexes[i];
			index2 = storyParams.theEqualizedWindSpeedIndexes[i+1];
			index3 = storyParams.theEqualizedWindSpeedIndexes[i+2];

			double lineBegX = index1;
			double lineBegY = storyParams.theRawDataVector[index1]->theWindDirection.value();
			double lineEndX = index3;
			double lineEndY = storyParams.theRawDataVector[index3]->theWindDirection.value();
			double pointX = index2;
			double pointY = storyParams.theRawDataVector[index2]->theWindDirection.value();

			/*
			double lineBegX = storyParams.theEqualizedWindDirectionIndexes[i];
			double lineBegY = storyParams.theRawDataVector[i]->theWindDirection.value();
			double lineEndX = storyParams.theEqualizedWindDirectionIndexes[i+2];
			double lineEndY = storyParams.theRawDataVector[i+2]->theWindDirection.value();
			double pointX = storyParams.theEqualizedWindDirectionIndexes[i+1];
			double pointY = storyParams.theRawDataVector[i+1]->theWindDirection.value();
			*/
			NFmiPoint point(pointX, pointY);
			NFmiPoint lineBegPoint(lineBegX, lineBegY);
			NFmiPoint lineEndPoint(lineEndX, lineEndY);
			double deviation_from_line = distance_from_line(point, lineBegPoint, lineEndPoint);
			if(deviation_from_line < minError)
			  {
				minError = deviation_from_line;
				minErrorIndex = i+1;
			  }
		  }

		if(minError > storyParams.theMaxError)
		  {
			return;
		  }		

		storyParams.theEqualizedWindDirectionIndexes.erase(storyParams.theEqualizedWindDirectionIndexes.begin()+minErrorIndex);
	  }

	/*
	while(1)
	  {
		double minError = 1000000.0;
		
		list<unsigned int>::iterator minErrorIndex;	
		list<unsigned int>::iterator it = storyParams.theEqualizedWindDirectionIndexes.begin();
		list<unsigned int>::iterator it1 = storyParams.theEqualizedWindDirectionIndexes.begin();
		list<unsigned int>::iterator it2 = storyParams.theEqualizedWindDirectionIndexes.begin();
		it1++;
		it2++;
		it2++;

		for(it = storyParams.theEqualizedWindDirectionIndexes.begin();
			it != storyParams.theEqualizedWindDirectionIndexes.end() &&
			  it1 != storyParams.theEqualizedWindDirectionIndexes.end() &&
			  it2 != storyParams.theEqualizedWindDirectionIndexes.end(); 
			it++, it1++, it2++)
		  {
			double lineBegX = *it;
			double lineBegY = storyParams.theRawDataVector[*it]->theWindDirection.value();
			double lineEndX = *(it2);
			double lineEndY = storyParams.theRawDataVector[*it2]->theWindDirection.value();
			double pointX = *it1;
			double pointY = storyParams.theRawDataVector[*it1]->theWindDirection.value();
			NFmiPoint point(pointX, pointY);
			NFmiPoint lineBegPoint(lineBegX, lineBegY);
			NFmiPoint lineEndPoint(lineEndX, lineEndY);
			double deviation_from_line = distance_from_line(point, lineBegPoint, lineEndPoint);
			if(deviation_from_line < minError)
			  {
				minError = deviation_from_line;
				minErrorIndex = it1;
			  }
		  }

		if(minError > storyParams.theMaxError)
		  {
			return;
		  }
		storyParams.theEqualizedWindDirectionIndexes.erase(minErrorIndex);
	  }
	*/
  }

  void calculate_equalized_data(wo_story_params& storyParams)
  {
	// first calculate the indexes
	if(storyParams.theEqualizedWindSpeedIndexes.size() > 3)
	  calculate_equalized_wind_speed_indexes(storyParams);
	if(storyParams.theEqualizedWindDirectionIndexes.size() > 3)
	  calculate_equalized_wind_direction_indexes(storyParams);

	// then calculate 
	//	claculate_equalized_wind_data(storyParams);


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
	//	cout << "** " << areaName << " **" << endl;

    // Generate the story
    //
	Paragraph paragraph;

	double maxError = Settings::optional_double(itsVar+"::max_error", 0.20);

	wo_story_params storyParams(itsVar,
								areaName,
								itsArea,
								itsPeriod,
								itsPeriod,
								itsForecastTime,
								itsSources,
								maxError,
								logger);

	allocate_data_structures(storyParams);

	populate_wind_time_series(storyParams);
	
	calculate_equalized_data(storyParams);

	find_out_wind_speed_periods(storyParams);

	find_out_wind_direction_periods(storyParams);

	find_out_wind_direction_large_periods(storyParams);

	find_out_wind_events(storyParams);

	log_raw_data_vector(storyParams);

	log_equalized_wind_speed_data_vector(storyParams);

	log_equalized_wind_direction_data_vector(storyParams);

	log_wind_speed_periods(storyParams);

	log_wind_direction_periods(storyParams);

	log_wind_events(storyParams);

	WindForecast windForecast(storyParams);

	paragraph << windForecast.windSentence(itsPeriod);


	deallocate_data_structure(storyParams);

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
  ["tyynt‰"]["tyynt‰"]= f_tuuli_on_heikkoa,
  ["tyynt‰"]["heikkoa"]= f_tuuli_on_heikkoa,
  ["heikkoa"]["tyynt‰"]= f_tuuli_on_heikkoa,
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
