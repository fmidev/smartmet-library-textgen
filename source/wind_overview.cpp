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
		retval = "itä";
		break;
	  case IDAN_PUOLEINEN:
		retval = "idän puoleinen";
		break;
	  case KAAKKO:
		retval = "kaakko";
		break;
	  case KAAKON_PUOLEINEN:
		retval = "kaakon puoleinen";
		break;
	  case ETELA:
		retval = "etelä";
		break;
	  case ETELAN_PUOLEINEN:
		retval = "etelän puoleinen";
		break;
	  case LOUNAS:
		retval = "lounas";
		break;
	  case LOUNAAN_PUOLEINEN:
		retval = "lounaan puoleinen";
		break;
	  case LANSI:
		retval = "länsi";
		break;
	  case LANNEN_PUOLEINEN:
		retval = "lännen puoleinen";
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
		retval = "pohjoisen ja koillisen välinen";
		break;
	  case KOILLINEN_:
		retval = "koillinen";
		break;
	  case KOILLINEN_ITA:
		retval = "koillisen ja idän välinen";
		break;
	  case ITA_:
		retval = "itä";
		break;
	  case ITA_KAAKKO:
		retval = "idän ja akaakon välinen";
		break;
	  case KAAKKO_:
		retval = "kaakko";
		break;
	  case KAAKKO_ETELA:
		retval = "kaakon ja etelän välinen";
		break;
	  case ETELA_:
		retval = "etelä";
		break;
	  case ETELA_LOUNAS:
		retval = "etelän ja lounaan välinen";
		break;
	  case LOUNAS_:
		retval = "lounas";
		break;
	  case LOUNAS_LANSI:
		retval = "lounaan ja lännen välinen";
		break;
	  case LANSI_:
		retval = "länsi";
		break;
	  case LANSI_LUODE:
		retval = "lännen ja luoteen välinen";
		break;
	  case LUODE_:
		retval = "luode";
		break;
	  case LUODE_POHJOINEN:
		retval = "luoteen ja pohjoisen välinen";
		break;
	  case VAIHTELEVA_:
		retval = "vaihteleva";
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
						const wind_raw_data_vector& theRawDataVector)
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

	for(unsigned int i = 0; i < theRawDataVector.size(); i++)
	  {
		const WindDataItem& theWindDataItem = *(theRawDataVector[i]);
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
		
		output_file << directed_speed_string(theRawDataVector[i]->theWindSpeedMean, 
											 theRawDataVector[i]->theWindDirection, 
											 theVar)
					<< " |" << endl;
	  }
  }

  /*
  void print_csv_table(const weather_result_list& theWeatherResults,
					   const list<unsigned int>& theIndexeList)
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

	list<unsigned int>::iterator it;

	for(list<unsigned int>::iterator it = theIndexList.begin(); it != theIndexList.end(); it++)
	  {
	  }

  }
  */

  void print_csv_table(const std::string& areaName,
					   const std::string& fileIdentifierString,
					   const std::string& theVar,
					   const wind_raw_data_vector& theRawDataVector,
					   const list<unsigned int>& theIndexList)
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

	list<unsigned int>::const_iterator it;

	for(it = theIndexList.begin(); it != theIndexList.end(); it++)
	  {
		const WindDataItem& theWindDataItem = *(theRawDataVector[*it]);
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
		  /*
					<< fixed << setprecision(1) << theWindDataItem.theMovingMean
					<< ", "
					<< fixed << setprecision(1) << theWindDataItem.theMovingMeanError
					<< ", "
		  */
					<< fixed << setprecision(1) << theWindDataItem.theWindDirection.value()
					<< ", "
					<< fixed << setprecision(1) << theWindDataItem.theWindDirection.error()
					<< ", "
					<< fixed << setprecision(1) << theWindDataItem.theWindDirectionStd.value();

		
		output_file << directed_speed_string(theRawDataVector[*it]->theWindSpeedMean, 
											 theRawDataVector[*it]->theWindDirection, 
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

	list<unsigned int>::iterator it;

	for(it = storyParams.theEqualizedWindSpeedIndexes.begin();
		it != storyParams.theEqualizedWindSpeedIndexes.end(); 
		it++)
	  {
		storyParams.theLog << *(storyParams.theRawDataVector[*it]) << endl;
	  }
  }

  void log_equalized_wind_direction_data_vector(wo_story_params& storyParams)
  {
	storyParams.theLog << "*********** EQUALIZED WIND DIRECTION DATA ***********" << endl;

	list<unsigned int>::iterator it;

	for(it = storyParams.theEqualizedWindDirectionIndexes.begin();
		it != storyParams.theEqualizedWindDirectionIndexes.end(); 
		it++)
	  {
		storyParams.theLog << *(storyParams.theRawDataVector[*it]) << endl;
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
																windDirection,
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

#ifdef LATER
		
		if(i > 0)//for(unsigned int k = 1; i < storyParams.theRawDataVector.size(); i++)
		  {
			const double diff = storyParams.theRawDataVector[i]->theWindDirection.value() - storyParams.theRawDataVector[i-1]->theWindDirection.value();
			double dir = storyParams.theRawDataVector[i-1]->theWindDirection.value() + diff;
			if(diff < 180.0)
			  {
				while(dir < 180.0)
				  dir += 180.0;
			  }
			else if(diff > 180)
			  {
				while(dir > 180.0)
				  dir -= 180.0;
			  }
			storyParams.theRawDataVector[i]->theWindDirectionStd = WeatherResult(dir, storyParams.theRawDataVector[i]->theWindDirection.error());
			
		  }
		else
		  storyParams.theRawDataVector[i]->theWindDirectionStd = storyParams.theRawDataVector[i]->theWindDirection;
#endif
		/*
		if(storyParams.theRawDataVector[i]->theWindDirection.value() > 180)
		  storyParams.theRawDataVector[i]->theWindDirectionStd = WeatherResult(storyParams.theRawDataVector[i]->theWindDirection.value() - 360, storyParams.theRawDataVector[i]->theWindDirection.error());
		*/

		storyParams.theOriginalWindSpeedIndexes.push_back(i);
		storyParams.theEqualizedWindSpeedIndexes.push_back(i);
		storyParams.theOriginalWindDirectionIndexes.push_back(i);
		storyParams.theEqualizedWindDirectionIndexes.push_back(i);
	  }

	/*
	// calculate the moving mean
	for(unsigned int i = 0; i < storyParams.theRawDataVector.size(); i++)
	  {
		if( i >= 3 && i < storyParams.theRawDataVector.size() - 4)
		  {
			storyParams.theRawDataVector[i]->theMovingMean = 
			  (storyParams.theRawDataVector[i-3]->theWindSpeedMean.value() + 
			   storyParams.theRawDataVector[i-2]->theWindSpeedMean.value() + 
			   storyParams.theRawDataVector[i-1]->theWindSpeedMean.value() +
			   storyParams.theRawDataVector[i]->theWindSpeedMean.value() +
			   storyParams.theRawDataVector[i+1]->theWindSpeedMean.value() +
			   storyParams.theRawDataVector[i+2]->theWindSpeedMean.value() +
			   storyParams.theRawDataVector[i+3]->theWindSpeedMean.value()) / 7.0;
			storyParams.theRawDataVector[i]->theMovingMeanError = 
			  (storyParams.theRawDataVector[i-3]->theWindSpeedMean.error() + 
			   storyParams.theRawDataVector[i-2]->theWindSpeedMean.error() + 
			   storyParams.theRawDataVector[i-1]->theWindSpeedMean.error() +
			   storyParams.theRawDataVector[i]->theWindSpeedMean.error() +
			   storyParams.theRawDataVector[i+1]->theWindSpeedMean.error() +
			   storyParams.theRawDataVector[i+2]->theWindSpeedMean.error() +
			   storyParams.theRawDataVector[i+3]->theWindSpeedMean.error()) / 7.0;
		  }
		else
		  {
			storyParams.theRawDataVector[i]->theMovingMean =  storyParams.theRawDataVector[i]->theWindSpeedMean.value();
		  }
	  }
	*/
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

	if(windDirection.value() >= 337.5 || windDirection.value() < 22.5)
	  return (accuracy == good_accuracy ? POHJOINEN : POHJOISEN_PUOLEINEN);
	else if(windDirection.value() >= 22.5 || windDirection.value() < 67.5)
	  return (accuracy == good_accuracy ? KOILLINEN : KOILLISEN_PUOLEINEN);
	else if(windDirection.value() >= 67.5 || windDirection.value() < 112.5)
	  return (accuracy == good_accuracy ? ITA : IDAN_PUOLEINEN);
	else if(windDirection.value() >= 112.5 || windDirection.value() < 157.5)
	  return (accuracy == good_accuracy ? KAAKKO : KAAKON_PUOLEINEN);
	else if(windDirection.value() >= 157.5 || windDirection.value() < 202.5)
	  return (accuracy == good_accuracy ? ETELA : ETELAN_PUOLEINEN);
	else if(windDirection.value() >= 202.5 || windDirection.value() < 247.5)
	  return (accuracy == good_accuracy ? LOUNAS : LOUNAAN_PUOLEINEN);
	else if(windDirection.value() >= 247.5 || windDirection.value() < 292.5)
	  return (accuracy == good_accuracy ? LANSI : LANNEN_PUOLEINEN);
	else
	  return (accuracy == good_accuracy ? LUODE : LUOTEEN_PUOLEINEN);
  }


  wind_direction_large_id get_wind_direction_large_id(const WeatherResult& windDirection, const string& var)
  {
	WindDirectionAccuracy accuracy(direction_accuracy(windDirection.error(), var));

	if(accuracy == bad_accuracy)
	  return VAIHTELEVA_;

	if(windDirection.value() >= 348.75 || windDirection.value() < 11.25)
	  return POHJOINEN_;
	else if(windDirection.value() >= 11.25 || windDirection.value() < 33.75)
	  return POHJOINEN_KOILLINEN;
	else if(windDirection.value() >= 33.75 || windDirection.value() < 56.25)
	  return KOILLINEN_;
	else if(windDirection.value() >= 56.25 || windDirection.value() < 78.75)
	  return KOILLINEN_ITA;
	else if(windDirection.value() >= 78.75 || windDirection.value() < 101.25)
	  return ITA_;
	else if(windDirection.value() >= 101.25 || windDirection.value() < 123.75)
	  return ITA_KAAKKO;
	else if(windDirection.value() >= 123.75 || windDirection.value() < 146.25)
	  return KAAKKO_;
	else if(windDirection.value() >= 146.25 || windDirection.value() < 168.75)
	  return KAAKKO_ETELA;
	else if(windDirection.value() >= 168.75 || windDirection.value() < 191.25)
	  return ETELA_;
	else if(windDirection.value() >= 191.25 || windDirection.value() < 213.75)
	  return ETELA_LOUNAS;
	else if(windDirection.value() >= 213.75 || windDirection.value() < 236.25)
	  return LOUNAS_;
	else if(windDirection.value() >= 236.25 || windDirection.value() < 258.75)
	  return LOUNAS_LANSI;
	else if(windDirection.value() >= 258.75 || windDirection.value() < 281.25)
	  return LANSI_;
	else if(windDirection.value() >= 281.25 || windDirection.value() < 303.75)
	  return LANSI_LUODE;
	else if(windDirection.value() >= 303.75 || windDirection.value() < 326.25)
	  return LUODE_;
	else if(windDirection.value() >= 326.25 || windDirection.value() < 348.75)
	  return LUODE_POHJOINEN;
	else
	  return VAIHTELEVA_;
  }

  void find_out_wind_speed_periods(wo_story_params& storyParams)
  {
	if(storyParams.theRawDataVector.size() == 0)
	  return;
	else if(storyParams.theRawDataVector.size() == 1)
	  {
		storyParams.theWindSpeedVector.push_back(new WindSpeedPeriodDataItem(storyParams.theRawDataVector[0]->thePeriod,
																				   get_wind_speed_id(storyParams.theRawDataVector[0]->theWindSpeedMean)));
		return;
	  }

	unsigned int periodStartIndex = 0;
	wind_speed_id previous_wind_speed_id(get_wind_speed_id(storyParams.theRawDataVector[periodStartIndex]->theWindSpeedMean));	  
	for(unsigned int i = 1; i < storyParams.theRawDataVector.size(); i++)
	  {
		wind_speed_id current_wind_speed_id(get_wind_speed_id(storyParams.theRawDataVector[i]->theWindSpeedMean));
		
		if(current_wind_speed_id != previous_wind_speed_id)
		  {
			WeatherPeriod windSpeedPeriod(storyParams.theRawDataVector[periodStartIndex]->thePeriod.localStartTime(),
											 storyParams.theRawDataVector[i-1]->thePeriod.localEndTime());
			storyParams.theWindSpeedVector.push_back(new WindSpeedPeriodDataItem(windSpeedPeriod, previous_wind_speed_id));
			periodStartIndex = i;
			previous_wind_speed_id = current_wind_speed_id;
		  }
	  }
	WeatherPeriod windSpeedPeriod(storyParams.theRawDataVector[periodStartIndex]->thePeriod.localStartTime(),
									 storyParams.theRawDataVector[storyParams.theRawDataVector.size() - 1]->thePeriod.localEndTime());

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


  void find_out_wind_events(wo_story_params& storyParams)
  {
	if(storyParams.theRawDataVector.size() == 0)
	  return;


	for(unsigned int i = 1; i < storyParams.theWindSpeedVector.size(); i++)
	  {
		wind_event_id windEventId(MISSING_WIND_EVENT);
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
  }

  double distance_from_line(const NFmiPoint& point,  const NFmiPoint& lineBeg, const NFmiPoint& lineEnd)
  {
	double slope = (lineEnd.Y() - lineBeg.Y()) / (lineEnd.X() - lineBeg.X());
	double angle = abs(atan(slope));
	double distance = (lineEnd.X() - lineBeg.X()) * sin(angle);
	return distance;
  }

  void calculate_equalized_wind_speed_indexes(wo_story_params& storyParams)
  {
	//unsigned int step = 1;

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

		/*
		minErrorIndex--;

		cout << 
		  "ennen poistettavaa(index): (" << 
		  *minErrorIndex << 
		  ") "  << 
		  storyParams.theRawDataVector[*minErrorIndex]->theWindSpeedMean.value() << endl;

		minErrorIndex++;
		
		cout << 
		  "poistettava(index): (" << 
		  *minErrorIndex << 
		  ") "  << 
		  storyParams.theRawDataVector[*minErrorIndex]->theWindSpeedMean.value() << endl;
		
		minErrorIndex++;
		
		cout << 
		  "poistettavan jälkeen(index): (" << 
		  *minErrorIndex << 
		  ") "  << 
		  storyParams.theRawDataVector[*minErrorIndex]->theWindSpeedMean.value() << endl;

		minErrorIndex--;

		cout << "index(error): " << *minErrorIndex << "(" << minError << ")" << endl;

		*/

		storyParams.theEqualizedWindSpeedIndexes.erase(minErrorIndex);
		/*
		std::ostringstream stringStream;
		stringStream << "_step" << step++;

		print_csv_table(storyParams.theAreaName,
						stringStream.str(),
						storyParams.theVar,
						storyParams.theRawDataVector,
						storyParams.theEqualizedWindSpeedIndexes);
		*/

	  }
  }

  void calculate_equalized_wind_direction_indexes(wo_story_params& storyParams)
  {

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
  }

  void calculate_equalized_indexes(wo_story_params& storyParams)
  {
	if(storyParams.theEqualizedWindSpeedIndexes.size() > 3)
	  calculate_equalized_wind_speed_indexes(storyParams);
	if(storyParams.theEqualizedWindDirectionIndexes.size() > 3)
	  calculate_equalized_wind_direction_indexes(storyParams);
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
	
	find_out_wind_speed_periods(storyParams);

	find_out_wind_direction_periods(storyParams);

	find_out_wind_direction_large_periods(storyParams);

	find_out_wind_events(storyParams);

	calculate_equalized_indexes(storyParams);

	log_raw_data_vector(storyParams);

	log_equalized_wind_speed_data_vector(storyParams);

	log_equalized_wind_direction_data_vector(storyParams);

	log_wind_speed_periods(storyParams);

	log_wind_direction_periods(storyParams);

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
