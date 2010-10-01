// ======================================================================
/*!
 * \file
 * \brief Implementation of PrecipitationForecast class
 */
// ======================================================================

#include "WeatherStory.h"
#include "CloudinessStory.h"
#include "CloudinessStoryTools.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "HourPeriodGenerator.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "NightAndDayPeriodGenerator.h"
#include "PeriodPhraseFactory.h"
#include "PrecipitationPeriodTools.h"
#include "PrecipitationStoryTools.h"
#include "RangeAcceptor.h"
#include "ValueAcceptor.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "TimeTools.h"
#include "WeatherResult.h"
#include "WeekdayTools.h"
#include "NullPeriodGenerator.h"
#include "WeatherPeriodTools.h"
#include "AreaTools.h"
#include "MathTools.h"
#include "SeasonTools.h"
#include "SubMaskExtractor.h"
#include "CloudinessForecast.h"

#include <boost/lexical_cast.hpp>
#include <vector>
#include <map>

namespace TextGen
{

  using namespace Settings;
  using namespace WeatherAnalysis;
  using namespace AreaTools;
  using namespace boost;
  using namespace std;


  std::ostream& operator<<(std::ostream & theOutput,
						   const CloudinessDataItemData& theCloudinessDataItemData)
  {
	string cloudinessIdStr(cloudiness_string(theCloudinessDataItemData.theId));

	string trendIdStr = trend_string(theCloudinessDataItemData.theTrendId);

	theOutput << "    " << cloudinessIdStr << ": ";
	theOutput << "min=" << theCloudinessDataItemData.theMin << " ";
	theOutput << "mean=" << theCloudinessDataItemData.theMean << " ";
	theOutput << "max=" << theCloudinessDataItemData.theMax << " ";
	theOutput << "std.dev=" << theCloudinessDataItemData.theStandardDeviation << endl;
	theOutput << "    trend: " << trendIdStr << endl;
	theOutput << "    pearson coefficient: " << theCloudinessDataItemData.thePearsonCoefficient << endl;

	return theOutput;
  }

  std::ostream& operator<<(std::ostream & theOutput,
						   const CloudinessDataItem& theCloudinessDataItem)
  {
	if(theCloudinessDataItem.theCoastalData)
	  {
		theOutput << "  Coastal" << endl;
		theOutput << *theCloudinessDataItem.theCoastalData;
	  }
	if(theCloudinessDataItem.theInlandData)
	  {
		theOutput << "  Inland" << endl;
		theOutput << *theCloudinessDataItem.theInlandData;
	  }
	if(theCloudinessDataItem.theFullData)
	  {
		theOutput << "  Full area" << endl;
		theOutput << *theCloudinessDataItem.theFullData;
	  }
	return theOutput;
  }

  bool puolipilvisesta_pilviseen(const cloudiness_id& theCloudinessId1,
								 const cloudiness_id& theCloudinessId2)
  {
	if(theCloudinessId1 != MISSING_CLOUDINESS_ID && 
	   theCloudinessId1 == PUOLIPILVISTA_JA_PILVISTA &&
	   theCloudinessId2 == PUOLIPILVISTA_JA_PILVISTA)
	  return true;

	return false;
  }

  Sentence cloudiness_sentence(const cloudiness_id& theCloudinessId, 
							   const bool& theShortForm /*= false*/)
  {
	Sentence sentence;

	std::string cloudiness_phrase;
	switch(theCloudinessId)
	  {
	  case PUOLIPILVISTA_JA_PILVISTA:
		cloudiness_phrase = SAA_VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE;
		break;
	  case SELKEAA:
		cloudiness_phrase = SELKEAA_WORD;
		break;
	  case MELKO_SELKEAA:
		cloudiness_phrase = MELKO_SELKEAA_PHRASE;
		break;
	  case PUOLIPILVISTA:
		cloudiness_phrase = PUOLIPILVISTA_WORD;
		break;
	  case VERRATTAIN_PILVISTA:
		cloudiness_phrase = VERRATTAIN_PILVISTA_PHRASE;
		break;
	  case PILVISTA:
		cloudiness_phrase = PILVISTA_WORD;
		break;
	  default:
		break;
	  }

	if(cloudiness_phrase.length() > 0)
	  {
		if(theCloudinessId == PUOLIPILVISTA_JA_PILVISTA)
		  sentence << cloudiness_phrase;
		else
		  if(theShortForm)
			sentence << cloudiness_phrase;
		  else
			sentence << SAA_WORD << ON_WORD << cloudiness_phrase;
	  }
	
	return sentence;
  }

  cloudiness_id get_cloudiness_id(const float& theMin, 
								  const float& theMean, 
								  const float& theMax, 
								  const float& theStandardDeviation)
  {
	cloudiness_id id(MISSING_CLOUDINESS_ID);

	if(theMean == -1)
	  return id;

	/*
#define SELKEAA_UPPER_LIMIT 9.9
#define MELKEIN_SELKEAA_LOWER_LIMIT 9.9
#define MELKEIN_SELKEAA_UPPER_LIMIT 35
#define PUOLIPILVISTA_LOWER_LIMIT 35.0
#define PUOLIPILVISTA_UPPER_LIMIT 65.0
#define VERRATTAIN_PILVISTA_LOWER_LIMIT 65.0
#define VERRATTAIN_PILVISTA_UPPER_LIMIT 85.0
#define PILVISTA_LOWER_LIMIT 85.0
	*/

	if(theMin >= PUOLIPILVISTA_LOWER_LIMIT &&
	   theMin <= PUOLIPILVISTA_UPPER_LIMIT && 
	   theMax >= PILVISTA_LOWER_LIMIT)
	  {
		id = PUOLIPILVISTA_JA_PILVISTA;
	  }
	else if(theMean <= SELKEAA_UPPER_LIMIT)
	  {
		id = SELKEAA;
	  }
	else if(theMean <= MELKEIN_SELKEAA_UPPER_LIMIT)
	  {
		id = MELKO_SELKEAA;
	  }
	else if(theMean <= PUOLIPILVISTA_UPPER_LIMIT)
	  {
		id = PUOLIPILVISTA;
	  }
	else if(theMean <= VERRATTAIN_PILVISTA_UPPER_LIMIT)
	  {
		  id = VERRATTAIN_PILVISTA;
	  }
	else
	  {
		id = PILVISTA;
	  }

	return id;
  }

  cloudiness_id get_cloudiness_id(const float& theCloudiness) 
  {
	cloudiness_id id(MISSING_CLOUDINESS_ID);

	if(theCloudiness < 0)
	  return id;

	if(theCloudiness <= SELKEAA_UPPER_LIMIT)
	  {
		id = SELKEAA;
	  }
	else if(theCloudiness  <= MELKEIN_SELKEAA_UPPER_LIMIT)
	  {
		id = MELKO_SELKEAA;
	  }
	else if(theCloudiness <= PUOLIPILVISTA_UPPER_LIMIT)
	  {
		id = PUOLIPILVISTA;
	  }
	else if(theCloudiness <= VERRATTAIN_PILVISTA_UPPER_LIMIT)
	  {
		id = VERRATTAIN_PILVISTA;
	  }
	else
	  {
		id = PILVISTA;
	  }

	return id;
  }

  const char* cloudiness_string(const cloudiness_id& theCloudinessId)
  {
	const char* retval = "";

	switch(theCloudinessId)
	  {
	  case SELKEAA:
		retval = SELKEAA_WORD;
	  break;
	  case MELKO_SELKEAA:
		retval = MELKO_SELKEAA_PHRASE;
	  break;
	  case PUOLIPILVISTA:
		retval = PUOLIPILVISTA_WORD;
	  break;
	  case VERRATTAIN_PILVISTA:
		retval = VERRATTAIN_PILVISTA_PHRASE;
	  break;
	  case PILVISTA:
		retval = PILVISTA_WORD;
	  break;
	  case PUOLIPILVISTA_JA_PILVISTA:
		retval = SAA_VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE;
	  break;
	  default:
		retval = "missing cloudiness id";
	  break;
	  }

	return retval;
  }


  CloudinessForecast::CloudinessForecast(wf_story_params& parameters) : theParameters(parameters),
																		theCoastalData(0),
																		theInlandData(0),
																		theFullData(0)

  {
	if(theParameters.theForecastArea & FULL_AREA)
	  theFullData = ((*theParameters.theCompleteData[FULL_AREA])[CLOUDINESS_DATA]);
	if(theParameters.theForecastArea & COASTAL_AREA)
	  theCoastalData = ((*theParameters.theCompleteData[COASTAL_AREA])[CLOUDINESS_DATA]);
	if(theParameters.theForecastArea & INLAND_AREA)
	  theInlandData = ((*theParameters.theCompleteData[INLAND_AREA])[CLOUDINESS_DATA]);

	findOutCloudinessPeriods(); 
	joinPeriods();
	findOutCloudinessTrends();
  }

  Sentence CloudinessForecast::cloudinessChangeSentence(const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;

	for(unsigned int i = 0; i < theCloudinessTrendsFull.size(); i++)
	  {
		WeatherPeriod period(theCloudinessTrendsFull.at(i).first.localStartTime(),
							 theCloudinessTrendsFull.at(i).first.localEndTime());

		if(!(period.localStartTime() >= thePeriod.localStartTime() &&
			 period.localEndTime() <= thePeriod.localEndTime()))
		  {
			//cout << "PERIOD1: " << thePeriod.localStartTime() << "..." << thePeriod.localEndTime() << endl;
			//cout << "PERIOD2: " << period.localStartTime() << "..." << period.localEndTime() << endl;
			continue;
		  }

		trend_id trid(theCloudinessTrendsFull.at(i).second);
		
		
		NFmiTime previousStartTime(period.localStartTime());
		NFmiTime previousEndTime(period.localStartTime());
		previousEndTime.ChangeByHours(-1);
		previousStartTime.ChangeByHours(-4);
		WeatherPeriod previousPeriod(previousStartTime, previousEndTime);
		sentence << cloudiness_string(getCloudinessId(previousPeriod,
													  theFullData));
		sentence << Delimiter(",");
		if(trid == PILVISTYY)
		  {
			sentence << PILVISTYVAA_WORD;
		  }
		else
		  {
			sentence << SELKENEVAA_WORD;
		  }

		sentence << get_time_phrase(period, true);
	  }

	return sentence;
  }

  float CloudinessForecast::getMeanCloudiness(const WeatherPeriod& theWeatherPeriod,
											  const weather_result_data_item_vector& theDataVector) const
  {
	float cloudinessSum(0.0);
	unsigned int count = 0;
	for(unsigned int i = 0; i < theDataVector.size(); i++)
	  {
		if(theDataVector[i]->thePeriod.localStartTime() >= theWeatherPeriod.localStartTime() &&
		   theDataVector[i]->thePeriod.localStartTime() <= theWeatherPeriod.localEndTime() &&
		   theDataVector[i]->thePeriod.localEndTime() >= theWeatherPeriod.localStartTime() &&
		   theDataVector[i]->thePeriod.localEndTime() <= theWeatherPeriod.localEndTime())
		  {
			cloudinessSum += theDataVector[i]->theResult.value();
			count++;
		  }
	  }
	return (count == 0 ? 0.0 : (cloudinessSum / count));
  }

  bool CloudinessForecast::separateCoastInlandCloudiness(const WeatherPeriod& theWeatherPeriod) const
  {
	weather_result_data_item_vector theInterestingDataCoastal;
	weather_result_data_item_vector theInterestingDataInland;
	
	get_sub_time_series(theWeatherPeriod,
						*theCoastalData,
						theInterestingDataCoastal);

	float mean_coastal = get_mean(theInterestingDataCoastal);

	get_sub_time_series(theWeatherPeriod,
						*theInlandData,
						theInterestingDataInland);

	float mean_inland = get_mean(theInterestingDataInland);

	bool retval  = (mean_inland <= 5.0 && mean_coastal >= 70.0 ||
					mean_inland >= 70.0 && mean_coastal <= 5.0);

	return retval;
  }

  // TODO: onko OK
  bool CloudinessForecast::separateWeatherPeriodCloudiness(const WeatherPeriod& theWeatherPeriod1, 
														   const WeatherPeriod& theWeatherPeriod2,
														   const weather_result_data_item_vector& theCloudinessData) const
  {
	weather_result_data_item_vector theInterestingDataPeriod1;
	weather_result_data_item_vector theInterestingDataPeriod2;

	//		bool separateMorningAfternoon = (abs(theCloudinessIdFullMorning - theCloudinessIdFullAfternoon) >= 2);

	get_sub_time_series(theWeatherPeriod1,
						theCloudinessData,
						theInterestingDataPeriod1);

	cloudiness_id cloudinessId1 = get_cloudiness_id(get_mean(theInterestingDataPeriod1));
	
	//	float mean_period1 = get_mean(theInterestingDataPeriod1);
	
	get_sub_time_series(theWeatherPeriod2,
						theCloudinessData,
						theInterestingDataPeriod2);
	
	cloudiness_id cloudinessId2 = get_cloudiness_id(get_mean(theInterestingDataPeriod2));
	//	float mean_period2 = get_mean(theInterestingDataPeriod2);

	return (abs(cloudinessId1 - cloudinessId2) >= 2);

	/*													
	bool retval  = (mean_period1 <= 5.0 && mean_period2 >= 70.0 ||
					mean_period1 >= 70.0 && mean_period2 <= 5.0);
	
	return retval;
	*/
  }  
  
  void CloudinessForecast::findOutCloudinessTrends(const weather_result_data_item_vector* theData,
												   trend_id_vector& theCloudinessTrends)
  {
	// check 4-hour periods if there is trend
	const unsigned int periodLength = 8;
	unsigned int startIndex = 0;
	while(startIndex < theData->size() - (periodLength + 1))
	  {
		WeatherPeriod firstHalfPeriod(theData->at(startIndex)->thePeriod.localStartTime(),
									  theData->at(startIndex + (periodLength/2) - 1)->thePeriod.localEndTime());
		WeatherPeriod secondHalfPeriod(theData->at(startIndex + (periodLength/2))->thePeriod.localStartTime(),
									   theData->at(startIndex + periodLength - 1)->thePeriod.localEndTime());
		WeatherPeriod wholePeriod(theData->at(startIndex)->thePeriod.localStartTime(),
									  theData->at(startIndex + periodLength - 1)->thePeriod.localEndTime());

		weather_result_data_item_vector theFirstHalfData;
		weather_result_data_item_vector theSecondHalfData;
		weather_result_data_item_vector theWholePeriodData;
		
		get_sub_time_series(firstHalfPeriod,
							*theData,
							theFirstHalfData);
		
		get_sub_time_series(secondHalfPeriod,
							*theData,
							theSecondHalfData);

		get_sub_time_series(wholePeriod,
							*theData,
							theWholePeriodData);
		/*
		float min, max;
		float mean = get_mean(theInterestingData);
		get_min_max(theInterestingData, min, max);
		float standard_deviation = get_standard_deviation(theInterestingData);	
		theCloudinessId = get_cloudiness_id(min, mean, max, standard_deviation);
		*/

		
		float pearson_coefficient = get_pearson_coefficient(theWholePeriodData);
		/*
		float minCloudinessInTheFirstHalf;
		float maxCloudinessInTheFirstHalf;
		get_min_max(theFirstHalfData, minCloudinessInTheFirstHalf, maxCloudinessInTheFirstHalf);
		*/
		float meanCloudinessInTheFirstHalf = get_mean(theFirstHalfData);
		float meanCloudinessInTheSecondHalf = get_mean(theSecondHalfData);
		/*
		float minCloudinessInTheSecondHalf;
		float maxCloudinessInTheSecondHalf;
		get_min_max(theFirstHalfData, minCloudinessInTheSecondHalf, maxCloudinessInTheSecondHalf);
		*/
		/*
		cloudiness_id cloudinessAtStart = 
		  get_cloudiness_id(meanCloudinessInTheFirstHalf);
		cloudiness_id cloudinessAtEnd = 
		  get_cloudiness_id(meanCloudinessInTheSecondHalf);
		*/
		trend_id trendId = NO_TREND;

		if(meanCloudinessInTheFirstHalf >= VERRATTAIN_PILVISTA_LOWER_LIMIT && 
		   meanCloudinessInTheSecondHalf <= SELKEAA_UPPER_LIMIT &&
		   pearson_coefficient < -0.65)
		  {
			trendId = SELKENEE;
			NFmiTime endTime(secondHalfPeriod.localStartTime());
			endTime.ChangeByHours(1);
		  }
		else if(meanCloudinessInTheFirstHalf <= MELKEIN_SELKEAA_UPPER_LIMIT && 
				meanCloudinessInTheSecondHalf >= VERRATTAIN_PILVISTA_LOWER_LIMIT && 
				pearson_coefficient >= 0.65)
		  {
			trendId = PILVISTYY;
			NFmiTime endTime(secondHalfPeriod.localStartTime());
			endTime.ChangeByHours(1);
		  }

		if(trendId != NO_TREND)
		  {
			NFmiTime endTime(secondHalfPeriod.localStartTime());
			endTime.ChangeByHours(1);
			theCloudinessTrends.push_back(make_pair(WeatherPeriod(secondHalfPeriod.localStartTime(), endTime), trendId));			
		  }

		startIndex += (trendId == NO_TREND ? 1 : periodLength);
	  }
 }

  void CloudinessForecast::findOutCloudinessTrends()
  {
	if(theCoastalData)
	  findOutCloudinessTrends(theCoastalData, theCloudinessTrendsCoastal);
	if(theInlandData)
	  findOutCloudinessTrends(theInlandData, theCloudinessTrendsInland);
	if(theFullData)
	  findOutCloudinessTrends(theFullData, theCloudinessTrendsFull);
  }

  void CloudinessForecast::findOutCloudinessPeriods(const weather_result_data_item_vector* theData, 
													cloudiness_period_vector& theCloudinessPeriods)
  {
	if(theData)
	  {
		NFmiTime previousStartTime;
		NFmiTime previousEndTime;
		cloudiness_id previousCloudinessId;
		for(unsigned int i = 0; i < theData->size(); i++)
		  {
			if(i== 0)
			  {
				previousStartTime = theData->at(i)->thePeriod.localStartTime();
				previousEndTime = theData->at(i)->thePeriod.localEndTime();
				previousCloudinessId = get_cloudiness_id(theData->at(i)->theResult.value());
			  }
			else
			  {
				if(previousCloudinessId != get_cloudiness_id(theData->at(i)->theResult.value()))
				  {
					pair<WeatherPeriod, cloudiness_id> item = 
					  make_pair(WeatherPeriod(previousStartTime, previousEndTime), previousCloudinessId);
					theCloudinessPeriods.push_back(item);
					previousStartTime = theData->at(i)->thePeriod.localStartTime();
					previousEndTime = theData->at(i)->thePeriod.localEndTime();
					previousCloudinessId = get_cloudiness_id(theData->at(i)->theResult.value());

				  }
				else if(i == theData->size() - 1)
				  {
					pair<WeatherPeriod, cloudiness_id> item = 
					  make_pair(WeatherPeriod(previousStartTime, theData->at(i)->thePeriod.localEndTime()), previousCloudinessId );
					theCloudinessPeriods.push_back(item);

				  }
				else
				  {
					previousEndTime = theData->at(i)->thePeriod.localEndTime();
				  }
			  }
		  }
	  }
  }

  void CloudinessForecast::findOutCloudinessPeriods()
  {
	findOutCloudinessPeriods(theCoastalData, theCloudinessPeriodsCoastal);
	findOutCloudinessPeriods(theInlandData, theCloudinessPeriodsInland);
	findOutCloudinessPeriods(theFullData, theCloudinessPeriodsFull);
  }

  void joinPuolipilvisestaPilviseen(const weather_result_data_item_vector* theData,
									vector<int>& theCloudinessPuolipilvisestaPilviseen)
  {
	if(!theData)
	  return;

	int index = -1;
	for(unsigned int i = 0; i < theData->size(); i++)
	  {
		if(get_cloudiness_id(theData->at(i)->theResult.value()) >= PUOLIPILVISTA && 
		   get_cloudiness_id(theData->at(i)->theResult.value()) <= PILVISTA &&
		   i != theData->size() - 1)
		  {
			if(index == -1)
			  index = i;
		  }
		else if(get_cloudiness_id(theData->at(i)->theResult.value()) == SELKEAA || 
				get_cloudiness_id(theData->at(i)->theResult.value()) == MELKO_SELKEAA ||
				get_cloudiness_id(theData->at(i)->theResult.value()) == PILVISTA ||
				i == theData->size() - 1)
		  {
			if(index != -1 && i-1-index > 1)
			  {
				for(int k = index; k < static_cast<int>(i); k++)
				  {
					WeatherPeriod period(theData->at(k)->thePeriod.localStartTime(),
										 theData->at(k)->thePeriod.localEndTime());

					theCloudinessPuolipilvisestaPilviseen.push_back(k);
				  }
			  }
			index = -1;
		  }
	  }
  }

  void CloudinessForecast::joinPeriods(const weather_result_data_item_vector* theCloudinessDataSource,
									   const cloudiness_period_vector& theCloudinessPeriodsSource,
									   cloudiness_period_vector& theCloudinessPeriodsDestination)
  {
	if(theCloudinessPeriodsSource.size() == 0)
	  return;

	vector<int> theCloudinessPuolipilvisestaPilviseen;


	int periodStartIndex = 0;
	cloudiness_id clidPrevious(theCloudinessPeriodsSource.at(0).second);
	cloudiness_id clidCurrent(clidPrevious);

	for(unsigned int i = 1; i < theCloudinessPeriodsSource.size(); i++)
	  {
		bool lastPeriod = (i == theCloudinessPeriodsSource.size() - 1);
		clidCurrent = theCloudinessPeriodsSource.at(i).second;

		if(abs(clidPrevious - clidCurrent) >= 2 || lastPeriod)
		  {
			// check if "puolipilvisestä pilviseen"
			if(clidPrevious == PUOLIPILVISTA && clidCurrent == PILVISTA || 
			   clidPrevious == PILVISTA && clidCurrent == PUOLIPILVISTA)
			  {
			  }

			NFmiTime startTime(theCloudinessPeriodsSource.at(periodStartIndex).first.localStartTime());
			NFmiTime endTime(theCloudinessPeriodsSource.at(lastPeriod ? i : i-1).first.localEndTime());

			weather_result_data_item_vector thePeriodCloudiness;

			get_sub_time_series(WeatherPeriod(startTime, endTime),
								*theCloudinessDataSource,
								thePeriodCloudiness);

			float min, max, mean, stddev;

			get_min_max(thePeriodCloudiness, min, max);
			mean = get_mean(thePeriodCloudiness);
			stddev = get_standard_deviation(thePeriodCloudiness);

			
			cloudiness_id actual_clid = get_cloudiness_id(min, mean, max, stddev);

			  
			pair<WeatherPeriod, cloudiness_id> item = make_pair(WeatherPeriod(startTime, endTime), actual_clid);

			theCloudinessPeriodsDestination.push_back(item);

			clidPrevious = clidCurrent;
			periodStartIndex = i;
		  }
	  }	
  }

  void CloudinessForecast::joinPeriods()
  {
	joinPeriods(theCoastalData, theCloudinessPeriodsCoastal, theCloudinessPeriodsCoastalJoined);
	joinPeriods(theInlandData, theCloudinessPeriodsInland, theCloudinessPeriodsInlandJoined);
	joinPeriods(theFullData, theCloudinessPeriodsFull, theCloudinessPeriodsFullJoined);	
  }

  void CloudinessForecast::printOutCloudinessData(std::ostream& theOutput) const
  {
	theOutput << "** CLOUDINESS DATA **" << endl; 
	if(theCoastalData)
	  {
	  theOutput << "Coastal cloudiness: " << endl; 
	  printOutCloudinessData(theOutput, theCoastalData);
	  }
	if(theInlandData)
	  {
		theOutput << "Inland cloudiness: " << endl; 
		printOutCloudinessData(theOutput, theInlandData);
	  }
	if(theFullData)
	  {
		theOutput << "Full area cloudiness: " << endl; 
		printOutCloudinessData(theOutput, theFullData);
	  }
  }

  void CloudinessForecast::printOutCloudinessData(std::ostream& theOutput,
												  const weather_result_data_item_vector* theDataVector) const
  {
	for(unsigned int i = 0; i < theDataVector->size(); i++)
	  {
		theOutput << theDataVector->at(i)->thePeriod.localStartTime()
				  << "..."
				  << theDataVector->at(i)->thePeriod.localEndTime()
				  << ": "
				  << theDataVector->at(i)->theResult.value()
				  << endl;
	  }
	/*
 struct WeatherResultDataItem
  {
	WeatherResultDataItem(const WeatherPeriod& period, 
						  const WeatherResult& result, 
						  const part_of_the_day_id& partOfTheDay) :
	  thePeriod(period),
	  theResult(result),
	  thePartOfTheDay(partOfTheDay)
	{}
	
	WeatherPeriod thePeriod;
	WeatherResult theResult;
	part_of_the_day_id thePartOfTheDay;
  };
	*/

	  /*
	const weather_result_data_item_vector* theCoastalData;
	const weather_result_data_item_vector* theInlandData;
	const weather_result_data_item_vector* theFullData;

	  */
  }

  void CloudinessForecast::printOutCloudinessPeriods(std::ostream& theOutput, 
													 const cloudiness_period_vector& theCloudinessPeriods) const
  {
	for(unsigned int i = 0; i < theCloudinessPeriods.size(); i++)
	  {
		WeatherPeriod period(theCloudinessPeriods.at(i).first.localStartTime(),
							 theCloudinessPeriods.at(i).first.localEndTime());
		cloudiness_id clid(theCloudinessPeriods.at(i).second);
		theOutput << period.localStartTime()
				  << "..."
				  << period.localEndTime()
				  << ": "
				  << cloudiness_string(clid)
				  << endl;
	  }
  }
  
  void CloudinessForecast::printOutCloudinessTrends(std::ostream& theOutput, 
													const trend_id_vector& theCloudinessTrends) const
  {
	for(unsigned int i = 0; i < theCloudinessTrends.size(); i++)
	  {
		WeatherPeriod period(theCloudinessTrends.at(i).first.localStartTime(),
							 theCloudinessTrends.at(i).first.localEndTime());
		trend_id trid(theCloudinessTrends.at(i).second);
		theOutput << period.localStartTime()
				  << "..."
				  << period.localEndTime()
				  << ": "
				  << trend_string(trid)
				  << endl;
	  }
  }

  void CloudinessForecast::printOutCloudinessTrends(std::ostream& theOutput) const
  {
	theOutput << "** CLOUDINESS TRENDS **" << endl; 
	bool isTrends = false;
	if(theCloudinessTrendsCoastal.size() > 0)
	  {
		theOutput << "Coastal trends: " << endl; 
		printOutCloudinessTrends(theOutput, theCloudinessTrendsCoastal);
		isTrends = true;
	  }
	if(theCloudinessTrendsInland.size() > 0)
	  {
		theOutput << "Inland trends: " << endl; 
		printOutCloudinessTrends(theOutput, theCloudinessTrendsInland);
		isTrends = true;
	  }
	if(theCloudinessTrendsFull.size() > 0)
	  {
		theOutput << "Full area trends: " << endl; 
		printOutCloudinessTrends(theOutput, theCloudinessTrendsFull);
		isTrends = true;
	  }

	if(!isTrends)
		theOutput << "No trends!" << endl; 
  }

  void CloudinessForecast::printOutCloudinessPeriods(std::ostream& theOutput) const
  {
	theOutput << "** CLOUDINESS PERIODS **" << endl;

	if(theCoastalData)
	  {
	  theOutput << "Coastal cloudiness: " << endl; 
	  printOutCloudinessPeriods(theOutput, theCloudinessPeriodsCoastal);
	  theOutput << "Coastal cloudiness joined: " << endl; 
	  printOutCloudinessPeriods(theOutput, theCloudinessPeriodsCoastalJoined);
	  vector<int> theCloudinessPuolipilvisestaPilviseen;
	  joinPuolipilvisestaPilviseen(theCoastalData, theCloudinessPuolipilvisestaPilviseen);
	  }
	if(theInlandData)
	  {
		theOutput << "Inland cloudiness: " << endl; 
		printOutCloudinessPeriods(theOutput, theCloudinessPeriodsInland);
		theOutput << "Inland cloudiness joined: " << endl; 
		printOutCloudinessPeriods(theOutput, theCloudinessPeriodsInlandJoined);
		vector<int> theCloudinessPuolipilvisestaPilviseen;
		joinPuolipilvisestaPilviseen(theInlandData, theCloudinessPuolipilvisestaPilviseen);
	  }
	if(theFullData)
	  {
		theOutput << "Full area cloudiness: " << endl; 
		printOutCloudinessPeriods(theOutput, theCloudinessPeriodsFull);
		theOutput << "Full area cloudiness joined: " << endl; 
		printOutCloudinessPeriods(theOutput, theCloudinessPeriodsFullJoined);
		vector<int> theCloudinessPuolipilvisestaPilviseen;
		joinPuolipilvisestaPilviseen(theFullData, theCloudinessPuolipilvisestaPilviseen);
	  }
  }

  void CloudinessForecast::getWeatherPeriodCloudiness(const WeatherPeriod& thePeriod,
													  const cloudiness_period_vector& theSourceCloudinessPeriods,
													  cloudiness_period_vector& theWeatherPeriodCloudiness) const
  {
	for(unsigned int i = 0; i < theSourceCloudinessPeriods.size(); i++)
	  {
		if(thePeriod.localStartTime() >= theSourceCloudinessPeriods.at(i).first.localStartTime() &&
		   thePeriod.localEndTime() <= theSourceCloudinessPeriods.at(i).first.localEndTime())
		  {
			NFmiTime startTime(thePeriod.localStartTime());
			NFmiTime endTime(thePeriod.localEndTime());
			cloudiness_id clid(theSourceCloudinessPeriods.at(i).second);
			pair<WeatherPeriod, cloudiness_id> item = make_pair(WeatherPeriod(startTime, endTime), clid);
			theWeatherPeriodCloudiness.push_back(item);
		  }
		else if(thePeriod.localStartTime() >= theSourceCloudinessPeriods.at(i).first.localStartTime() &&
				thePeriod.localStartTime() < theSourceCloudinessPeriods.at(i).first.localEndTime() &&
				thePeriod.localEndTime() > theSourceCloudinessPeriods.at(i).first.localEndTime())
		  {
			NFmiTime startTime(thePeriod.localStartTime());
			NFmiTime endTime(theSourceCloudinessPeriods.at(i).first.localEndTime());
			cloudiness_id clid(theSourceCloudinessPeriods.at(i).second);
			pair<WeatherPeriod, cloudiness_id> item = make_pair(WeatherPeriod(startTime, endTime), clid);
			theWeatherPeriodCloudiness.push_back(item);
		  }
		else if(thePeriod.localStartTime() < theSourceCloudinessPeriods.at(i).first.localStartTime() &&
				thePeriod.localEndTime() > theSourceCloudinessPeriods.at(i).first.localStartTime() &&
				thePeriod.localEndTime() <= theSourceCloudinessPeriods.at(i).first.localEndTime())
		  {
			NFmiTime startTime(theSourceCloudinessPeriods.at(i).first.localStartTime());
			NFmiTime endTime(thePeriod.localEndTime());
			cloudiness_id clid(theSourceCloudinessPeriods.at(i).second);
			pair<WeatherPeriod, cloudiness_id> item = make_pair(WeatherPeriod(startTime, endTime), clid);
			theWeatherPeriodCloudiness.push_back(item);				
		  }
		else if(thePeriod.localStartTime() < theSourceCloudinessPeriods.at(i).first.localStartTime() &&
				thePeriod.localEndTime() > theSourceCloudinessPeriods.at(i).first.localEndTime())
		  {
			NFmiTime startTime(theSourceCloudinessPeriods.at(i).first.localStartTime());
			NFmiTime endTime( theSourceCloudinessPeriods.at(i).first.localEndTime());
			cloudiness_id clid(theSourceCloudinessPeriods.at(i).second);
			pair<WeatherPeriod, cloudiness_id> item = make_pair(WeatherPeriod(startTime, endTime), clid);
			theWeatherPeriodCloudiness.push_back(item);				
		  }
	  }
  }

  Sentence CloudinessForecast::cloudinessSentence(const WeatherPeriod& thePeriod,
												  const bool& theCheckCloudinessChange/* = true*/) const
  {
	if(theCheckCloudinessChange)
	  {
		Sentence cloudinessChange;
		cloudinessChange << cloudinessChangeSentence(thePeriod);
		if(cloudinessChange.size() > 0)
		  return cloudinessChange;
	  }

	Sentence sentence;

	NFmiTime previousPeriodStartTime(thePeriod.localStartTime());
	NFmiTime previousPeriodEndTime(thePeriod.localStartTime());
	previousPeriodStartTime.ChangeByHours(-20);
	WeatherPeriod previousPeriod(previousPeriodStartTime, previousPeriodEndTime);
	
	cloudiness_id coastalCloudinessId = getCloudinessId(thePeriod, theCoastalData);
	cloudiness_id inlandCloudinessId = getCloudinessId(thePeriod, theInlandData);
	cloudiness_id fullAreaCloudinessId = getCloudinessId(thePeriod, theFullData);
	float meanCloudinessFull(0.0);
	float meanCloudinessInland(0.0);
	float meanCloudinessCoastal(0.0);
	float meanCloudinessFullPrevious(0.0);
	float meanCloudinessInlandPrevious(0.0);
	float meanCloudinessCoastalPrevious(0.0);

	Sentence todaySentence;
	todaySentence << PeriodPhraseFactory::create("today",
												 theParameters.theVariable,
												 theParameters.theForecastTime,
												 thePeriod);
	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  {
		if(separateCoastInlandCloudiness(thePeriod))
		  {
			sentence << COAST_PHRASE
					 << cloudiness_sentence(coastalCloudinessId);
			sentence << Delimiter(",");
			sentence << INLAND_PHRASE;
			sentence << cloudiness_sentence(inlandCloudinessId, true);
		  }
		else
		  {
			meanCloudinessFull = getMeanCloudiness(thePeriod, *theFullData);
			meanCloudinessFullPrevious = getMeanCloudiness(previousPeriod, *theFullData);

			if(todaySentence.size() > 0 &&
			   meanCloudinessFull >= PILVISTA_LOWER_LIMIT &&
			   meanCloudinessFullPrevious >= PILVISTA_LOWER_LIMIT)
			  {
				sentence << SAA_JATKUU_PILVISENA_PHRASE;
			  }
			else
			  {
				sentence << cloudiness_sentence(fullAreaCloudinessId);
			  }
		  }
	  }
	else if(theParameters.theForecastArea & INLAND_AREA)
	  {
		meanCloudinessInland = getMeanCloudiness(thePeriod, *theInlandData);
		meanCloudinessInlandPrevious = getMeanCloudiness(previousPeriod, *theInlandData);
		/*
		theParameters.theLog << "CURRENT PERIOD: " << thePeriod.localStartTime() << "," <<  thePeriod.localEndTime() << endl;
		theParameters.theLog << "PREVIOUS PERIOD: " << previousPeriod.localStartTime() << "," <<  previousPeriod.localEndTime() << endl;

		theParameters.theLog << "CLOUDINESS CURRENT: " << meanCloudinessInland << endl;
		theParameters.theLog << "CLOUDINESS PREVIOUS: " << meanCloudinessInlandPrevious << endl;
		*/
		if(todaySentence.size() > 0 &&
		   meanCloudinessInland >= PILVISTA_LOWER_LIMIT &&
		   meanCloudinessInlandPrevious >= PILVISTA_LOWER_LIMIT)
		  {
			sentence << SAA_JATKUU_PILVISENA_PHRASE;
		  }
		else
		  {
			sentence << cloudiness_sentence(inlandCloudinessId);
		  }
	  }
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		meanCloudinessCoastal = getMeanCloudiness(thePeriod, *theCoastalData);
		meanCloudinessCoastalPrevious = getMeanCloudiness(previousPeriod, *theCoastalData);
		if(todaySentence.size() > 0 &&
		   meanCloudinessCoastal >= PILVISTA_LOWER_LIMIT &&
		   meanCloudinessCoastalPrevious >= PILVISTA_LOWER_LIMIT)
		  {
			sentence << SAA_JATKUU_PILVISENA_PHRASE;
		  }
		else
		  {
			sentence << cloudiness_sentence(coastalCloudinessId);
		  }
	  }

	if(sentence.size() > 0)
	  {
		sentence << PeriodPhraseFactory::create("today",
												theParameters.theVariable,
												theParameters.theForecastTime,
												thePeriod);
	  }

	return sentence;
  }

  Sentence CloudinessForecast::cloudinessSentence(const unsigned int& thePeriodNumber,
												  const bool& theCheckCloudinessChange /*= true*/)
  {
	Sentence sentence;

 	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	WeatherPeriod thePeriod(generator.period(thePeriodNumber));

	if(generator.size() < thePeriodNumber)
	   return sentence;

	if(theCheckCloudinessChange)
	  {
		Sentence cloudinessChange;
		cloudinessChange << cloudinessChangeSentence(thePeriod);
		if(cloudinessChange.size() > 0)
		  return cloudinessChange;
	  }

	if(generator.isday(thePeriodNumber))
	  {
		WeatherPeriod morningWeatherPeriod(thePeriod.localStartTime(),
										   thePeriod.localEndTime());
		WeatherPeriod afternoonWeatherPeriod(thePeriod.localStartTime(),
										   thePeriod.localEndTime());
		get_part_of_the_day(thePeriod, AAMUPAIVA, morningWeatherPeriod);
		get_part_of_the_day(thePeriod, ILTAPAIVA, afternoonWeatherPeriod);

		bool inlandAndCoastalIncluded = (theParameters.theForecastArea & INLAND_AREA && 
										 theParameters.theForecastArea & COASTAL_AREA);
		bool inlandIncluded = theParameters.theForecastArea & INLAND_AREA ;
		bool coastalIncluded = theParameters.theForecastArea & COASTAL_AREA;


		cloudiness_id cloudinessIdFullMorning(getCloudinessId(morningWeatherPeriod, theFullData));
		cloudiness_id cloudinessIdCoastalMorning(getCloudinessId(morningWeatherPeriod, theCoastalData));
		cloudiness_id cloudinessIdInlandMorning(getCloudinessId(morningWeatherPeriod, theInlandData));
		cloudiness_id cloudinessIdFullAfternoon(getCloudinessId(afternoonWeatherPeriod, theFullData));
		cloudiness_id cloudinessIdCoastalAfternoon(getCloudinessId(afternoonWeatherPeriod, theCoastalData));
		cloudiness_id cloudinessIdInlandAfternoon(getCloudinessId(afternoonWeatherPeriod, theInlandData));

		if(inlandAndCoastalIncluded)
		  {
			bool separateCoastInlandMorning = separateCoastInlandCloudiness(morningWeatherPeriod);
			bool separateCoastInlandAfternoon = separateCoastInlandCloudiness(afternoonWeatherPeriod);
			bool separateMorningAfternoon = separateWeatherPeriodCloudiness(morningWeatherPeriod, 
																			afternoonWeatherPeriod,
																			*theFullData);
			if(!separateCoastInlandMorning && !separateCoastInlandAfternoon && !separateMorningAfternoon)
			  {
				sentence << cloudiness_sentence(cloudinessIdFullMorning);
			  }
			else
			  {
				if(separateCoastInlandMorning)
				  {
					sentence << AAMUPAIVALLA_WORD << COAST_PHRASE
							 << cloudiness_sentence(cloudinessIdCoastalMorning);
					sentence << Delimiter(",");
					sentence << INLAND_PHRASE << cloudiness_sentence(cloudinessIdInlandMorning, true);
				  }
				else
				  {
					sentence << AAMUPAIVALLA_WORD << cloudiness_sentence(cloudinessIdFullMorning);
				  }
				sentence << Delimiter(",");
				if(separateCoastInlandAfternoon)
				  {
					sentence << ILTAPAIVALLA_WORD << COAST_PHRASE
							 << cloudiness_sentence(cloudinessIdCoastalAfternoon);
					sentence << Delimiter(",");
					sentence << INLAND_PHRASE << cloudiness_sentence(cloudinessIdInlandAfternoon, true);
				  }
				else
				  {
					sentence << ILTAPAIVALLA_WORD << cloudiness_sentence(cloudinessIdFullAfternoon);
				  }
			  }
		  }
		else if(inlandIncluded)
		  {
			if(puolipilvisesta_pilviseen(cloudinessIdInlandMorning,
										 cloudinessIdInlandAfternoon))
			  {
				sentence << SAA_VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE;
			  }
			else if(cloudinessIdInlandMorning != MISSING_CLOUDINESS_ID &&
					cloudinessIdInlandMorning != PUOLIPILVISTA_JA_PILVISTA && 
					cloudinessIdInlandAfternoon != PUOLIPILVISTA_JA_PILVISTA &&
					abs(cloudinessIdInlandMorning - cloudinessIdInlandAfternoon) < 2)
			  {
				sentence << cloudiness_sentence(cloudinessIdInlandMorning);
			  }
			else
			  {
				if(cloudinessIdInlandMorning != MISSING_CLOUDINESS_ID)
				  {
					sentence << AAMUPAIVALLA_WORD << cloudiness_sentence(cloudinessIdInlandMorning);
					sentence << Delimiter(",");
					sentence << ILTAPAIVALLA_WORD << cloudiness_sentence(cloudinessIdInlandAfternoon, true);
				  }
				else
				  {
					sentence << cloudiness_sentence(cloudinessIdInlandAfternoon);
				  }
			  }
		  }
		else if(coastalIncluded)
		  {
			if(puolipilvisesta_pilviseen(cloudinessIdCoastalMorning,
										 cloudinessIdCoastalAfternoon))
			  {
				sentence << SAA_VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE;
			  }
			else if(cloudinessIdCoastalMorning != MISSING_CLOUDINESS_ID &&
					cloudinessIdCoastalMorning != PUOLIPILVISTA_JA_PILVISTA && 
					cloudinessIdCoastalAfternoon != PUOLIPILVISTA_JA_PILVISTA &&
					abs(cloudinessIdCoastalMorning - cloudinessIdCoastalAfternoon) < 2)
			  {
				sentence << cloudiness_sentence(cloudinessIdCoastalMorning);
			  }
			else
			  {
				if(cloudinessIdCoastalMorning != MISSING_CLOUDINESS_ID)
				  {
					sentence << AAMUPAIVALLA_WORD << cloudiness_sentence(cloudinessIdCoastalMorning);
					sentence << Delimiter(",");
					sentence << ILTAPAIVALLA_WORD << cloudiness_sentence(cloudinessIdCoastalAfternoon, true);
				  }
				else
				  {
					sentence << cloudiness_sentence(cloudinessIdCoastalAfternoon);
				  }
			  }
		  }
	  }

	return sentence;
  }

  cloudiness_id CloudinessForecast::getCloudinessPeriodId(const NFmiTime& theObservationTime,
													const cloudiness_period_vector& theCloudinessPeriodVector) const
  {
	for(unsigned int i = 0; i < theCloudinessPeriodVector.size(); i++)
	  {
		if(theObservationTime >= theCloudinessPeriodVector.at(i).first.localStartTime() &&
		   theObservationTime <= theCloudinessPeriodVector.at(i).first.localEndTime())
		  return theCloudinessPeriodVector.at(i).second;
	  }

	return MISSING_CLOUDINESS_ID;
  }

  cloudiness_id CloudinessForecast::getCloudinessId(const WeatherPeriod& thePeriod,
													const weather_result_data_item_vector* theCloudinessData) const
  {
	if(!theCloudinessData)
	  return MISSING_CLOUDINESS_ID;

	weather_result_data_item_vector thePeriodCloudiness;
	
	get_sub_time_series(thePeriod,
						*theCloudinessData,
						thePeriodCloudiness);

	float min, max, mean, stddev;
	get_min_max(thePeriodCloudiness, min, max);
	mean = get_mean(thePeriodCloudiness);
	stddev = get_standard_deviation(thePeriodCloudiness);
	cloudiness_id clid = get_cloudiness_id(min, mean, max, stddev);

	return clid;
  }

  Sentence CloudinessForecast::cloudinessSentence(const WeatherPeriod& thePeriod,
												  const weather_result_data_item_vector& theCloudinessData) const
  {
	Sentence sentence;

	cloudiness_id clid(getCloudinessId(thePeriod, &theCloudinessData));

	sentence << cloudiness_string(clid);

	return sentence;
  }

}
