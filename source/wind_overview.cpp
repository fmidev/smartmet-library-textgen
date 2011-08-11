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
#include <cmath>

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
	  pohjoisen_puoleinen,
	  koillinen,
	  koillisen_puoleinen,
	  ita,
	  idan_puoleinen,
	  kaakko,
	  kaakon_puoleinen,
	  etela,
	  etelan_puoleinen,
	  lounas,
	  lounaan_puoleinen,
	  lansi,
	  lannen_puoleinen,
	  luode,
	  luoteen_puoleinen,
	  vaihteleva
	};

  struct WindDataItem
  {
	WindDataItem(const WeatherPeriod& period, 
				 const WeatherResult& windspeedMin, 
				 const WeatherResult& windspeedMax, 
				 const WeatherResult& windspeedMean,
				 const WeatherResult& windDirection,
				 const unsigned int& timeIndex)
	  : thePeriod(period),
		theWindspeedMin(windspeedMin),
		theWindspeedMax(windspeedMax),
		theWindspeedMean(windspeedMean),
		theWindDirection(windDirection),
		theTimeIndex(timeIndex)
	{}
	
	WeatherPeriod thePeriod;
	WeatherResult theWindspeedMin;
	WeatherResult theWindspeedMax;
	WeatherResult theWindspeedMean;
	WeatherResult theWindDirection;
	unsigned int  theTimeIndex; // running number to help in calculation
  };

  struct WindStrengthPeriodDataItem
  {
	WindStrengthPeriodDataItem(const WeatherPeriod& period,
						 const wind_strength_id& windstrength)
	  : thePeriod(period),
		theWindStrength(windstrength)
	{}
	WeatherPeriod thePeriod;
	wind_strength_id theWindStrength;
  };

  struct WindDirectionPeriodDataItem
  {
	WindDirectionPeriodDataItem(const WeatherPeriod& period,
						  const wind_direction_id& winddirection)
	  : thePeriod(period),
		theWindDirection(winddirection)
	{}
	WeatherPeriod thePeriod;
	wind_direction_id theWindDirection;
  };

  typedef vector<WindDataItem*> wind_raw_data_vector;
  typedef list<WeatherResult&> weather_result_list; // refers to the item in raw data vector
  typedef vector<WindStrengthPeriodDataItem*> wind_strength_period_data_item_vector;
  typedef vector<WindDirectionPeriodDataItem*> wind_direction_period_data_item_vector;


  struct wo_story_params
  {
	wo_story_params(const string& var,
					const string& areaName,
					const WeatherArea& area,
					const WeatherPeriod& dataPeriod,
					const WeatherPeriod& forecastPeriod,
					const NFmiTime& forecastTime,
					const AnalysisSources& sources,
					MessageLogger& log) :
	  theVar(var),
	  theAreaName(areaName),
	  theArea(area),
	  theDataPeriod(dataPeriod),
	  theForecastPeriod(forecastPeriod),
	  theForecastTime(forecastTime),
	  theSources(sources),
	  theLog(log)
	  
	{}

	const string& theVar;
	const string& theAreaName;
	const WeatherArea& theArea;
	const WeatherPeriod& theDataPeriod;
	const WeatherPeriod& theForecastPeriod;
	const NFmiTime& theForecastTime;
	const AnalysisSources& theSources;
	MessageLogger& theLog;
	wind_raw_data_vector theRawDataVector;
	
	wind_strength_period_data_item_vector theWindStrengthVector;
	wind_direction_period_data_item_vector theWindDirectionVector;
	list<unsigned int> theOriginalWindStrengthIndexes;
	list<unsigned int> theEqualizedWindStrengthIndexes;
	list<unsigned int> theEqualizedWindDirectionIndexes;
  };

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
	  case pohjoisen_puoleinen:
		retval = "pohjoisen puoleinen";
		break;
	  case koillinen:
		retval = "koillinen";
		break;
	  case koillisen_puoleinen:
		retval = "koillisen puoleinen";
		break;
	  case ita:
		retval = "itä";
		break;
	  case idan_puoleinen:
		retval = "idän puoleinen";
		break;
	  case kaakko:
		retval = "kaakko";
		break;
	  case kaakon_puoleinen:
		retval = "kaakon puoleinen";
		break;
	  case etela:
		retval = "etelä";
		break;
	  case etelan_puoleinen:
		retval = "etelän puoleinen";
		break;
	  case lounas:
		retval = "lounas";
		break;
	  case lounaan_puoleinen:
		retval = "lounaan puoleinen";
		break;
	  case lansi:
		retval = "länsi";
		break;
	  case lannen_puoleinen:
		retval = "lannen puoleinen";
		break;
	  case luode:
		retval = "luode";
		break;
	  case luoteen_puoleinen:
		retval = "luoteen puoleinen";
		break;
	  case vaihteleva:
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
						   const WindStrengthPeriodDataItem& theWindStrengthPeriodDataItem)
  {
	theOutput << theWindStrengthPeriodDataItem.thePeriod.localStartTime()
			  << " ... "
			  << theWindStrengthPeriodDataItem.thePeriod.localEndTime()
			  << ": "
			  << get_wind_strength_string(theWindStrengthPeriodDataItem.theWindStrength)
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
		
		output_file << directed_speed_string(theRawDataVector[i]->theWindspeedMean, 
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
		
		output_file << directed_speed_string(theRawDataVector[*it]->theWindspeedMean, 
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
					storyParams.theOriginalWindStrengthIndexes);

	print_csv_table(storyParams.theAreaName,
					"_equalized",
					storyParams.theVar,
					storyParams.theRawDataVector,
					storyParams.theEqualizedWindStrengthIndexes);

	print_wiki_table(storyParams.theAreaName,
					 storyParams.theVar,
					 storyParams.theRawDataVector);
	
	for(unsigned int i = 0; i < storyParams.theRawDataVector.size(); i++)
	  {
		storyParams.theLog << *(storyParams.theRawDataVector[i]);
		storyParams.theLog << "; fraasi: "<< directed_speed_string(storyParams.theRawDataVector[i]->theWindspeedMean, 
																   storyParams.theRawDataVector[i]->theWindDirection, 
																   storyParams.theVar)
						   << endl;
	  }
  }

  void log_equalized_wind_strength_data_vector(wo_story_params& storyParams)
  {
	storyParams.theLog << "*********** EQUALIZED WIND STRENGTH DATA ***********" << endl;

	list<unsigned int>::iterator it;

	for(it = storyParams.theEqualizedWindStrengthIndexes.begin();
		it != storyParams.theEqualizedWindStrengthIndexes.end(); 
		it++)
	  {
		storyParams.theLog << *(storyParams.theRawDataVector[*it]) << endl;
	  }
  }


  void log_wind_strength_periods(wo_story_params& storyParams)
  {
	storyParams.theLog << "*********** WIND STRENGTH PERIODS ***********" << endl;
	for(unsigned int i = 0; i < storyParams.theWindStrengthVector.size(); i++)
	  {
		storyParams.theLog << *(storyParams.theWindStrengthVector[i]);
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

  void allocate_data_structures(wo_story_params& storyParams)
  {
	NFmiTime periodStartTime = storyParams.theDataPeriod.localStartTime();
	
	unsigned int timeIndex = 0;
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
																timeIndex++));

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

	for(unsigned int i = 0; i < storyParams.theWindStrengthVector.size(); i++)
	  {
		delete storyParams.theWindStrengthVector[i];
	  }	
	storyParams.theWindStrengthVector.clear();

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
		storyParams.theRawDataVector[i]->theWindspeedMin =
		  forecaster.analyze(storyParams.theVar,//itsVar+"::fake::"+daystr+"::speed::minimum",
							 storyParams.theSources,
							 WindSpeed,
							 Minimum,
							 Mean,
							 storyParams.theArea,
							 storyParams.theRawDataVector[i]->thePeriod);

		storyParams.theRawDataVector[i]->theWindspeedMax =
		  forecaster.analyze(storyParams.theVar,//itsVar+"::fake::"+daystr+"::speed::maximum",
							 storyParams.theSources,
							 WindSpeed,
							 Maximum,
							 Mean,
							 storyParams.theArea,
							 storyParams.theRawDataVector[i]->thePeriod);

		storyParams.theRawDataVector[i]->theWindspeedMean =
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

		storyParams.theOriginalWindStrengthIndexes.push_back(i);
		storyParams.theEqualizedWindStrengthIndexes.push_back(i);
		storyParams.theEqualizedWindDirectionIndexes.push_back(i);
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

  wind_direction_id get_wind_direction_id(const WeatherResult& windDirection, const string& var)
  {
	WindDirectionAccuracy accuracy(direction_accuracy(windDirection.error(), var));

	if(accuracy == bad_accuracy)
	  return vaihteleva;

	if(windDirection.value() >= 337.5 || windDirection.value() < 22.5)
	  return (accuracy == good_accuracy ? pohjoinen : pohjoisen_puoleinen);
	else if(windDirection.value() >= 22.5 || windDirection.value() < 67.5)
	  return (accuracy == good_accuracy ? koillinen : koillisen_puoleinen);
	else if(windDirection.value() >= 67.5 || windDirection.value() < 112.5)
	  return (accuracy == good_accuracy ? ita : idan_puoleinen);
	else if(windDirection.value() >= 112.5 || windDirection.value() < 157.5)
	  return (accuracy == good_accuracy ? kaakko : kaakon_puoleinen);
	else if(windDirection.value() >= 157.5 || windDirection.value() < 202.5)
	  return (accuracy == good_accuracy ? etela : etelan_puoleinen);
	else if(windDirection.value() >= 202.5 || windDirection.value() < 247.5)
	  return (accuracy == good_accuracy ? lounas : lounaan_puoleinen);
	else if(windDirection.value() >= 247.5 || windDirection.value() < 292.5)
	  return (accuracy == good_accuracy ? lansi : lannen_puoleinen);
	else
	  return (accuracy == good_accuracy ? luode : luoteen_puoleinen);

  }
  void find_out_wind_strength_periods(wo_story_params& storyParams)
  {
	if(storyParams.theRawDataVector.size() == 0)
	  return;
	else if(storyParams.theRawDataVector.size() == 1)
	  {
		storyParams.theWindStrengthVector.push_back(new WindStrengthPeriodDataItem(storyParams.theRawDataVector[0]->thePeriod,
																				   get_wind_strength_id(storyParams.theRawDataVector[0]->theWindspeedMean)));
		return;
	  }

	unsigned int periodStartIndex = 0;
	wind_strength_id previous_wind_strength_id(get_wind_strength_id(storyParams.theRawDataVector[periodStartIndex]->theWindspeedMean));	  
	for(unsigned int i = 1; i < storyParams.theRawDataVector.size(); i++)
	  {
		wind_strength_id current_wind_strength_id(get_wind_strength_id(storyParams.theRawDataVector[i]->theWindspeedMean));
		
		if(current_wind_strength_id != previous_wind_strength_id)
		  {
			WeatherPeriod windStrengthPeriod(storyParams.theRawDataVector[periodStartIndex]->thePeriod.localStartTime(),
											 storyParams.theRawDataVector[i-1]->thePeriod.localEndTime());
			storyParams.theWindStrengthVector.push_back(new WindStrengthPeriodDataItem(windStrengthPeriod, previous_wind_strength_id));
			periodStartIndex = i;
			previous_wind_strength_id = current_wind_strength_id;
		  }
	  }
	WeatherPeriod windStrengthPeriod(storyParams.theRawDataVector[periodStartIndex]->thePeriod.localStartTime(),
									 storyParams.theRawDataVector[storyParams.theRawDataVector.size() - 1]->thePeriod.localEndTime());

	storyParams.theWindStrengthVector.push_back(new WindStrengthPeriodDataItem(windStrengthPeriod, previous_wind_strength_id));
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

  double distance_from_line(const NFmiPoint& point,  const NFmiPoint& lineBeg, const NFmiPoint& lineEnd)
  {
	double slope = (lineEnd.Y() - lineBeg.Y()) / (lineEnd.X() - lineBeg.X());
	double angle = abs(atan(slope));
	double distance = (lineEnd.X() - lineBeg.X()) * sin(angle);
	return distance;
  }

  void calculate_equalized_windspeed_indexes(wo_story_params& storyParams)
  {
	//unsigned int step = 1;

	while(1)
	  {
		double minError = 1000000.0;
		
		list<unsigned int>::iterator minErrorIndex;	
		list<unsigned int>::iterator it = storyParams.theEqualizedWindStrengthIndexes.begin();
		list<unsigned int>::iterator it1 = storyParams.theEqualizedWindStrengthIndexes.begin();
		list<unsigned int>::iterator it2 = storyParams.theEqualizedWindStrengthIndexes.begin();
		it1++;
		it2++;
		it2++;

		for(it = storyParams.theEqualizedWindStrengthIndexes.begin();
			it != storyParams.theEqualizedWindStrengthIndexes.end() &&
			  it1 != storyParams.theEqualizedWindStrengthIndexes.end() &&
			  it2 != storyParams.theEqualizedWindStrengthIndexes.end(); 
			it++, it1++, it2++)
		  {
			double lineBegX = *it;
			double lineBegY = storyParams.theRawDataVector[*it]->theWindspeedMean.value();
			double lineEndX = *(it2);
			double lineEndY = storyParams.theRawDataVector[*it2]->theWindspeedMean.value();
			double pointX = *it1;
			double pointY = storyParams.theRawDataVector[*it1]->theWindspeedMean.value();
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

		if(minError > 0.20)
		  {
			return;
		  }

		/*
		minErrorIndex--;

		cout << 
		  "ennen poistettavaa(index): (" << 
		  *minErrorIndex << 
		  ") "  << 
		  storyParams.theRawDataVector[*minErrorIndex]->theWindspeedMean.value() << endl;

		minErrorIndex++;
		
		cout << 
		  "poistettava(index): (" << 
		  *minErrorIndex << 
		  ") "  << 
		  storyParams.theRawDataVector[*minErrorIndex]->theWindspeedMean.value() << endl;
		
		minErrorIndex++;
		
		cout << 
		  "poistettavan jälkeen(index): (" << 
		  *minErrorIndex << 
		  ") "  << 
		  storyParams.theRawDataVector[*minErrorIndex]->theWindspeedMean.value() << endl;

		minErrorIndex--;

		cout << "index(error): " << *minErrorIndex << "(" << minError << ")" << endl;

		*/

		storyParams.theEqualizedWindStrengthIndexes.erase(minErrorIndex);
		/*
		std::ostringstream stringStream;
		stringStream << "_step" << step++;

		print_csv_table(storyParams.theAreaName,
						stringStream.str(),
						storyParams.theVar,
						storyParams.theRawDataVector,
						storyParams.theEqualizedWindStrengthIndexes);
		*/

	  }
  }

  void calculate_equalized_indexes(wo_story_params& storyParams)
  {
	if(storyParams.theEqualizedWindStrengthIndexes.size() <= 3)
	  return;

	calculate_equalized_windspeed_indexes(storyParams);

	/*
	double minError = 1000000.0;
	unsigned int minErrorIndex = 0;	

	list<unsigned int>::iterator it = storyParams.theEqualizedWindSpeedIndexes.begin();
	list<unsigned int>::iterator it1 = it;
	list<unsigned int>::iterator it2 = it;
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
		double lineBegY = storyParams.theRawDataVector[*it]->theWindspeedMean.value();
		double lineEndX = *(it2);
		double lineEndY = storyParams.theRawDataVector[*it2]->theWindspeedMean.value();
		double pointX = *it1;
		double pointY = storyParams.theRawDataVector[*it1]->theWindspeedMean.value();
		NFmiPoint point(pointX, pointY);
		NFmiPoint lineBegPoint(lineBegX, lineBegY);
		NFmiPoint lineEndPoint(lineEndX, lineEndY);
		double deviation_from_line = distance_from_line(point, lineBegPoint, lineEndPoint);
		if(deviation_from_line < minError)
		  {
			minError = deviation_from_line;
			minErrorIndex = *it1;
		  }
	  }

	cout << "index(error): " << minErrorIndex << "(" << minError << ")" << endl;
	*/
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
								areaName,
								itsArea,
								itsPeriod,
								itsPeriod,
								itsForecastTime,
								itsSources,
								logger);


	allocate_data_structures(storyParams);

	populate_wind_time_series(storyParams);
	
	find_out_wind_strength_periods(storyParams);

	find_out_wind_direction_periods(storyParams);

	calculate_equalized_indexes(storyParams);

	log_raw_data_vector(storyParams);

	log_equalized_wind_strength_data_vector(storyParams);

	log_wind_strength_periods(storyParams);

	log_wind_direction_periods(storyParams);

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
