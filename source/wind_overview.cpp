#include "WindStory.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "WindStoryTools.h"
#include "GridForecaster.h"
#include "ParameterAnalyzer.h"
#include "PositiveValueAcceptor.h"
#include "WeatherResult.h"
#include "Sentence.h"
#include "NFmiSettings.h"
#include "Settings.h"
#include "WeatherForecast.h"
#include "WindForecast.h"
#include <bitset>
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
	  case POHJOINEN_:
		retval = "pohjoinen";
		break;
	  case POHJOINEN_PUOLEINEN:
		retval = "pohjoisen puoleinen";
		break;
	  case POHJOINEN_KOILLINEN:
		retval = "pohjoisen ja koillisen v‰linen";
		break;
	  case KOILLINEN_:
		retval = "koillinen";
		break;
	  case KOILLINEN_PUOLEINEN:
		retval = "koillisen puoleinen";
		break;
	  case KOILLINEN_ITA:
		retval = "koillisen ja id‰n v‰linen";
		break;
	  case ITA_:
		retval = "it‰";
		break;
	  case ITA_PUOLEINEN:
		retval = "id‰n puoleinen";
		break;
	  case ITA_KAAKKO:
		retval = "id‰n ja kaakon v‰linen";
		break;
	  case KAAKKO_:
		retval = "kaakko";
		break;
	  case KAAKKO_PUOLEINEN:
		retval = "kaakon puoleinen";
		break;
	  case KAAKKO_ETELA:
		retval = "kaakon ja etel‰n v‰linen";
		break;
	  case ETELA_:
		retval = "etel‰";
		break;
	  case ETELA_PUOLEINEN:
		retval = "etel‰n puoleinen";
		break;
	  case ETELA_LOUNAS:
		retval = "etel‰n ja lounaan v‰linen";
		break;
	  case LOUNAS_:
		retval = "lounas";
		break;
	  case LOUNAS_PUOLEINEN:
		retval = "lounaan puoleinen";
		break;
	  case LOUNAS_LANSI:
		retval = "lounaan ja l‰nnen v‰linen";
		break;
	  case LANSI_:
		retval = "l‰nsi";
		break;
	  case LANSI_PUOLEINEN:
		retval = "l‰nnen puoleinen";
		break;
	  case LANSI_LUODE:
		retval = "l‰nnen ja luoteen v‰linen";
		break;
	  case LUODE_:
		retval = "luode";
		break;
	  case LUODE_PUOLEINEN:
		retval = "luoteen puoleinen";
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
						  const WindDataItemUnit& theWindDataItem)
  {
	theOutput << theWindDataItem.thePeriod.localStartTime()
			  << " ... "
			  << theWindDataItem.thePeriod.localEndTime()
			  << ": min: "
			  << fixed << setprecision(4) << theWindDataItem.theWindSpeedMin.value()
			  << "; max: "
			  << fixed << setprecision(4) << theWindDataItem.theWindSpeedMax.value()
			  << "; ka,k-hajonta: ("
			  << fixed << setprecision(4) << theWindDataItem.theWindSpeedMean.value()
			  << ", "
			  << fixed << setprecision(4) << theWindDataItem.theWindSpeedMean.error()
			  << ")"
			  << "; huipputuuli: "
			  << fixed << setprecision(4) << theWindDataItem.theWindMaximum.value()
			  << "; suunta "
			  << fixed << setprecision(4) << theWindDataItem.theWindDirection.value()
			  << "; suunnan k-hajonta: "
			  << fixed << setprecision(4) << theWindDataItem.theWindDirection.error()
			  << "; puuska: "
			  <<  fixed << setprecision(4) << theWindDataItem.theGustSpeed.value();

;

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
	
	output_file << "|| aika || min || max || ka, k-hajonta || maksimituuli || puuska || suunta || suunnan k-hajonta || fraasi ||" << endl;

	for(unsigned int i = 0; i < theWindDataItemVector.size(); i++)
	  {
		const WindDataItemUnit& theWindDataItem = (*theWindDataItemVector[i])();
		output_file << "| "
					<< theWindDataItem.thePeriod.localEndTime()
					<< " | "
					<< fixed << setprecision(4) << theWindDataItem.theWindSpeedMin.value()
					<< " | "
					<< fixed << setprecision(4) << theWindDataItem.theWindSpeedMax.value()
					<< " | ("
					<< fixed << setprecision(4) << theWindDataItem.theWindSpeedMean.value()
					<< ", "
					<< fixed << setprecision(4) << theWindDataItem.theWindSpeedMean.error()
					<< ")"
					<< " | "
					<< fixed << setprecision(4) << theWindDataItem.theWindMaximum.value()
					<< " | "
					<< fixed << setprecision(4) << theWindDataItem.theGustSpeed.value()
					<< " | "
					<< fixed << setprecision(4) << theWindDataItem.theWindDirection.value()
					<< " | "
					<< fixed << setprecision(4) << theWindDataItem.theWindDirection.error()
					<< " | ";
		
		output_file << directed_speed_string(theWindDataItem.theWindSpeedMean, 
											 theWindDataItem.theWindDirection, 
											 theVar)
					<< " |" << endl;
	  }
  }

  void print_csv_table(const std::string& areaName,
					   const std::string& fileIdentifierString,
					   const std::string& theVar,
					   const wind_data_item_vector& theWindDataItemVector,
					   const vector<unsigned int>& theIndexVector,
					   const bitset<13>& theColumnSelectionBitset)
  {
	if(areaName.empty())
	  return;

	std::string filename("./"+areaName+fileIdentifierString+".csv");

	ofstream output_file(filename.c_str(), ios::out);

	if(output_file.fail())
	  {
		throw std::runtime_error("wind_overview failed to open '" + filename + "' for writing");
	  }
	
	const char* column_names[] = {"aika", ", min", ", max", ", mediaani", ", tasoitettu mediaani", ", ka", ", nopeuden k-hajonta", ", puuska", ", maksimituuli", ", suunta", ", tasoitettu suunta", ", suunnan k-hajonta", ", fraasi"};
	
	const unsigned int number_of_columns = 13;
	for(unsigned int i = 0; i < number_of_columns; i++)
	  if(theColumnSelectionBitset.test(number_of_columns-i-1))
		output_file << column_names[i];
	output_file << endl;

	for(unsigned int i = 0; i < theIndexVector.size(); i++)
	  {
		const unsigned int& index = theIndexVector[i];

		const WindDataItemUnit& theWindDataItem = (*theWindDataItemVector[index])();
		if(theColumnSelectionBitset.test(12))
		  output_file << theWindDataItem.thePeriod.localEndTime();
		if(theColumnSelectionBitset.test(11))
		  {
			output_file << ", "	<< fixed << setprecision(4) << theWindDataItem.theWindSpeedMin.value();
		  }
		if(theColumnSelectionBitset.test(10))
		  {
			output_file	<< ", "	<< fixed << setprecision(4) << theWindDataItem.theWindSpeedMax.value();
		  }
		if(theColumnSelectionBitset.test(9))
		  {
			output_file	<< ", "	<< fixed << setprecision(4) << theWindDataItem.theWindSpeedMedian.value();
		  }
		if(theColumnSelectionBitset.test(8))
		  {
			output_file	<< ", "	<< fixed << setprecision(4) << theWindDataItem.theEqualizedMedianWindSpeed;
		  }
		if(theColumnSelectionBitset.test(7))
		  {
			output_file	<< ", "	<< fixed << setprecision(4) << theWindDataItem.theWindSpeedMean.value();
		  }
		if(theColumnSelectionBitset.test(6))
		  {
			output_file << ", "	<< fixed << setprecision(4) << theWindDataItem.theWindSpeedMean.error();
		  }
		if(theColumnSelectionBitset.test(5))
		  {
			output_file	<< ", "	<< fixed << setprecision(4) << theWindDataItem.theGustSpeed.value();
		  }
		if(theColumnSelectionBitset.test(4))
		  {
			output_file	<< ", "	<< fixed << setprecision(4) << theWindDataItem.theWindMaximum.value();
		  }
		if(theColumnSelectionBitset.test(3))
		  {
			output_file	<< ", "	<< fixed << setprecision(4) << theWindDataItem.theWindDirection.value();
		  }
		if(theColumnSelectionBitset.test(2))
		  {
			output_file	<< ", "	<< fixed << setprecision(4) << theWindDataItem.theEqualizedWindDirection;
		  }
		if(theColumnSelectionBitset.test(1))
		  {
			output_file << ", " << fixed << setprecision(4) << theWindDataItem.theWindDirection.error();
					   
		  }
		if(theColumnSelectionBitset.test(0))
		  {
			output_file << ", " << directed_speed_string(theWindDataItem.theWindSpeedMean, 
														 theWindDataItem.theWindDirection, 
														 theVar);
		  }
		output_file	<< endl;
	  }
  }
  
  void log_raw_data_vector(wo_story_params& storyParams)
  {
	bitset<13> columnSelectionBitset;

	// aika == 12
	// min, == 11
	// max == 10
	// mediaani == 9
	// tasoitettu mediaani == 8
	// ka == 7
	// nopeuden k-hajonta == 6
	// puuska == 5
	// maksimituuli == 4
	// suunta == 3
	// tasoitettu suunta == 2
	// suunnan k-hajonta == 1
	// fraasi == 0

	columnSelectionBitset.set();
	print_csv_table(storyParams.theAreaName,
					"_original",
					storyParams.theVar,
					storyParams.theRawDataVector,
					storyParams.theOriginalWindSpeedIndexes,
					columnSelectionBitset);

	/*
	columnSelectionBitset.set();
	columnSelectionBitset.set(0, false);
	columnSelectionBitset.set(1, false);
	columnSelectionBitset.set(2, false);
	columnSelectionBitset.set(3, false);
	print_csv_table(storyParams.theAreaName,
					"_equalized_windspeed",
					storyParams.theVar,
					storyParams.theRawDataVector,
					storyParams.theOriginalWindSpeedIndexes,
					columnSelectionBitset);

	columnSelectionBitset.set();
	columnSelectionBitset.set(0, false);
	columnSelectionBitset.set(4, false);
	columnSelectionBitset.set(5, false);
	columnSelectionBitset.set(6, false);
	columnSelectionBitset.set(7, false);
	columnSelectionBitset.set(8, false);
	columnSelectionBitset.set(9, false);
	columnSelectionBitset.set(10, false);
	columnSelectionBitset.set(11, false);
	print_csv_table(storyParams.theAreaName,
					"_equalized_winddirection",
					storyParams.theVar,
					storyParams.theRawDataVector,
					storyParams.theOriginalWindSpeedIndexes,
					columnSelectionBitset);
	*/

	columnSelectionBitset.set();
	columnSelectionBitset.set(0, false);
	columnSelectionBitset.set(1, false);
	columnSelectionBitset.set(2, false);
	columnSelectionBitset.set(3, false);
	print_csv_table(storyParams.theAreaName,
					"_equalized_windspeed_changes",
					storyParams.theVar,
					storyParams.theRawDataVector,
					storyParams.theEqualizedWindSpeedIndexes,
					columnSelectionBitset);

	columnSelectionBitset.set();
	columnSelectionBitset.set(0, false);
	columnSelectionBitset.set(4, false);
	columnSelectionBitset.set(5, false);
	columnSelectionBitset.set(6, false);
	columnSelectionBitset.set(7, false);
	columnSelectionBitset.set(8, false);
	columnSelectionBitset.set(9, false);
	columnSelectionBitset.set(10, false);
	columnSelectionBitset.set(11, false);
	print_csv_table(storyParams.theAreaName,
					"_equalized_winddirection_changes",
					storyParams.theVar,
					storyParams.theRawDataVector,
					storyParams.theEqualizedWindDirectionIndexes,
					columnSelectionBitset);

	print_wiki_table(storyParams.theAreaName,
					 storyParams.theVar,
					 storyParams.theRawDataVector);
	
	for(unsigned int k = 0; k < storyParams.theNamedWeatherAreas.size(); k++)
	  {
		const std::string& areaIdentifier = storyParams.theNamedWeatherAreas[k].first;

		storyParams.theLog << "*********** RAW DATA (" << areaIdentifier << ") ***********" << endl;

		for(unsigned int i = 0; i < storyParams.theRawDataVector.size(); i++)
		  {
			const WindDataItemUnit& windDataItem = (*storyParams.theRawDataVector[i])(areaIdentifier);
			storyParams.theLog << windDataItem;
			storyParams.theLog << "; fraasi: "<< directed_speed_string(windDataItem.theWindSpeedMean, 
																	   windDataItem.theWindDirection, 
																	   storyParams.theVar)
							   << endl;
		  }
	  }
  }

  void log_equalized_wind_speed_data_vector(wo_story_params& storyParams)
  {
	for(unsigned int k = 0; k < storyParams.theNamedWeatherAreas.size(); k++)
	  {
		const std::string& areaIdentifier = storyParams.theNamedWeatherAreas[k].first;

		storyParams.theLog << "*********** EQUALIZED WIND SPEED DATA (" << areaIdentifier << ") ***********" << endl;


		for(unsigned int i = 0; i < storyParams.theEqualizedWindSpeedIndexes.size(); i++)
		  {
			const unsigned int& index = storyParams.theEqualizedWindSpeedIndexes[i];
			storyParams.theLog << (*storyParams.theRawDataVector[index])(areaIdentifier) << endl;
		  }
	  }
  }

  void log_equalized_wind_direction_data_vector(wo_story_params& storyParams)
  {
	storyParams.theLog << "*********** EQUALIZED WIND DIRECTION DATA ***********" << endl;

	for(unsigned int i = 0; i < storyParams.theEqualizedWindDirectionIndexes.size(); i++)
	  {
		const unsigned int& index = storyParams.theEqualizedWindDirectionIndexes[i];
		storyParams.theLog << (*storyParams.theRawDataVector[index])() << endl;
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
			if(windEventId >= TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE)
			  windDirectionIndex++;
		  }
		else if(windEventId == TUULI_KAANTYY)
		  {
			storyParams.theLog << "(";
			storyParams.theLog << get_wind_direction_string(storyParams.theWindDirectionVector[windDirectionIndex++]->theWindDirection);
			storyParams.theLog <<  " -> ";
			storyParams.theLog <<  get_wind_direction_string(storyParams.theWindDirectionVector[windDirectionIndex]->theWindDirection);			
			storyParams.theLog << ")" << endl;
		  }
		else if(windEventId >= TUULI_KAANTYY_JA_HEIKEKNEE && windEventId <= TUULI_KAANTYY_JA_TYYNTYY)
		  {
			storyParams.theLog << "(";
			storyParams.theLog << get_wind_direction_string(storyParams.theWindDirectionVector[windDirectionIndex++]->theWindDirection);
			storyParams.theLog <<  " -> ";
			storyParams.theLog << get_wind_direction_string(storyParams.theWindDirectionVector[windDirectionIndex]->theWindDirection);
			storyParams.theLog << "; ";
			storyParams.theLog << get_wind_speed_string(storyParams.theWindSpeedVector[windSpeedIndex++]->theWindSpeedId);
			storyParams.theLog <<  " -> ";
			storyParams.theLog <<  get_wind_speed_string(storyParams.theWindSpeedVector[windSpeedIndex]->theWindSpeedId);
			storyParams.theLog << ")" << endl;
		  }
		else
		  {
			storyParams.theLog << "" << endl;

			if(windEventId == TUULI_MUUTTUU_VAIHTELEVAKSI)
			  windDirectionIndex++;
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
		WeatherResult meanWind(kFloatMissing, kFloatMissing);
		WeatherResult medianWind(kFloatMissing, kFloatMissing);
		WeatherResult maxWind(kFloatMissing, kFloatMissing);
		WeatherResult maximumWind(kFloatMissing, kFloatMissing);
		WeatherResult maximumWindMedian(kFloatMissing, kFloatMissing);
		WeatherResult windDirection(kFloatMissing, kFloatMissing);
		WeatherResult gustSpeed(kFloatMissing, kFloatMissing);

		WindDataItemContainer* dataItemContainer = new WindDataItemContainer();

		for(unsigned int k = 0; k < storyParams.theNamedWeatherAreas.size(); k++)
		  {
			const std::string& areaIdentifier = storyParams.theNamedWeatherAreas[k].first;

			dataItemContainer->addItem(weatherPeriod,
									   minWind,
									   maxWind,
									   meanWind,
									   medianWind,
									   maximumWind,
									   maximumWindMedian,
									   windDirection,
									   gustSpeed,
									   areaIdentifier);
		  }

		storyParams.theRawDataVector.push_back(dataItemContainer);

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

	/*
	for(unsigned int i = 0; i < storyParams.theWindDirectionVector.size(); i++)
	  {
		delete storyParams.theWindDirectionVector[i];
	  }	
	storyParams.theWindDirectionVector.clear();
	*/

  }

  void populate_wind_time_series(wo_story_params& storyParams)
  {
	GridForecaster forecaster;

	for(unsigned int i = 0; i < storyParams.theRawDataVector.size(); i++)
	  {
		for(unsigned int k = 0; k < storyParams.theNamedWeatherAreas.size(); k++)
		  {
			const std::string& itemName = storyParams.theNamedWeatherAreas[k].first;
			const WeatherArea& weatherArea = storyParams.theNamedWeatherAreas[k].second;


			WindDataItemUnit& dataItem = storyParams.theRawDataVector[i]->getDataItem(itemName);

			//			cout << "dataItem.thePeriod.localStartTime(): " << dataItem.thePeriod.localStartTime() << endl;

			dataItem.theWindSpeedMin =
			  forecaster.analyze(storyParams.theVar+"::fake::wind::speed::minimum",
								 storyParams.theSources,
								 WindSpeed,
								 Minimum,
								 Mean,
								 weatherArea,
								 dataItem.thePeriod);

			dataItem.theWindSpeedMax =
			  forecaster.analyze(storyParams.theVar+"::fake::wind::speed::maximum",
								 storyParams.theSources,
								 WindSpeed,
								 Maximum,
								 Mean,
								 weatherArea,
								 dataItem.thePeriod);

			dataItem.theWindSpeedMean =
			  forecaster.analyze(storyParams.theVar+"::fake::wind::speed::mean",
								 storyParams.theSources,
								 WindSpeed,
								 Mean,
								 Mean,
								 weatherArea,
								 dataItem.thePeriod);

			dataItem.theWindSpeedMedian =
			  forecaster.analyze(storyParams.theVar+"::fake::wind::medianwind",
								 storyParams.theSources,
								 WindSpeed,
								 Median,
								 Mean,
								 weatherArea,
								 dataItem.thePeriod);

			dataItem.theEqualizedMedianWindSpeed = dataItem.theWindSpeedMedian.value();

			dataItem.theWindMaximum =
			  forecaster.analyze(storyParams.theVar+"::fake::wind::maximumwind",
								 storyParams.theSources,
								 MaximumWind,
								 Maximum,
								 Mean,
								 weatherArea,
								 dataItem.thePeriod);

			dataItem.theWindMaximumMedian =
			  forecaster.analyze(storyParams.theVar+"::fake::wind::maximumwind",
								 storyParams.theSources,
								 MaximumWind,
								 Median,
								 Mean,
								 weatherArea,
								 dataItem.thePeriod);

			dataItem.theWindDirection =
			  forecaster.analyze(storyParams.theVar+"::fake::wind:direction",
								 storyParams.theSources,
								 WindDirection,
								 Mean,
								 Mean,
								 weatherArea,
								 dataItem.thePeriod);

			dataItem.theEqualizedWindDirection = dataItem.theWindDirection.value();

			dataItem.theGustSpeed =
			  forecaster.analyze(storyParams.theVar+"::fake::gust::speed",
								 storyParams.theSources,
								 GustSpeed,
								 Maximum,
								 Mean,
								 weatherArea,
								 dataItem.thePeriod);
		  }

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
	  return VAIHTELEVA_;

	int windDirectionId = direction16th(windDirection.value());

	return static_cast<wind_direction_id>(windDirectionId);

	/*
	if(windDirectionId == POHJOINEN_)
	  return (accuracy == good_accuracy ? POHJOINEN_ : POHJOINEN_PUOLEINEN);
	else if(windDirectionId == POHJOINEN_KOILLINEN)
	  return POHJOINEN_KOILLINEN;
	else if(windDirectionId == KOILLINEN_)
	  return (accuracy == good_accuracy ? KOILLINEN_ : KOILLINEN_PUOLEINEN);
	else if(windDirectionId == KOILLINEN_ITA)
	  return KOILLINEN_ITA;
	else if(windDirectionId == ITA_)
	  return (accuracy == good_accuracy ? ITA_ : ITA_PUOLEINEN);
	else if(windDirectionId == ITA_KAAKKO)
	  return ITA_KAAKKO;
	else if(windDirectionId == KAAKKO_)
	  return (accuracy == good_accuracy ? KAAKKO_ : KAAKKO_PUOLEINEN);
	else if(windDirectionId == KAAKKO_ETELA)
	  return KAAKKO_ETELA;
	else if(windDirectionId == ETELA_)
	  return (accuracy == good_accuracy ? ETELA_ : ETELA_PUOLEINEN);
	else if(windDirectionId == ETELA_LOUNAS)
	  return ETELA_LOUNAS;
	else if(windDirectionId == LOUNAS_)
	  return (accuracy == good_accuracy ? LOUNAS_ : LOUNAS_PUOLEINEN);
	else if(windDirectionId == LOUNAS_LANSI)
	  return LOUNAS_LANSI;
	else if(windDirectionId == LANSI_)
	  return (accuracy == good_accuracy ? LANSI_ : LANSI_PUOLEINEN);
	else if(windDirectionId == LANSI_LUODE)
	  return LANSI_LUODE;
	else if(windDirectionId == LUODE_)
	  return (accuracy == good_accuracy ? LUODE_ : LUODE_PUOLEINEN);
	else
	  return LUODE_POHJOINEN;
	*/
  }

  void find_out_wind_speed_periods(wo_story_params& storyParams)
  {
	unsigned int equalizedDataIndex;

	if(storyParams.theEqualizedWindSpeedIndexes.size() == 0)
	  return;
	else if(storyParams.theEqualizedWindSpeedIndexes.size() == 1)
	  {
		equalizedDataIndex = storyParams.theEqualizedWindSpeedIndexes[0];
		const WindDataItemUnit& dataItem = (*storyParams.theRawDataVector[equalizedDataIndex])();
		storyParams.theWindSpeedVector.push_back(new WindSpeedPeriodDataItem(dataItem.thePeriod,
																			 get_wind_speed_id(dataItem.theWindSpeedMean)));
		return;
	  }

	unsigned int periodStartEqualizedDataIndex = storyParams.theEqualizedWindSpeedIndexes[0];
	unsigned int periodEndEqualizedDataIndex = storyParams.theEqualizedWindSpeedIndexes[0];
	const WindDataItemUnit& dataItemFirst = (*storyParams.theRawDataVector[periodStartEqualizedDataIndex])();

	wind_speed_id previous_wind_speed_id(get_wind_speed_id(dataItemFirst.theWindSpeedMean));	  

	for(unsigned int i = 1; i < storyParams.theEqualizedWindSpeedIndexes.size(); i++)
	  {
		equalizedDataIndex = storyParams.theEqualizedWindSpeedIndexes[i];
		const WindDataItemUnit& dataItemCurrent = (*storyParams.theRawDataVector[equalizedDataIndex])();
  
		wind_speed_id current_wind_speed_id(get_wind_speed_id(dataItemCurrent.theWindSpeedMean));
		
		if(current_wind_speed_id != previous_wind_speed_id)
		  {
			periodEndEqualizedDataIndex = storyParams.theEqualizedWindSpeedIndexes[i-1];
			const WindDataItemUnit& dataItemPrevious = (*storyParams.theRawDataVector[periodStartEqualizedDataIndex])();
			
			NFmiTime periodStartTime(dataItemPrevious.thePeriod.localStartTime());
			NFmiTime periodEndTime(dataItemCurrent.thePeriod.localStartTime());
			periodEndTime.ChangeByHours(-1);

			//			cout << "ADD: " << periodStartTime << "..." << periodEndTime << endl;

			WeatherPeriod windSpeedPeriod(periodStartTime, periodEndTime);

			storyParams.theWindSpeedVector.push_back(new WindSpeedPeriodDataItem(windSpeedPeriod, previous_wind_speed_id));
			periodStartEqualizedDataIndex = equalizedDataIndex;
			previous_wind_speed_id = current_wind_speed_id;

		  }
	  }
	const WindDataItemUnit& dataItemBeforeLast = (*storyParams.theRawDataVector[periodStartEqualizedDataIndex])();
	const WindDataItemUnit& dataItemLast = (*storyParams.theRawDataVector[storyParams.theRawDataVector.size()-1])();
	
	WeatherPeriod windSpeedPeriod(dataItemBeforeLast.thePeriod.localStartTime(),
								  dataItemLast.thePeriod.localEndTime());

	storyParams.theWindSpeedVector.push_back(new WindSpeedPeriodDataItem(windSpeedPeriod, previous_wind_speed_id));

  }
  
  /*
  void find_out_wind_direction_periods(wo_story_params& storyParams)
  {
	const WindDataItemUnit& dataItemFirst = (*storyParams.theRawDataVector[0])();

	if(storyParams.theRawDataVector.size() == 0)
	  return;
	else if(storyParams.theRawDataVector.size() == 1)
	  {
		WeatherPeriod period(dataItemFirst.thePeriod);
		WeatherResult directionResult(dataItemFirst.theWindDirection);
		wind_direction_id directionId(get_wind_direction_id(directionResult, storyParams.theVar));
		storyParams.theWindDirectionVector.push_back(new WindDirectionPeriodDataItem(period, directionId));
		return;
	  }

	unsigned int periodStartIndex = 0;

	WeatherResult previousDirectionResult(dataItemFirst.theWindDirection);
	wind_direction_id previousWindDirectionId(get_wind_direction_id(previousDirectionResult, storyParams.theVar));	  
	for(unsigned int i = 1; i < storyParams.theRawDataVector.size(); i++)
	  {
		const WindDataItemUnit& dataItemCurrent = (*storyParams.theRawDataVector[i])();
		WeatherResult currentDirectionResult(dataItemCurrent.theWindDirection);
		wind_direction_id currentWindDirectionId(get_wind_direction_id(currentDirectionResult, storyParams.theVar));
		
		if(currentWindDirectionId != previousWindDirectionId)
		  {
			const WindDataItemUnit& dataItemStart = (*storyParams.theRawDataVector[periodStartIndex])();
			const WindDataItemUnit& dataItemEnd = (*storyParams.theRawDataVector[i-1])();
			
			WeatherPeriod windDirectionPeriod(dataItemStart.thePeriod.localStartTime(),
											  dataItemEnd.thePeriod.localEndTime());
			storyParams.theWindDirectionVector.push_back(new WindDirectionPeriodDataItem(windDirectionPeriod, previousWindDirectionId));

			if(i == storyParams.theRawDataVector.size() -1)
			  return;

			periodStartIndex = i;
			previousWindDirectionId = currentWindDirectionId;			
		  }
	  }

	const WindDataItemUnit& dataItemBeforeLast = (*storyParams.theRawDataVector[periodStartIndex])();
	const WindDataItemUnit& dataItemLast = (*storyParams.theRawDataVector[storyParams.theRawDataVector.size() - 1])();

	WeatherPeriod windDirectionPeriod(dataItemBeforeLast.thePeriod.localStartTime(),
									  dataItemLast.thePeriod.localEndTime());

	storyParams.theWindDirectionVector.push_back(new WindDirectionPeriodDataItem(windDirectionPeriod, previousWindDirectionId));
  }
  */

  void find_out_wind_direction_periods(wo_story_params& storyParams)
  {
	unsigned int equalizedDataIndex;

	if(storyParams.theEqualizedWindDirectionIndexes.size() == 0)
	  return;
	else if(storyParams.theEqualizedWindDirectionIndexes.size() == 1)
	  {
		equalizedDataIndex = storyParams.theEqualizedWindDirectionIndexes[0];
		const WindDataItemUnit& dataItem = (*storyParams.theRawDataVector[equalizedDataIndex])();
		storyParams.theWindDirectionVector.push_back(new WindDirectionPeriodDataItem(dataItem.thePeriod,
																						 get_wind_direction_id(dataItem.theWindDirection, storyParams.theVar)));
		return;
	  }

	unsigned int periodStartEqualizedDataIndex = storyParams.theEqualizedWindDirectionIndexes[0];
	unsigned int periodEndEqualizedDataIndex = storyParams.theEqualizedWindDirectionIndexes[0];
	const WindDataItemUnit& dataItemFirst = (*storyParams.theRawDataVector[periodStartEqualizedDataIndex])();
	wind_direction_id previous_wind_direction_id(get_wind_direction_id(dataItemFirst.theWindDirection, 
																		   storyParams.theVar));	  

	for(unsigned int i = 1; i < storyParams.theEqualizedWindDirectionIndexes.size(); i++)
	  {
		equalizedDataIndex = storyParams.theEqualizedWindDirectionIndexes[i];
		const WindDataItemUnit& dataItemCurrent = (*storyParams.theRawDataVector[equalizedDataIndex])();
  
		wind_direction_id current_wind_direction_id(get_wind_direction_id(dataItemCurrent.theWindDirection, 
																		   storyParams.theVar));
		
		if(current_wind_direction_id != previous_wind_direction_id)
		  {
			periodEndEqualizedDataIndex = storyParams.theEqualizedWindDirectionIndexes[i-1];
			const WindDataItemUnit& dataItemPrevious = (*storyParams.theRawDataVector[periodStartEqualizedDataIndex])();
			
			NFmiTime periodStartTime(dataItemPrevious.thePeriod.localStartTime());
			NFmiTime periodEndTime(dataItemCurrent.thePeriod.localStartTime());
			periodEndTime.ChangeByHours(-1);

			//			cout << "ADD: " << periodStartTime << "..." << periodEndTime << endl;

			WeatherPeriod windDirectionPeriod(periodStartTime, periodEndTime);

			storyParams.theWindDirectionVector.push_back(new WindDirectionPeriodDataItem(windDirectionPeriod, previous_wind_direction_id));
			periodStartEqualizedDataIndex = equalizedDataIndex;
			previous_wind_direction_id = current_wind_direction_id;

		  }
	  }
	const WindDataItemUnit& dataItemBeforeLast = (*storyParams.theRawDataVector[periodStartEqualizedDataIndex])();
	const WindDataItemUnit& dataItemLast = (*storyParams.theRawDataVector[storyParams.theRawDataVector.size()-1])();
	
	WeatherPeriod windDirectionPeriod(dataItemBeforeLast.thePeriod.localStartTime(),
									  dataItemLast.thePeriod.localEndTime());

	storyParams.theWindDirectionVector.push_back(new WindDirectionPeriodDataItem(windDirectionPeriod, 
																					 previous_wind_direction_id));


	/*
	unsigned int periodStartIndex = 0;

	WeatherResult previousDirectionResult(dataItemFirst.theWindDirection);
	wind_direction_id previousWindDirectionId(get_wind_direction_id(previousDirectionResult, storyParams.theVar));	  
	for(unsigned int i = 1; i < storyParams.theRawDataVector.size(); i++)
	  {
		const WindDataItemUnit& dataItemCurrent = (*storyParams.theRawDataVector[i])();
		WeatherResult currentDirectionResult(dataItemCurrent.theWindDirection);
		wind_direction_id currentWindDirectionId(get_wind_direction_id(currentDirectionResult, storyParams.theVar));
		
		if(currentWindDirectionId != previousWindDirectionId)
		  {
			const WindDataItemUnit& dataItemStart = (*storyParams.theRawDataVector[periodStartIndex])();
			const WindDataItemUnit& dataItemEnd = (*storyParams.theRawDataVector[i-1])();
			
			WeatherPeriod windDirectionPeriod(dataItemStart.thePeriod.localStartTime(),
											  dataItemEnd.thePeriod.localEndTime());
			storyParams.theWindDirectionVector.push_back(new WindDirectionPeriodDataItem(windDirectionPeriod, previousWindDirectionId));

			if(i == storyParams.theRawDataVector.size() -1)
			  return;

			periodStartIndex = i;
			previousWindDirectionId = currentWindDirectionId;			
		  }
	  }

	const WindDataItemUnit& dataItemBeforeLast = (*storyParams.theRawDataVector[periodStartIndex])();
	const WindDataItemUnit& dataItemLast = (*storyParams.theRawDataVector[storyParams.theRawDataVector.size() - 1])();

	WeatherPeriod windDirectionPeriod(dataItemBeforeLast.thePeriod.localStartTime(),
									  dataItemLast.thePeriod.localEndTime());

	storyParams.theWindDirectionVector.push_back(new WindDirectionPeriodDataItem(windDirectionPeriod, previousWindDirectionId));
	*/
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

	wind_direction_id wind_dir_id(VAIHTELEVA_);

	for(unsigned int i = 1; i < storyParams.theWindDirectionVector.size(); i++)
	  {
		bool simultaneousEventsOccurred(false);
		wind_dir_id = storyParams.theWindDirectionVector[i]->theWindDirection;

		// check if event with same timestamp exists
		for(unsigned int k = 0; k < storyParams.theWindEventVector.size(); k++)
		  {
			if(storyParams.theWindEventVector[k].first == 
			   storyParams.theWindDirectionVector[i]->thePeriod.localStartTime())
			  {
				
				storyParams.theWindEventVector[k].second = static_cast<wind_event_id>(storyParams.theWindEventVector[k].second +
																	   (wind_dir_id == VAIHTELEVA_ ? TUULI_MUUTTUU_VAIHTELEVAKSI : TUULI_KAANTYY));
				simultaneousEventsOccurred = true;
				break;
			  }
		  }

		if(simultaneousEventsOccurred)
		  continue;

		storyParams.theWindEventVector.push_back(make_pair(storyParams.theWindDirectionVector[i]->thePeriod.localStartTime(), 
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
	//	double distance = (0.5*baseLen == 0 ? 0 : (A/(0.5*baseLen)));
	double distance = (A/(0.5*baseLen));

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
			
			const WindDataItemUnit& dataItemIndex1 = (*storyParams.theRawDataVector[index1])();
			const WindDataItemUnit& dataItemIndex2 = (*storyParams.theRawDataVector[index2])();
			const WindDataItemUnit& dataItemIndex3 = (*storyParams.theRawDataVector[index3])();

			double lineBegX = index1;
			double lineBegY = dataItemIndex1.theWindSpeedMedian.value();
			double lineEndX = index3;
			double lineEndY = dataItemIndex3.theWindSpeedMedian.value();
			double pointX = index2;
			double pointY = dataItemIndex2.theWindSpeedMedian.value();
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

		if(minError > storyParams.theMaxErrorWindSpeed)
		  {
			break;
		  }
		// remove the point with minimum error
		storyParams.theEqualizedWindSpeedIndexes.erase(storyParams.theEqualizedWindSpeedIndexes.begin()+minErrorIndex);
	  }

	// re-calculate equalized values for the removed points
	for(unsigned int i = 1; i < storyParams.theEqualizedWindSpeedIndexes.size() - 1; i++)
	  {
		unsigned int currentIndex = storyParams.theEqualizedWindSpeedIndexes[i];
		unsigned int previousIndex = storyParams.theEqualizedWindSpeedIndexes[i-1];
		const WindDataItemUnit& previousItem = (*storyParams.theRawDataVector[previousIndex])();
		const WindDataItemUnit& currentItem = (*storyParams.theRawDataVector[currentIndex])();

		if(currentIndex == previousIndex + 1)
		  continue;
		
		/*
		cout << "i: " << i << endl;
		cout << "previousIndex: " << previousIndex << endl;
		cout << "currentIndex: " << currentIndex << endl;
		cout << "previous windspeed: " << previousItem.theWindSpeedMedian.value() << endl;
		cout << "current windspeed: " << currentItem.theWindSpeedMedian.value() << endl;
		*/

		float oppositeLen = currentItem.theWindSpeedMedian.value() - previousItem.theWindSpeedMedian.value();
		float adjacentLen = currentIndex - previousIndex;
		float slope = oppositeLen / adjacentLen;

		for(unsigned int k = previousIndex+1; k < currentIndex; k++)
		  {
			float yValue = (slope * (k-previousIndex)) + previousItem.theWindSpeedMedian.value();

			/*
			cout << "k: " << k << endl;
			cout << "slope: " << slope << endl;
			cout << "yValue: " << yValue << endl;
			*/

			WindDataItemUnit& item = storyParams.theRawDataVector[k]->getDataItem();
			item.theEqualizedMedianWindSpeed = yValue;			
		  }		
	  }
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
			index1 = storyParams.theEqualizedWindDirectionIndexes[i];
			index2 = storyParams.theEqualizedWindDirectionIndexes[i+1];
			index3 = storyParams.theEqualizedWindDirectionIndexes[i+2];

			const WindDataItemUnit& dataItemIndex1 = (*storyParams.theRawDataVector[index1])();
			const WindDataItemUnit& dataItemIndex2 = (*storyParams.theRawDataVector[index2])();
			const WindDataItemUnit& dataItemIndex3 = (*storyParams.theRawDataVector[index3])();


			double lineBegX = index1;
			double lineBegY = dataItemIndex1.theWindDirection.value();
			double lineEndX = index3;
			double lineEndY = dataItemIndex3.theWindDirection.value();
			double pointX = index2;
			double pointY = dataItemIndex2.theWindDirection.value();

			NFmiPoint point(pointX, pointY);
			NFmiPoint lineBegPoint(lineBegX, lineBegY);
			NFmiPoint lineEndPoint(lineEndX, lineEndY);
			double deviation_from_line = distance_from_line(point, lineBegPoint, lineEndPoint);

			/*
			cout << "direction1: " << dataItemIndex1.theWindDirection.value() << endl;
			cout << "direction2: " << dataItemIndex2.theWindDirection.value() << endl;
			cout << "direction3: " << dataItemIndex3.theWindDirection.value() << endl;
			cout << "deviation_from_line: " << deviation_from_line << endl;
			*/
			if(deviation_from_line < minError)
			  {
				minError = deviation_from_line;
				minErrorIndex = i+1;
			  }
		  }

		//		cout << "min error: " << minError << endl << "XXXX" << endl;
		if(minError > storyParams.theMaxErrorWindDirection)
		  {
			break;
		  }		

		storyParams.theEqualizedWindDirectionIndexes.erase(storyParams.theEqualizedWindDirectionIndexes.begin()+minErrorIndex);
	  }


	// re-calculate equalized values for the removed points
	for(unsigned int i = 1; i < storyParams.theEqualizedWindDirectionIndexes.size() - 1; i++)
	  {
		unsigned int currentIndex = storyParams.theEqualizedWindDirectionIndexes[i];
		unsigned int previousIndex = storyParams.theEqualizedWindDirectionIndexes[i-1];
		const WindDataItemUnit& previousItem = (*storyParams.theRawDataVector[previousIndex])();
		const WindDataItemUnit& currentItem = (*storyParams.theRawDataVector[currentIndex])();

		if(currentIndex == previousIndex + 1)
		  continue;
		
		/*
		cout << "i: " << i << endl;
		cout << "previousIndex: " << previousIndex << endl;
		cout << "currentIndex: " << currentIndex << endl;
		cout << "previous winddirection: " << previousItem.theWindDirection.value() << endl;
		cout << "current winddirection: " << currentItem.theWindDirection.value() << endl;
		*/

		float oppositeLen = currentItem.theWindDirection.value() - previousItem.theWindDirection.value();
		float adjacentLen = currentIndex - previousIndex;
		float slope = oppositeLen / adjacentLen;

		for(unsigned int k = previousIndex+1; k < currentIndex; k++)
		  {
			float yValue = (slope * (k-previousIndex)) + previousItem.theWindDirection.value();

			/*
			cout << "k: " << k << endl;
			cout << "slope: " << slope << endl;
			cout << "yValue: " << yValue << endl;
			*/

			WindDataItemUnit& item = storyParams.theRawDataVector[k]->getDataItem();
			item.theEqualizedWindDirection = yValue;			
		  }
	  }
  }

  void calculate_equalized_data(wo_story_params& storyParams)
  {
	// first calculate the indexes
	if(storyParams.theEqualizedWindSpeedIndexes.size() > 3)
	  calculate_equalized_wind_speed_indexes(storyParams);

	if(storyParams.theEqualizedWindDirectionIndexes.size() > 3)
	  calculate_equalized_wind_direction_indexes(storyParams);

  }

  void read_configuration_params(wo_story_params& storyParams)
  {
	double maxErrorWindSpeed = Settings::optional_double(storyParams.theVar+"::max_error_wind_speed", 0.20);
	double maxErrorWindDirection = Settings::optional_double(storyParams.theVar+"::max_error_wind_direction", 2.0);
	double windSpeedThreshold = Settings::optional_double(storyParams.theVar+"::wind_speed_treshold ", 2.0);
	double windDirectionTreshold = Settings::optional_double(storyParams.theVar+"::wind_direction_treshold", 30.0);
	storyParams.theMaxErrorWindSpeed = maxErrorWindSpeed;
	storyParams.theMaxErrorWindDirection = maxErrorWindDirection;
	storyParams.theWindSpeedThreshold = windSpeedThreshold;
	storyParams.theWindDirectionTreshold = windDirectionTreshold;

	storyParams.theNamedWeatherAreas.push_back(make_pair("full", (storyParams.theArea)));

	std::string split_section_name("textgen::split_the_area::" + storyParams.theAreaName);
	std::string split_method_name(split_section_name + "::method");

	if(NFmiSettings::IsSet(split_method_name))
	  {
		WeatherArea northernArea(storyParams.theArea);
		WeatherArea southernArea(storyParams.theArea);
		WeatherArea easternArea(storyParams.theArea);
		WeatherArea westernArea(storyParams.theArea);
		northernArea.type(WeatherArea::Northern);
		southernArea.type(WeatherArea::Southern);
		easternArea.type(WeatherArea::Eastern);
		westernArea.type(WeatherArea::Western);
		
		std::string split_method = Settings::require_string(split_method_name);
		if(split_method.compare("vertical") == 0)
		  {
			storyParams.theNamedWeatherAreas.push_back(make_pair("east",easternArea));
			storyParams.theNamedWeatherAreas.push_back(make_pair("west",westernArea));
		  }
		else if(split_method.compare("horizontal") == 0)
		  {
			storyParams.theNamedWeatherAreas.push_back(make_pair("south",southernArea));
			storyParams.theNamedWeatherAreas.push_back(make_pair("north",northernArea));
		  }
	  }

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

	wo_story_params storyParams(itsVar,
								areaName,
								itsArea,
								itsPeriod,
								itsPeriod,
								itsForecastTime,
								itsSources,
								logger);

	// read the configuration parameters
	read_configuration_params(storyParams);

	// allocate data structures for hourly data
	allocate_data_structures(storyParams);

	// populate the data structures with the relevant data
	populate_wind_time_series(storyParams);
	
	// equalize the data
	calculate_equalized_data(storyParams);

	// find out the wind speed periods
	find_out_wind_speed_periods(storyParams);

	// find out the wind direction periods of -direction compass
	find_out_wind_direction_periods(storyParams);

	// find out the events (changing point) during the forecast period
	find_out_wind_events(storyParams);

	// log functions
	log_raw_data_vector(storyParams);
	log_equalized_wind_speed_data_vector(storyParams);
	log_equalized_wind_direction_data_vector(storyParams);
	log_wind_speed_periods(storyParams);
	log_wind_direction_periods(storyParams);
	log_wind_events(storyParams);

	WindForecast windForecast(storyParams);

	paragraph << windForecast.windForecastBasedOnEvents(itsPeriod);


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
