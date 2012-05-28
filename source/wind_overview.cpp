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
#include "SubMaskExtractor.h"

#include <newbase/NFmiSettings.h>

#include <bitset>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <algorithm>

using namespace boost;
using namespace WeatherAnalysis;
using namespace TextGen::WindStoryTools;
using namespace std;

namespace TextGen
{
  bool wind_event_period_sort(const WindEventPeriodDataItem* first, const WindEventPeriodDataItem* second) 
  {	
	return (first->thePeriod.localStartTime() < second->thePeriod.localStartTime());
  }

  bool wind_direction_item_sort(pair<float, WeatherResult> firstItem, pair<float, WeatherResult> secondItem)
  {
	return firstItem.second.value() > secondItem.second.value();
  }

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
	std::string retval(theArea.isNamed() ? theArea.name() : "");
	retval += "_";
	retval += get_area_type_string(theArea.type());

	return retval;
  }

  std::ostream& operator<<(std::ostream& theOutput,
						   const WeatherPeriod& period)
  {
	theOutput << period.localStartTime() << "..." << period.localEndTime();

	return theOutput;
  }

  std::ostream& operator<<(std::ostream& theOutput,
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
			  << fixed << setprecision(4) << theWindDataItem.theEqualizedMedianWind.value()
			  << "; ka,k-hajonta: ("
			  << fixed << setprecision(4) << theWindDataItem.theWindSpeedMean.value()
			  << ", "
			  << fixed << setprecision(4) << theWindDataItem.theWindSpeedMean.error()
			  << ")"
			  << "; huipputuuli: "
			  << fixed << setprecision(4) << theWindDataItem.theWindMaximum.value()
			  << "; tasoitettu huipputuuli: "
			  << fixed << setprecision(4) << theWindDataItem.theEqualizedMaximumWind.value()
			  << "; suunta: "
			  << fixed << setprecision(4) << theWindDataItem.theCorrectedWindDirection.value()
			  << "; suunnan k-hajonta: "
			  << fixed << setprecision(4) << theWindDataItem.theCorrectedWindDirection.error()
			  << "; tasoitettu suunta: "
			  << fixed << setprecision(4) << theWindDataItem.theEqualizedWindDirection.value()
			  << " (" << wind_direction_string(wind_direction_id(theWindDataItem.theEqualizedWindDirection,
																 theWindDataItem.theEqualizedMaximumWind,
																 "")) << ") "
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
					<< fixed << setprecision(4) << theWindDataItem.theCorrectedWindDirection.value()
					<< " | "
					<< fixed << setprecision(4) << theWindDataItem.theWindDirection.error()
					<< " | ";
		
		output_file << directed16_speed_string(theWindDataItem.theWindSpeedMean, 
											   theWindDataItem.theCorrectedWindDirection, //theWindDataItem.theWindDirection, 
											   theVar)
					<< " |" << endl;
	  }
  }

#ifndef NDEBUG
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
			output_file	<< ", "	<< fixed << setprecision(4) << theWindDataItem.theEqualizedMedianWind.value();
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
			output_file	<< ", "	<< fixed << setprecision(4) << theWindDataItem.theEqualizedMaximumWind.value();
		  }
		if(theColumnSelectionBitset.test(3))
		  {
			output_file	<< ", "	<< fixed << setprecision(4) << theWindDataItem.theCorrectedWindDirection.value();
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
														   theWindDataItem.theCorrectedWindDirection, //theWindDataItem.theWindDirection, 
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

  void print_winddirection_distribution(const WeatherArea& theArea,
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
	
	for(unsigned int i = POHJOINEN; i <= POHJOINEN_LUODE; i++)
	  {
		output_file << ", ";
		output_file << wind_direction_string(static_cast<WindDirectionId>(i));
	  }
	output_file << ", ";
	output_file << "keskihajonta";
	output_file << endl;
 
	for(unsigned int i = 0; i < theIndexVector.size(); i++)
	  {
		const unsigned int& index = theIndexVector[i];

		const WindDataItemUnit& theWindDataItem = (*theWindDataItemVector[index])(theArea.type());
			
		output_file << theWindDataItem.thePeriod.localEndTime();

		for(unsigned int i = POHJOINEN; i <= POHJOINEN_LUODE; i++)
		  {
			output_file << ", ";			
			output_file << fixed 
						<< setprecision(2) 
						<< theWindDataItem.getWindDirectionShare(static_cast<WindDirectionId>(i));
		  }
		output_file << ", ";			
		output_file << fixed
					<< setprecision(2) 
					<< theWindDataItem.theWindDirection.error();
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
			output_file	<< fixed << setprecision(2) << theWindDataItem.theEqualizedMedianWind.value();
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
			output_file	<< fixed << setprecision(2) << theWindDataItem.theEqualizedMaximumWind.value();
			output_file << "</td>" << endl;
		  }
		if(theColumnSelectionBitset.test(3))
		  {
			output_file << "<td>";
			output_file	<< fixed << setprecision(2) << theWindDataItem.theCorrectedWindDirection.value();
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
												   theWindDataItem.theCorrectedWindDirection,
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




	/*
	for(unsigned int i = 0; i < storyParams.theWindDataVector.size(); i++)
	  {
		for(unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
		  {
			const WeatherArea& weatherArea = storyParams.theWeatherAreas[k];
			WeatherArea::Type areaType(weatherArea.type());

			WindDataItemUnit& dataItem = (storyParams.theWindDataVector[i])->getDataItem(areaType);

	 */

	for(unsigned int i = 0; i < storyParams.theWeatherAreas.size(); i++)
	  {
		const WeatherArea& weatherArea = storyParams.theWeatherAreas[i];

		columnSelectionBitset.set();
		print_csv_table(weatherArea,
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

		print_windspeed_distribution(weatherArea,
									 "_windspeed_distribution",
									 storyParams.theVar,
									 storyParams.theWindDataVector,
									 storyParams.originalWindDataIndexes(storyParams.theArea.type()));

		print_winddirection_distribution(weatherArea,
										 "_winddirection_distribution",
										 storyParams.theVar,
										 storyParams.theWindDataVector,
										 storyParams.originalWindDataIndexes(storyParams.theArea.type()));
	  }

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
			WindDirectionId windDirectionId = wind_direction_id(windDataItem.theCorrectedWindDirection,
																windDataItem.theEqualizedMaximumWind,
																storyParams.theVar);
			storyParams.theLog << windDataItem;
			storyParams.theLog << "; fraasi: "<< wind_direction_string(windDirectionId)
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

  void log_wind_event_periods(wo_story_params& storyParams)
  {
	storyParams.theLog << "*********** WIND EVENT PERIODS ***********" << endl;

	wind_event_period_data_item_vector windEventPeriodVector;
	windEventPeriodVector.insert(windEventPeriodVector.end(), 
								 storyParams.theWindSpeedEventPeriodVector.begin(),
								 storyParams.theWindSpeedEventPeriodVector.end());
	windEventPeriodVector.insert(windEventPeriodVector.end(), 
								 storyParams.theWindDirectionEventPeriodVector.begin(),
								 storyParams.theWindDirectionEventPeriodVector.end());

	std::sort(windEventPeriodVector.begin(),
			  windEventPeriodVector.end(), 
			  wind_event_period_sort);

	for(unsigned int i = 0; i < windEventPeriodVector.size(); i++)
	  {
		WindEventId windEventId = windEventPeriodVector[i]->theWindEvent;

		storyParams.theLog << windEventPeriodVector[i]->thePeriod.localStartTime() 
						   << "..."
						   << windEventPeriodVector[i]->thePeriod.localEndTime()
						   << ": "
						   << get_wind_event_string(windEventId);

		if(windEventId == TUULI_HEIKKENEE ||
		   windEventId == TUULI_VOIMISTUU ||
		   windEventId == TUULI_TYYNTYY ||
		   windEventId == MISSING_WIND_SPEED_EVENT)
		  {
			storyParams.theLog << ": "
							   << fixed << setprecision(4) 
							   << windEventPeriodVector[i]->thePeriodBeginDataItem.theEqualizedMaximumWind.value()
							   << "->"
							   << windEventPeriodVector[i]->thePeriodEndDataItem.theEqualizedMaximumWind.value()
							   << endl;
		  }
		else if(windEventId == TUULI_KAANTYY ||
				windEventId == TUULI_MUUTTUU_VAIHTELEVAKSI ||
				windEventId == MISSING_WIND_DIRECTION_EVENT)
		  {
			WeatherResult directionBeg(windEventPeriodVector[i]->thePeriodBeginDataItem.theEqualizedWindDirection);
			WeatherResult directionEnd(windEventPeriodVector[i]->thePeriodEndDataItem.theEqualizedWindDirection);
			WeatherResult speedBeg(windEventPeriodVector[i]->thePeriodBeginDataItem.theEqualizedMaximumWind);
			WeatherResult speedEnd(windEventPeriodVector[i]->thePeriodEndDataItem.theEqualizedMaximumWind);

			WindDirectionId directionIdBeg = wind_direction_id(directionBeg, speedBeg, storyParams.theVar);
			WindDirectionId directionIdEnd = wind_direction_id(directionEnd, speedEnd, storyParams.theVar);

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

		WeatherResult directionBeg(storyParams.theMergedWindEventPeriodVector[i]->thePeriodBeginDataItem.theCorrectedWindDirection);
		WeatherResult directionEnd(storyParams.theMergedWindEventPeriodVector[i]->thePeriodEndDataItem.theCorrectedWindDirection);
		WeatherResult speedBeg(storyParams.theMergedWindEventPeriodVector[i]->thePeriodBeginDataItem.theEqualizedMaximumWind);
		WeatherResult speedEnd(storyParams.theMergedWindEventPeriodVector[i]->thePeriodEndDataItem.theEqualizedMaximumWind);

		WindDirectionId directionIdBeg = wind_direction_id(directionBeg, speedBeg, storyParams.theVar);
		WindDirectionId directionIdEnd = wind_direction_id(directionEnd, speedEnd, storyParams.theVar);
		float maxWindBeg = storyParams.theMergedWindEventPeriodVector[i]->thePeriodBeginDataItem.theEqualizedMaximumWind.value();
		float maxWindEnd = storyParams.theMergedWindEventPeriodVector[i]->thePeriodEndDataItem.theEqualizedMaximumWind.value();


		if((windEventId >= 0x0 && windEventId <= TUULI_TYYNTYY) ||
		   (windEventId >= TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE && windEventId <= TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY))
		  {
			storyParams.theLog << "("
							   << fixed << setprecision(4) << maxWindBeg
							   <<  " -> "
							   << fixed << setprecision(4) << maxWindEnd
							   << ")";
		  }
		else if(windEventId == TUULI_KAANTYY)
		  {
			storyParams.theLog << "("
							   << wind_direction_string(directionIdBeg)
							   << " " << directionBeg.value()
							   <<  " -> "
							   << wind_direction_string(directionIdEnd)
							   << " " << directionEnd.value()
							   << ")";
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
							   << ")"; 
		  }
		if(ppp->theWeakWindPeriodFlag)
		  storyParams.theLog << " - weak";
		storyParams.theLog << "" << endl;
	  }
  }
#endif // NDEBUG





  void allocate_data_structures(wo_story_params& storyParams)
  {
	NFmiTime periodStartTime = storyParams.theDataPeriod.localStartTime();
	
	while(periodStartTime.IsLessThan(storyParams.theDataPeriod.localEndTime()))
	  {
		WeatherPeriod weatherPeriod(periodStartTime, periodStartTime);
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

  void populate_windspeed_distribution_time_series(const AnalysisSources& theSources,
												   const WeatherArea& theArea,
												   const WeatherPeriod& thePeriod,
												   const string& theVar,
												   vector <pair<float, WeatherResult> >& theWindSpeedDistribution)
  {
	GridForecaster forecaster;

	float ws_lower_limit(0.0);
	float ws_upper_limit(1.0);

	while (ws_lower_limit < HIRMUMYRSKY_LOWER_LIMIT)
	  {
		RangeAcceptor acceptor;
		acceptor.lowerLimit(ws_lower_limit);
		if(ws_lower_limit < HIRMUMYRSKY_LOWER_LIMIT + 1)
		  acceptor.upperLimit(ws_upper_limit-0.0001);

				
		WeatherResult share =
		  forecaster.analyze(theVar+"::fake::tyyni::share",
							 theSources,
							 WindSpeed,
							 Mean,
							 Percentage,
							 theArea,
							 thePeriod,
							 DefaultAcceptor(),
							 DefaultAcceptor(),
							 acceptor);

		pair<float, WeatherResult> shareItem(ws_lower_limit, share);
		theWindSpeedDistribution.push_back(shareItem);

		ws_lower_limit += 1.0;
		ws_upper_limit += 1.0;

	  }
  }

  pair<float, WeatherResult> get_share_item(const AnalysisSources& theSources,
											const WeatherArea& theArea,
											const WeatherPeriod& thePeriod,
											const string& theVar,
											const WindDirectionId& theWindDirection)
  {
 	GridForecaster forecaster;
	RangeAcceptor acceptor;
	float ws_lower_limit(0.0);
	float ws_upper_limit(360.0);

	switch(theWindDirection)
	  {
	  case POHJOINEN:
		{
		  ws_lower_limit = 0.0;
		  ws_upper_limit = 22.5;
		}
		break;
	  case ETELA:
		{
		  ws_lower_limit = 157.5;
		  ws_upper_limit = 202.5;
		}
		break;
	  case ITA:
		{
		  ws_lower_limit = 67.5;
		  ws_upper_limit = 112.5;
		}
		break;
	  case LANSI:
		{
		  ws_lower_limit = 247.5;
		  ws_upper_limit = 292.5;
		}
		break;
	  case KOILLINEN:
		{
		  ws_lower_limit = 22.5;
		  ws_upper_limit = 67.5;
		}
		break;
	  case KAAKKO:
		{
		  ws_lower_limit = 112.5;
		  ws_upper_limit = 157.5;
		}
		break;
	  case LOUNAS:
		{
		  ws_lower_limit = 202.5;
		  ws_upper_limit = 247.5;
		}
		break;
	  case LUODE:
		{
		  ws_lower_limit = 292.5;
		  ws_upper_limit = 337.5;
		}
		break;
	  default:
		break;
	  }

	acceptor.lowerLimit(ws_lower_limit);
	acceptor.upperLimit(ws_upper_limit-0.0001);
	WeatherResult share = forecaster.analyze(theVar+"::fake::tyyni::share",
											 theSources,
											 WindDirection,
											 Mean,
											 Percentage,
											 theArea,
											 thePeriod,
											 DefaultAcceptor(),
											 DefaultAcceptor(),
											 acceptor);

	if(theWindDirection == POHJOINEN)
	  {
		ws_lower_limit = 337.5;
		ws_upper_limit = 360.0;
		acceptor.lowerLimit(ws_lower_limit);
		acceptor.upperLimit(ws_upper_limit);
		WeatherResult share2 = forecaster.analyze(theVar+"::fake::tyyni::share",
												  theSources,
												  WindDirection,
												  Mean,
												  Percentage,
												  theArea,
												  thePeriod,
												  DefaultAcceptor(),
												  DefaultAcceptor(),
												  acceptor);
		share = WeatherResult(share.value()+share2.value(), 0.0);
	  }

	pair<float, WeatherResult> shareItem(ws_lower_limit+22.5, share);

	return shareItem;
  }

  void populate_winddirection_distribution_time_series(const AnalysisSources& theSources,
													   const WeatherArea& theArea,
													   const WeatherPeriod& thePeriod,
													   const string& theVar,
													   vector<pair<float, WeatherResult> >& theWindDirectionDistribution)
  {

	theWindDirectionDistribution.push_back(get_share_item(theSources,
														  theArea,
														  thePeriod,
														  theVar,
														  POHJOINEN));
	theWindDirectionDistribution.push_back(get_share_item(theSources,
														  theArea,
														  thePeriod,
														  theVar,
														  ETELA));
	theWindDirectionDistribution.push_back(get_share_item(theSources,
														  theArea,
														  thePeriod,
														  theVar,
														  ITA));
	theWindDirectionDistribution.push_back(get_share_item(theSources,
														  theArea,
														  thePeriod,
														  theVar,
														  LANSI));
	theWindDirectionDistribution.push_back(get_share_item(theSources,
														  theArea,
														  thePeriod,
														  theVar,
														  KOILLINEN));
	theWindDirectionDistribution.push_back(get_share_item(theSources,
														  theArea,
														  thePeriod,
														  theVar,
														  KAAKKO));
	theWindDirectionDistribution.push_back(get_share_item(theSources,
														  theArea,
														  thePeriod,
														  theVar,
														  LOUNAS));
	theWindDirectionDistribution.push_back(get_share_item(theSources,
														  theArea,
														  thePeriod,
														  theVar,
														  LUODE));
  }

  WeatherResult mean_wind_direction(const AnalysisSources& theSources,
									const WeatherArea& theArea,
									const WeatherPeriod& thePeriod,
									const WeatherResult& theEqualizedWindMaximum,
									const string& theVar)
  {
	GridForecaster forecaster;

	/*
	WeatherResult windMaximum =
	  forecaster.analyze(theVar+"::fake::wind::maximumwind",
						 theSources,
						 MaximumWind,
						 Maximum,
						 Mean,
						 theArea,
						 thePeriod);
	*/
	WeatherResult meanDirection = forecaster.analyze(theVar+"::fake::wind:direction",
													 theSources,
													 WindDirection,
													 Mean,
													 Mean,
													 theArea,
													 thePeriod);

	if(direction_accuracy(meanDirection.error(), theVar) == bad_accuracy &&
	   theEqualizedWindMaximum.value() > 6.5)
	  {
		value_distribution_data_vector directionDistribution;
		
		populate_winddirection_distribution_time_series(theSources,
														theArea,
														thePeriod,
														theVar,
														directionDistribution);

		std::sort(directionDistribution.begin(), 
				  directionDistribution.end(),
				  wind_direction_item_sort);

		// lets set moderate accuracy
		WeatherResult retval(directionDistribution[0].first, 30.0);
		return  retval;
	  }

	return meanDirection;
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

			dataItem.theEqualizedMedianWind = dataItem.theWindSpeedMedian;

			dataItem.theWindMaximum =
			  forecaster.analyze(storyParams.theVar+"::fake::wind::maximumwind",
								 storyParams.theSources,
								 MaximumWind,
								 Maximum,
								 Mean,
								 weatherArea,
								 dataItem.thePeriod);

			// ARE 2012-04-26: if MaximumWind parameter is missing, use WindSpeed maximum + 1.0 instead
			if(dataItem.theWindMaximum.value() == kFloatMissing)
			  dataItem.theWindMaximum = WeatherResult(dataItem.theWindSpeedMax.value()+1.0,dataItem.theWindSpeedMax.error());

			dataItem.theEqualizedMaximumWind = dataItem.theWindMaximum;

 			dataItem.theWindDirection =
			  forecaster.analyze(storyParams.theVar+"::fake::wind:direction",
								 storyParams.theSources,
								 WindDirection,
								 Mean,
								 Mean,
								 weatherArea,
								 dataItem.thePeriod);

			dataItem.theCorrectedWindDirection = dataItem.theWindDirection;
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

			populate_winddirection_distribution_time_series(storyParams.theSources,
															weatherArea,
															dataItem.thePeriod,
															storyParams.theVar,
															dataItem.theWindDirectionDistribution);

			if(direction_accuracy(dataItem.theWindDirection.error(), storyParams.theVar) == bad_accuracy &&
			   dataItem.theWindMaximum.value() > 6.5)
			  {
				value_distribution_data_vector directionDistribution(dataItem.theWindDirectionDistribution);

				std::sort(directionDistribution.begin(), 
						  directionDistribution.end(),
						  wind_direction_item_sort);

				// first implementation: lets just take the biggest share
				WeatherResult correctedDirection(directionDistribution[0].first, dataItem.theWindDirection.error());

				dataItem.theCorrectedWindDirection = correctedDirection;
				dataItem.theEqualizedWindDirection =  correctedDirection;
			  }

			storyParams.originalWindDataIndexes(areaType).push_back(i);
			storyParams.equalizedWSIndexesMedian(areaType).push_back(i);
			storyParams.equalizedWSIndexesMaximum(areaType).push_back(i);
			storyParams.equalizedWDIndexes(areaType).push_back(i);
		  }
	  }
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
																			 wind_speed_id(dataItem.theEqualizedMedianWind)));
		return;
	  }

	unsigned int periodStartEqualizedDataIndex = equalizedWSIndexesMedian[0];
	unsigned int periodEndEqualizedDataIndex = equalizedWSIndexesMedian[0];
	const WindDataItemUnit& dataItemFirst = (*storyParams.theWindDataVector[periodStartEqualizedDataIndex])(areaType);

	WindSpeedId previous_wind_speed_id(wind_speed_id(dataItemFirst.theEqualizedMedianWind));

	for(unsigned int i = 1; i < equalizedWSIndexesMedian.size(); i++)
	  {
		equalizedDataIndex = equalizedWSIndexesMedian[i];
		const WindDataItemUnit& dataItemCurrent = (*storyParams.theWindDataVector[equalizedDataIndex])(areaType);
  
		WindSpeedId current_wind_speed_id(wind_speed_id(dataItemCurrent.theEqualizedMedianWind));
		
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
																					 wind_direction_id(dataItem.theEqualizedWindDirection, dataItem.theEqualizedMaximumWind, storyParams.theVar)));
		return;
	  }

	const WindDataItemUnit& dataItemFirst = (*dataVector[0])(areaType);
	WindDirectionId previous_wind_direction_id(wind_direction_id(dataItemFirst.theEqualizedWindDirection,
																 dataItemFirst.theEqualizedMaximumWind,
																 storyParams.theVar));

	NFmiTime periodStartTime(dataItemFirst.thePeriod.localStartTime());
	NFmiTime periodEndTime(dataItemFirst.thePeriod.localStartTime());

	for(unsigned int i = 1; i < dataVector.size(); i++)
	  {
		const WindDataItemUnit& dataItemCurrent = (*dataVector[i])(areaType);
  
		WindDirectionId current_wind_direction_id(wind_direction_id(dataItemCurrent.theEqualizedWindDirection, 
																	dataItemCurrent.theEqualizedMaximumWind,
																	storyParams.theVar));

		if(current_wind_direction_id != previous_wind_direction_id)
		  {
			if(i < dataVector.size() - 1)
			  {
				const WindDataItemUnit& dataItemPrevious = (*dataVector[i-1])(areaType);
				const WindDataItemUnit& dataItemNext = (*dataVector[i+1])(areaType);
				if(wind_direction_id(dataItemPrevious.theEqualizedWindDirection, 
									 dataItemPrevious.theEqualizedMaximumWind, 
									 storyParams.theVar) == 
				   wind_direction_id(dataItemNext.theEqualizedWindDirection, 
									 dataItemNext.theEqualizedMaximumWind,
									 storyParams.theVar))
				  {
					continue;
				  }
			  }

			periodEndTime = dataItemCurrent.thePeriod.localStartTime();
			periodEndTime.ChangeByHours(-1);

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

	const WindDataItemUnit& dataItemLast = (*dataVector[storyParams.theWindDataVector.size()-1])(areaType);

	WeatherPeriod windDirectionLastPeriod(periodEndTime, dataItemLast.thePeriod.localEndTime());

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
									   const WeatherResult& maximumWind1, 
									   const WeatherResult& maximumWind2,
									   const string& var,
									   const double& windDirectionThreshold)
  {
	WindDirectionId direction1 = wind_direction_id(windDirection1, maximumWind1, var);
	WindDirectionId direction2 = wind_direction_id(windDirection2, maximumWind2, var);

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

	return (difference > windDirectionThreshold && direction1 != direction2  ? TUULI_KAANTYY : MISSING_WIND_DIRECTION_EVENT);
  }

  WindEventId get_wind_direction_event(const WindEventPeriodDataItem& windEventPeriodDataItem,
									   const string& var,
									   const double& windDirectionThreshold)
  {
	const WeatherResult& windDirection1(windEventPeriodDataItem.thePeriodBeginDataItem.theEqualizedWindDirection);
	const WeatherResult& windDirection2(windEventPeriodDataItem.thePeriodEndDataItem.theEqualizedWindDirection);
	const WeatherResult& maximumWind1(windEventPeriodDataItem.thePeriodBeginDataItem.theEqualizedMaximumWind);
	const WeatherResult& maximumWind2(windEventPeriodDataItem.thePeriodEndDataItem.theEqualizedMaximumWind);
	
	return get_wind_direction_event(windDirection1, 
									windDirection2,
									maximumWind1, 
									maximumWind2,
									var,
									windDirectionThreshold);
  }

  // find out wind events: strengthening wind, weakening wind, 
  // or missing wind speed event (no signifigant change in speed
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
		if(storyParams.theWindSpeedEventPeriodVector.size() > 0)
		  {
			WindEventPeriodDataItem* previousEventPeriod = storyParams.theWindSpeedEventPeriodVector[storyParams.theWindSpeedEventPeriodVector.size()-1];

			if(previousEventPeriod->theWindEvent == windEvent)
			  {
				WeatherPeriod mergedPeriod(previousEventPeriod->thePeriod.localStartTime(),
										   windEventPeriod.localEndTime());
				
				WindEventPeriodDataItem* newEventPeriod = new WindEventPeriodDataItem(mergedPeriod,
																					  windEvent,
																					  previousEventPeriod->thePeriodBeginDataItem,
																					  dataItemPeriodEnd);
		
				delete previousEventPeriod;

				storyParams.theWindSpeedEventPeriodVector.erase(storyParams.theWindSpeedEventPeriodVector.begin()+storyParams.theWindSpeedEventPeriodVector.size()-1);
				
				storyParams.theWindSpeedEventPeriodVector.push_back(newEventPeriod);

				continue;
			  }

		  }

		storyParams.theWindSpeedEventPeriodVector.push_back(new WindEventPeriodDataItem(windEventPeriod,
																						windEvent,
																						dataItemPeriodBegin,
																						dataItemPeriodEnd));
	  }
  }

  void find_out_wind_direction_event_periods(wo_story_params& storyParams)
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
		const WindDataItemUnit& dataItemPeriodBegin = (*storyParams.theWindDataVector[periodBeginDataIndex])(storyParams.theArea.type());
		const WindDataItemUnit& dataItemPeriodEnd = (*storyParams.theWindDataVector[periodEndDataIndex])(storyParams.theArea.type());

		// define the event period
		WeatherPeriod windEventPeriod(dataItemPeriodBegin.thePeriod.localStartTime(), 
									  dataItemPeriodEnd.thePeriod.localStartTime());
		WindEventId windEvent(MISSING_WIND_DIRECTION_EVENT);
		
		windEvent = get_wind_direction_event(dataItemPeriodBegin.theEqualizedWindDirection,
											 dataItemPeriodEnd.theEqualizedWindDirection,
											 dataItemPeriodBegin.theEqualizedMaximumWind,
											 dataItemPeriodEnd.theEqualizedMaximumWind,
											 storyParams.theVar,
											 storyParams.theWindDirectionThreshold);

		// merge the similar wind events
		if(storyParams.theWindDirectionEventPeriodVector.size() > 0)
		  {
			WindEventPeriodDataItem* previousEventPeriod = storyParams.theWindDirectionEventPeriodVector[storyParams.theWindDirectionEventPeriodVector.size()-1];

			if(previousEventPeriod->theWindEvent == windEvent)
			  {
				// if wind turns to another direction, don't merge
				if(same_direction(previousEventPeriod->thePeriodBeginDataItem.theEqualizedWindDirection,
								  dataItemPeriodEnd.theEqualizedWindDirection,
								  previousEventPeriod->thePeriodBeginDataItem.theEqualizedMaximumWind,
								  dataItemPeriodEnd.theEqualizedMaximumWind,
								  storyParams.theVar,
								  true) ||
				   ((windEvent & TUULI_KAANTYY) &&
					wind_turns_to_the_same_direction(previousEventPeriod->thePeriodBeginDataItem.theEqualizedWindDirection.value(),
													 previousEventPeriod->thePeriodEndDataItem.theEqualizedWindDirection.value(),
													 dataItemPeriodEnd.theEqualizedWindDirection.value())))
				  {
					windEvent = get_wind_direction_event(previousEventPeriod->thePeriodBeginDataItem.theEqualizedWindDirection,
														 dataItemPeriodEnd.theEqualizedWindDirection,
														 previousEventPeriod->thePeriodBeginDataItem.theEqualizedMaximumWind,
														 dataItemPeriodEnd.theEqualizedMaximumWind,
														 storyParams.theVar,
														 storyParams.theWindDirectionThreshold);

					WeatherPeriod mergedPeriod(previousEventPeriod->thePeriod.localStartTime(),
											   windEventPeriod.localEndTime());
					WindEventPeriodDataItem* newEventPeriod = new WindEventPeriodDataItem(mergedPeriod,
																						  windEvent,
																						  previousEventPeriod->thePeriodBeginDataItem,
																						  dataItemPeriodEnd);

					delete previousEventPeriod;
					
					storyParams.theWindDirectionEventPeriodVector.erase(storyParams.theWindDirectionEventPeriodVector.begin()+storyParams.theWindDirectionEventPeriodVector.size()-1);
					
					storyParams.theWindDirectionEventPeriodVector.push_back(newEventPeriod);
					continue;
				  }
			  }

		  }

		storyParams.theWindDirectionEventPeriodVector.push_back(new WindEventPeriodDataItem(windEventPeriod,
																							windEvent,
																							dataItemPeriodBegin,
																							dataItemPeriodEnd));
	  }

	// after merging several missing wind direction events it may be that 
	// direction has changed gradually
	for(unsigned int i = 0; i < storyParams.theWindDirectionEventPeriodVector.size(); i++)
	  {
		WindEventPeriodDataItem* item = storyParams.theWindDirectionEventPeriodVector[i];
		if(item->theWindEvent == MISSING_WIND_DIRECTION_EVENT)
		  {
			storyParams.theWindDirectionEventPeriodVector[i]->theWindEvent = 
			  get_wind_direction_event(item->thePeriodBeginDataItem.theEqualizedWindDirection,
									   item->thePeriodEndDataItem.theEqualizedWindDirection,
									   item->thePeriodBeginDataItem.theEqualizedMaximumWind,
									   item->thePeriodEndDataItem.theEqualizedMaximumWind,
									   storyParams.theVar,
									   storyParams.theWindDirectionThreshold);
		  }
	  }
  }


  // check weather the wind is only some hours in some direction
  // only vaihteleva wind checked
  void find_out_transient_wind_direction_periods(wo_story_params& storyParams)
  {
	for(unsigned int i = 0; i < storyParams.theMergedWindEventPeriodVector.size(); i++)
	  {
		WindEventPeriodDataItem* eventItem(storyParams.theMergedWindEventPeriodVector[i]);
		WindEventId windEventIdCurrent = eventItem->theWindEvent;

		if(windEventIdCurrent & TUULI_MUUTTUU_VAIHTELEVAKSI)
		  {
			if(storyParams.theMergedWindEventPeriodVector.size() > i + 1)
			  {
				WindEventPeriodDataItem* eventItemNext(storyParams.theMergedWindEventPeriodVector[i+1]);

				WindDirectionId windDirectionIdAvg(get_wind_direction_id_at(storyParams.theWindDataVector,
																			storyParams.theArea,
																			storyParams.theSources,
																			eventItemNext->thePeriod,
																			storyParams.theVar));

				if(eventItemNext->theWindEvent == MISSING_WIND_EVENT && 
				   windDirectionIdAvg != VAIHTELEVA &&
				   get_period_length(eventItem->thePeriod) <= 3)
				  {
					eventItem->theTransientDirectionChangeFlag = true;
					continue;
				  }
			  }

			for(unsigned int k = i+1; k < storyParams.theMergedWindEventPeriodVector.size(); k++)
			  {
				WindEventPeriodDataItem* eventItemLater(storyParams.theMergedWindEventPeriodVector[k]);
				WindEventId windEventLater = eventItemLater->theWindEvent;
				if((windEventLater & TUULI_KAANTYY || windEventLater & TUULI_VOIMISTUU) &&
				   eventItemLater->thePeriod.localStartTime().DifferenceInHours(eventItem->thePeriod.localStartTime()) <= 3)
				  {
					eventItem->theTransientDirectionChangeFlag = true;
					break;
				  }
			  }
		  }
	  }
  }

  void find_out_long_term_wind_speed_periods(wo_story_params& storyParams)
  {
	for(unsigned int i = 0; i < storyParams.theMergedWindEventPeriodVector.size(); i++)
	  {
		const WindEventPeriodDataItem* eventItemCurrent(storyParams.theMergedWindEventPeriodVector[i]);
		WindEventId windEventCurrent = eventItemCurrent->theWindEvent;

		if(windEventCurrent & TUULI_HEIKKENEE ||
		   windEventCurrent & TUULI_VOIMISTUU ||
		   windEventCurrent & TUULI_TYYNTYY)
		  {
			for(unsigned int k = i+1; k < storyParams.theMergedWindEventPeriodVector.size(); k++)
			  {
				const WindEventPeriodDataItem* eventItemLater(storyParams.theMergedWindEventPeriodVector[k]);
				WindEventId windEventLater = eventItemLater->theWindEvent;
				if(windEventLater & TUULI_HEIKKENEE ||
				   windEventLater & TUULI_VOIMISTUU ||
				   windEventLater & TUULI_TYYNTYY)
				  {
					if(eventItemLater->thePeriod.localStartTime().DifferenceInHours(eventItemCurrent->thePeriod.localEndTime()) >= 6)
					  {
						storyParams.theMergedWindEventPeriodVector[i]->theLongTermSpeedChangeFlag = true;
					  }
					break;
				  }
				if(k == storyParams.theMergedWindEventPeriodVector.size() - 1 &&
				   eventItemLater->thePeriod.localEndTime().DifferenceInHours(eventItemCurrent->thePeriod.localEndTime()) >= 6)
				  {
					storyParams.theMergedWindEventPeriodVector[i]->theLongTermSpeedChangeFlag = true;
				  }
			  }
		  }
	  }
  }

  WindEventId sort_out_wind_event(const WindEventId& windEvent1, 
								   const WindEventId& windEvent2)
  {
	WindEventId eventId1 = (windEvent1 == MISSING_WIND_SPEED_EVENT || windEvent1 == MISSING_WIND_DIRECTION_EVENT ?
							MISSING_WIND_EVENT : windEvent1);
	WindEventId eventId2 = (windEvent2 == MISSING_WIND_SPEED_EVENT || windEvent2 == MISSING_WIND_DIRECTION_EVENT ?
							MISSING_WIND_EVENT : windEvent2);

	return static_cast<WindEventId>(eventId1 | eventId2);
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


	if(is_inside(theSpeedEventItem.thePeriod,
				 theDirectionEventItem.thePeriod))
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

			WeatherResult startDirection(theDirectionEventItem.thePeriodBeginDataItem.theEqualizedWindDirection);
			WeatherResult endDirection(theSpeedEventItem.thePeriodEndDataItem.theEqualizedWindDirection);
			WeatherResult startMaxSpeed(theDirectionEventItem.thePeriodBeginDataItem.theEqualizedMaximumWind);
			WeatherResult endMaxSpeed(theSpeedEventItem.thePeriodEndDataItem.theEqualizedMaximumWind);

			bool directionTurns(wind_direction_turns(startDirection,
													 endDirection,
													 startMaxSpeed,
													 endMaxSpeed,
													 ""));

			add_merged_period(period,
							  (directionTurns ? windDirectionEvent : MISSING_WIND_EVENT),
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
		// direction period continues after speed period ends
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
  // theOldItem2 is the item that is merged to another and later removed
  bool re_create_merged_item(WindEventPeriodDataItem** theOldItem1, 
							 WindEventPeriodDataItem* theOldItem2,
							 const WeatherPeriod& thePeriod,
							 const wo_story_params& storyParams)
  {
	const WindEventPeriodDataItem* pEarlierPeriodItem = (*theOldItem1)->thePeriod.localStartTime() < theOldItem2->thePeriod.localStartTime() ? (*theOldItem1) : theOldItem2;
	const WindEventPeriodDataItem* pLaterPeriodItem = pEarlierPeriodItem == (*theOldItem1) ? theOldItem2 : (*theOldItem1);
	
	if(((!pEarlierPeriodItem->theWeakWindPeriodFlag && pLaterPeriodItem->theWeakWindPeriodFlag) ||
		(pEarlierPeriodItem->theWeakWindPeriodFlag && !pLaterPeriodItem->theWeakWindPeriodFlag)))
	  {
		if(!(pEarlierPeriodItem->theWindEvent == MISSING_WIND_EVENT && pLaterPeriodItem->theWindEvent == MISSING_WIND_EVENT))
		  return false;
	  }

	WeatherPeriod period(pEarlierPeriodItem->thePeriod.localStartTime(),
						 pLaterPeriodItem->thePeriod.localEndTime());

	WindEventId compositeWindEvent(sort_out_wind_event(pEarlierPeriodItem->theWindEvent, pLaterPeriodItem->theWindEvent));

	// check if direction changes enough
	/*
	if(compositeWindEvent & TUULI_KAANTYY)
	  {
		WindEventId windDirectionEvent = 
		  get_wind_direction_event(pEarlierPeriodItem->thePeriodBeginDataItem.theEqualizedWindDirection,
								   pLaterPeriodItem->thePeriodBeginDataItem.theEqualizedWindDirection,
								   pEarlierPeriodItem->thePeriodBeginDataItem.theEqualizedMaximumWind,
								   pLaterPeriodItem->thePeriodBeginDataItem.theEqualizedMaximumWind,
								   storyParams.theVar,
								   storyParams.theWindDirectionThreshold);

		if(windDirectionEvent == MISSING_WIND_DIRECTION_EVENT)
		  compositeWindEvent = mask_wind_event(compositeWindEvent, TUULI_KAANTYY);
	  }
	*/
	// check if speed changes enough
	/*
	if(compositeWindEvent & TUULI_HEIKKENEE ||
	   compositeWindEvent & TUULI_VOIMISTUU)
	  {
		bool windSpeedDifferEnough = wind_speed_differ_enough(storyParams.theSources,
															  storyParams.theArea,
															  period,
															  storyParams.theVar,
															  storyParams.theWindDataVector);
		if(!windSpeedDifferEnough)
		  {
			WindEventId mask(compositeWindEvent & TUULI_HEIKKENEE ? TUULI_HEIKKENEE : TUULI_VOIMISTUU);
			compositeWindEvent = mask_wind_event(compositeWindEvent, mask);
		  }
	  }
	*/
	WindEventPeriodDataItem* pNewItem = new WindEventPeriodDataItem(period,
																	compositeWindEvent,
																	pEarlierPeriodItem->thePeriodBeginDataItem,
																	pLaterPeriodItem->thePeriodEndDataItem);

	pNewItem->theWeakWindPeriodFlag = is_weak_period(storyParams, period);

	delete (*theOldItem1);

	(*theOldItem1) = pNewItem; 

	theOldItem2->theReportThisEventPeriodFlag = false;


	return true;
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

		bool vaihtelevaTuuli((get_wind_direction_id_at(storyParams.theWindDataVector,
													   storyParams.theArea,
													   storyParams.theSources,
													   currentDataItem->thePeriod,
													   storyParams.theVar) == VAIHTELEVA &&
							  get_wind_direction_id_at(storyParams.theWindDataVector,
													   storyParams.theArea,
													   storyParams.theSources,
													   previousDataItem->thePeriod,
													   storyParams.theVar) != VAIHTELEVA) ||
							 (get_wind_direction_id_at(storyParams.theWindDataVector,
													   storyParams.theArea,
													   storyParams.theSources,
													   currentDataItem->thePeriod,
													   storyParams.theVar) != VAIHTELEVA &&
							  get_wind_direction_id_at(storyParams.theWindDataVector,
													   storyParams.theArea,
													   storyParams.theSources,
													   previousDataItem->thePeriod,
													   storyParams.theVar) == VAIHTELEVA));

		if(!vaihtelevaTuuli &&
		   currentDataItem->theWindEvent == TUULI_KAANTYY &&
		   previousDataItem->theWindEvent == TUULI_KAANTYY_JA_VOIMISTUU)
		  {
			WindDirectionId directionBeg = wind_direction_id(currentDataItem->thePeriodBeginDataItem.theEqualizedWindDirection, currentDataItem->thePeriodBeginDataItem.theEqualizedMaximumWind, storyParams.theVar);
			WindDirectionId directionEnd = wind_direction_id(currentDataItem->thePeriodEndDataItem.theEqualizedWindDirection, currentDataItem->thePeriodEndDataItem.theEqualizedMaximumWind, storyParams.theVar);

			if(directionBeg == directionEnd)
			  {
				if(re_create_merged_item(&previousDataItem,
										 currentDataItem,
										 WeatherPeriod(previousDataItem->thePeriod.localStartTime(),
													   currentDataItem->thePeriod.localEndTime()),
										 storyParams))
				  storyParams.theMergedWindEventPeriodVector[i-1] = previousDataItem;
				i++;				
			  }
		  }
		else if(/*(currentDataItem->theWindEvent == TUULI_VOIMISTUU &&
				  previousDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI) ||
				(currentDataItem->theWindEvent == TUULI_HEIKKENEE &&
				previousDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI) ||*/
				!vaihtelevaTuuli &&
				((currentDataItem->theWindEvent == TUULI_VOIMISTUU &&
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
				  previousDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE)))
		  {
			if(re_create_merged_item(&previousDataItem,
									 currentDataItem,
									 WeatherPeriod(previousDataItem->thePeriod.localStartTime(),
												currentDataItem->thePeriod.localEndTime()),
									 storyParams))
			  {
				storyParams.theMergedWindEventPeriodVector[i-1] = previousDataItem;
				i++;
			  }
		  }
		else if(!vaihtelevaTuuli &&
				((currentDataItem->theWindEvent == TUULI_KAANTYY_JA_VOIMISTUU &&
				  previousDataItem->theWindEvent == TUULI_KAANTYY) ||
				 (currentDataItem->theWindEvent == TUULI_KAANTYY_JA_HEIKKENEE &&
				  previousDataItem->theWindEvent == TUULI_KAANTYY) ||
				 (currentDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU &&
				  previousDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI) ||
				 (currentDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE &&
				  previousDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI)))
		  {
			if(previousDataItem->theWindEvent == TUULI_KAANTYY &&
			   !wind_turns_to_the_same_direction(previousDataItem->thePeriodBeginDataItem.theEqualizedWindDirection.value(),
												 previousDataItem->thePeriodEndDataItem.theEqualizedWindDirection.value(),
												 currentDataItem->thePeriodEndDataItem.theEqualizedWindDirection.value()))
			  continue;

			if(re_create_merged_item(&currentDataItem,
								  previousDataItem,
								  WeatherPeriod(previousDataItem->thePeriod.localStartTime(),
												currentDataItem->thePeriod.localEndTime()),
									 storyParams))
			  storyParams.theMergedWindEventPeriodVector[i] = currentDataItem;

		  }		
		else if(!vaihtelevaTuuli &&
				((currentDataItem->theWindEvent == TUULI_KAANTYY_JA_VOIMISTUU &&
				  previousDataItem->theWindEvent == TUULI_VOIMISTUU) ||
				 (currentDataItem->theWindEvent == TUULI_KAANTYY_JA_HEIKKENEE &&
				  previousDataItem->theWindEvent == TUULI_HEIKKENEE)))
		  {
			WindDirectionId windDirectionId(wind_direction_id(previousDataItem->thePeriodBeginDataItem.theEqualizedWindDirection,
															  previousDataItem->thePeriodBeginDataItem.theEqualizedMaximumWind,
															  storyParams.theVar));
			
			if((windDirectionId != VAIHTELEVA && 
				get_period_length(previousDataItem->thePeriod) <= 4) ||
			   get_period_length(previousDataItem->thePeriod) <= 1)
			  {
				if(re_create_merged_item(&currentDataItem,
										 previousDataItem,
										 WeatherPeriod(previousDataItem->thePeriod.localStartTime(),
													currentDataItem->thePeriod.localEndTime()),
										 storyParams))
				  storyParams.theMergedWindEventPeriodVector[i] = currentDataItem;
			  }
		  }
		else if(!vaihtelevaTuuli &&
				((previousDataItem->theWindEvent == TUULI_KAANTYY_JA_VOIMISTUU &&
				  currentDataItem->theWindEvent == TUULI_HEIKKENEE) ||
				 (previousDataItem->theWindEvent == TUULI_KAANTYY_JA_HEIKKENEE &&
				  currentDataItem->theWindEvent == TUULI_VOIMISTUU)))
		  {
			if(get_part_of_the_day_id_narrow(previousDataItem->thePeriod) ==
			   get_part_of_the_day_id_narrow(currentDataItem->thePeriod) &&
			   get_part_of_the_day_id_narrow(previousDataItem->thePeriod) != MISSING_PART_OF_THE_DAY_ID)
			  {
				if(re_create_merged_item(&previousDataItem,
										 currentDataItem,
										 WeatherPeriod(previousDataItem->thePeriod.localStartTime(),
													   currentDataItem->thePeriod.localEndTime()),
										 storyParams))

				  previousDataItem->theWindEvent =  
					static_cast<WindEventId>(get_wind_direction_event(*previousDataItem,
																	  storyParams.theVar,
																	  storyParams.theWindDirectionThreshold) | 
											 get_wind_speed_event(previousDataItem->thePeriodBeginDataItem.theEqualizedMaximumWind,
																  previousDataItem->thePeriodEndDataItem.theEqualizedMaximumWind,
																  storyParams.theWindSpeedThreshold));
				if(previousDataItem->theWindEvent == MISSING_WIND_SPEED_EVENT ||
				   previousDataItem->theWindEvent == MISSING_WIND_DIRECTION_EVENT)
				  previousDataItem->theWindEvent = MISSING_WIND_EVENT;

				  storyParams.theMergedWindEventPeriodVector[i-1] = previousDataItem;
			  }
		  }
	  }
  }

  bool is_weak_period(const wo_story_params& theParameters, const WeatherPeriod& thePeriod)
  {
	unsigned int counter(0);
	float cumulativeMaxWind(0.0);

 	for(unsigned int i = 0; i < theParameters.theWindDataVector.size() ; i++)
	  {
		WindDataItemUnit& item = theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());
		if(is_inside(item.thePeriod.localStartTime(), thePeriod))
		  {
			if(round(item.theEqualizedMaximumWind.value()) <= WEAK_WIND_SPEED_UPPER_LIMIT)
			  {
				cumulativeMaxWind += item.theEqualizedMaximumWind.value();
				counter++;
			  }
		  }
	  }

	return (counter > 0 ? (cumulativeMaxWind / counter) <= WEAK_WIND_SPEED_UPPER_LIMIT : false);
 }

  WindEventPeriodDataItem* find_weak_period(WindEventPeriodDataItem** theDataItem, wo_story_params& theParameters)
  {
	unsigned int startIndex(UINT_MAX);
	unsigned int endIndex(UINT_MAX);

	WeatherPeriod originalPeriod((*theDataItem)->thePeriod);
	WindEventPeriodDataItem* retval(0);

	for(unsigned int i = 0; i < theParameters.theWindDataVector.size() ; i++)
	  {
		WindDataItemUnit& item = theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());
		if(is_inside(item.thePeriod.localStartTime(), originalPeriod))
		  {
			if(item.theEqualizedMaximumWind.value() <= WEAK_WIND_SPEED_UPPER_LIMIT)
			  {
				if(startIndex == UINT_MAX)
				  startIndex = i;
				endIndex = i;
			  }
		  }
	  }

	if(startIndex != UINT_MAX)
	  {
		WindDataItemUnit* begItem = &(theParameters.theWindDataVector[startIndex]->getDataItem(theParameters.theArea.type()));
		WindDataItemUnit* endItem = &(theParameters.theWindDataVector[endIndex]->getDataItem(theParameters.theArea.type()));

		WeatherPeriod period(begItem->thePeriod.localStartTime(), endItem->thePeriod.localStartTime());

		if(period == originalPeriod) // the whole period is weak
		  {
			(*theDataItem)->theWindEvent = MISSING_WIND_EVENT;
			(*theDataItem)->theWeakWindPeriodFlag = true;
		  }
		else
		  {
			retval = new WindEventPeriodDataItem(period,
												 MISSING_WIND_EVENT,
												 *begItem,
												 *endItem);

			retval->theWeakWindPeriodFlag = true;

			// create a new WindEventPeriodDataItem
			NFmiTime startTime;
			NFmiTime endTime;
			
			// period of the new WindEventPeriodDataItem
			if(period.localStartTime() == originalPeriod.localStartTime())
			  {
				startTime = period.localEndTime();
				startTime.ChangeByHours(1);
				endTime = originalPeriod.localEndTime();
			  }
			else
			  {
				startTime = originalPeriod.localStartTime();
				endTime = period.localStartTime();
				endTime.ChangeByHours(-1);
			  }

			WeatherPeriod modifiedPeriod(startTime, endTime);

			for(unsigned int i = 0; i < theParameters.theWindDataVector.size() ; i++)
			  {
				WindDataItemUnit& item = theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());


				if(item.thePeriod.localStartTime() == startTime)
				  {
					begItem = &item;
				  }
				if(item.thePeriod.localEndTime() == endTime)
				  {
					endItem = &item;
				  }
			  }

			WindEventPeriodDataItem* modifiedItem = new WindEventPeriodDataItem(modifiedPeriod,
																				(*theDataItem)->theWindEvent,
																				*begItem,
																				*endItem);
			delete (*theDataItem);

			(*theDataItem) = modifiedItem;
		  }
	  }

	return retval;
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
					break;
				  }
			  }
		  }
	  }

	remove_reduntant_periods(storyParams);

#ifndef NDEBUG
	storyParams.theLog << "* STEP 1 *" << endl;
	log_merged_wind_event_periods(storyParams);	
#endif

	// 2) separate the weak wind periods (turn them to MISSING_WIND_EVENT periods)
	for(unsigned int i = 0; i < storyParams.theMergedWindEventPeriodVector.size(); i++)
	  {
		currentDataItem = storyParams.theMergedWindEventPeriodVector[i];

		if(currentDataItem->theWindEvent & TUULI_HEIKKENEE ||
		   currentDataItem->theWindEvent & TUULI_VOIMISTUU)
		  {
			WindEventPeriodDataItem* weakItem = find_weak_period(&currentDataItem, storyParams);
			if(weakItem)
			  {
				// update the new pointer
				storyParams.theMergedWindEventPeriodVector[i] = currentDataItem;

				vector<WindEventPeriodDataItem*>::iterator it = find(storyParams.theMergedWindEventPeriodVector.begin(),
																	 storyParams.theMergedWindEventPeriodVector.end(),
																	 currentDataItem);
				if(weakItem->thePeriod.localStartTime() > currentDataItem->thePeriod.localStartTime())
				  {
					// insert after
					if(it == storyParams.theMergedWindEventPeriodVector.end())
					  {
						storyParams.theMergedWindEventPeriodVector.push_back(weakItem);
					  }
					else
					  {
						it++;
						storyParams.theMergedWindEventPeriodVector.insert(it, weakItem);
					  }
				  }
				else
				  {
					storyParams.theMergedWindEventPeriodVector.insert(it, weakItem);
				  }
				i++;
			  }
		  }
		else if(is_weak_period(storyParams, currentDataItem->thePeriod))
		  {
			currentDataItem->theWeakWindPeriodFlag = true;
			currentDataItem->theWindEvent = MISSING_WIND_EVENT;
		  }
	  }

	remove_reduntant_periods(storyParams);

#ifndef NDEBUG
	storyParams.theLog << "* STEP 2 *" << endl;
	log_merged_wind_event_periods(storyParams);
#endif
 	// 3) a) if wind direction doesn't change (e.g. itätuuli -> idän puoleinen tuuli), remove the KAANTYY-event
	// b) merge successive periods when tuuli kaantyy and direction remains the same 
	unsigned int previousMergedIndex(UINT_MAX);
	nextDataItem = 0;
	for(unsigned int i = 1; i < storyParams.theMergedWindEventPeriodVector.size(); i++)
	  {
		unsigned int previousPeriodIndex(previousMergedIndex == UINT_MAX ? i - 1 : previousMergedIndex);
		currentDataItem = storyParams.theMergedWindEventPeriodVector[i];
		previousDataItem = storyParams.theMergedWindEventPeriodVector[previousPeriodIndex];
		if(i < storyParams.theMergedWindEventPeriodVector.size() - 1)
		  nextDataItem = storyParams.theMergedWindEventPeriodVector[i+1];
		else
		  nextDataItem = 0;

		WindDirectionId currentWindDirectionId(get_wind_direction_id_at(storyParams.theWindDataVector,
																		storyParams.theArea,
																		storyParams.theSources,
																		currentDataItem->thePeriod,
																		storyParams.theVar));
		WindDirectionId previousWindDirectionId(get_wind_direction_id_at(storyParams.theWindDataVector,
																		 storyParams.theArea,
																		 storyParams.theSources,
																		 previousDataItem->thePeriod,
																		 storyParams.theVar));

		if(previousWindDirectionId == VAIHTELEVA &&
		   currentWindDirectionId != VAIHTELEVA)
		  continue;

		bool merge = false;
		if(currentDataItem->theWindEvent & TUULI_KAANTYY)
		  {
			if(previousDataItem->theWindEvent & TUULI_KAANTYY &&
			   same_direction(currentWindDirectionId,
							  previousWindDirectionId,
							  true))
			  {
				merge = true;
			  }
			else if(same_direction(currentDataItem->thePeriodBeginDataItem.theEqualizedWindDirection,
								   currentDataItem->thePeriodEndDataItem.theEqualizedWindDirection,
								   currentDataItem->thePeriodBeginDataItem.theEqualizedMaximumWind,
								   currentDataItem->thePeriodEndDataItem.theEqualizedMaximumWind,
								   storyParams.theVar,
								   true) ||
					previousWindDirectionId == wind_direction_id(currentDataItem->thePeriodEndDataItem.theEqualizedWindDirection, 
																 currentDataItem->thePeriodEndDataItem.theEqualizedMaximumWind, 
																 storyParams.theVar))
			  {
				if(!nextDataItem || !(nextDataItem->theWindEvent & TUULI_KAANTYY))
				  currentDataItem->theWindEvent = mask_wind_event(currentDataItem->theWindEvent, TUULI_KAANTYY);
			  }
		  }

		previousMergedIndex = UINT_MAX;
		if(merge)
		  {
			// merge
			if(re_create_merged_item(&previousDataItem,
									 currentDataItem,
									 WeatherPeriod(previousDataItem->thePeriod.localStartTime(),
												   currentDataItem->thePeriod.localEndTime()),
									 storyParams))
			  {
				storyParams.theMergedWindEventPeriodVector[previousPeriodIndex] = previousDataItem;		    	
				previousMergedIndex = previousPeriodIndex;
			  }
		  }
	  }
	
	remove_reduntant_periods(storyParams);
	
#ifndef NDEBUG
	storyParams.theLog << "* STEP 3 *" << endl;
	log_merged_wind_event_periods(storyParams);
#endif

	// 4) Remove short MISSING_WIND_EVENT period from the beginning
	if(storyParams.theMergedWindEventPeriodVector.size() > 1)
	  {
		previousDataItem = storyParams.theMergedWindEventPeriodVector[0];
		currentDataItem = storyParams.theMergedWindEventPeriodVector[1];

		if(currentDataItem->theWindEvent != MISSING_WIND_EVENT &&
		   previousDataItem->theWindEvent == MISSING_WIND_EVENT &&
		   (get_period_length(previousDataItem->thePeriod) <= 2))
		  {
			if(re_create_merged_item(&currentDataItem,
									 previousDataItem,
									 WeatherPeriod(previousDataItem->thePeriod.localStartTime(),
												currentDataItem->thePeriod.localEndTime()),
									 storyParams))
			  storyParams.theMergedWindEventPeriodVector[1] = currentDataItem;
		  }
		remove_reduntant_periods(storyParams);
		
#ifndef NDEBUG
		storyParams.theLog << "* STEP 4 *" << endl;
		log_merged_wind_event_periods(storyParams);
#endif
	  }

	// 5) merge simple period events with composite period events
	merge_simple_and_composite_period_events(storyParams);
	remove_reduntant_periods(storyParams);

#ifndef NDEBUG
	storyParams.theLog << "* STEP 5 *" << endl;
	log_merged_wind_event_periods(storyParams);
#endif

	// 6) Several cases...
	previousMergedIndex = UINT_MAX;
	for(unsigned int i = 1; i < storyParams.theMergedWindEventPeriodVector.size(); i++)
	  {
		unsigned int previousPeriodIndex(previousMergedIndex == UINT_MAX ? i - 1 : previousMergedIndex);
		currentDataItem = storyParams.theMergedWindEventPeriodVector[i];
		previousDataItem = storyParams.theMergedWindEventPeriodVector[previousPeriodIndex];

		if(i < storyParams.theMergedWindEventPeriodVector.size() - 1)
		  nextDataItem = storyParams.theMergedWindEventPeriodVector[i+1];
		else
		  nextDataItem = 0;

		WindDirectionId windDirectionIdCurrent(get_wind_direction_id_at(storyParams.theWindDataVector,
																		storyParams.theArea,
																		storyParams.theSources,
																		currentDataItem->thePeriod,
																		storyParams.theVar));
		WindDirectionId windDirectionIdPrevious(get_wind_direction_id_at(storyParams.theWindDataVector,
																		 storyParams.theArea,
																		 storyParams.theSources,
																		 previousDataItem->thePeriod,
																		 storyParams.theVar));
		WindDirectionId windDirectionIdNext(MISSING_WIND_DIRECTION_ID);
		if(nextDataItem)
		  windDirectionIdNext = get_wind_direction_id_at(storyParams.theWindDataVector,
														 storyParams.theArea,
														 storyParams.theSources,
														 nextDataItem->thePeriod,
														 storyParams.theVar);
	
		bool mergePeriods(false);

		if((currentDataItem->theWindEvent == MISSING_WIND_EVENT &&
			previousDataItem->theWindEvent == MISSING_WIND_EVENT) ||
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
			// Merge similar successive periods
			storyParams.theLog << "* Merge case #1 *" << endl;
			mergePeriods = true;
		  }
		else if(nextDataItem &&
				(((previousDataItem->theWindEvent == TUULI_HEIKKENEE &&
				   currentDataItem->theWindEvent == TUULI_KAANTYY_JA_HEIKKENEE &&				 
				   nextDataItem->theWindEvent == TUULI_KAANTYY_JA_VOIMISTUU) ||
				  (previousDataItem->theWindEvent == TUULI_VOIMISTUU &&
				   currentDataItem->theWindEvent == TUULI_KAANTYY_JA_VOIMISTUU &&				 
				   nextDataItem->theWindEvent == TUULI_KAANTYY_JA_HEIKKENEE)) &&
				 get_period_length(currentDataItem->thePeriod) <= 2))
		  {
			// previous period: weakening
			// current period (is short): weakening continues and direction starts to change
			// next period: direction change continues wind starts to strenghten
			storyParams.theLog << "* Merge case #2 *" << endl;
			mergePeriods = true;
		  }
		else if(currentDataItem->theWindEvent == MISSING_WIND_EVENT &&
				//				!currentDataItem->theWeakWindPeriodFlag &&
				get_period_length(currentDataItem->thePeriod) <= 2)
		  {
			// if period is short and event is MISSING_WIND_EVENT, merge it to the previous
			storyParams.theLog << "* Merge case #3 *" << endl;
			mergePeriods = true;
		  }
		else if((windDirectionIdPrevious == VAIHTELEVA &&
				 currentDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI))/* ||
				(currentDataItem->theWindEvent == MISSING_WIND_EVENT &&
				 previousDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI &&
				 !currentDataItem->theWeakWindPeriodFlag))*/
		  {
			// If wind direction in the previous period is VAIHTELEVA, 
			// the event on this period can not be TUULI_MUUTTUU_VAIHTELEVAKSI
			// Or if previous event is TUULI_MUUTTUU_VAIHTELEVAKSI and the
			// current event is MISSING_WIND_EVENT an it is not weaK merge them
			if(windDirectionIdPrevious == VAIHTELEVA)
			  {
				//previousDataItem->theWindEvent = MISSING_WIND_EVENT;
				currentDataItem->theWindEvent = MISSING_WIND_EVENT;
			  }
			storyParams.theLog << "* Merge case #4 *" << endl;
			mergePeriods = true;
		  }
		else if(windDirectionIdPrevious == VAIHTELEVA && 
				(nextDataItem &&  windDirectionIdNext == VAIHTELEVA) &&
				get_period_length(currentDataItem->thePeriod) <= 2)
		  {
			// if short directed wind period is between two vaihteleva periods, merge it to previous 
			storyParams.theLog << "* Merge case #5 *" << endl;
			currentDataItem->theWindEvent = MISSING_WIND_EVENT;
			mergePeriods = true;
		  }
		else if(!nextDataItem && 
				(windDirectionIdPrevious == VAIHTELEVA &&
				 get_period_length(currentDataItem->thePeriod) <= 2))
		  {
			// if the last period is short and the previous period is vaihteleva, merge them
			storyParams.theLog << "* Merge case #6 *" << endl;
			currentDataItem->theWindEvent = MISSING_WIND_EVENT;
			mergePeriods = true;
		  }
		else if(previousDataItem->theWindEvent == TUULI_KAANTYY && 
				currentDataItem->theWindEvent == MISSING_WIND_EVENT)
		  {
			storyParams.theLog << "* Merge case #7 *" << endl;
			mergePeriods = true;
		  }

		previousMergedIndex = UINT_MAX;
		if(mergePeriods)
		  {
			storyParams.theLog 
			  << "Merging periods: " 
			  <<  previousDataItem->thePeriod
			  << " and "
			  << currentDataItem->thePeriod
			  << endl;

			if(re_create_merged_item(&previousDataItem,
									 currentDataItem,
									 WeatherPeriod(previousDataItem->thePeriod.localStartTime(),
												   currentDataItem->thePeriod.localEndTime()),
									 storyParams))
			  {
				storyParams.theMergedWindEventPeriodVector[previousPeriodIndex] = previousDataItem;				
				previousMergedIndex = previousPeriodIndex;
			  }			  
		  }
	  }

	remove_reduntant_periods(storyParams);

#ifndef NDEBUG
	storyParams.theLog << "* STEP 6 *" << endl;
	log_merged_wind_event_periods(storyParams);
#endif

    // 7) Merge simple and composite periods
	merge_simple_and_composite_period_events(storyParams);
	remove_reduntant_periods(storyParams);

#ifndef NDEBUG
	storyParams.theLog << "* STEP 7 *" << endl;
	log_merged_wind_event_periods(storyParams);
#endif

	// 8) Merge successive missing event periods. Merge if previous period directions is VAIHTELEVA
	// and on current period event is TUULI_MUUTTUU_VAIHTELEVAKSI.
	previousMergedIndex = UINT_MAX;
	for(unsigned int i = 1; i < storyParams.theMergedWindEventPeriodVector.size(); i++)
	  {
		unsigned int previousPeriodIndex(previousMergedIndex == UINT_MAX ? i - 1 : previousMergedIndex);
		currentDataItem = storyParams.theMergedWindEventPeriodVector[i];
		previousDataItem = storyParams.theMergedWindEventPeriodVector[previousPeriodIndex];
		WindDirectionId windDirectionIdPrevious(get_wind_direction_id_at(storyParams.theWindDataVector,
																		 storyParams.theArea,
																		 storyParams.theSources,
																		 previousDataItem->thePeriod,
																		 storyParams.theVar));

		bool merge(false);
		bool mergePreviousToCurrent(false);
		if(currentDataItem->theWindEvent == MISSING_WIND_EVENT &&
		   previousDataItem->theWindEvent == MISSING_WIND_EVENT)
		  {
			merge = true;
		  }
		if(currentDataItem->theWindEvent == MISSING_WIND_EVENT &&
		   currentDataItem->theWeakWindPeriodFlag &&
		   previousDataItem->theWindEvent == TUULI_HEIKKENEE)
		  {
			merge = true;
		  }
		else if(windDirectionIdPrevious == VAIHTELEVA &&
				currentDataItem->theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI)
		  {
			currentDataItem->theWindEvent = MISSING_WIND_EVENT;
			merge = true;
		  }
		else if((previousDataItem->theWindEvent == TUULI_KAANTYY_JA_HEIKKENEE&&
				 currentDataItem->theWindEvent == TUULI_KAANTYY) &&
				(previousDataItem->theWindEvent == TUULI_KAANTYY_JA_VOIMISTUU &&
				 currentDataItem->theWindEvent == TUULI_VOIMISTUU) &&
				get_period_length(previousDataItem->thePeriod) <= 1)
		  {
			merge = true;
			mergePreviousToCurrent = true;
		  }

		previousMergedIndex = UINT_MAX;
		if(merge)
		  {			
			if(mergePreviousToCurrent)
			  {
				WindEventId originalCurrentWindEventId(currentDataItem->theWindEvent);
				if(re_create_merged_item(&currentDataItem,
										 previousDataItem,
										 WeatherPeriod(previousDataItem->thePeriod.localStartTime(),
													   currentDataItem->thePeriod.localEndTime()),
										 storyParams))
				  {
					if(!is_valid_wind_event_id(currentDataItem->theWindEvent))
					  currentDataItem->theWindEvent = originalCurrentWindEventId;
					storyParams.theMergedWindEventPeriodVector[i] = currentDataItem;					
					previousMergedIndex = i;
				  }
			  }
			else
			  {
				if(re_create_merged_item(&previousDataItem,
										 currentDataItem,
										 WeatherPeriod(previousDataItem->thePeriod.localStartTime(),
													currentDataItem->thePeriod.localEndTime()),
										 storyParams))
				  {
					storyParams.theMergedWindEventPeriodVector[previousPeriodIndex] = previousDataItem;
					previousMergedIndex = previousPeriodIndex;
				  }
			  }
		  }
	  }

	remove_reduntant_periods(storyParams);

#ifndef NDEBUG
	storyParams.theLog << "* STEP 8 *" << endl;
	log_merged_wind_event_periods(storyParams);
#endif

	// 9) if wind speed weakens/strengthens, check wheather it starts or stops on that period
	for(unsigned int i = 0; i < storyParams.theMergedWindEventPeriodVector.size(); i++)
	  {		
		currentDataItem = storyParams.theMergedWindEventPeriodVector[i];
		if(i == 0)
		  previousDataItem = 0;
		else
		  previousDataItem = storyParams.theMergedWindEventPeriodVector[i-1];
		if(i < storyParams.theMergedWindEventPeriodVector.size() - 1)
		  nextDataItem = storyParams.theMergedWindEventPeriodVector[i+1];
		else 
		  nextDataItem = 0;

		if(currentDataItem->theWindEvent & TUULI_HEIKKENEE)
		  {
			if(!previousDataItem || !(previousDataItem->theWindEvent & TUULI_HEIKKENEE))
			  currentDataItem->theWindSpeedChangeStarts = true;
			if(!nextDataItem || !(nextDataItem->theWindEvent & TUULI_HEIKKENEE))
			  currentDataItem->theWindSpeedChangeEnds = true;				
		  }
		else if(currentDataItem->theWindEvent & TUULI_VOIMISTUU)
		  {
			if(!previousDataItem || !(previousDataItem->theWindEvent & TUULI_VOIMISTUU))
			  currentDataItem->theWindSpeedChangeStarts = true;
			if(!nextDataItem || !(nextDataItem->theWindEvent & TUULI_VOIMISTUU))
			  currentDataItem->theWindSpeedChangeEnds = true;
		  }
	  }
	remove_reduntant_periods(storyParams);
	
#ifndef NDEBUG
	storyParams.theLog << "* STEP 9 *" << endl;
	log_merged_wind_event_periods(storyParams);
#endif


  }

  void find_out_wind_event_periods(wo_story_params& storyParams)
  {
	find_out_wind_speed_event_periods(storyParams);
	find_out_wind_direction_event_periods(storyParams);

#ifndef NDEBUG
	log_wind_event_periods(storyParams);
	log_equalized_wind_direction_data_vector(storyParams);
#endif

	if(storyParams.theWindSpeedEventPeriodVector.size() > 1)
	  std::sort(storyParams.theWindSpeedEventPeriodVector.begin(),
				storyParams.theWindSpeedEventPeriodVector.end(), 
				wind_event_period_sort);
	if(storyParams.theWindDirectionEventPeriodVector.size() > 1)
	  std::sort(storyParams.theWindDirectionEventPeriodVector.begin(),
				storyParams.theWindDirectionEventPeriodVector.end(), 
				wind_event_period_sort);

	create_union_periods_of_wind_speed_and_direction_event_periods(storyParams);

#ifndef NDEBUG
	log_merged_wind_event_periods(storyParams);
#endif

	merge_fragment_periods_when_feasible(storyParams);

#ifndef NDEBUG
	log_merged_wind_event_periods(storyParams);
#endif

	find_out_transient_wind_direction_periods(storyParams);

#ifndef NDEBUG
	log_merged_wind_event_periods(storyParams);
#endif

	find_out_long_term_wind_speed_periods(storyParams);

#ifndef NDEBUG
	log_merged_wind_event_periods(storyParams);
#endif
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

		for(unsigned int k = previousIndex+1; k < currentIndex; k++)
		  {
			const WindDataItemUnit& itemK = ((*storyParams.theWindDataVector[k])(areaType));
			float currentValue = (medianTimeSeries ? itemK.theWindSpeedMedian.value() : itemK.theWindMaximum.value());

			if(currentValue > localMax)
			  {
				localMax = currentValue;
				localMaxIndex = k;
			  }
			else if(currentValue < localMin)
			  {
				localMin = currentValue;
				localMinIndex = k;
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
	for(unsigned int j = 0; j < storyParams.theWeatherAreas.size(); j++)
	  {
		const WeatherArea& weatherArea = storyParams.theWeatherAreas[j];

		unsigned int index1, index2, index3;
		WeatherArea::Type areaType(weatherArea.type());//storyParams.theArea.type());

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
				item.theEqualizedMedianWind = WeatherResult(yValue, 0.0);
			  }		
		  }
	  }
  }

  void calculate_equalized_wind_speed_indexes_for_maximum_wind(wo_story_params& storyParams)
  {
	for(unsigned int j = 0; j < storyParams.theWeatherAreas.size(); j++)
	  {
		const WeatherArea& weatherArea = storyParams.theWeatherAreas[j];

		unsigned int index1, index2, index3;
		WeatherArea::Type areaType(weatherArea.type());//storyParams.theArea.type());

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
				item.theEqualizedMaximumWind = WeatherResult(yValue, 0.0);
			  }		
		  }
	  }
  }

  void calculate_equalized_wind_direction_indexes(wo_story_params& storyParams)
  {
	for(unsigned int j = 0; j < storyParams.theWeatherAreas.size(); j++)
	  {
		const WeatherArea& weatherArea = storyParams.theWeatherAreas[j];

		unsigned int index1, index2, index3;
		WeatherArea::Type areaType(weatherArea.type());

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

				directionIdIndex1 = wind_direction_id(dataItemIndex1.theEqualizedWindDirection,
													  dataItemIndex1.theEqualizedMaximumWind,
													  storyParams.theVar);
				directionIdIndex2 = wind_direction_id(dataItemIndex2.theEqualizedWindDirection,
													  dataItemIndex3.theEqualizedMaximumWind,
													  storyParams.theVar);
				directionIdIndex3 = wind_direction_id(dataItemIndex3.theEqualizedWindDirection,
													  dataItemIndex3.theEqualizedMaximumWind,
													  storyParams.theVar);
				// dont remove variable wind
				if(directionIdIndex1 == VAIHTELEVA || 
				   directionIdIndex3 == VAIHTELEVA || 
				   directionIdIndex2 == VAIHTELEVA)
				  continue;

				double lineBegX = index1;
				double lineBegY = dataItemIndex1.theEqualizedWindDirection.value();
				double lineEndX = index3;
				double lineEndY = dataItemIndex3.theEqualizedWindDirection.value();
				double coordX = index2;
				double coordY = dataItemIndex2.theEqualizedWindDirection.value();

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

			if(minError > storyParams.theMaxErrorWindDirection)
			  {
				break;
			  }	

			/*
			WeatherResult windDirectionMinus1(((*storyParams.theWindDataVector[minErrorIndex-1])(areaType)).theEqualizedWindDirection);
			WeatherResult windDirection(((*storyParams.theWindDataVector[minErrorIndex])(areaType)).theEqualizedWindDirection);
			WeatherResult windDirectionPlus1(((*storyParams.theWindDataVector[minErrorIndex+1])(areaType)).theEqualizedWindDirection);

			WeatherResult maxWindSpeedMinus1(((*storyParams.theWindDataVector[minErrorIndex-1])(areaType)).theEqualizedMaximumWind);
			WeatherResult maxWindSpeed(((*storyParams.theWindDataVector[minErrorIndex])(areaType)).theEqualizedMaximumWind);
			WeatherResult maxWindSpeedPlus1(((*storyParams.theWindDataVector[minErrorIndex+1])(areaType)).theEqualizedMaximumWind);

			directionIdIndex1 = wind_direction_id(windDirectionMinus1, maxWindSpeedMinus1, storyParams.theVar);
			directionIdIndex2 = wind_direction_id(windDirection, maxWindSpeed, storyParams.theVar);
			directionIdIndex3 = wind_direction_id(windDirectionPlus1, maxWindSpeedPlus1, storyParams.theVar);

			const WindDataItemUnit& dataItemIndexTmp = (*storyParams.theWindDataVector[minErrorIndex])(areaType);
			cout << "erasing index at: " 
				 << dataItemIndexTmp.thePeriod.localStartTime() 
				 << "directions: " 
				 << wind_direction_string(directionIdIndex1) 
				 << ","
				 << wind_direction_string(directionIdIndex2) 
				 << ","
				 << wind_direction_string(directionIdIndex3) 
				 << endl;
			*/

			eqIndexVector.erase(eqIndexVector.begin()+minErrorIndex);
		  }

		// re-calculate equalized values for the removed points
		for(unsigned int i = 1; i < eqIndexVector.size(); i++)
		  {
			unsigned int currentIndex = eqIndexVector[i];
			unsigned int previousIndex = eqIndexVector[i-1];
			const WindDataItemUnit& previousItem = (*storyParams.theWindDataVector[previousIndex])(areaType);
			const WindDataItemUnit& currentItem = (*storyParams.theWindDataVector[currentIndex])(areaType);

			if(currentIndex == previousIndex + 1)
			  continue;
		
			float oppositeLen = currentItem.theEqualizedWindDirection.value() - previousItem.theEqualizedWindDirection.value();
			float adjacentLen = currentIndex - previousIndex;
			float slope = oppositeLen / adjacentLen;

			for(unsigned int k = previousIndex+1; k < currentIndex; k++)
			  {
				float yValue = (slope * (k-previousIndex)) + previousItem.theEqualizedWindDirection.value();

				WindDataItemUnit& item = (storyParams.theWindDataVector[k])->getDataItem(areaType);
				item.theEqualizedWindDirection = WeatherResult(yValue, item.theWindDirection.error());
			  }
		  }

		unsigned int startIndex(eqIndexVector[0]);
		unsigned int endIndex(eqIndexVector[eqIndexVector.size()-1]);

	
		for(unsigned int i = startIndex+1; i < endIndex-1; i++)
		  {
			WindDataItemUnit& item1 = (storyParams.theWindDataVector[i-1])->getDataItem(areaType);
			WindDataItemUnit& item2 = (storyParams.theWindDataVector[i])->getDataItem(areaType);
			WindDataItemUnit& item3 = (storyParams.theWindDataVector[i+1])->getDataItem(areaType);

			WindDirectionId directionId1(wind_direction_id(item1.theEqualizedWindDirection,
														   item1.theEqualizedMaximumWind,
														   storyParams.theVar));
			WindDirectionId directionId2(wind_direction_id(item2.theEqualizedWindDirection,
														   item2.theEqualizedMaximumWind,
														   storyParams.theVar));
			WindDirectionId directionId3(wind_direction_id(item3.theEqualizedWindDirection,
														   item3.theEqualizedMaximumWind,
														   storyParams.theVar));
			if(directionId1 != VAIHTELEVA &&
			   directionId2 == VAIHTELEVA &&
			   directionId3 != VAIHTELEVA)
			  {
				item2.theEqualizedWindDirection = WeatherResult(item2.theEqualizedWindDirection.value(), 40.0);
			  }
		  }
	  }
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
	double windDirectionThreshold = Settings::optional_double(storyParams.theVar+"::wind_direction_threshold", 45.0);
	double gustyWindMaximumWindDifference = Settings::optional_double(storyParams.theVar+"::gusty_wind_max_wind_difference", 5.0);
	string rangeSeparator = Settings::optional_string(storyParams.theVar+"::rangeseparator","-");
	unsigned int minIntervalSize = optional_int(storyParams.theVar+"::wind_speed_interval_min_size",2);
	unsigned int maxIntervalSize = optional_int(storyParams.theVar+"::wind_speed_interval_max_size",5);
	
	string metersPerSecondFormat = Settings::optional_string("textgen::units::meterspersecond::format", "SI");

	storyParams.theMaxErrorWindSpeed = maxErrorWindSpeed;
	storyParams.theMaxErrorWindDirection = maxErrorWindDirection;
	storyParams.theWindSpeedThreshold = windSpeedThreshold;
	storyParams.theWindDirectionThreshold = windDirectionThreshold;
	storyParams.theGustyWindMaximumWindDifference = gustyWindMaximumWindDifference;
	storyParams.theRangeSeparator = rangeSeparator;
	storyParams.theMinIntervalSize = minIntervalSize;
	storyParams.theMaxIntervalSize = maxIntervalSize;
	storyParams.theMetersPerSecondFormat = metersPerSecondFormat;
	
	storyParams.theWeatherAreas.push_back(storyParams.theArea);

	WeatherArea coastalArea = storyParams.theArea;
	coastalArea.type(WeatherArea::Coast);
	storyParams.theWeatherAreas.push_back(coastalArea);
	WeatherArea inlandArea = storyParams.theArea;
	inlandArea.type(WeatherArea::Inland);
	storyParams.theWeatherAreas.push_back(inlandArea);

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


  Paragraph WindStory::overview() const
  {
	MessageLogger logger("WeatherStory::overview");

	std::string areaName("");
	if(itsArea.isNamed())
	  {
		areaName = itsArea.name();
		logger << "** " << areaName << " **" << endl;
	  }
	// cout << "** " << areaName << " **" << endl;

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

	// find out wind event periods:
	// event periods are used to produce the story
	find_out_wind_event_periods(storyParams);
	
#ifndef NDEBUG
	log_raw_data(storyParams);

	// find out the wind speed periods (for logging purposes)
	find_out_wind_speed_periods(storyParams);
	// find out the wind direction periods of 16-direction compass (for logging purposes)
	find_out_wind_direction_periods(storyParams);

	// log functions
	save_raw_data(storyParams);
	log_raw_data(storyParams);
	log_equalized_wind_speed_data_vector(storyParams);
	log_equalized_wind_direction_data_vector(storyParams);
	log_wind_speed_periods(storyParams);
	log_wind_direction_periods(storyParams);
	//	log_wind_events(storyParams);
	log_wind_event_periods(storyParams);
	//	log_merged_wind_event_periods(storyParams);
#endif

	WindForecast windForecast(storyParams);

	paragraph << windForecast.getWindStory(itsPeriod);
 
	deallocate_data_structures(storyParams);

	logger << paragraph;

	return paragraph;
  }
} // namespace TextGen
  
// ======================================================================

