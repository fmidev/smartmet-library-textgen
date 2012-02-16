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
#include "RangeAcceptor.h"

#include <newbase/NFmiSettings.h>

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

  std::string get_area_type_string(const WeatherArea::Type& theAreaType)
  {
	std::string retval;

	switch (theAreaType)
	  {
	  case WeatherArea::Full:
		retval = "full";
		break;
	  case WeatherArea::Land:
		retval = "land";
		break;
	  case WeatherArea::Coast:
		retval = "coast";
		break;
	  case WeatherArea::Inland:
		retval = "inland";
		break;
	  case WeatherArea::Northern:
		retval = "northern";
		break;
	  case WeatherArea::Southern:
		retval = "southern";
		break;
	  case WeatherArea::Eastern:
		retval = "eastern";
		break;
	  case WeatherArea::Western:
		retval = "western";
		break;
	  default:
		break;
	  }

	return retval;
  }

  std::string get_area_name_string(const WeatherArea& theArea)
  {
	return (theArea.isNamed() ? theArea.name() : "");
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
			  << "; mediaani: "
			  << fixed << setprecision(4) << theWindDataItem.theWindSpeedMedian.value()
			  << "; tasoitettu mediaani: "
			  << fixed << setprecision(4) << theWindDataItem.theEqualizedMedianWindSpeed
			  << "; ka,k-hajonta: ("
			  << fixed << setprecision(4) << theWindDataItem.theWindSpeedMean.value()
			  << ", "
			  << fixed << setprecision(4) << theWindDataItem.theWindSpeedMean.error()
			  << ")"
			  << "; huipputuuli: "
			  << fixed << setprecision(4) << theWindDataItem.theWindMaximum.value()
			  << "; tasoitettu huipputuuli: "
			  << fixed << setprecision(4) << theWindDataItem.theEqualizedMaximumWind
			  << "; suunta: "
			  << fixed << setprecision(4) << theWindDataItem.theWindDirection.value()
			  << "; suunnan k-hajonta: "
			  << fixed << setprecision(4) << theWindDataItem.theWindDirection.error()
			  << " (" << wind_direction_string(wind_direction_id(theWindDataItem.theWindDirection, "")) << ") "
			  << "; puuska: "
			  <<  fixed << setprecision(4) << theWindDataItem.theGustSpeed.value();

	return theOutput;
  }

  std::ostream& operator<<(std::ostream & theOutput,
						   const WindSpeedPeriodDataItem& theWindSpeedPeriodDataItem)
  {
	theOutput << theWindSpeedPeriodDataItem.thePeriod.localStartTime()
			  << " ... "
			  << theWindSpeedPeriodDataItem.thePeriod.localEndTime()
			  << ": "
			  << wind_speed_string(theWindSpeedPeriodDataItem.theWindSpeedId)
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
			  << wind_direction_string(theWindDirectionPeriodDataItem.theWindDirection)
			  << endl;

	return theOutput;
  }

  void print_wiki_table(const WeatherArea& theArea,
						const string& theVar,
						const wind_data_item_vector& theWindDataItemVector)
  {
	std::string filename("./"+get_area_name_string(theArea)+"_rawdata.txt");

	ofstream output_file(filename.c_str(), ios::out);

	if(output_file.fail())
	  {
		throw std::runtime_error("wind_overview failed to open '" + filename + "' for writing");
	  }
	
	output_file << "|| aika || min || max || ka, k-hajonta || maksimituuli || puuska || suunta || suunnan k-hajonta || fraasi ||" << endl;

	for(unsigned int i = 0; i < theWindDataItemVector.size(); i++)
	  {
		const WindDataItemUnit& theWindDataItem = (*theWindDataItemVector[i])(theArea.type());
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
		
		output_file << directed16_speed_string(theWindDataItem.theWindSpeedMean, 
											 theWindDataItem.theWindDirection, 
											 theVar)
					<< " |" << endl;
	  }
  }

  void print_csv_table(const WeatherArea& theArea,
					   const std::string& fileIdentifierString,
					   const std::string& theVar,
					   const wind_data_item_vector& theWindDataItemVector,
					   const vector<unsigned int>& theIndexVector,
					   const bitset<14>& theColumnSelectionBitset)
  {
	std::string filename("./"+get_area_name_string(theArea)+fileIdentifierString+".csv");

	ofstream output_file(filename.c_str(), ios::out);

	if(output_file.fail())
	  {
		throw std::runtime_error("wind_overview failed to open '" + filename + "' for writing");
	  }
	
	const char* column_names[] = {"aika", ", min", ", max", ", mediaani", ", tasoitettu mediaani", ", ka", ", nopeuden k-hajonta", ", puuska", ", maksimituuli", ", tasoitettu maksimituuli", ", suunta", ", tasoitettu suunta", ", suunnan k-hajonta", ", fraasi"};
	
	const unsigned int number_of_columns = 14;
	for(unsigned int i = 0; i < number_of_columns; i++)
	  if(theColumnSelectionBitset.test(number_of_columns-i-1))
		output_file << column_names[i];
	output_file << endl;

	for(unsigned int i = 0; i < theIndexVector.size(); i++)
	  {
		const unsigned int& index = theIndexVector[i];

		const WindDataItemUnit& theWindDataItem = (*theWindDataItemVector[index])(theArea.type());
		if(theColumnSelectionBitset.test(13))
		  output_file << theWindDataItem.thePeriod.localEndTime();
		if(theColumnSelectionBitset.test(12))
		  {
			output_file << ", "	<< fixed << setprecision(4) << theWindDataItem.theWindSpeedMin.value();
		  }
		if(theColumnSelectionBitset.test(11))
		  {
			output_file	<< ", "	<< fixed << setprecision(4) << theWindDataItem.theWindSpeedMax.value();
		  }
		if(theColumnSelectionBitset.test(10))
		  {
			output_file	<< ", "	<< fixed << setprecision(4) << theWindDataItem.theWindSpeedMedian.value();
		  }
		if(theColumnSelectionBitset.test(9))
		  {
			output_file	<< ", "	<< fixed << setprecision(4) << theWindDataItem.theEqualizedMedianWindSpeed;
		  }
		if(theColumnSelectionBitset.test(8))
		  {
			output_file	<< ", "	<< fixed << setprecision(4) << theWindDataItem.theWindSpeedMean.value();
		  }
		if(theColumnSelectionBitset.test(7))
		  {
			output_file << ", "	<< fixed << setprecision(4) << theWindDataItem.theWindSpeedMean.error();
		  }
		if(theColumnSelectionBitset.test(6))
		  {
			output_file	<< ", "	<< fixed << setprecision(4) << theWindDataItem.theGustSpeed.value();
		  }
		if(theColumnSelectionBitset.test(5))
		  {
			output_file	<< ", "	<< fixed << setprecision(4) << theWindDataItem.theWindMaximum.value();
		  }
		if(theColumnSelectionBitset.test(4))
		  {
			output_file	<< ", "	<< fixed << setprecision(4) << theWindDataItem.theEqualizedMaximumWind;
		  }
		if(theColumnSelectionBitset.test(3))
		  {
			output_file	<< ", "	<< fixed << setprecision(4) << theWindDataItem.theWindDirection.value();
		  }
		if(theColumnSelectionBitset.test(2))
		  {
			output_file	<< ", "	<< fixed << setprecision(4) << theWindDataItem.theEqualizedWindDirection.value();
		  }
		if(theColumnSelectionBitset.test(1))
		  {
			output_file << ", " << fixed << setprecision(4) << theWindDataItem.theWindDirection.error();
					   
		  }
		if(theColumnSelectionBitset.test(0))
		  {
			output_file << ", " << directed16_speed_string(theWindDataItem.theWindSpeedMean, 
														   theWindDataItem.theWindDirection, 
														   theVar);
		  }
		output_file	<< endl;
	  }
  }

  
  void print_windspeed_distribution(const WeatherArea& theArea,
									const std::string& fileIdentifierString,
									const std::string& theVar,
									const wind_data_item_vector& theWindDataItemVector,
									const vector<unsigned int>& theIndexVector)
  {
	std::string filename("./"+get_area_name_string(theArea)+fileIdentifierString+".csv");

	ofstream output_file(filename.c_str(), ios::out);

	if(output_file.fail())
	  {
		throw std::runtime_error("wind_overview failed to open '" + filename + "' for writing");
	  }
	
	const WindDataItemUnit& firstWindDataItem = (*theWindDataItemVector[0])(theArea.type());
	
	unsigned int numberOfWindSpeedCategories = firstWindDataItem.theWindSpeedDistribution.size();
	
	for(unsigned int i = 0; i < numberOfWindSpeedCategories; i++)
	  {
		if(i == numberOfWindSpeedCategories - 1 )
		  output_file << ", > " << i+1 << " m/s";
		else
		  output_file << ", " << i+1 << " m/s";
	  }
	output_file << endl;
 
	for(unsigned int i = 0; i < theIndexVector.size(); i++)
	  {
		const unsigned int& index = theIndexVector[i];

		const WindDataItemUnit& theWindDataItem = (*theWindDataItemVector[index])(theArea.type());
			
		output_file << theWindDataItem.thePeriod.localEndTime();

		for(unsigned int k = 0; k < numberOfWindSpeedCategories; k++)
		  {
			output_file << ", ";			
			output_file << fixed << setprecision(2) << theWindDataItem.getWindSpeedShare(k, k+1);
		  }
		output_file << endl;
	  }
  }

  void print_html_table(const WeatherArea::Type& theAreaType,
						const std::string& fileIdentifierString,
						const std::string& theVar,
						const wind_data_item_vector& theWindDataItemVector,
						const vector<unsigned int>& theIndexVector,
						const bitset<14>& theColumnSelectionBitset)
  {
	std::string filename("./"+get_area_type_string(theAreaType)+fileIdentifierString+".html");

	ofstream output_file(filename.c_str(), ios::out);

	if(output_file.fail())
	  {
		throw std::runtime_error("wind_overview failed to open '" + filename + "' for writing");
	  }
	
	output_file << "<h1>" << get_area_type_string(theAreaType) << "</h1>" << endl;
	output_file << "<table border=\"1\">" << endl;

	const char* column_names[] = {"aika", "min",  "max", "mediaani", "tasoitettu mediaani", "ka", "nopeuden k-hajonta", "puuska", "maksimituuli", "tasoitettu maksimituuli", "suunta", "tasoitettu suunta", "suunnan k-hajonta", "fraasi"};
	
	output_file << "<tr>" << endl;
	const unsigned int number_of_columns = 14;
	for(unsigned int i = 0; i < number_of_columns; i++)
	  if(theColumnSelectionBitset.test(number_of_columns-i-1))
		{
		  output_file << "<td>";
		  output_file << column_names[i];
		  output_file << "</td>";
		}
	output_file << "</tr>" << endl;

	for(unsigned int i = 0; i < theIndexVector.size(); i++)
	  {
		output_file << "<tr>" << endl;
		const unsigned int& index = theIndexVector[i];

		const WindDataItemUnit& theWindDataItem = (*theWindDataItemVector[index])(theAreaType);
		if(theColumnSelectionBitset.test(13))
		  {
			output_file << "<td style=\"white-space: nowrap;\">";
			output_file << theWindDataItem.thePeriod.localEndTime();
			output_file << "</td>" << endl;
		  }
		if(theColumnSelectionBitset.test(12))
		  {
			output_file << "<td>";
			output_file << fixed << setprecision(2) << theWindDataItem.theWindSpeedMin.value();
			output_file << "</td>" << endl;
		  }
		if(theColumnSelectionBitset.test(11))
		  {
			output_file << "<td>";
			output_file	<< fixed << setprecision(2) << theWindDataItem.theWindSpeedMax.value();
			output_file << "</td>" << endl;
		  }
		if(theColumnSelectionBitset.test(10))
		  {
			output_file << "<td>";
			output_file	<< fixed << setprecision(2) << theWindDataItem.theWindSpeedMedian.value();
			output_file << "</td>" << endl;
		  }
		if(theColumnSelectionBitset.test(9))
		  {
			output_file << "<td>";
			output_file	<< fixed << setprecision(2) << theWindDataItem.theEqualizedMedianWindSpeed;
			output_file << "</td>" << endl;
		  }
		if(theColumnSelectionBitset.test(8))
		  {
			output_file << "<td>";
			output_file	<< fixed << setprecision(2) << theWindDataItem.theWindSpeedMean.value();
			output_file << "</td>" << endl;
		  }
		if(theColumnSelectionBitset.test(7))
		  {
			output_file << "<td>";
			output_file << fixed << setprecision(2) << theWindDataItem.theWindSpeedMean.error();
			output_file << "</td>" << endl;
		  }
		if(theColumnSelectionBitset.test(6))
		  {
			output_file << "<td>";
			output_file	<< fixed << setprecision(2) << theWindDataItem.theGustSpeed.value();
			output_file << "</td>" << endl;
		  }
		if(theColumnSelectionBitset.test(5))
		  {
			output_file << "<td>";
			output_file	<< fixed << setprecision(2) << theWindDataItem.theWindMaximum.value();
			output_file << "</td>" << endl;
		  }
		if(theColumnSelectionBitset.test(4))
		  {
			output_file << "<td>";
			output_file	<< fixed << setprecision(2) << theWindDataItem.theEqualizedMaximumWind;
			output_file << "</td>" << endl;
		  }
		if(theColumnSelectionBitset.test(3))
		  {
			output_file << "<td>";
			output_file	<< fixed << setprecision(2) << theWindDataItem.theWindDirection.value();
			output_file << "</td>" << endl;
		  }
		if(theColumnSelectionBitset.test(2))
		  {
			output_file << "<td>";
			output_file	<< fixed << setprecision(2) << theWindDataItem.theEqualizedWindDirection.value();
			output_file << "</td>" << endl;
		  }
		if(theColumnSelectionBitset.test(1))
		  {
			output_file << "<td>";
			output_file << fixed << setprecision(2) << theWindDataItem.theWindDirection.error();
			output_file << "</td>" << endl;					   
		  }
		if(theColumnSelectionBitset.test(0))
		  {
			output_file << "<td style=\"white-space: nowrap;\">";
			output_file << directed16_speed_string(theWindDataItem.theWindSpeedMean, 
												   theWindDataItem.theWindDirection, 
												   theVar);
			output_file << "</td>" << endl;
		  }
		output_file << "</tr>" << endl;
	  } // for

	output_file << "</table>";
  }
  

  void save_raw_data(wo_story_params& storyParams, const string& id_str = "_original")
  {
	bitset<14> columnSelectionBitset;

	// aika == 13
	// min, == 12
	// max == 11
	// mediaani == 10
	// tasoitettu mediaani == 9
	// ka == 8
	// nopeuden k-hajonta == 7
	// puuska == 6
	// maksimituuli == 5
	// tasoitettu maksimituuli == 4
	// suunta == 3
	// tasoitettu suunta == 2
	// suunnan k-hajonta == 1
	// fraasi == 0

	columnSelectionBitset.set();
	print_csv_table(storyParams.theArea,
					id_str,
					storyParams.theVar,
					storyParams.theWindDataVector,
					storyParams.originalWindDataIndexes(storyParams.theArea.type()),
					columnSelectionBitset);

	/*
	print_html_table(storyParams.theAreaName,
					id_str,
					storyParams.theVar,
					storyParams.theWindDataVector,
					storyParams.theOriginalWindSpeedIndexes,
					columnSelectionBitset);
	*/

	print_windspeed_distribution(storyParams.theArea,
								 "_windspeed_distribution",
								 storyParams.theVar,
								 storyParams.theWindDataVector,
								 storyParams.originalWindDataIndexes(storyParams.theArea.type()));

	/*
	columnSelectionBitset.set();
	columnSelectionBitset.set(0, false);
	columnSelectionBitset.set(1, false);
	columnSelectionBitset.set(2, false);
	columnSelectionBitset.set(3, false);
	print_csv_table(storyParams.theAreaName,
					"_equalized_median_windspeed_changes",
					storyParams.theVar,
					storyParams.theWindDataVector,
					storyParams.theEqualizedWindSpeedIndexesForMedianWind,
					columnSelectionBitset);

	columnSelectionBitset.set();
	columnSelectionBitset.set(0, false);
	columnSelectionBitset.set(1, false);
	columnSelectionBitset.set(2, false);
	columnSelectionBitset.set(3, false);
	print_csv_table(storyParams.theAreaName,
					"_equalized_maximum_windspeed_changes",
					storyParams.theVar,
					storyParams.theWindDataVector,
					storyParams.theEqualizedWindSpeedIndexesForMaximumWind,
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
	columnSelectionBitset.set(12, false);
	print_csv_table(storyParams.theAreaName,
					"_equalized_winddirection_changes",
					storyParams.theVar,
					storyParams.theWindDataVector,
					storyParams.theEqualizedWindDirectionIndexes,
					columnSelectionBitset);

	print_wiki_table(storyParams.theAreaName,
					 storyParams.theVar,
					 storyParams.theWindDataVector);
	*/
	

  }

  void log_raw_data(wo_story_params& storyParams)
  {
	for(unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
	  {
		WeatherArea::Type areaType(storyParams.theWeatherAreas[k].type());
		std::string areaIdentifier(get_area_type_string(areaType));

		storyParams.theLog << "*********** RAW DATA (" << areaIdentifier << ") ***********" << endl;

		for(unsigned int i = 0; i < storyParams.theWindDataVector.size(); i++)
		  {
			const WindDataItemUnit& windDataItem = (*storyParams.theWindDataVector[i])(areaType);
			storyParams.theLog << windDataItem;
			storyParams.theLog << "; fraasi: "<< directed16_speed_string(windDataItem.theWindSpeedMean, 
																		 windDataItem.theWindDirection, 
																		 storyParams.theVar)
							   << endl;
		  }
	  }
  }

  void log_equalized_wind_speed_data_vector(wo_story_params& storyParams)
  {
	for(unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
	  {
		WeatherArea::Type areaType(storyParams.theWeatherAreas[k].type());
		std::string areaIdentifier(get_area_type_string(areaType));

		storyParams.theLog << "*********** EQUALIZED MEDIAN WIND SPEED DATA (" << areaIdentifier << ") ***********" << endl;

		vector<unsigned int>& indexVector =  storyParams.equalizedWSIndexesMedian(areaType);

		for(unsigned int i = 0; i < indexVector.size(); i++)
		  {
			const unsigned int& index = indexVector[i];
			storyParams.theLog << (*storyParams.theWindDataVector[index])(areaType) << endl;
		  }
	  }

	for(unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
	  {
		WeatherArea::Type areaType(storyParams.theWeatherAreas[k].type());
		std::string areaIdentifier(get_area_type_string(areaType));

		storyParams.theLog << "*********** EQUALIZED MAXIMUM WIND SPEED DATA (" << areaIdentifier << ") ***********" << endl;

		vector<unsigned int>& indexVector =  storyParams.equalizedWSIndexesMaximum(areaType);

		for(unsigned int i = 0; i < indexVector.size(); i++)
		  {
			const unsigned int& index = indexVector[i];
			storyParams.theLog << (*storyParams.theWindDataVector[index])(areaType) << endl;
		  }
	  }
  }

  void log_equalized_wind_direction_data_vector(wo_story_params& storyParams)
  {
	for(unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
	  {
		WeatherArea::Type areaType(storyParams.theWeatherAreas[k].type());
		std::string areaIdentifier(get_area_type_string(areaType));

		storyParams.theLog << "*********** EQUALIZED WIND DIRECTION DATA (" << areaIdentifier << ") ***********" << endl;

		vector<unsigned int>& indexVector =  storyParams.equalizedWDIndexes(areaType);

		for(unsigned int i = 0; i < indexVector.size(); i++)
		  {
			const unsigned int& index = indexVector[i];
			storyParams.theLog << (*storyParams.theWindDataVector[index])(areaType) << endl;
		  }
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

#ifdef LATER
  void log_wind_events(wo_story_params& storyParams)
  {
	storyParams.theLog << "*********** WIND EVENTS ***********" << endl;

	unsigned int windDirectionIndex = 0;
	unsigned int windSpeedIndex = 0;

	for(unsigned int i = 0; i < storyParams.theWindEventVector.size(); i++)
	  {
		WindEventId windEventId = storyParams.theWindEventVector[i].second;

		storyParams.theLog << storyParams.theWindEventVector[i].first 
						   << " "
						   <<  get_wind_event_string(storyParams.theWindEventVector[i].second)
						   << " ";

		if((windEventId > 0x0 && windEventId <= TUULI_TYYNTYY) ||
		   (windEventId >= TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE && windEventId <= TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY))
		  {
			storyParams.theLog << "(";
			storyParams.theLog << wind_speed_string(storyParams.theWindSpeedVector[windSpeedIndex++]->theWindSpeedId);
			storyParams.theLog <<  " -> ";
			storyParams.theLog << wind_speed_string(storyParams.theWindSpeedVector[windSpeedIndex]->theWindSpeedId);
			storyParams.theLog << ")" << endl;
			if(windEventId >= TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE)
			  windDirectionIndex++;
		  }
		else if(windEventId == TUULI_KAANTYY)
		  {
			storyParams.theLog << "(";
			storyParams.theLog << wind_direction_string(storyParams.theWindDirectionVector[windDirectionIndex++]->theWindDirection);
			storyParams.theLog <<  " -> ";
			storyParams.theLog <<  wind_direction_string(storyParams.theWindDirectionVector[windDirectionIndex]->theWindDirection);			
			storyParams.theLog << ")" << endl;
		  }
		else if(windEventId >= TUULI_KAANTYY_JA_HEIKKENEE && windEventId <= TUULI_KAANTYY_JA_TYYNTYY)
		  {
			storyParams.theLog << "(";
			storyParams.theLog << wind_direction_string(storyParams.theWindDirectionVector[windDirectionIndex++]->theWindDirection);
			storyParams.theLog <<  " -> ";
			storyParams.theLog << wind_direction_string(storyParams.theWindDirectionVector[windDirectionIndex]->theWindDirection);
			storyParams.theLog << "; ";
			storyParams.theLog << wind_speed_string(storyParams.theWindSpeedVector[windSpeedIndex++]->theWindSpeedId);
			storyParams.theLog <<  " -> ";
			storyParams.theLog << wind_speed_string(storyParams.theWindSpeedVector[windSpeedIndex]->theWindSpeedId);
			storyParams.theLog << ")" << endl;
		  }
		else
		  {
			storyParams.theLog << "" << endl;

			if(windEventId == TUULI_MUUTTUU_VAIHTELEVAKSI)
			  windDirectionIndex++;
		  }
	  }
  }
#endif

  void log_wind_event_periods(wo_story_params& storyParams)
  {
	storyParams.theLog << "*********** WIND EVENT PERIODS ***********" << endl;

	for(unsigned int i = 0; i < storyParams.theWindEventPeriodVector.size(); i++)
	  {
		WindEventId windEventId = storyParams.theWindEventPeriodVector[i]->theWindEvent;

		storyParams.theLog << storyParams.theWindEventPeriodVector[i]->thePeriod.localStartTime() 
						   << "..."
						   << storyParams.theWindEventPeriodVector[i]->thePeriod.localEndTime()
						   << ": "
						   << get_wind_event_string(windEventId);

		if(windEventId == TUULI_HEIKKENEE ||
		   windEventId == TUULI_VOIMISTUU ||
		   windEventId == TUULI_TYYNTYY ||
		   windEventId == MISSING_WIND_SPEED_EVENT)
		  {
			storyParams.theLog << ": "
							   << fixed << setprecision(4) 
							   << storyParams.theWindEventPeriodVector[i]->thePeriodBeginDataItem.theEqualizedMaximumWind
							   << "->"
							   << storyParams.theWindEventPeriodVector[i]->thePeriodEndDataItem.theEqualizedMaximumWind
							   << endl;
		  }
		else if(windEventId == TUULI_KAANTYY ||
				windEventId == TUULI_MUUTTUU_VAIHTELEVAKSI ||
				windEventId == MISSING_WIND_DIRECTION_EVENT)
		  {
			WeatherResult weatherResultBeg(storyParams.theWindEventPeriodVector[i]->thePeriodBeginDataItem.theEqualizedWindDirection);
			WeatherResult weatherResultEnd(storyParams.theWindEventPeriodVector[i]->thePeriodEndDataItem.theEqualizedWindDirection);

			WindDirectionId directionIdBeg = wind_direction_id(weatherResultBeg, storyParams.theVar);
			WindDirectionId directionIdEnd = wind_direction_id(weatherResultEnd, storyParams.theVar);

			storyParams.theLog << ": "
							   << wind_direction_string(directionIdBeg)
							   << "->"
							   << wind_direction_string(directionIdEnd)
							   << endl;
		  }
	  }
  }

  void log_merged_wind_event_periods(wo_story_params& storyParams)
  {
	storyParams.theLog << "*********** MERGED WIND EVENT PERIODS ***********" << endl;

	WindEventPeriodDataItem* ppp = 0;
	for(unsigned int i = 0; i < storyParams.theMergedWindEventPeriodVector.size(); i++)
	  {
		ppp = storyParams.theMergedWindEventPeriodVector[i];
		WindEventId windEventId = storyParams.theMergedWindEventPeriodVector[i]->theWindEvent;

		storyParams.theLog << storyParams.theMergedWindEventPeriodVector[i]->thePeriod.localStartTime() 
						   << "..."
						   << storyParams.theMergedWindEventPeriodVector[i]->thePeriod.localEndTime()
						   << ": "
						   << get_wind_event_string(windEventId);

		WeatherResult directionBeg = storyParams.theMergedWindEventPeriodVector[i]->thePeriodBeginDataItem.theWindDirection;
		WeatherResult directionEnd = storyParams.theMergedWindEventPeriodVector[i]->thePeriodEndDataItem.theWindDirection;
		WindDirectionId directionIdBeg = wind_direction_id(directionBeg, storyParams.theVar);
		WindDirectionId directionIdEnd = wind_direction_id(directionEnd, storyParams.theVar);
		float maxWindBeg = storyParams.theMergedWindEventPeriodVector[i]->thePeriodBeginDataItem.theEqualizedMaximumWind;
		float maxWindEnd = storyParams.theMergedWindEventPeriodVector[i]->thePeriodEndDataItem.theEqualizedMaximumWind;


		if((windEventId > 0x0 && windEventId <= TUULI_TYYNTYY) ||
		   (windEventId >= TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE && windEventId <= TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY))
		  {
			storyParams.theLog << "("
							   << fixed << setprecision(4) << maxWindBeg
							   <<  " -> "
							   << fixed << setprecision(4) << maxWindEnd
							   << ")" << endl;
		  }
		else if(windEventId == TUULI_KAANTYY)
		  {
			storyParams.theLog << "("
							   << wind_direction_string(directionIdBeg)
							   << " " << directionBeg.value()
							   <<  " -> "
							   << wind_direction_string(directionIdEnd)
							   << " " << directionEnd.value()
							   << ")" << endl;
		  }
		else if(windEventId >= TUULI_KAANTYY_JA_HEIKKENEE && windEventId <= TUULI_KAANTYY_JA_TYYNTYY)
		  {
			storyParams.theLog << "("
							   << wind_direction_string(directionIdBeg)
							   << " " << directionBeg.value()
							   <<  " -> "
							   << wind_direction_string(directionIdEnd)
							   << " " << directionEnd.value() << " "
							   << ")"
							   << "; "
							   << "("
							   << fixed << setprecision(4) << maxWindBeg
							   <<  " -> "
							   << fixed << setprecision(4) << maxWindEnd
							   << ")" << endl;
		  }
		else
		  {
			storyParams.theLog << "" << endl;
		  }
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
		WeatherResult windDirection(kFloatMissing, kFloatMissing);
		WeatherResult gustSpeed(kFloatMissing, kFloatMissing);

		WindDataItemsByArea* dataItemsByArea = new WindDataItemsByArea();

		for(unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
		  {
			WeatherArea::Type type = storyParams.theWeatherAreas[k].type();

			dataItemsByArea->addItem(weatherPeriod,
									 minWind,
									 maxWind,
									 meanWind,
									 medianWind,
									 maximumWind,
									 windDirection,
									 gustSpeed,
									 type);
		  }

		storyParams.theWindDataVector.push_back(dataItemsByArea);

		periodStartTime.ChangeByHours(1);
	  }

		for(unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
		  {
			WeatherArea::Type type = storyParams.theWeatherAreas[k].type();

			storyParams.indexes.insert(make_pair(type, new index_vectors()));
		  }

  }

  void deallocate_data_structures(wo_story_params& storyParams)
  {
	for(unsigned int i = 0; i < storyParams.theWindDataVector.size(); i++)
	  {
		delete storyParams.theWindDataVector[i];
	  }	
	storyParams.theWindDataVector.clear();

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

	for(unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
	  {
		delete storyParams.indexes[storyParams.theWeatherAreas[k].type()];
	  }
	storyParams.indexes.clear();
  }

  void populate_time_series(wo_story_params& storyParams)
  {
	GridForecaster forecaster;

	for(unsigned int i = 0; i < storyParams.theWindDataVector.size(); i++)
	  {
		for(unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
		  {
			const WeatherArea& weatherArea = storyParams.theWeatherAreas[k];
			WeatherArea::Type areaType(weatherArea.type());

			WindDataItemUnit& dataItem = (storyParams.theWindDataVector[i])->getDataItem(areaType);

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

			dataItem.theEqualizedMaximumWind = dataItem.theWindMaximum.value();

 			dataItem.theWindDirection =
			  forecaster.analyze(storyParams.theVar+"::fake::wind:direction",
								 storyParams.theSources,
								 WindDirection,
								 Mean,
								 Mean,
								 weatherArea,
								 dataItem.thePeriod);

			dataItem.theEqualizedWindDirection = dataItem.theWindDirection;

			dataItem.theGustSpeed =
			  forecaster.analyze(storyParams.theVar+"::fake::gust::speed",
								 storyParams.theSources,
								 GustSpeed,
								 Maximum,
								 Mean,
								 weatherArea,
								 dataItem.thePeriod);

			populate_windspeed_distribution_time_series(storyParams.theSources,
														weatherArea,
														dataItem.thePeriod,
														storyParams.theVar,
														dataItem.theWindSpeedDistribution);

			storyParams.originalWindDataIndexes(areaType).push_back(i);
			storyParams.equalizedWSIndexesMedian(areaType).push_back(i);
			storyParams.equalizedWSIndexesMaximum(areaType).push_back(i);
			storyParams.equalizedWDIndexes(areaType).push_back(i);
		  }
	  }

	/*
	for(unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
	  {
		const std::string& areaPart = storyParams.theWeatherAreas[k].first;
		for(unsigned int i = 1; i < storyParams.theWindDataVector.size() - 1; i++)
		  {
			WindDataItemUnit& previousDataItem = storyParams.theWindDataVector[i-1]->getDataItem(areaPart);
			WindDataItemUnit& currentDataItem = storyParams.theWindDataVector[i]->getDataItem(areaPart);
			WindDataItemUnit& nextDataItem = storyParams.theWindDataVector[i+1]->getDataItem(areaPart);
			if(wind_direction_id(previousDataItem.theWindDirection, storyParams.theVar) ==
			   wind_direction_id(nextDataItem.theWindDirection, storyParams.theVar) &&
			   wind_direction_id(previousDataItem.theWindDirection, storyParams.theVar) !=
			   wind_direction_id(currentDataItem.theWindDirection, storyParams.theVar) &&
			   wind_direction_id(currentDataItem.theWindDirection, storyParams.theVar) == VAIHTELEVA)
			  {
				currentDataItem.theWindDirection = previousDataItem.theWindDirection;
			  }
		  }
	  }
	*/
  }
    
  void find_out_wind_speed_periods(wo_story_params& storyParams)
								   
  {
	unsigned int equalizedDataIndex;
	WeatherArea::Type areaType(storyParams.theArea.type());

	vector<unsigned int>& equalizedWSIndexesMedian = storyParams.equalizedWSIndexesMedian(areaType);

	if(equalizedWSIndexesMedian.size() == 0)
	  return;
	else if(equalizedWSIndexesMedian.size() == 1)
	  {
		equalizedDataIndex = equalizedWSIndexesMedian[0];
		const WindDataItemUnit& dataItem = (*storyParams.theWindDataVector[equalizedDataIndex])(areaType);
		storyParams.theWindSpeedVector.push_back(new WindSpeedPeriodDataItem(dataItem.thePeriod,
																			 wind_speed_id(dataItem.theEqualizedMedianWindSpeed)));
		return;
	  }

	unsigned int periodStartEqualizedDataIndex = equalizedWSIndexesMedian[0];
	unsigned int periodEndEqualizedDataIndex = equalizedWSIndexesMedian[0];
	const WindDataItemUnit& dataItemFirst = (*storyParams.theWindDataVector[periodStartEqualizedDataIndex])(areaType);

	WindSpeedId previous_wind_speed_id(wind_speed_id(dataItemFirst.theEqualizedMedianWindSpeed));	  

	for(unsigned int i = 1; i < equalizedWSIndexesMedian.size(); i++)
	  {
		equalizedDataIndex = equalizedWSIndexesMedian[i];
		const WindDataItemUnit& dataItemCurrent = (*storyParams.theWindDataVector[equalizedDataIndex])(areaType);
  
		WindSpeedId current_wind_speed_id(wind_speed_id(dataItemCurrent.theEqualizedMedianWindSpeed));
		
		if(current_wind_speed_id != previous_wind_speed_id)
		  {
			periodEndEqualizedDataIndex = equalizedWSIndexesMedian[i-1];
			const WindDataItemUnit& dataItemPrevious = (*storyParams.theWindDataVector[periodStartEqualizedDataIndex])(areaType);
			
			NFmiTime periodStartTime(dataItemPrevious.thePeriod.localStartTime());
			NFmiTime periodEndTime(dataItemCurrent.thePeriod.localStartTime());
			periodEndTime.ChangeByHours(-1);

			WeatherPeriod windSpeedPeriod(periodStartTime, periodEndTime);

			storyParams.theWindSpeedVector.push_back(new WindSpeedPeriodDataItem(windSpeedPeriod, previous_wind_speed_id));
			periodStartEqualizedDataIndex = equalizedDataIndex;
			previous_wind_speed_id = current_wind_speed_id;

		  }
	  }
	const WindDataItemUnit& dataItemBeforeLast = (*storyParams.theWindDataVector[periodStartEqualizedDataIndex])(areaType);
	const WindDataItemUnit& dataItemLast = (*storyParams.theWindDataVector[storyParams.theWindDataVector.size()-1])(areaType);
	
	WeatherPeriod windSpeedPeriod(dataItemBeforeLast.thePeriod.localStartTime(),
								  dataItemLast.thePeriod.localEndTime());

	storyParams.theWindSpeedVector.push_back(new WindSpeedPeriodDataItem(windSpeedPeriod, previous_wind_speed_id));

  }
  
#ifdef LATER
  void find_out_wind_direction_periods(wo_story_params& storyParams)
  {
	unsigned int equalizedDataIndex;
	const string& areaPart = storyParams.theAreaPart;

	const vector<unsigned int>& indexVector = storyParams.equalizedWDIndexes(areaPart);

	if(indexVector.size() == 0)
	  return;
	else if(indexVector.size() == 1)
	  {
		equalizedDataIndex = indexVector[0];
		const WindDataItemUnit& dataItem = (*storyParams.theWindDataVector[equalizedDataIndex])(areaPart);
		storyParams.theWindDirectionVector.push_back(new WindDirectionPeriodDataItem(dataItem.thePeriod,
																					 wind_direction_id(dataItem.theEqualizedWindDirection, storyParams.theVar)));
		return;
	  }

	unsigned int periodStartEqualizedDataIndex = indexVector[0];
	unsigned int periodEndEqualizedDataIndex = indexVector[0];
	const WindDataItemUnit& dataItemFirst = (*storyParams.theWindDataVector[periodStartEqualizedDataIndex])(areaPart);
	WindDirectionId previous_wind_direction_id(wind_direction_id(dataItemFirst.theEqualizedWindDirection, 
																	   storyParams.theVar));
	cout << dataItemFirst.thePeriod.localStartTime() 
		 << "..." 
		 << dataItemFirst.thePeriod.localEndTime() 
		 << ": " << dataItemFirst.theEqualizedWindDirection.value()
		 << "; error: " << dataItemFirst.theEqualizedWindDirection.error()
		 << endl;

	for(unsigned int i = 1; i < indexVector.size(); i++)
	  {
		equalizedDataIndex = indexVector[i];
		const WindDataItemUnit& dataItemCurrent = (*storyParams.theWindDataVector[equalizedDataIndex])(areaPart);
  
		cout << dataItemCurrent.thePeriod.localStartTime() 
			 << "..." 
			 << dataItemCurrent.thePeriod.localEndTime() 
			 << ": " << dataItemCurrent.theEqualizedWindDirection.value()
			 << "; error: " << dataItemCurrent.theEqualizedWindDirection.error()
			 << endl;

		WindDirectionId current_wind_direction_id(wind_direction_id(dataItemCurrent.theEqualizedWindDirection, 
																		   storyParams.theVar));
		
		if(current_wind_direction_id != previous_wind_direction_id)
		  {
			periodEndEqualizedDataIndex = indexVector[i-1];
			const WindDataItemUnit& dataItemPrevious = (*storyParams.theWindDataVector[periodStartEqualizedDataIndex])(areaPart);

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
	const WindDataItemUnit& dataItemBeforeLast = (*storyParams.theWindDataVector[periodStartEqualizedDataIndex])(areaPart);
	const WindDataItemUnit& dataItemLast = (*storyParams.theWindDataVector[storyParams.theWindDataVector.size()-1])(areaPart);
	
	WeatherPeriod windDirectionPeriod(dataItemBeforeLast.thePeriod.localStartTime(),
									  dataItemLast.thePeriod.localEndTime());

	storyParams.theWindDirectionVector.push_back(new WindDirectionPeriodDataItem(windDirectionPeriod, 
																				 previous_wind_direction_id));
  }
#endif

  void find_out_wind_direction_periods(wo_story_params& storyParams)
  {
	WeatherArea::Type areaType(storyParams.theArea.type());

	const wind_data_item_vector& dataVector = storyParams.theWindDataVector;

	if(dataVector.size() == 0)
	  return;
	else if(dataVector.size() == 1)
	  {
		const WindDataItemUnit& dataItem = (*dataVector[0])(areaType);

		storyParams.theWindDirectionVector.push_back(new WindDirectionPeriodDataItem(dataItem.thePeriod,
																					 wind_direction_id(dataItem.theEqualizedWindDirection, storyParams.theVar)));
		return;
	  }

	const WindDataItemUnit& dataItemFirst = (*dataVector[0])(areaType);
	WindDirectionId previous_wind_direction_id(wind_direction_id(dataItemFirst.theEqualizedWindDirection, 
																	   storyParams.theVar));
	/*
	cout << dataItemFirst.thePeriod.localStartTime() 
		 << "..." 
		 << dataItemFirst.thePeriod.localEndTime() 
		 << ": " << dataItemFirst.theEqualizedWindDirection.value()
		 << "; error: " << dataItemFirst.theEqualizedWindDirection.error()
		 << endl;
	*/
	NFmiTime periodStartTime(dataItemFirst.thePeriod.localStartTime());
	NFmiTime periodEndTime(dataItemFirst.thePeriod.localStartTime());

	for(unsigned int i = 1; i < dataVector.size(); i++)
	  {
		const WindDataItemUnit& dataItemCurrent = (*dataVector[i])(areaType);
  
		WindDirectionId current_wind_direction_id(wind_direction_id(dataItemCurrent.theEqualizedWindDirection, 
																		  storyParams.theVar));

		/*
		cout << "current period: " << dataItemCurrent.thePeriod.localStartTime() 
			 << "..." 
			 << dataItemCurrent.thePeriod.localEndTime() 
			 << ": " << dataItemCurrent.theEqualizedWindDirection.value()
			 << "; error: " << dataItemCurrent.theEqualizedWindDirection.error()
			 << "; direction: " << wind_direction_string(current_wind_direction_id)
			 << endl;
		*/
		if(current_wind_direction_id != previous_wind_direction_id)
		  {
			if(i < dataVector.size() - 1)
			  {
				const WindDataItemUnit& dataItemPrevious = (*dataVector[i-1])(areaType);
				const WindDataItemUnit& dataItemNext = (*dataVector[i+1])(areaType);
				if(wind_direction_id(dataItemPrevious.theEqualizedWindDirection, 
										 storyParams.theVar) == 
				   wind_direction_id(dataItemNext.theEqualizedWindDirection, 
										 storyParams.theVar))
				  {
					//					cout << "previous: " << wind_direction_string(wind_direction_id(dataItemPrevious.theEqualizedWindDirection, storyParams.theVar)) << endl;
					//					cout << "next: " << wind_direction_string(wind_direction_id(dataItemNext.theEqualizedWindDirection, storyParams.theVar)) << endl;
					
					continue;
				  }
			  }

			periodEndTime = dataItemCurrent.thePeriod.localStartTime();
			periodEndTime.ChangeByHours(-1);

			//	cout << "ADD0: " << periodStartTime << "..." << periodEndTime << endl;

			WeatherPeriod windDirectionPeriod(periodStartTime, periodEndTime);

			storyParams.theWindDirectionVector.push_back(new WindDirectionPeriodDataItem(windDirectionPeriod, previous_wind_direction_id));
			previous_wind_direction_id = current_wind_direction_id;

			periodStartTime = periodEndTime;
			periodStartTime.ChangeByHours(1);
		  }
	  }
	if(periodEndTime != dataItemFirst.thePeriod.localStartTime())
	  {
		periodEndTime.ChangeByHours(1);
	  }

	//	const WindDataItemUnit& dataItemBeforeLast = (*dataVector[storyParams.theWindDataVector.size()-2])(areaPart);
	const WindDataItemUnit& dataItemLast = (*dataVector[storyParams.theWindDataVector.size()-1])(areaType);

	WeatherPeriod windDirectionLastPeriod(periodEndTime, dataItemLast.thePeriod.localEndTime());


	/*
	WeatherPeriod windDirectionPeriod(dataItemBeforeLast.thePeriod.localStartTime(),
									  dataItemLast.thePeriod.localEndTime());
	*/

	//cout << "ADD1: " << windDirectionLastPeriod.localStartTime() << "..." << windDirectionLastPeriod.localEndTime() << endl;
	storyParams.theWindDirectionVector.push_back(new WindDirectionPeriodDataItem(windDirectionLastPeriod, 
																				 previous_wind_direction_id));


  }

  WindEventId get_wind_speed_event(const WeatherResult& windSpeed1, 
									 const WeatherResult& windSpeed2,
									 const double& windSpeedThreshold)
  {
	double difference = windSpeed2.value() - windSpeed1.value();

	if(abs(difference) <= windSpeedThreshold)
	  return MISSING_WIND_SPEED_EVENT;
	else if(windSpeed2.value() >= 0.0 &&
			windSpeed2.value() < 0.5)
	  return TUULI_TYYNTYY;
	else if(difference < 0.0)
	  return TUULI_HEIKKENEE;
	else
	  return TUULI_VOIMISTUU;
  }

  WindEventId get_wind_direction_event(const WeatherResult& windDirection1, 
										 const WeatherResult& windDirection2,
										 const string& var,
										 const double& windDirectionThreshold)
  {
	WindDirectionId direction1 = wind_direction_id(windDirection1, var);
	WindDirectionId direction2 = wind_direction_id(windDirection2, var);

	if(direction1 != VAIHTELEVA && direction2 == VAIHTELEVA)
	  return TUULI_MUUTTUU_VAIHTELEVAKSI;
	else if(direction1 == direction2)
	  return MISSING_WIND_DIRECTION_EVENT;
	else if(direction1 == VAIHTELEVA)
	  return TUULI_KAANTYY;

	float dire1 = windDirection1.value();
	float dire2 = windDirection2.value();

	float difference = abs(dire2 - dire1);
	if(difference > 180.0)
	  difference = abs((dire2 > dire1 ? dire2 - (dire1+360) : dire1 - (dire2+360)));


	//	cout << "dire1: " << dire1 << "; dire2: " << dire2 << "; difference: " << difference << endl;

	return (difference > windDirectionThreshold && direction1 != direction2  ? TUULI_KAANTYY : MISSING_WIND_DIRECTION_EVENT);
  }

  void find_out_wind_speed_event_periods(wo_story_params& storyParams)
  {
	const vector<unsigned int>& theEqualizedIndexes = storyParams.equalizedWSIndexesMaximum(storyParams.theArea.type());
	WeatherArea::Type areaType(storyParams.theArea.type());

	if(theEqualizedIndexes.size() == 0)
	  {
		return;
	  }
	else if(theEqualizedIndexes.size() == 1)
	  {
		unsigned int dataIndex = theEqualizedIndexes[0];
		const WindDataItemUnit& dataItem = (*storyParams.theWindDataVector[dataIndex])(areaType);

		
		storyParams.theWindEventPeriodVector.push_back(new WindEventPeriodDataItem(dataItem.thePeriod, 
																				   MISSING_WIND_SPEED_EVENT,
																				   dataItem,
																				   dataItem));
		storyParams.theWindSpeedEventPeriodVector.push_back(new WindEventPeriodDataItem(dataItem.thePeriod, 
																						MISSING_WIND_SPEED_EVENT,
																						dataItem,
																						dataItem));
		return;
	  }
	for(unsigned int i = 1; i < theEqualizedIndexes.size(); i++)
	  {

		unsigned int periodBeginDataIndex = theEqualizedIndexes[i-1];
		unsigned int periodEndDataIndex = theEqualizedIndexes[i];
		
		// find the data item for the period start and end
		//		unsigned int begIndex = (i == 1 ? periodBeginDataIndex : periodBeginDataIndex + 1);
		const WindDataItemUnit& dataItemPeriodBegin = (*storyParams.theWindDataVector[periodBeginDataIndex])(areaType);
		const WindDataItemUnit& dataItemPeriodEnd = (*storyParams.theWindDataVector[periodEndDataIndex])(areaType);
												   
		// define the event period
		WeatherPeriod windEventPeriod(dataItemPeriodBegin.thePeriod.localStartTime(), 
									  dataItemPeriodEnd.thePeriod.localStartTime());
		WindEventId windEvent(MISSING_WIND_SPEED_EVENT);
		
		windEvent = get_wind_speed_event(dataItemPeriodBegin.theWindMaximum,
										 dataItemPeriodEnd.theWindMaximum,
										 storyParams.theWindSpeedThreshold);

		// merge the similar wind events
		if(storyParams.theWindEventPeriodVector.size() > 0)
		  {
			WindEventPeriodDataItem* previousEventPeriod = storyParams.theWindEventPeriodVector[storyParams.theWindEventPeriodVector.size()-1];

			if(previousEventPeriod->theWindEvent == windEvent)
			  {
				WeatherPeriod mergedPeriod(previousEventPeriod->thePeriod.localStartTime(),
										   windEventPeriod.localEndTime());
				WindEventPeriodDataItem* newEventPeriod = new WindEventPeriodDataItem(mergedPeriod,
																					  windEvent,
																					  previousEventPeriod->thePeriodBeginDataItem,
																					  dataItemPeriodEnd);
		
				delete previousEventPeriod;
				storyParams.theWindEventPeriodVector.erase(storyParams.theWindEventPeriodVector.begin()+storyParams.theWindEventPeriodVector.size()-1);

				storyParams.theWindSpeedEventPeriodVector.erase(storyParams.theWindSpeedEventPeriodVector.begin()+storyParams.theWindSpeedEventPeriodVector.size()-1);

				storyParams.theWindEventPeriodVector.push_back(newEventPeriod);
				
				storyParams.theWindSpeedEventPeriodVector.push_back(newEventPeriod);

				continue;
			  }

		  }

		storyParams.theWindEventPeriodVector.push_back(new WindEventPeriodDataItem(windEventPeriod,
																				   windEvent,
																				   dataItemPeriodBegin,
																				   dataItemPeriodEnd));

		storyParams.theWindSpeedEventPeriodVector.push_back(new WindEventPeriodDataItem(windEventPeriod,
																						windEvent,
																						dataItemPeriodBegin,
																						dataItemPeriodEnd));
	  }
  }


  void find_out_wind_direction_event_periods(wo_story_params& storyParams)
  {
	const vector<unsigned int>& theEqualizedIndexes = storyParams.equalizedWDIndexes(storyParams.theArea.type());

	WeatherArea::Type areaType(storyParams.theArea.type());
	
	if(theEqualizedIndexes.size() == 0)
	  {
		return;
	  }
	else if(theEqualizedIndexes.size() == 1)
	  {
		unsigned int dataIndex = theEqualizedIndexes[0];
		const WindDataItemUnit& dataItem = (*storyParams.theWindDataVector[dataIndex])(areaType);
		storyParams.theWindEventPeriodVector.push_back(new WindEventPeriodDataItem(dataItem.thePeriod, 
																				   MISSING_WIND_DIRECTION_EVENT,
																				   dataItem,
																				   dataItem));

		storyParams.theWindDirectionEventPeriodVector.push_back(new WindEventPeriodDataItem(dataItem.thePeriod, 
																							MISSING_WIND_DIRECTION_EVENT,
																							dataItem,
																							dataItem));
		return;
	  }
	
	//	const WindDataItemUnit* pDataItemPeriodPreviousBeg = 0;
	const WindDataItemUnit* dataItemPeriodBegin = 0;
	const WindDataItemUnit* dataItemPeriodEnd = 0;
	unsigned int periodBeginDataIndex(0);
	unsigned int periodEndDataIndex(0);
	WindEventId previousWindEvent(MISSING_WIND_DIRECTION_EVENT);

	for(unsigned int i = 1; i < theEqualizedIndexes.size(); i++)
	  {
		if(dataItemPeriodBegin == 0)
		  periodBeginDataIndex = theEqualizedIndexes[i-1];
		periodEndDataIndex = theEqualizedIndexes[i];
		
		// keep the previous period start time if there was not enough change in direction
		// during the previous period
		if(dataItemPeriodBegin == 0)
		  dataItemPeriodBegin = &(*storyParams.theWindDataVector[periodBeginDataIndex])(areaType);
		dataItemPeriodEnd = &(*storyParams.theWindDataVector[periodEndDataIndex])(areaType);

		// define the event period
		WeatherPeriod windEventPeriod(dataItemPeriodBegin->thePeriod.localStartTime(), 
									  dataItemPeriodEnd->thePeriod.localStartTime());

		WindEventId windEvent(MISSING_WIND_DIRECTION_EVENT);
		
		windEvent = get_wind_direction_event(dataItemPeriodBegin->theWindDirection,
											 dataItemPeriodEnd->theWindDirection,
											 storyParams.theVar,
											 storyParams.theWindDirectionThreshold);

		WindDirectionId windDirectionIdBeg = wind_direction_id(dataItemPeriodBegin->theWindDirection, storyParams.theVar);
	
		//		WindDirectionId windDirectionIdEnd = wind_direction_id(dataItemPeriodEnd->theWindDirection, storyParams.theVar);

		/*
		cout << "area: " << storyParams.theAreaName << endl;
		cout << "windEvent: " << get_wind_event_string(windEvent) << endl;
		cout << "period: " << windEventPeriod.localStartTime() << "..." << windEventPeriod.localEndTime() << endl;
		cout << "wind direction beg: " << wind_direction_string(windDirectionIdBeg) << endl;
		cout << "wind direction end: " << wind_direction_string(windDirectionIdEnd) << endl;
		*/
		// merge the similar wind events
		if(storyParams.theWindEventPeriodVector.size() > 0 && windEvent != MISSING_WIND_DIRECTION_EVENT)
		  {
			WindEventPeriodDataItem* previousEventPeriod = storyParams.theWindEventPeriodVector[storyParams.theWindEventPeriodVector.size()-1];

			if(previousEventPeriod->theWindEvent == windEvent)
			  {
				WeatherPeriod mergedPeriod(previousEventPeriod->thePeriod.localStartTime(),
										   windEventPeriod.localEndTime());
				WindEventPeriodDataItem* newEventPeriod = new WindEventPeriodDataItem(mergedPeriod,
																					  previousEventPeriod->theWindEvent,//windEvent,
																					  previousEventPeriod->thePeriodBeginDataItem,
																					  *dataItemPeriodEnd);
		
				delete previousEventPeriod;
				storyParams.theWindEventPeriodVector.erase(storyParams.theWindEventPeriodVector.begin()+storyParams.theWindEventPeriodVector.size()-1);

				storyParams.theWindDirectionEventPeriodVector.erase(storyParams.theWindDirectionEventPeriodVector.begin()+storyParams.theWindDirectionEventPeriodVector.size()-1);

				storyParams.theWindEventPeriodVector.push_back(newEventPeriod);
				
				storyParams.theWindDirectionEventPeriodVector.push_back(newEventPeriod);

				previousWindEvent = windEvent;
				continue;
			  }

		  }
		/*
		if(windEvent == TUULI_MUUTTUU_VAIHTELEVAKSI)
		  {

			continue;
		  }
		*/

		if(windEvent != MISSING_WIND_DIRECTION_EVENT)
		  {
			// wind direction does not change in the beginning
			// find out when it starts to change
			if(previousWindEvent == MISSING_WIND_DIRECTION_EVENT && i > 1)
			  {
				unsigned startIndex = 0;

				while(&((*storyParams.theWindDataVector[startIndex])(areaType)) != dataItemPeriodBegin &&
					  startIndex < storyParams.theWindDataVector.size())
				  startIndex++;

				for(unsigned k = startIndex; k < storyParams.theWindDataVector.size(); k++ )
				  {
					float windDirectionAtPeriodStart(((*storyParams.theWindDataVector[startIndex])(areaType)).theEqualizedWindDirection.value());
					float windDirectionCurrent(((*storyParams.theWindDataVector[k])(areaType)).theEqualizedWindDirection.value());
					float diff = abs(windDirectionCurrent - windDirectionAtPeriodStart);

					WindDirectionId windDirectionIdCurrent = wind_direction_id(((*storyParams.theWindDataVector[k])(areaType)).theEqualizedWindDirection, storyParams.theVar);

					// after direction has turned 25 degrees we say that wind has started to turn
					if(diff >= 25.0 || 
					   (windDirectionIdBeg == VAIHTELEVA &&
						windDirectionIdCurrent != VAIHTELEVA) ||
					   (windDirectionIdBeg != VAIHTELEVA &&
						windDirectionIdCurrent == VAIHTELEVA))
					  {
						NFmiTime intermediateTime(((*storyParams.theWindDataVector[k])(areaType)).thePeriod.localStartTime());
						/*
						cout << "windEventPeriod.localStartTime(): " << windEventPeriod.localStartTime() << endl;
						cout << "intermediateTime: " << intermediateTime << endl;
						cout << "windEventPeriod.localEndTime(): " << windEventPeriod.localEndTime() << endl;
						*/
						if(intermediateTime.DifferenceInHours(windEventPeriod.localStartTime()) > 0)
						  intermediateTime.ChangeByHours(-1);

						//cout << "intermediateTime1: " << intermediateTime << endl;
						//cout << "windEventPeriod.localStartTime()2: " << windEventPeriod.localStartTime() << endl;

						WeatherPeriod period1(windEventPeriod.localStartTime(), intermediateTime);
						intermediateTime.ChangeByHours(+1);

						//cout << "intermediateTime2: " << intermediateTime << endl;
						//cout << "windEventPeriod.localEndTime()2: " << windEventPeriod.localEndTime() << endl;

						WeatherPeriod period2(intermediateTime, windEventPeriod.localEndTime());
						
						//cout << "period1: " << period1.localStartTime() << "..." << period1.localEndTime() << endl;
						//cout << "period2: " << period2.localStartTime() << "..." << period2.localEndTime() << endl;

						const WindDataItemUnit* dataItemBegin1 = dataItemPeriodBegin;
						const WindDataItemUnit* dataItemBegin2 = &(*storyParams.theWindDataVector[k-1])(areaType);
						const WindDataItemUnit* dataItemEnd1 = &(*storyParams.theWindDataVector[k])(areaType);
						const WindDataItemUnit* dataItemEnd2 = dataItemPeriodEnd;

						WindEventPeriodDataItem* period1WDEItem = new WindEventPeriodDataItem(period1,
																							  MISSING_WIND_DIRECTION_EVENT,//windEvent,
																							  *dataItemBegin1,
																							  *dataItemEnd1);
						WindEventPeriodDataItem* period2WDEItem = new WindEventPeriodDataItem(period2,
																							  windEvent,
																							  *dataItemBegin2,
																							  *dataItemEnd2);
						
						
						storyParams.theWindEventPeriodVector.push_back(period1WDEItem);
						storyParams.theWindEventPeriodVector.push_back(period2WDEItem);
						storyParams.theWindDirectionEventPeriodVector.push_back(period1WDEItem);
						storyParams.theWindDirectionEventPeriodVector.push_back(period2WDEItem);
						break;
					  }
				  }
			  }
			else
			  {
				storyParams.theWindEventPeriodVector.push_back(new WindEventPeriodDataItem(windEventPeriod,
																						   windEvent,
																						   *dataItemPeriodBegin,
																						   *dataItemPeriodEnd));
				
				storyParams.theWindDirectionEventPeriodVector.push_back(new WindEventPeriodDataItem(windEventPeriod,
																									windEvent,
																									*dataItemPeriodBegin,
																									*dataItemPeriodEnd));
			  }
			dataItemPeriodBegin = 0;
		  }

		previousWindEvent = windEvent;
	  }
  }

  void find_out_wind_direction_event_periods2(wo_story_params& storyParams)
  {
	const vector<unsigned int>& theEqualizedIndexes = storyParams.equalizedWDIndexes(storyParams.theArea.type());
	
	WindEventId missingWindEvent(MISSING_WIND_DIRECTION_EVENT);

	if(theEqualizedIndexes.size() == 0)
	  {
		return;
	  }
	else if(theEqualizedIndexes.size() == 1)
	  {
		unsigned int dataIndex = theEqualizedIndexes[0];
		const WindDataItemUnit& dataItem = (*storyParams.theWindDataVector[dataIndex])(storyParams.theArea.type());
		storyParams.theWindEventPeriodVector.push_back(new WindEventPeriodDataItem(dataItem.thePeriod, 
																				   missingWindEvent,
																				   dataItem,
																				   dataItem));
		storyParams.theWindDirectionEventPeriodVector.push_back(new WindEventPeriodDataItem(dataItem.thePeriod, 
																							missingWindEvent,
																							dataItem,
																							dataItem));
		return;
	  }

	// iterate through the equalized indexes
	for(unsigned int i = 1; i < theEqualizedIndexes.size(); i++)
	  {
		unsigned int periodBeginDataIndex = theEqualizedIndexes[i-1];
		unsigned int periodEndDataIndex = theEqualizedIndexes[i];
		
		// find the data item for the period start and end
		//unsigned int begIndex = (i == 1 ? periodBeginDataIndex : periodBeginDataIndex + 1);
		const WindDataItemUnit& dataItemPeriodBegin = (*storyParams.theWindDataVector[periodBeginDataIndex])(storyParams.theArea.type());
		const WindDataItemUnit& dataItemPeriodEnd = (*storyParams.theWindDataVector[periodEndDataIndex])(storyParams.theArea.type());

		//		WindDirectionId windDirectionIdBeg(wind_direction_id(dataItemPeriodBegin.theWindDirection, storyParams.theVar));
		//		WindDirectionId windDirectionIdEnd(wind_direction_id(dataItemPeriodEnd.theWindDirection, storyParams.theVar));												   
		// define the event period
		WeatherPeriod windEventPeriod(dataItemPeriodBegin.thePeriod.localStartTime(), 
									  dataItemPeriodEnd.thePeriod.localStartTime());
		WindEventId windEvent(MISSING_WIND_DIRECTION_EVENT);
		
		windEvent = get_wind_direction_event(dataItemPeriodBegin.theWindDirection,
											 dataItemPeriodEnd.theWindDirection,
											 storyParams.theVar,
											 storyParams.theWindDirectionThreshold);

		// merge the similar wind events
		if(storyParams.theWindEventPeriodVector.size() > 0)
		  {
			WindEventPeriodDataItem* previousEventPeriod = storyParams.theWindEventPeriodVector[storyParams.theWindEventPeriodVector.size()-1];

			//			WindDirectionId prevDirectionId(wind_direction_id(previousEventPeriod->thePeriodEndDataItem.theWindDirection, storyParams.theVar));

			if(previousEventPeriod->theWindEvent == windEvent)
			  /* &&
			  prevDirectionId == windDirectionIdBeg &&
			  windDirectionIdBeg == windDirectionIdEnd)*/
			  {
				WeatherPeriod mergedPeriod(previousEventPeriod->thePeriod.localStartTime(),
										   windEventPeriod.localEndTime());
				WindEventPeriodDataItem* newEventPeriod = new WindEventPeriodDataItem(mergedPeriod,
																					  previousEventPeriod->theWindEvent,//windEvent,
																					  previousEventPeriod->thePeriodBeginDataItem,
																					  dataItemPeriodEnd);

				delete previousEventPeriod;
				storyParams.theWindEventPeriodVector.erase(storyParams.theWindEventPeriodVector.begin()+storyParams.theWindEventPeriodVector.size()-1);

				storyParams.theWindDirectionEventPeriodVector.erase(storyParams.theWindDirectionEventPeriodVector.begin()+storyParams.theWindDirectionEventPeriodVector.size()-1);

				storyParams.theWindEventPeriodVector.push_back(newEventPeriod);
				
				storyParams.theWindDirectionEventPeriodVector.push_back(newEventPeriod);
				continue;
			  }

		  }
#ifdef LATER
		if(windDirectionIdBeg == VAIHTELEVA &&
		   windDirectionIdEnd != VAIHTELEVA)
		  {
			// TODO: tsekkaa
			const WindDataItemUnit& dataItemPeriodBeg = (*storyParams.theWindDataVector[periodBeginDataIndex+1])(storyParams.theArea.type());
			windEvent = get_wind_direction_event(dataItemPeriodBeg.theWindDirection,
												 dataItemPeriodEnd.theWindDirection,
												 storyParams.theVar,
												 storyParams.theWindDirectionThreshold);

			WeatherPeriod period(dataItemPeriodBeg.thePeriod.localStartTime(), windEventPeriod.localEndTime());
			storyParams.theWindEventPeriodVector.push_back(new WindEventPeriodDataItem(period,
																					   windEvent,
																					   dataItemPeriodBeg,
																					   dataItemPeriodEnd));
			
			storyParams.theWindDirectionEventPeriodVector.push_back(new WindEventPeriodDataItem(windEventPeriod,
																								windEvent,
																								dataItemPeriodBegin,
																								dataItemPeriodEnd));			
		  }
		else
#endif
		  {
			//		cout <<  "new event: " << get_wind_event_string(windEvent) << endl;
			storyParams.theWindEventPeriodVector.push_back(new WindEventPeriodDataItem(windEventPeriod,
																					   windEvent,
																					   dataItemPeriodBegin,
																					   dataItemPeriodEnd));

			storyParams.theWindDirectionEventPeriodVector.push_back(new WindEventPeriodDataItem(windEventPeriod,
																								windEvent,
																								dataItemPeriodBegin,
																								dataItemPeriodEnd));
		  }
	  }

	// after merging several missing wind direction events it may be that 
	// direction has changed gradually
	for(unsigned int i = 0; i < storyParams.theWindDirectionEventPeriodVector.size(); i++)
	  {
		WindEventPeriodDataItem* item = storyParams.theWindDirectionEventPeriodVector[i];
		if(item->theWindEvent == MISSING_WIND_DIRECTION_EVENT)
		  {
			storyParams.theWindDirectionEventPeriodVector[i]->theWindEvent = 
			  get_wind_direction_event(item->thePeriodBeginDataItem.theWindDirection,
									   item->thePeriodEndDataItem.theWindDirection,
									   storyParams.theVar,
									   storyParams.theWindDirectionThreshold);
			
			//			cout <<  "changed missing direction period to: " << get_wind_event_string(storyParams.theWindDirectionEventPeriodVector[i]->theWindEvent) << endl;
		  }
	  }

  }


  // check weather the wind is only some hours in some direction
  // only vaihteleva wind checked
  void find_out_transient_wind_direction_periods(wo_story_params& storyParams)
  {
	for(unsigned int i = 0; i < storyParams.theWindEventPeriodVector.size(); i++)
	  {
		WindEventPeriodDataItem* eventItem(storyParams.theWindEventPeriodVector[i]);
		WindEventId windEventCurrent = eventItem->theWindEvent;

		if(windEventCurrent & TUULI_MUUTTUU_VAIHTELEVAKSI && get_period_length(eventItem->thePeriod) <= 3)
		  {
			for(unsigned int k = i+1; k < storyParams.theWindEventPeriodVector.size(); k++)
			  {
				WindEventId windEventLater = storyParams.theWindEventPeriodVector[k]->theWindEvent;
				if(windEventLater & TUULI_KAANTYY)
				  {
					storyParams.theWindEventPeriodVector[i]->theTransientFlag = true;
					break;
				  }
			  }
		  }
	  }
  }

  WindEventId sort_out_wind_event2(const WindEventId& windEvent1, 
									  const WindEventId& windEvent2)
  {
	WindEventId eventId1 = (windEvent1 == MISSING_WIND_SPEED_EVENT || windEvent1 == MISSING_WIND_DIRECTION_EVENT ?
							  MISSING_WIND_EVENT : eventId1);
	WindEventId eventId2 = (windEvent1 == MISSING_WIND_SPEED_EVENT || windEvent2 == MISSING_WIND_DIRECTION_EVENT ?
							  MISSING_WIND_EVENT : eventId2);

	return static_cast<WindEventId>(windEvent1 | windEvent2);
  }

  WindEventId sort_out_wind_event(const WindEventId& windDirectionEvent, 
									const WindEventId& windSpeedEvent)
  {
	if(windDirectionEvent == MISSING_WIND_DIRECTION_EVENT)
	  {
		if(windSpeedEvent == MISSING_WIND_SPEED_EVENT)
		  return MISSING_WIND_EVENT;
		else
		  return windSpeedEvent;
	  }
	
	if(windSpeedEvent == MISSING_WIND_SPEED_EVENT)
	  {
		if(windDirectionEvent == MISSING_WIND_DIRECTION_EVENT)
		  return MISSING_WIND_EVENT;
		else
		  return windDirectionEvent;
	  }
	

	return static_cast<WindEventId>(windDirectionEvent | windSpeedEvent);

  }

  void add_merged_period(const WeatherPeriod& thePeriod,
						 const WindEventId& theWindEvent,
						 const WindDataItemUnit& thePeriodBeginDataItem,
						 const WindDataItemUnit& thePeriodEndDataItem,
						 wind_event_period_data_item_vector& theResultVector)
  {
	if(thePeriod.localEndTime() > thePeriod.localStartTime())
	  {
		theResultVector.push_back(new WindEventPeriodDataItem(thePeriod,
															  theWindEvent,
															  thePeriodBeginDataItem,
															  thePeriodEndDataItem));
	  }
  }

  WindEventPeriodDataItem* sort_out_periods(const WindEventPeriodDataItem& theDirectionEventItem, 
											const WindEventPeriodDataItem& theSpeedEventItem, 
											wind_event_period_data_item_vector& theResultVector)
  {

	WindEventId windSpeedEvent = (theSpeedEventItem.theWindEvent == MISSING_WIND_SPEED_EVENT ?
									   MISSING_WIND_EVENT : theSpeedEventItem.theWindEvent);
	WindEventId windDirectionEvent = (theDirectionEventItem.theWindEvent == MISSING_WIND_DIRECTION_EVENT ?
										   MISSING_WIND_EVENT : theDirectionEventItem.theWindEvent);
	WindEventId compositeWindEvent = sort_out_wind_event(theDirectionEventItem.theWindEvent,
														   theSpeedEventItem.theWindEvent);


	if(same_period(theDirectionEventItem.thePeriod, theSpeedEventItem.thePeriod))
	  {
		// direction and speed periods are the same
		// |------------|   direction
		// |------------|   speed
		// |------------|   speed-direction
		WeatherPeriod period(theSpeedEventItem.thePeriod.localStartTime(), 
							 theSpeedEventItem.thePeriod.localEndTime());
		
		if(theDirectionEventItem.theWindEvent == MISSING_WIND_DIRECTION_EVENT)
		  {
			add_merged_period(period,
							  windSpeedEvent,
							  theSpeedEventItem.thePeriodBeginDataItem,
							  theSpeedEventItem.thePeriodEndDataItem,
							  theResultVector);
		  }
		else if(theSpeedEventItem.theWindEvent == MISSING_WIND_SPEED_EVENT)
		  {
			add_merged_period(period,
							  windDirectionEvent,
							  theDirectionEventItem.thePeriodBeginDataItem,
							  theDirectionEventItem.thePeriodEndDataItem,
							  theResultVector);
		  }
		else
		  {
			add_merged_period(period,
							  compositeWindEvent,
							  theDirectionEventItem.thePeriodBeginDataItem,
							  theDirectionEventItem.thePeriodEndDataItem,
							  theResultVector);
		  }
		return 0;
	  }

	if(is_inside(theDirectionEventItem.thePeriod,
				 theSpeedEventItem.thePeriod))
	  {
		// direction period is inside speed period
		//    |------|      direction
		// |------------|   speed
		// |--|------|--|   speed, speed-direction, returned period (speed)

		// if both are missing event periods
		if(theDirectionEventItem.theWindEvent == MISSING_WIND_DIRECTION_EVENT)
		  {
			WeatherPeriod period(theSpeedEventItem.thePeriod.localStartTime(), 
								 theDirectionEventItem.thePeriod.localEndTime());

			add_merged_period(period,
							  windSpeedEvent,
							  theSpeedEventItem.thePeriodBeginDataItem,
							  theDirectionEventItem.thePeriodEndDataItem,
							  theResultVector);
		  }
		else
		  {
			WeatherPeriod period1(theSpeedEventItem.thePeriod.localStartTime(), 
								  theDirectionEventItem.thePeriod.localStartTime());

			WeatherPeriod period2(theDirectionEventItem.thePeriod.localStartTime(),
								  theDirectionEventItem.thePeriod.localEndTime());
			
			add_merged_period(period1,
							  windSpeedEvent,
							  theSpeedEventItem.thePeriodBeginDataItem,
							  theDirectionEventItem.thePeriodBeginDataItem,
							  theResultVector);

			add_merged_period(period2,
							  compositeWindEvent,
							  theDirectionEventItem.thePeriodBeginDataItem,
							  theDirectionEventItem.thePeriodEndDataItem,
							  theResultVector);
		  }

		if(theSpeedEventItem.thePeriod.localEndTime() >
		   theDirectionEventItem.thePeriod.localEndTime())
		  {
			WeatherPeriod returnPeriod(theDirectionEventItem.thePeriod.localEndTime(),
									   theSpeedEventItem.thePeriod.localEndTime());
			
			return new WindEventPeriodDataItem(returnPeriod,
											   theSpeedEventItem.theWindEvent,
											   theDirectionEventItem.thePeriodEndDataItem,
											   theSpeedEventItem.thePeriodEndDataItem);
		  }
		else
		  {
			return 0;
		  }
	  }


	if(is_inside(theDirectionEventItem.thePeriod,
				 theSpeedEventItem.thePeriod))
	  {
		// speed period is inside direction period
		// |------------|   direction
		//    |------|      speed
		// |--|------|--|   direction, direction-speed, returned period (direction)

		// if both are missing event periods
		if(theSpeedEventItem.theWindEvent == MISSING_WIND_SPEED_EVENT)
		  {

			WeatherPeriod period(theDirectionEventItem.thePeriod.localStartTime(), 
								 theSpeedEventItem.thePeriod.localEndTime());
			
			add_merged_period(period,
							  windDirectionEvent,
							  theDirectionEventItem.thePeriodBeginDataItem,
							  theSpeedEventItem.thePeriodEndDataItem,
							  theResultVector);
		  }
		else
		  {
			WeatherPeriod period1(theDirectionEventItem.thePeriod.localStartTime(), 
								  theSpeedEventItem.thePeriod.localStartTime());
			
			WeatherPeriod period2(theSpeedEventItem.thePeriod.localStartTime(),
								  theSpeedEventItem.thePeriod.localEndTime());
			add_merged_period(period1,
							  windDirectionEvent,
							  theDirectionEventItem.thePeriodBeginDataItem,
							  theSpeedEventItem.thePeriodBeginDataItem,
							  theResultVector);

			add_merged_period(period2,
							  compositeWindEvent,
							  theSpeedEventItem.thePeriodBeginDataItem,
							  theSpeedEventItem.thePeriodEndDataItem,
							  theResultVector);
		  }

		if(theDirectionEventItem.thePeriod.localEndTime() > 
		   theSpeedEventItem.thePeriod.localEndTime())
		  {
			WeatherPeriod returnPeriod(theSpeedEventItem.thePeriod.localEndTime(),
									   theDirectionEventItem.thePeriod.localEndTime());
			
			return new WindEventPeriodDataItem(returnPeriod,
											   theDirectionEventItem.theWindEvent,
											   theSpeedEventItem.thePeriodEndDataItem,
											   theDirectionEventItem.thePeriodEndDataItem);
		  }
		else
		  {
			return 0;
		  }
	  }

	if(theDirectionEventItem.thePeriod.localEndTime() > theSpeedEventItem.thePeriod.localEndTime())
	  {
		// direction period contimues after speed period ends
		//    |---------|   direction
		// |---------|      speed
		// |--|------|--|   speed, speed-direction, returned period (direction)

		if(theDirectionEventItem.theWindEvent == MISSING_WIND_DIRECTION_EVENT &&
		   theSpeedEventItem.theWindEvent == MISSING_WIND_SPEED_EVENT)
		  {
			WeatherPeriod period(theSpeedEventItem.thePeriod.localStartTime(), 
								 theSpeedEventItem.thePeriod.localEndTime());
			
			add_merged_period(period,
							  MISSING_WIND_EVENT,
							  theSpeedEventItem.thePeriodBeginDataItem,
							  theSpeedEventItem.thePeriodEndDataItem,
							  theResultVector);

			WeatherPeriod returnPeriod(theSpeedEventItem.thePeriod.localEndTime(),
									   theDirectionEventItem.thePeriod.localEndTime());

			return new WindEventPeriodDataItem(returnPeriod,
											   theDirectionEventItem.theWindEvent,
											   theSpeedEventItem.thePeriodEndDataItem,
											   theDirectionEventItem.thePeriodEndDataItem);	
		  }

		if(theDirectionEventItem.thePeriod.localStartTime() > theSpeedEventItem.thePeriod.localStartTime())
		  {
			WeatherPeriod period(theSpeedEventItem.thePeriod.localStartTime(), 
								  theDirectionEventItem.thePeriod.localStartTime());

			add_merged_period(period,
							  windSpeedEvent,
							  theSpeedEventItem.thePeriodBeginDataItem,
							  theDirectionEventItem.thePeriodBeginDataItem,
							  theResultVector);

		  }

		WeatherPeriod period(theDirectionEventItem.thePeriod.localStartTime(),
							 theSpeedEventItem.thePeriod.localEndTime());


		add_merged_period(period,
						  compositeWindEvent,
						  theDirectionEventItem.thePeriodBeginDataItem,
						  theSpeedEventItem.thePeriodEndDataItem,
						  theResultVector);

		WeatherPeriod returnPeriod(theSpeedEventItem.thePeriod.localEndTime(),
								   theDirectionEventItem.thePeriod.localEndTime());

		return new WindEventPeriodDataItem(returnPeriod,
										   theDirectionEventItem.theWindEvent,
										   theSpeedEventItem.thePeriodEndDataItem,
										   theDirectionEventItem.thePeriodEndDataItem);
	  }


	if(theSpeedEventItem.thePeriod.localEndTime() > theDirectionEventItem.thePeriod.localEndTime())
	  {
		// direction period contimues after speed period ends
		// |---------|    direction
		//    |---------| speed
		// |--|------|--| direction, direction-speed, returned period (direction)

		if(theDirectionEventItem.theWindEvent == MISSING_WIND_DIRECTION_EVENT &&
		   theSpeedEventItem.theWindEvent == MISSING_WIND_SPEED_EVENT)
		  {
			WeatherPeriod period(theDirectionEventItem.thePeriod.localStartTime(), 
								 theDirectionEventItem.thePeriod.localEndTime());
			
			add_merged_period(period,
							  MISSING_WIND_EVENT,
							  theDirectionEventItem.thePeriodBeginDataItem,
							  theDirectionEventItem.thePeriodEndDataItem,
							  theResultVector);

			WeatherPeriod returnPeriod(theDirectionEventItem.thePeriod.localEndTime(),
									   theSpeedEventItem.thePeriod.localEndTime());

			return new WindEventPeriodDataItem(returnPeriod,
											   theSpeedEventItem.theWindEvent,
											   theDirectionEventItem.thePeriodEndDataItem,
											   theSpeedEventItem.thePeriodEndDataItem);	
		  }

		if(theSpeedEventItem.thePeriod.localStartTime() > theDirectionEventItem.thePeriod.localStartTime())
		  {
			WeatherPeriod period(theDirectionEventItem.thePeriod.localStartTime(), 
								  theSpeedEventItem.thePeriod.localEndTime());

			add_merged_period(period,
							  windDirectionEvent,
							  theDirectionEventItem.thePeriodBeginDataItem,
							  theSpeedEventItem.thePeriodBeginDataItem,
							  theResultVector);

		  }

		WeatherPeriod period(theSpeedEventItem.thePeriod.localStartTime(),
							 theDirectionEventItem.thePeriod.localEndTime());

		add_merged_period(period,
						  compositeWindEvent,
						  theSpeedEventItem.thePeriodBeginDataItem,
						  theDirectionEventItem.thePeriodEndDataItem,
						  theResultVector);
		
		WeatherPeriod returnPeriod(theDirectionEventItem.thePeriod.localEndTime(),
								   theSpeedEventItem.thePeriod.localEndTime());
		
		return new WindEventPeriodDataItem(returnPeriod,
										   theSpeedEventItem.theWindEvent,
										   theDirectionEventItem.thePeriodEndDataItem,
										   theSpeedEventItem.thePeriodEndDataItem);
	  }

	cout << "EI TÄNNE !!" << endl;

	return 0;//theDirectionEventItem;
  }

  void create_union_periods_of_wind_speed_and_direction_event_periods(wo_story_params& storyParams)
  {
	unsigned int speedEventIndex(0);
	unsigned int directionEventIndex(0);

	WindEventPeriodDataItem* remainderItem = 0;
	//int i(0);

	while(speedEventIndex < storyParams.theWindSpeedEventPeriodVector.size() ||
		  directionEventIndex < storyParams.theWindDirectionEventPeriodVector.size())
	  {
		WindEventPeriodDataItem* directionItem = 0;
		WindEventPeriodDataItem* speedItem = 0;

		// first round or remainderItem == 0
		if( (speedEventIndex == 0 && directionEventIndex == 0) || remainderItem == 0)
		  {
			if(speedEventIndex < storyParams.theWindSpeedEventPeriodVector.size())
			  {
				speedItem = storyParams.theWindSpeedEventPeriodVector[speedEventIndex];
				speedEventIndex++;
			  }
			if(directionEventIndex < storyParams.theWindDirectionEventPeriodVector.size())
			  {
				directionItem = storyParams.theWindDirectionEventPeriodVector[directionEventIndex];
				directionEventIndex++;
			  }
		  }
		else
		  {
			if(remainderItem->getWindEventType() == WIND_DIRECTION_EVENT)
			  {
				directionItem = remainderItem;
				if(speedEventIndex < storyParams.theWindSpeedEventPeriodVector.size())
				  {
					speedItem = storyParams.theWindSpeedEventPeriodVector[speedEventIndex];
					speedEventIndex++;
				  }
			  }
			else
			  {
				speedItem = remainderItem;
				if(directionEventIndex < storyParams.theWindDirectionEventPeriodVector.size())
				  {
					directionItem = storyParams.theWindDirectionEventPeriodVector[directionEventIndex];
					directionEventIndex++;
				  }
			  }
		  }
		/*
		cout << "*** " << i++ << " ****" << endl;
		if(directionItem)
		  cout << "direction: " 
			   << directionItem->thePeriod.localStartTime() 
			   << ".."
			   << directionItem->thePeriod.localEndTime() << endl;
		if(speedItem)
		  cout << "speed: " 
			   << speedItem->thePeriod.localStartTime() 
			   << ".."
			   << speedItem->thePeriod.localEndTime() << endl;
		if(remainderItem)
		  cout << "remainder: "
			   << (remainderItem->getWindEventType() == WIND_DIRECTION_EVENT ? " direction event: " : " speed event: ")
			   << remainderItem->thePeriod.localStartTime() 
			   << ".."
			   << remainderItem->thePeriod.localEndTime() << endl;
		*/

		if(directionItem == 0)
		  {
			WindEventId compositeWindEvent = sort_out_wind_event(MISSING_WIND_DIRECTION_EVENT,
																   speedItem->theWindEvent);

			storyParams.theMergedWindEventPeriodVector.push_back(new WindEventPeriodDataItem(speedItem->thePeriod,
																							 compositeWindEvent,
																							 speedItem->thePeriodBeginDataItem,
																							 speedItem->thePeriodEndDataItem));
			if(remainderItem != 0)
			  delete remainderItem;
			remainderItem = 0;
		  }
		else if(speedItem == 0)
		  {
			WindEventId compositeWindEvent = sort_out_wind_event(directionItem->theWindEvent,
																   MISSING_WIND_SPEED_EVENT);

			storyParams.theMergedWindEventPeriodVector.push_back(new WindEventPeriodDataItem(directionItem->thePeriod,
																							 compositeWindEvent,
																							 directionItem->thePeriodBeginDataItem,
																							 directionItem->thePeriodEndDataItem));				
			if(remainderItem != 0)
			  delete remainderItem;
			remainderItem = 0;
		  }
		else
		  {
			remainderItem = sort_out_periods(*directionItem, *speedItem, storyParams.theMergedWindEventPeriodVector);
		  }
	  }
  }

  // retrns difference between directions: if direction increases value is positive, if direction decreases value is negative

  void remove_reduntant_periods(wo_story_params& storyParams)
  {
	unsigned int endIndex = storyParams.theMergedWindEventPeriodVector.size()-1;

	for(int i = endIndex; i >= 0; i--)
	  {
		if(storyParams.theMergedWindEventPeriodVector[i]->theReportThisEventPeriodFlag == false)
		  {
			delete storyParams.theMergedWindEventPeriodVector[i];
			storyParams.theMergedWindEventPeriodVector.erase(storyParams.theMergedWindEventPeriodVector.begin()+i);
		  }
	  }
  }

  void print_merged_wind_event_period_vector(wo_story_params& storyParams, const int& id)
  {
	WindEventPeriodDataItem* currentDataItem = 0;

	for(unsigned int i = 1; i < storyParams.theMergedWindEventPeriodVector.size(); i++)
	  {
		currentDataItem = storyParams.theMergedWindEventPeriodVector[i];

		if(currentDataItem->thePeriod.localStartTime() != 
		   currentDataItem->thePeriodBeginDataItem.thePeriod.localStartTime() ||
		   currentDataItem->thePeriod.localEndTime() !=
		   currentDataItem->thePeriodEndDataItem.thePeriod.localEndTime())
		  cout << "THE PERIOD " << id << ": "
			   << currentDataItem->thePeriod.localStartTime()
			   << "..."
			   << currentDataItem->thePeriod.localEndTime()
			   << endl << "THE BEGIN PERIOD: " 
			   << currentDataItem->thePeriodBeginDataItem.thePeriod.localStartTime()
			   << "..."
			   << currentDataItem->thePeriodBeginDataItem.thePeriod.localEndTime()
			   << endl << "THE END PERIOD: "
			   << currentDataItem->thePeriodEndDataItem.thePeriod.localStartTime()
			   << "..."
			   << currentDataItem->thePeriodEndDataItem.thePeriod.localEndTime()
			   << endl;
	  }
  }


  // theOldItem1 is the item that will be updated
  // theOldItem1 is the item that is merged to another and later removed
  void re_create_merged_item(WindEventPeriodDataItem** theOldItem1, 
							 const WindEventPeriodDataItem* theOldItme2,
							 const WeatherPeriod& thePeriod)
  {
	const WindEventPeriodDataItem* pEarlierPeriodItem = (*theOldItem1)->thePeriod.localStartTime() < theOldItme2->thePeriod.localStartTime() ? (*theOldItem1) : theOldItme2;
	const WindEventPeriodDataItem* pLaterPeriodItem = pEarlierPeriodItem == (*theOldItem1) ? theOldItme2 : (*theOldItem1);

	WeatherPeriod period(pEarlierPeriodItem->thePeriod.localStartTime(),
						 pLaterPeriodItem->thePeriod.localEndTime());

	WindEventId compositeWindEvent(sort_out_wind_event2(pEarlierPeriodItem->theWindEvent,
														  pLaterPeriodItem->theWindEvent));


	WindEventPeriodDataItem* pNewItem = new WindEventPeriodDataItem(period,
																	compositeWindEvent,//(*theOldItem1)->theWindEvent,
																	pEarlierPeriodItem->thePeriodBeginDataItem,
																	pLaterPeriodItem->thePeriodEndDataItem);
	
	delete (*theOldItem1);

	(*theOldItem1) = pNewItem; 
  }


  void merge_simple_and_composite_period_events(wo_story_params& storyParams)
  {
	WindEventPeriodDataItem* currentDataItem = 0;
	WindEventPeriodDataItem* previousDataItem = 0;

	// merge simple period events with composite period events
	for(unsigned int i = 1; i < storyParams.theMergedWindEventPeriodVector.size(); i++)
	  {
		currentDataItem = storyParams.theMergedWindEventPeriodVector[i];
		previousDataItem = storyParams.theMergedWindEventPeriodVector[i-1];

		//		cout << "currentDataItem->theWindEvent : "<< get_wind_event_string(currentDataItem->theWindEvent) << endl;
		//		cout << "previousDataItem->theWindEvent: " << get_wind_event_string(previousDataItem->theWindEvent) << endl;
		
		if(currentDataItem->theWindEvent == TUULI_KAANTYY &&
		   previousDataItem->theWindEvent == TUULI_KAANTYY_JA_VOIMISTUU)
		  {
			/*
			WindDirectionId directionBeg = static_cast<WindDirectionId>(direction16th(currentDataItem->thePeriodBeginDataItem.theEqualizedWindDirection));
			WindDirectionId directionEnd = static_cast<WindDirectionId>(direction16th(currentDataItem->thePeriodEndDataItem.theEqualizedWindDirection));
			*/			
			WindDirectionId directionBeg = wind_direction_id(currentDataItem->thePeriodBeginDataItem.theEqualizedWindDirection, storyParams.theVar);
			WindDirectionId directionEnd = wind_direction_id(currentDataItem->thePeriodEndDataItem.theEqualizedWindDirection, storyParams.theVar);


			/*
			cout << "directionBeg: " << wind_direction_string(directionBeg) 
				 << " " 
				 << currentDataItem->thePeriodBeginDataItem.theEqualizedWindDirection
				 << " " << currentDataItem->thePeriodBeginDataItem.thePeriod.localStartTime()
				 << "..." << currentDataItem->thePeriodBeginDataItem.thePeriod.localEndTime() << endl;
			cout << "directionEnd: " << wind_direction_string(directionEnd) 
				 << " "
				 << currentDataItem->thePeriodEndDataItem.theEqualizedWindDirection
				 << " " << currentDataItem->thePeriodEndDataItem.thePeriod.localStartTime()
				 << " " << currentDataItem->thePeriodEndDataItem.thePeriod.localEndTime() << endl;
			*/

			if(directionBeg == directionEnd)
			  {
				/*
				cout << "merged periods1: " << endl
					 << "period1: " 
					 << previousDataItem->thePeriod.localStartTime()
					 << "..."
					 << previousDataItem->thePeriod.localEndTime()
					 << endl
					 << "period2: " 
					 << currentDataItem->thePeriod.localStartTime()
					 << "..."
					 << currentDataItem->thePeriod.localEndTime()
					 << endl;
				*/
				/*
				WeatherPeriod mergedPeriod(previousDataItem->thePeriod.localStartTime(),
										   currentDataItem->thePeriod.localEndTime());
				previousDataItem->thePeriod = mergedPeriod;
				*/
				;

				re_create_merged_item(&previousDataItem,
									  currentDataItem,
									  WeatherPeriod(previousDataItem->thePeriod.localStartTime(),
													currentDataItem->thePeriod.localEndTime()));
				storyParams.theMergedWindEventPeriodVector[i-1] = previousDataItem;
				/*
				cout << "merged period1: " << endl
					 << previousDataItem->thePeriod.localStartTime()
					 << "..."
					 << previousDataItem->thePeriod.localEndTime()
					 << endl
					 << "merged beriod begin data item unit: "
					 << previousDataItem->thePeriodBeginDataItem.thePeriod.localStartTime()
					 << "..."
					 << previousDataItem->thePeriodBeginDataItem.thePeriod.localEndTime()
					 << endl
					 << "merged beriod end data item unit: "
					 << previousDataItem->thePeriodEndDataItem.thePeriod.localStartTime()
					 << "..."
					 << previousDataItem->thePeriodEndDataItem.thePeriod.localEndTime()
					 << endl;
				*/				

				currentDataItem->theReportThisEventPeriodFlag = false;

				i++;				
			  }
		  }
		else if((currentDataItem->theWindEvent == TUULI_VOIMISTUU &&
				 previousDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI) ||
				(currentDataItem->theWindEvent == TUULI_HEIKKENEE &&
				 previousDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI) ||
				(currentDataItem->theWindEvent == TUULI_VOIMISTUU &&
				 previousDataItem->theWindEvent == TUULI_KAANTYY_JA_VOIMISTUU) ||
				(currentDataItem->theWindEvent == TUULI_HEIKKENEE &&
				 previousDataItem->theWindEvent == TUULI_KAANTYY_JA_HEIKKENEE) ||
				(currentDataItem->theWindEvent == TUULI_VOIMISTUU &&
				 previousDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU) ||
				(currentDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI &&
				 previousDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU) ||
				(currentDataItem->theWindEvent == TUULI_HEIKKENEE &&
				 previousDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE) ||
				(currentDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI &&
				 previousDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE))
		  {
			/*, storyParams.theVar
				cout << "merged periods2: " << endl
					 << "period1: " 
					 << previousDataItem->thePeriod.localStartTime()
					 << "..."
					 << previousDataItem->thePeriod.localEndTime()
					 << endl
					 << "period2: " 
					 << currentDataItem->thePeriod.localStartTime()
					 << "..."
					 << currentDataItem->thePeriod.localEndTime()
					 << endl;
			*/


			re_create_merged_item(&previousDataItem,
								  currentDataItem,
								  WeatherPeriod(previousDataItem->thePeriod.localStartTime(),
												currentDataItem->thePeriod.localEndTime()));
			storyParams.theMergedWindEventPeriodVector[i-1] = previousDataItem;
			/*
				cout << "merged period2: " << endl
					 << previousDataItem->thePeriod.localStartTime()
					 << "..."
					 << previousDataItem->thePeriod.localEndTime()
					 << endl
					 << "merged beriod begin data item unit: "
					 << previousDataItem->thePeriodBeginDataItem.thePeriod.localStartTime()
					 << "..."
					 << previousDataItem->thePeriodBeginDataItem.thePeriod.localEndTime()
					 << endl
					 << "merged beriod end data item unit: "
					 << previousDataItem->thePeriodEndDataItem.thePeriod.localStartTime()
					 << "..."
					 << previousDataItem->thePeriodEndDataItem.thePeriod.localEndTime()
					 << endl;
			*/
			currentDataItem->theReportThisEventPeriodFlag = false;

			/*
			cout << "merged periods3: " << endl
					 << "period1: " 
					 << previousDataItem->thePeriod.localStartTime()
					 << "..."
					 << previousDataItem->thePeriod.localEndTime()
					 << endl
					 << "period2: " 
					 << currentDataItem->thePeriod.localStartTime()
					 << "..."
					 << currentDataItem->thePeriod.localEndTime()
					 << endl;
			*/

			i++;
		  }
		else if((currentDataItem->theWindEvent == TUULI_KAANTYY_JA_VOIMISTUU &&
				 previousDataItem->theWindEvent == TUULI_KAANTYY) ||
				(currentDataItem->theWindEvent == TUULI_KAANTYY_JA_HEIKKENEE &&
				 previousDataItem->theWindEvent == TUULI_KAANTYY) ||
				(currentDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU &&
				 previousDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI) ||
				(currentDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE &&
				 previousDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI))
		  {
			if(previousDataItem->theWindEvent == TUULI_KAANTYY &&
			   !wind_turns_to_the_same_direction(previousDataItem->thePeriodBeginDataItem.theEqualizedWindDirection.value(),
												 previousDataItem->thePeriodEndDataItem.theEqualizedWindDirection.value(),
												 currentDataItem->thePeriodEndDataItem.theEqualizedWindDirection.value()))
			  continue;

			/*
				cout << "merged periods4: " << endl
					 << "period1: " 
					 << previousDataItem->thePeriod.localStartTime()
					 << "..."
					 << previousDataItem->thePeriod.localEndTime()
					 << endl
					 << "period2: " 
					 << currentDataItem->thePeriod.localStartTime()
					 << "..."
					 << currentDataItem->thePeriod.localEndTime()
					 << endl;
			*/

			/*
			WeatherPeriod mergedPeriod(previousDataItem->thePeriod.localStartTime(),
									   currentDataItem->thePeriod.localEndTime());
			currentDataItem->thePeriod = mergedPeriod;
			*/
			re_create_merged_item(&currentDataItem,
								  previousDataItem,
								  WeatherPeriod(previousDataItem->thePeriod.localStartTime(),
												currentDataItem->thePeriod.localEndTime()));
			storyParams.theMergedWindEventPeriodVector[i] = currentDataItem;

			/*				cout << "merged period3: " << endl
					 << currentDataItem->thePeriod.localStartTime()
					 << "..."
					 << currentDataItem->thePeriod.localEndTime()
					 << endl
					 << "merged beriod begin data item unit: "
					 << currentDataItem->thePeriodBeginDataItem.thePeriod.localStartTime()
					 << "..."
					 << currentDataItem->thePeriodBeginDataItem.thePeriod.localEndTime()
					 << endl
					 << "merged beriod end data item unit: "
					 << currentDataItem->thePeriodEndDataItem.thePeriod.localStartTime()
					 << "..."
					 << currentDataItem->thePeriodEndDataItem.thePeriod.localEndTime()
					 << endl;
			*/

			previousDataItem->theReportThisEventPeriodFlag = false;
		  }		
		else if((currentDataItem->theWindEvent == TUULI_KAANTYY_JA_VOIMISTUU &&
				 previousDataItem->theWindEvent == TUULI_VOIMISTUU) ||
				(currentDataItem->theWindEvent == TUULI_KAANTYY_JA_HEIKKENEE &&
				 previousDataItem->theWindEvent == TUULI_HEIKKENEE))
		  {
			WindDirectionId windDirectionId(wind_direction_id(previousDataItem->thePeriodBeginDataItem.theWindDirection, 
																	storyParams.theVar));

			if((windDirectionId != VAIHTELEVA && 
				get_period_length(previousDataItem->thePeriod) <= 4) ||
			   get_period_length(previousDataItem->thePeriod) <= 1)
			  {
				/*
				cout << "merged periods5: " << endl
					 << "period1: " 
					 << previousDataItem->thePeriod.localStartTime()
					 << "..."
					 << previousDataItem->thePeriod.localEndTime()
					 << endl
					 << "period2: " 
					 << currentDataItem->thePeriod.localStartTime()
					 << "..."
					 << currentDataItem->thePeriod.localEndTime()
					 << endl;
				*/
				/*
				WeatherPeriod mergedPeriod(previousDataItem->thePeriod.localStartTime(),
										   currentDataItem->thePeriod.localEndTime());
				currentDataItem->thePeriod = mergedPeriod;
				*/

				re_create_merged_item(&currentDataItem,
									  previousDataItem,
									  WeatherPeriod(previousDataItem->thePeriod.localStartTime(),
													currentDataItem->thePeriod.localEndTime()));
				storyParams.theMergedWindEventPeriodVector[i] = currentDataItem;

				/*
				cout << "merged period4: " << endl
					 << currentDataItem->thePeriod.localStartTime()
					 << "..."
					 << currentDataItem->thePeriod.localEndTime()
					 << endl
					 << "merged beriod begin data item unit: "
					 << currentDataItem->thePeriodBeginDataItem.thePeriod.localStartTime()
					 << "..."
					 << currentDataItem->thePeriodBeginDataItem.thePeriod.localEndTime()
					 << endl
					 << "merged beriod end data item unit: "
					 << currentDataItem->thePeriodEndDataItem.thePeriod.localStartTime()
					 << "..."
					 << currentDataItem->thePeriodEndDataItem.thePeriod.localEndTime()
					 << endl;
				*/

				previousDataItem->theReportThisEventPeriodFlag = false;
			  }
		  }		
	  }
  }

  void merge_fragment_periods_when_feasible(wo_story_params& storyParams)
  {
	WindEventPeriodDataItem* currentDataItem = 0;
	WindEventPeriodDataItem* previousDataItem = 0;
	WindEventPeriodDataItem* nextDataItem = 0;





	// 1) remove overlapping hours when the period changes
	for(unsigned int i = 1; i < storyParams.theMergedWindEventPeriodVector.size(); i++)
	  {
		WeatherArea::Type areaType(storyParams.theArea.type());
		currentDataItem = storyParams.theMergedWindEventPeriodVector[i];
		NFmiTime periodBegTime(currentDataItem->thePeriod.localStartTime());
		periodBegTime.ChangeByHours(1);
		NFmiTime periodEndTime(currentDataItem->thePeriod.localEndTime());
		WeatherPeriod period(periodBegTime, periodEndTime);
		currentDataItem->thePeriod = period;

		for(unsigned int k = 0; k < storyParams.theWindDataVector.size(); k++)
		  {
			if(currentDataItem->thePeriodBeginDataItem == ((*storyParams.theWindDataVector[k])(areaType)))
			  {
				if(k < storyParams.theWindDataVector.size() - 1)
				  {
					const WindDataItemUnit& newPeriodBeginDataItem = ((*storyParams.theWindDataVector[k+1])(areaType));
					WindEventPeriodDataItem* pNewItem = new WindEventPeriodDataItem(currentDataItem->thePeriod,
																					currentDataItem->theWindEvent,
																					newPeriodBeginDataItem,
																					currentDataItem->thePeriodEndDataItem);
					delete currentDataItem;
					storyParams.theMergedWindEventPeriodVector[i] = pNewItem;
					currentDataItem = pNewItem;
					
					/*
					cout << "pNewItem->thePeriod " 
						 << pNewItem->thePeriod.localStartTime() 
						 << "..."
						 << pNewItem->thePeriod.localEndTime() << endl;

					cout << "pNewItem->thePeriodBeginDataItem.thePeriod " 
						 << pNewItem->thePeriodBeginDataItem.thePeriod.localStartTime() 
						 << "..."
						 << pNewItem->thePeriodBeginDataItem.thePeriod.localEndTime() << endl;
					cout << "pNewItem->thePeriodEndDataItem.thePeriod " 
						 << pNewItem->thePeriodEndDataItem.thePeriod.localStartTime() 
						 << "..."
						 << pNewItem->thePeriodEndDataItem.thePeriod.localEndTime() << endl;
					*/

					break;
				  }
			  }
		  }
	  }

	remove_reduntant_periods(storyParams);

	storyParams.theLog << "* STEP 1 *" << endl;
	log_merged_wind_event_periods(storyParams);	



 	// 2) if wind speed weakens/strengthens, check that weakening/strengthening is big enough
	for(unsigned int i = 1; i < storyParams.theMergedWindEventPeriodVector.size() - 1; i++)
	  {
		currentDataItem = storyParams.theMergedWindEventPeriodVector[i];

		if(currentDataItem->theWindEvent & TUULI_HEIKKENEE || currentDataItem->theWindEvent & TUULI_VOIMISTUU)
		  {
			bool windSpeedDifferEnough = wind_speed_differ_enough(storyParams.theSources,
																  storyParams.theArea,
																  currentDataItem->thePeriod,
																  storyParams.theVar,
																  storyParams.theWindDataVector);
			if(!windSpeedDifferEnough)
			  {
				unsigned short mask(currentDataItem->theWindEvent & TUULI_HEIKKENEE ? TUULI_HEIKKENEE : TUULI_VOIMISTUU);
				unsigned short value = currentDataItem->theWindEvent;
				value &= (~mask);
				currentDataItem->theWindEvent = static_cast<WindEventId>(value);
			  }
			  
		  }
	  }
	remove_reduntant_periods(storyParams);

	storyParams.theLog << "* STEP 2 *" << endl;
	log_merged_wind_event_periods(storyParams);


	// 3) remove short periods when event is MISSING_WIND_EVENT, except the first one.
	// If the MISSING_WIND_EVENT is the last event merge it to the previous irrespective of period length
	for(unsigned int i = 1; i < storyParams.theMergedWindEventPeriodVector.size(); i++)
	  {
		currentDataItem = storyParams.theMergedWindEventPeriodVector[i];
		previousDataItem = storyParams.theMergedWindEventPeriodVector[i-1];

		if(currentDataItem->theWindEvent == MISSING_WIND_EVENT &&
		   (get_period_length(currentDataItem->thePeriod) <= 2))
		  //			i == storyParams.theMergedWindEventPeriodVector.size() - 1))
		  {
			/*
			WeatherPeriod mergedPeriod(previousDataItem->thePeriod.localStartTime(),
									   currentDataItem->thePeriod.localEndTime());
			previousDataItem->thePeriod = mergedPeriod;
			*/

			re_create_merged_item(&previousDataItem,
								  currentDataItem,
								  WeatherPeriod(previousDataItem->thePeriod.localStartTime(),
												currentDataItem->thePeriod.localEndTime()));
			storyParams.theMergedWindEventPeriodVector[i-1] = previousDataItem;

			currentDataItem->theReportThisEventPeriodFlag = false;
			i++;
		  }
	  }

	remove_reduntant_periods(storyParams);
	storyParams.theLog << "* STEP 3 *" << endl;
	log_merged_wind_event_periods(storyParams);	

	// 4) 
	for(unsigned int i = 1; i < storyParams.theMergedWindEventPeriodVector.size(); i++)
	  {
		currentDataItem = storyParams.theMergedWindEventPeriodVector[i];
		previousDataItem = storyParams.theMergedWindEventPeriodVector[i-1];

		if(previousDataItem->theWindEvent == TUULI_KAANTYY &&
		   wind_direction_id(previousDataItem->thePeriodBeginDataItem.theEqualizedWindDirection, storyParams.theVar) == wind_direction_id(previousDataItem->thePeriodEndDataItem.theEqualizedWindDirection, storyParams.theVar) &&
		   get_period_length(previousDataItem->thePeriod) == 0)
		  {
			re_create_merged_item(&currentDataItem,
								  previousDataItem,
								  WeatherPeriod(previousDataItem->thePeriod.localStartTime(),
												currentDataItem->thePeriod.localEndTime()));
			storyParams.theMergedWindEventPeriodVector[i] = currentDataItem;
			
			previousDataItem->theReportThisEventPeriodFlag = false;
		  }		
	  }

	remove_reduntant_periods(storyParams);
	storyParams.theLog << "* STEP 4 *" << endl;
	log_merged_wind_event_periods(storyParams);	

	// 5) merge short period to larger one if there is no change during short period)
	for(unsigned int i = 1; i < storyParams.theMergedWindEventPeriodVector.size(); i++)
	  {
		currentDataItem = storyParams.theMergedWindEventPeriodVector[i];
		previousDataItem = storyParams.theMergedWindEventPeriodVector[i-1];

		if(get_period_length(currentDataItem->thePeriod) <= 2)
		  {
			bool mergePeriods = false;

			/*
			if(currentDataItem->theWindEvent == TUULI_KAANTYY &&
			   previousDataItem->theWindEvent == MISSING_WIND_EVENT)
			  {
				WindDirectionId directionBeg = static_cast<WindDirectionId>(direction16th(currentDataItem->thePeriodBeginDataItem.theEqualizedWindDirection));
				WindDirectionId directionEnd = static_cast<WindDirectionId>(direction16th(currentDataItem->thePeriodEndDataItem.theEqualizedWindDirection));
				mergePeriods = (directionBeg == directionEnd);
			  }
			else if((currentDataItem->theWindEvent == TUULI_VOIMISTUU ||
					 currentDataItem->theWindEvent == TUULI_HEIKKENEE) &&
					previousDataItem->theWindEvent == MISSING_WIND_EVENT)
			  {
				float maxWindBeg = currentDataItem->thePeriodBeginDataItem.theEqualizedMaximumWind;
				float maxWindEnd = currentDataItem->thePeriodEndDataItem.theEqualizedMaximumWind;
				
				mergePeriods = (abs(maxWindEnd - maxWindBeg) < storyParams.theWindSpeedThreshold);
			  }
			*/
			/*
			if(((currentDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU &&
					 previousDataItem->theWindEvent == TUULI_VOIMISTUU)||
					(currentDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE &&
					 previousDataItem->theWindEvent == TUULI_HEIKKENEE)) &&
					get_period_length(currentDataItem->thePeriod) <= 2)
			  {
				mergePeriods = true;
			  }
			*/
			if(mergePeriods)
			  {
				/*
				WeatherPeriod mergedPeriod(previousDataItem->thePeriod.localStartTime(),
										   currentDataItem->thePeriod.localEndTime());
				previousDataItem->thePeriod = mergedPeriod;
				*/
				re_create_merged_item(&previousDataItem,
									  currentDataItem,
									  WeatherPeriod(previousDataItem->thePeriod.localStartTime(),
													currentDataItem->thePeriod.localEndTime()));
				storyParams.theMergedWindEventPeriodVector[i-1] = previousDataItem;

				currentDataItem->theReportThisEventPeriodFlag = false;
				i++;
			  }
		  }
	  }
	remove_reduntant_periods(storyParams);

	storyParams.theLog << "* STEP 5 *" << endl;
	log_merged_wind_event_periods(storyParams);
	
	// 6) merge simple period events with composite period events
	merge_simple_and_composite_period_events(storyParams);
	remove_reduntant_periods(storyParams);

	storyParams.theLog << "* STEP 6 *" << endl;
	log_merged_wind_event_periods(storyParams);

	unsigned int previousMergedIndex(UINT_MAX);
	// 7) merge similar successive periods
	for(unsigned int i = 1; i < storyParams.theMergedWindEventPeriodVector.size(); i++)
	  {
		unsigned int previousPeriodIndex(previousMergedIndex == UINT_MAX ? i - 1 : previousMergedIndex);
		currentDataItem = storyParams.theMergedWindEventPeriodVector[i];
		previousDataItem = storyParams.theMergedWindEventPeriodVector[previousPeriodIndex];
		/*
		const WeatherResult& windDirectionCurrent(currentDataItem->thePeriodBeginDataItem.theWindDirection);
		const WeatherResult& windDirectionPrevious(previousDataItem->thePeriodEndDataItem.theWindDirection);
		WindDirectionId windDirectionIdCurrent(wind_direction_id(windDirectionCurrent, storyParams.theVar));
		WindDirectionId windDirectionIdPrevious(wind_direction_id(windDirectionPrevious, storyParams.theVar));
		*/
		const WeatherResult& windDirectionPrevious(previousDataItem->thePeriodEndDataItem.theWindDirection);
		WindDirectionId windDirectionIdPrevious(wind_direction_id(windDirectionPrevious, storyParams.theVar));
		const WeatherResult& windDirectionCurrent(currentDataItem->thePeriodBeginDataItem.theWindDirection);
		WindDirectionId windDirectionIdCurrent(wind_direction_id(windDirectionCurrent, storyParams.theVar));

		if((currentDataItem->theWindEvent == MISSING_WIND_EVENT &&
			previousDataItem->theWindEvent == MISSING_WIND_EVENT &&
			!((windDirectionIdCurrent == VAIHTELEVA || windDirectionIdPrevious == VAIHTELEVA) &&
			  windDirectionIdCurrent != windDirectionIdPrevious)) ||
		   (currentDataItem->theWindEvent == TUULI_VOIMISTUU &&
			previousDataItem->theWindEvent == TUULI_VOIMISTUU &&
			windDirectionIdCurrent != VAIHTELEVA) ||
		   (currentDataItem->theWindEvent == TUULI_HEIKKENEE &&
			previousDataItem->theWindEvent == TUULI_HEIKKENEE &&
			windDirectionIdCurrent != VAIHTELEVA) ||
		   (currentDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI &&
			previousDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI) ||
		   (currentDataItem->theWindEvent == TUULI_KAANTYY_JA_VOIMISTUU &&
			previousDataItem->theWindEvent == TUULI_KAANTYY_JA_VOIMISTUU) ||
		   (currentDataItem->theWindEvent == TUULI_KAANTYY_JA_HEIKKENEE &&
			previousDataItem->theWindEvent == TUULI_KAANTYY_JA_HEIKKENEE))
		  {
			/*
			if((currentDataItem->theWindEvent == TUULI_KAANTYY_JA_VOIMISTUU &&
				previousDataItem->theWindEvent == TUULI_KAANTYY_JA_VOIMISTUU) ||
			   (currentDataItem->theWindEvent == TUULI_KAANTYY_JA_HEIKKENEE &&
				previousDataItem->theWindEvent == TUULI_KAANTYY_JA_HEIKKENEE))
			  {
				const WeatherResult& prevDirectionBeg(previousDataItem->thePeriodBeginDataItem.theWindDirection);
				const WeatherResult& prevDirectionEnd(previousDataItem->thePeriodEndDataItem.theWindDirection);

				if(wind_direction_id(prevDirectionBeg, storyParams.theVar) == VAIHTELEVA)
				  continue;

				WindEventId windEvent(get_wind_direction_event(prevDirectionBeg,
																 prevDirectionEnd,
																 storyParams.theVar,
																 storyParams.theWindDirectionThreshold));
				if(windEvent != MISSING_WIND_DIRECTION_EVENT)
				  continue;
			  }
			*/
			re_create_merged_item(&previousDataItem,
								  currentDataItem,
								  WeatherPeriod(previousDataItem->thePeriod.localStartTime(),
												currentDataItem->thePeriod.localEndTime()));
			storyParams.theMergedWindEventPeriodVector[previousPeriodIndex] = previousDataItem;

			currentDataItem->theReportThisEventPeriodFlag = false;
			previousMergedIndex = previousPeriodIndex;
		  }
		else
		  {
			previousMergedIndex = UINT_MAX;
		  }
	  }
	remove_reduntant_periods(storyParams);
	storyParams.theLog << "* STEP 7 *" << endl;
	log_merged_wind_event_periods(storyParams);

	// 8)
	for(unsigned int i = 1; i < storyParams.theMergedWindEventPeriodVector.size() - 1; i++)
	  {
		currentDataItem = storyParams.theMergedWindEventPeriodVector[i];
		previousDataItem = storyParams.theMergedWindEventPeriodVector[i-1];
		nextDataItem = storyParams.theMergedWindEventPeriodVector[i+1];

		if(((currentDataItem->theWindEvent == TUULI_KAANTYY_JA_HEIKKENEE &&
			previousDataItem->theWindEvent == TUULI_HEIKKENEE &&
			nextDataItem->theWindEvent == TUULI_KAANTYY_JA_VOIMISTUU) ||
		   (currentDataItem->theWindEvent == TUULI_KAANTYY_JA_VOIMISTUU &&
			previousDataItem->theWindEvent == TUULI_VOIMISTUU &&
			nextDataItem->theWindEvent == TUULI_KAANTYY_JA_HEIKKENEE)) &&
		   get_period_length(currentDataItem->thePeriod) <= 2)
		  {
			/*
			  WeatherPeriod mergedPeriod(previousDataItem->thePeriod.localStartTime(),
			  currentDataItem->thePeriod.localEndTime());
			  previousDataItem->thePeriod = mergedPeriod;
			*/
			re_create_merged_item(&previousDataItem,
								  currentDataItem,
								  WeatherPeriod(previousDataItem->thePeriod.localStartTime(),
												currentDataItem->thePeriod.localEndTime()));
			storyParams.theMergedWindEventPeriodVector[i-1] = previousDataItem;
			
			currentDataItem->theReportThisEventPeriodFlag = false;
			i++;
		  }
	  }
	remove_reduntant_periods(storyParams);
	storyParams.theLog << "* STEP 8 *" << endl;
	log_merged_wind_event_periods(storyParams);

	merge_simple_and_composite_period_events(storyParams);
	remove_reduntant_periods(storyParams);

	storyParams.theLog << "* STEP 9 *" << endl;
	log_merged_wind_event_periods(storyParams);



  }

  bool wind_event_period_sort(const WindEventPeriodDataItem* first, const WindEventPeriodDataItem* second) 
  {	
	return (first->thePeriod.localStartTime() < second->thePeriod.localStartTime());
  }

  void find_out_wind_event_periods(wo_story_params& storyParams)
  {
	find_out_wind_speed_event_periods(storyParams);
	find_out_wind_direction_event_periods2(storyParams);
	//		find_out_wind_direction_event_periods(storyParams);

	log_equalized_wind_direction_data_vector(storyParams);

	if(storyParams.theWindSpeedEventPeriodVector.size() > 1)
	  std::sort(storyParams.theWindSpeedEventPeriodVector.begin(),
				storyParams.theWindSpeedEventPeriodVector.end(), 
				wind_event_period_sort);
	if(storyParams.theWindDirectionEventPeriodVector.size() > 1)
	  std::sort(storyParams.theWindDirectionEventPeriodVector.begin(),
				storyParams.theWindDirectionEventPeriodVector.end(), 
				wind_event_period_sort);
	if(storyParams.theWindEventPeriodVector.size() > 1)
	  std::sort(storyParams.theWindEventPeriodVector.begin(),
				storyParams.theWindEventPeriodVector.end(), 
				wind_event_period_sort);

	create_union_periods_of_wind_speed_and_direction_event_periods(storyParams);

	log_merged_wind_event_periods(storyParams);	
	merge_fragment_periods_when_feasible(storyParams);
	log_merged_wind_event_periods(storyParams);
	find_out_transient_wind_direction_periods(storyParams); // wind direction
	log_merged_wind_event_periods(storyParams);
	
  }

  WindEventId merge_wind_events(const WindEventId& speedEvent, const WindEventId& directionEvent)
  {
	
	return static_cast<WindEventId>(speedEvent + directionEvent);
  }

  bool wind_event_sort(const timestamp_wind_event_id_pair& first, const timestamp_wind_event_id_pair& second) 
  { 
	return (first.first < second.first);
  }

#ifdef LATER
  void find_out_wind_events(wo_story_params& storyParams)
  {
	if(storyParams.theWindDataVector.size() == 0)
	  return;

	WindEventId windEventId(MISSING_WIND_EVENT);

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

	WindDirectionId wind_dir_id(VAIHTELEVA);

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
				
				storyParams.theWindEventVector[k].second = static_cast<WindEventId>(storyParams.theWindEventVector[k].second +
																	   (wind_dir_id == VAIHTELEVA ? TUULI_MUUTTUU_VAIHTELEVAKSI : TUULI_KAANTYY));
				simultaneousEventsOccurred = true;
				break;
			  }
		  }

		if(simultaneousEventsOccurred)
		  continue;

		storyParams.theWindEventVector.push_back(make_pair(storyParams.theWindDirectionVector[i]->thePeriod.localStartTime(), 
														   wind_dir_id == VAIHTELEVA ? TUULI_MUUTTUU_VAIHTELEVAKSI : TUULI_KAANTYY));
	  }

	std::sort(storyParams.theWindEventVector.begin(), storyParams.theWindEventVector.end(), wind_event_sort);

  }
#endif

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


  bool add_local_min_max_values(vector<unsigned int>& eqIndexVector,
								wo_story_params& storyParams,
								const bool& medianTimeSeries)
  {
	// include the local maximum and minimum values that were possibly removed
	// in the previous step
	WeatherArea::Type areaType(storyParams.theArea.type());
	vector<unsigned int> indexesToAdd;
	for(unsigned int i = 1; i < eqIndexVector.size(); i++)
	  {
		unsigned int currentIndex = eqIndexVector[i];
		unsigned int previousIndex = eqIndexVector[i-1];
		const WindDataItemUnit& previousItem = (*storyParams.theWindDataVector[previousIndex])(areaType);
		const WindDataItemUnit& currentItem = (*storyParams.theWindDataVector[currentIndex])(areaType);

		if(currentIndex == previousIndex + 1)
		  continue;

		float localMax = UINT_MAX;
		if(medianTimeSeries)
		  localMax = (previousItem.theWindSpeedMedian.value() > currentItem.theWindSpeedMedian.value() ?
					  previousItem.theWindSpeedMedian.value() : currentItem.theWindSpeedMedian.value());
		else
		  localMax = (previousItem.theWindMaximum.value() > currentItem.theWindMaximum.value() ?
					  previousItem.theWindMaximum.value() : currentItem.theWindMaximum.value());

		float localMin = UINT_MAX;
		if(medianTimeSeries)
		  localMin = (previousItem.theWindSpeedMedian.value() < currentItem.theWindSpeedMedian.value() ?
					  previousItem.theWindSpeedMedian.value() : currentItem.theWindSpeedMedian.value());
		else
		  localMin = (previousItem.theWindMaximum.value() < currentItem.theWindMaximum.value() ?
					  previousItem.theWindMaximum.value() : currentItem.theWindMaximum.value());


		unsigned localMaxIndex = UINT_MAX;
		unsigned localMinIndex = UINT_MAX;

		//cout << "localMax: " << localMax << "; localMin: " << localMin << endl;

		for(unsigned int k = previousIndex+1; k < currentIndex; k++)
		  {
			const WindDataItemUnit& itemK = ((*storyParams.theWindDataVector[k])(areaType));
			float currentValue = (medianTimeSeries ? itemK.theWindSpeedMedian.value() : itemK.theWindMaximum.value());

			if(currentValue > localMax)
			  {
				localMax = currentValue;
				localMaxIndex = k;
				//cout << "localMax changed: " << localMax << "; index " << localMaxIndex << endl;
			  }
			else if(currentValue < localMin)
			  {
				localMin = currentValue;
				localMinIndex = k;
				//	cout << "localMin changed: " << localMin << "; index " << localMinIndex << endl;
			  }
		  }
		if(localMaxIndex != UINT_MAX)
		  indexesToAdd.push_back(localMaxIndex);

		if(localMinIndex != UINT_MAX)
		  indexesToAdd.push_back(localMinIndex);
	  }
	eqIndexVector.insert(eqIndexVector.begin(), indexesToAdd.begin(), indexesToAdd.end());
	sort(eqIndexVector.begin(), eqIndexVector.end());

	return indexesToAdd.size() > 0;
  }

  void calculate_equalized_wind_speed_indexes_for_median_wind(wo_story_params& storyParams)
  {
	unsigned int index1, index2, index3;
	WeatherArea::Type areaType(storyParams.theArea.type());

	vector<unsigned int>& eqIndexVector = storyParams.equalizedWSIndexesMedian(areaType);

	while(1)
	  {
		double minError = UINT_MAX;
		unsigned int minErrorIndex = UINT_MAX;

		for(unsigned int i = 0; i < eqIndexVector.size() - 2; i++)
		  {	
			index1 = eqIndexVector[i];
			index2 = eqIndexVector[i+1];
			index3 = eqIndexVector[i+2];
			
			const WindDataItemUnit& dataItemIndex1 = (*storyParams.theWindDataVector[index1])(areaType);
			const WindDataItemUnit& dataItemIndex2 = (*storyParams.theWindDataVector[index2])(areaType);
			const WindDataItemUnit& dataItemIndex3 = (*storyParams.theWindDataVector[index3])(areaType);

			double lineBegX = index1;
			double lineBegY = dataItemIndex1.theWindSpeedMedian.value();
			double lineEndX = index3;
			double lineEndY = dataItemIndex3.theWindSpeedMedian.value();
			double coordX = index2;
			double coordY = dataItemIndex2.theWindSpeedMedian.value();
			NFmiPoint point(coordX, coordY);
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
		eqIndexVector.erase(eqIndexVector.begin()+minErrorIndex);
	  } // while

	bool valuesToAdd = true;
	do {
	  valuesToAdd = add_local_min_max_values(eqIndexVector, storyParams, true);
	} while(valuesToAdd);
	
	// re-calculate equalized values for the removed points
	for(unsigned int i = 1; i < eqIndexVector.size(); i++)
	  {
		unsigned int currentIndex = eqIndexVector[i];
		unsigned int previousIndex = eqIndexVector[i-1];
		const WindDataItemUnit& previousItem = (*storyParams.theWindDataVector[previousIndex])(areaType);
		const WindDataItemUnit& currentItem = (*storyParams.theWindDataVector[currentIndex])(areaType);

		if(currentIndex == previousIndex + 1)
		  continue;
		

		float oppositeLen = currentItem.theWindSpeedMedian.value() - previousItem.theWindSpeedMedian.value();
		float adjacentLen = currentIndex - previousIndex;
		float slope = oppositeLen / adjacentLen;

		for(unsigned int k = previousIndex+1; k < currentIndex; k++)
		  {
			float yValue = (slope * (k-previousIndex)) + previousItem.theWindSpeedMedian.value();
			WindDataItemUnit& item = (storyParams.theWindDataVector[k])->getDataItem(areaType);
			item.theEqualizedMedianWindSpeed = yValue;			
		  }		
	  }
  }

  void calculate_equalized_wind_speed_indexes_for_maximum_wind(wo_story_params& storyParams)
  {
	unsigned int index1, index2, index3;
	WeatherArea::Type areaType(storyParams.theArea.type());

	vector<unsigned int>& eqIndexVector = storyParams.equalizedWSIndexesMaximum(areaType);

	while(1)
	  {
		double minError = UINT_MAX;
		unsigned int minErrorIndex = UINT_MAX;

		for(unsigned int i = 0; i < eqIndexVector.size() - 2; i++)
		  {	
			index1 = eqIndexVector[i];
			index2 = eqIndexVector[i+1];
			index3 = eqIndexVector[i+2];
			
			const WindDataItemUnit& dataItemIndex1 = (*storyParams.theWindDataVector[index1])(areaType);
			const WindDataItemUnit& dataItemIndex2 = (*storyParams.theWindDataVector[index2])(areaType);
			const WindDataItemUnit& dataItemIndex3 = (*storyParams.theWindDataVector[index3])(areaType);

			double lineBegX = index1;
			double lineBegY = dataItemIndex1.theWindMaximum.value();
			double lineEndX = index3;
			double lineEndY = dataItemIndex3.theWindMaximum.value();
			double coordX = index2;
			double coordY = dataItemIndex2.theWindMaximum.value();
			NFmiPoint point(coordX, coordY);
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
		eqIndexVector.erase(eqIndexVector.begin()+minErrorIndex);
	  } //while

	bool valuesToAdd = true;
	do {
	  valuesToAdd = add_local_min_max_values(eqIndexVector, storyParams, false);
	} while(valuesToAdd);

	// re-calculate equalized values for the removed points
	for(unsigned int i = 1; i < eqIndexVector.size(); i++)
	  {
		unsigned int currentIndex = eqIndexVector[i];
		unsigned int previousIndex = eqIndexVector[i-1];
		const WindDataItemUnit& previousItem = (*storyParams.theWindDataVector[previousIndex])(areaType);
		const WindDataItemUnit& currentItem = (*storyParams.theWindDataVector[currentIndex])(areaType);

		if(currentIndex == previousIndex + 1)
		  continue;
		
		float oppositeLen = currentItem.theWindMaximum.value() - previousItem.theWindMaximum.value();
		float adjacentLen = currentIndex - previousIndex;
		float slope = oppositeLen / adjacentLen;


		for(unsigned int k = previousIndex+1; k < currentIndex; k++)
		  {
			float yValue = (slope * (k-previousIndex)) + previousItem.theWindMaximum.value();
			WindDataItemUnit& item = (storyParams.theWindDataVector[k])->getDataItem(areaType);
			item.theEqualizedMaximumWind = yValue;			
		  }		
	  }
  }

  void calculate_equalized_wind_direction_indexes(wo_story_params& storyParams)
  {
	unsigned int index1, index2, index3;
	WeatherArea::Type areaType(storyParams.theArea.type());

	vector<unsigned int>& eqIndexVector = storyParams.equalizedWDIndexes(areaType);

	while(1)
	  {
		double minError(UINT_MAX);
		unsigned int minErrorIndex(UINT_MAX);
		WindDirectionId directionIdIndex1(MISSING_WIND_DIRECTION_ID);
		WindDirectionId directionIdIndex2(MISSING_WIND_DIRECTION_ID);
		WindDirectionId directionIdIndex3(MISSING_WIND_DIRECTION_ID);

		for(unsigned int i = 0; i < eqIndexVector.size() - 2; i++)
		  {
			index1 = eqIndexVector[i];
			index2 = eqIndexVector[i+1];
			index3 = eqIndexVector[i+2];
			
			const WindDataItemUnit& dataItemIndex1 = (*storyParams.theWindDataVector[index1])(areaType);
			const WindDataItemUnit& dataItemIndex2 = (*storyParams.theWindDataVector[index2])(areaType);
			const WindDataItemUnit& dataItemIndex3 = (*storyParams.theWindDataVector[index3])(areaType);

			directionIdIndex1 = wind_direction_id(dataItemIndex1.theWindDirection, 
													  storyParams.theVar);
			directionIdIndex2 = wind_direction_id(dataItemIndex2.theWindDirection,
													  storyParams.theVar);
			directionIdIndex3 = wind_direction_id(dataItemIndex3.theWindDirection,
													  storyParams.theVar);

			if(directionIdIndex1 == VAIHTELEVA || 
			   directionIdIndex3 == VAIHTELEVA || 
			   directionIdIndex2 == VAIHTELEVA)
			  continue;

			double lineBegX = index1;
			double lineBegY = dataItemIndex1.theWindDirection.value();
			double lineEndX = index3;
			double lineEndY = dataItemIndex3.theWindDirection.value();
			double coordX = index2;
			double coordY = dataItemIndex2.theWindDirection.value();

			NFmiPoint point(coordX, coordY);
			NFmiPoint lineBegPoint(lineBegX, lineBegY);
			NFmiPoint lineEndPoint(lineEndX, lineEndY);
			double deviation_from_line = distance_from_line(point, lineBegPoint, lineEndPoint);

			//		cout << "direction1: " << wind_direction_string(directionIdIndex1) << endl;
			//cout << "direction2: " << wind_direction_string(directionIdIndex2) << endl;
			//cout << "direction3: " << wind_direction_string(directionIdIndex3) << endl;
			//			cout << "deviation_from_line: " << deviation_from_line << endl;

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

		directionIdIndex1 = wind_direction_id(((*storyParams.theWindDataVector[minErrorIndex-1])(areaType)).theWindDirection,
													  storyParams.theVar);
		directionIdIndex2 = wind_direction_id(((*storyParams.theWindDataVector[minErrorIndex])(areaType)).theWindDirection,
													  storyParams.theVar);
		directionIdIndex3 = wind_direction_id(((*storyParams.theWindDataVector[minErrorIndex+1])(areaType)).theWindDirection,
													  storyParams.theVar);

		/*
		cout << "direction1: " << wind_direction_string(directionIdIndex1) << ": " 
			 << ((*storyParams.theWindDataVector[minErrorIndex-1])(areaPart)).theWindDirection.value()  << ", " 
			 << ((*storyParams.theWindDataVector[minErrorIndex-1])(areaPart)).theWindDirection.error() << endl;
		cout << "direction2: " << wind_direction_string(directionIdIndex2) << ": " 
			 << ((*storyParams.theWindDataVector[minErrorIndex])(areaPart)).theWindDirection.value() << ", " 
			 << ((*storyParams.theWindDataVector[minErrorIndex])(areaPart)).theWindDirection.error() << endl;
		cout << "direction3: " << wind_direction_string(directionIdIndex3) << ": " 
			 << ((*storyParams.theWindDataVector[minErrorIndex+1])(areaPart)).theWindDirection.value() << ", "
			 << ((*storyParams.theWindDataVector[minErrorIndex+1])(areaPart)).theWindDirection.error() << endl;

		const WindDataItemUnit& dataItemTmp = (*storyParams.theWindDataVector[minErrorIndex])(areaPart);
		cout << "erasing: " << dataItemTmp.thePeriod.localStartTime() << "..." 
			 << dataItemTmp.thePeriod.localEndTime() << endl;
		*/

		eqIndexVector.erase(eqIndexVector.begin()+minErrorIndex);
	  }

	bool valuesToAdd = true;
	do {
	  valuesToAdd = add_local_min_max_values(eqIndexVector, storyParams, false);
	} while(valuesToAdd);

	// re-calculate equalized values for the removed points
	for(unsigned int i = 1; i < eqIndexVector.size(); i++)
	  {
		unsigned int currentIndex = eqIndexVector[i];
		unsigned int previousIndex = eqIndexVector[i-1];
		const WindDataItemUnit& previousItem = (*storyParams.theWindDataVector[previousIndex])(areaType);
		const WindDataItemUnit& currentItem = (*storyParams.theWindDataVector[currentIndex])(areaType);

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

			WindDataItemUnit& item = (storyParams.theWindDataVector[k])->getDataItem(areaType);
			item.theEqualizedWindDirection = WeatherResult(yValue, item.theWindDirection.error());
		  }
	  }
	// 	cout << "theEqualizedWindDirectionIndexes: " << storyParams.theEqualizedWindDirectionIndexes.size() << endl;
 }

  void calculate_equalized_data(wo_story_params& storyParams)
  {
	WeatherArea::Type areaType(storyParams.theArea.type());
	// first calculate the indexes
	if(storyParams.equalizedWSIndexesMedian(areaType).size() > 3)
	  calculate_equalized_wind_speed_indexes_for_median_wind(storyParams);

	if(storyParams.equalizedWSIndexesMaximum(areaType).size() > 3)
	  calculate_equalized_wind_speed_indexes_for_maximum_wind(storyParams);

	if(storyParams.equalizedWDIndexes(areaType).size() > 3)
	  calculate_equalized_wind_direction_indexes(storyParams);

  }

  void read_configuration_params(wo_story_params& storyParams)
  {
	double maxErrorWindSpeed = Settings::optional_double(storyParams.theVar+"::max_error_wind_speed", 2.0);
	double maxErrorWindDirection = Settings::optional_double(storyParams.theVar+"::max_error_wind_direction", 4.0);
	double windSpeedThreshold = Settings::optional_double(storyParams.theVar+"::wind_speed_threshold", 3.0);
	double windSpeedReportingThreshold = Settings::optional_double(storyParams.theVar+"::wind_speed_reporting_threshold", 4.0);
	double windDirectionThreshold = Settings::optional_double(storyParams.theVar+"::wind_direction_threshold", 45.0);
	string rangeSeparator = Settings::optional_string(storyParams.theVar+"::rangeseparator","-");
	unsigned int minIntervalSize = optional_int(storyParams.theVar+"::min_interval_size",0);
	unsigned int maxIntervalSize = optional_int(storyParams.theVar+"::max_interval_size",4);
	
	string metersPerSecondFormat = Settings::optional_string("textgen::units::meterspersecond::format", "SI");

	storyParams.theMaxErrorWindSpeed = maxErrorWindSpeed;
	storyParams.theMaxErrorWindDirection = maxErrorWindDirection;
	storyParams.theWindSpeedThreshold = windSpeedThreshold;
	storyParams.theWindDirectionThreshold = windDirectionThreshold;
	storyParams.windSpeedReportingThreshold =windSpeedReportingThreshold;
	storyParams.theRangeSeparator = rangeSeparator;
	storyParams.theMinIntervalSize = minIntervalSize;
	storyParams.theMaxIntervalSize = maxIntervalSize;
	storyParams.theMetersPerSecondFormat = metersPerSecondFormat;
	
	storyParams.theWeatherAreas.push_back(storyParams.theArea);

	std::string split_section_name("textgen::split_the_area::" + get_area_name_string(storyParams.theArea));
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
			storyParams.theWeatherAreas.push_back(easternArea);
			storyParams.theWeatherAreas.push_back(westernArea);
			storyParams.theSplitMethod = VERTICAL;
		  }
		else if(split_method.compare("horizontal") == 0)
		  {
			storyParams.theWeatherAreas.push_back(southernArea);
			storyParams.theWeatherAreas.push_back(northernArea);
			storyParams.theSplitMethod = HORIZONTAL;
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

    // Generate the story
    //
	Paragraph paragraph;

	wo_story_params storyParams(itsVar,
								itsArea,
								itsPeriod,
								itsForecastTime,
								itsSources,
								logger);

	// read the configuration parameters
	read_configuration_params(storyParams);

	// allocate data structures for hourly data
	allocate_data_structures(storyParams);

	// populate the data structures with the relevant data
	populate_time_series(storyParams);
	
	// equalize the data
	calculate_equalized_data(storyParams);

	// find out the wind speed periods
	find_out_wind_speed_periods(storyParams);

	// find out the wind direction periods of 16-direction compass
	find_out_wind_direction_periods(storyParams);

	// find out the events (changing point) during the forecast period
	//	find_out_wind_events(storyParams);

	// find out wind event periods
	find_out_wind_event_periods(storyParams);
	
	// log functions
	//	save_raw_data(storyParams, oss.str());
	log_raw_data(storyParams);
	log_equalized_wind_speed_data_vector(storyParams);
	log_equalized_wind_direction_data_vector(storyParams);
	log_wind_speed_periods(storyParams);
	log_wind_direction_periods(storyParams);
	//	log_wind_events(storyParams);
	log_wind_event_periods(storyParams);
	//	log_merged_wind_event_periods(storyParams);

	WindForecast windForecast(storyParams);

	paragraph << windForecast.getWindStory(itsPeriod);
 
	deallocate_data_structures(storyParams);

	logger << paragraph;

	return paragraph;
  }
} // namespace TextGen
  
// ======================================================================

