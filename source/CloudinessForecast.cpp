// ======================================================================
/*!
 * \file
 * \brief Implementation of CloudinessForecast class
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

	string weatherEventIdStr = weather_event_string(theCloudinessDataItemData.theWeatherEventId);

	theOutput << "    " << cloudinessIdStr << ": ";
	theOutput << "min=" << theCloudinessDataItemData.theMin << " ";
	theOutput << "mean=" << theCloudinessDataItemData.theMean << " ";
	theOutput << "max=" << theCloudinessDataItemData.theMax << " ";
	theOutput << "std.dev=" << theCloudinessDataItemData.theStandardDeviation << endl;
	theOutput << "    weather event: " << weatherEventIdStr << endl;
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
	   theCloudinessId1 == PUOLIPILVINEN_JA_PILVINEN &&
	   theCloudinessId2 == PUOLIPILVINEN_JA_PILVINEN)
	  return true;

	return false;
  }

  Sentence cloudiness_sentence(const cloudiness_id& theCloudinessId, 
							   const bool& theShortForm) 
  {
	Sentence sentence;
	Sentence cloudinessSentence;

	switch(theCloudinessId)
	  {
	  case PUOLIPILVINEN_JA_PILVINEN:
		cloudinessSentence << VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE;
		break;
	  case SELKEAA:
		cloudinessSentence << SELKEAA_WORD;
		break;
	  case MELKO_SELKEAA:
		cloudinessSentence << MELKO_SELKEAA_PHRASE;
		break;
	  case PUOLIPILVINEN:
		cloudinessSentence << PUOLIPILVINEN_WORD;
		break;
	  case VERRATTAIN_PILVINEN:
		cloudinessSentence << VERRATTAIN_PILVINEN_PHRASE;
		break;
	  case PILVINEN:
		cloudinessSentence << PILVINEN_WORD;
		break;
	  default:
		break;
	  }

	if(cloudinessSentence.size() > 0)
	  {
		if(theCloudinessId == PUOLIPILVINEN_JA_PILVINEN)
		  {
			if(!theShortForm)
			  sentence << SAA_WORD;
		  }
		else
		  {
			if(!theShortForm)
			  sentence << SAA_WORD << ON_WORD;
		  }
	  }

	sentence << cloudinessSentence;

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

	if(theMin >= PUOLIPILVISTA_LOWER_LIMIT &&
	   theMin <= PUOLIPILVISTA_UPPER_LIMIT && 
	   theMax >= PILVISTA_LOWER_LIMIT)
	  {
		id = PUOLIPILVINEN_JA_PILVINEN;
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
		id = PUOLIPILVINEN;
	  }
	else if(theMean <= VERRATTAIN_PILVISTA_UPPER_LIMIT)
	  {
		  id = VERRATTAIN_PILVINEN;
	  }
	else
	  {
		id = PILVINEN;
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
		id = PUOLIPILVINEN;
	  }
	else if(theCloudiness <= VERRATTAIN_PILVISTA_UPPER_LIMIT)
	  {
		id = VERRATTAIN_PILVINEN;
	  }
	else
	  {
		id = PILVINEN;
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
	  case PUOLIPILVINEN:
		retval = PUOLIPILVINEN_WORD;
	  break;
	  case VERRATTAIN_PILVINEN:
		retval = VERRATTAIN_PILVINEN_PHRASE;
	  break;
	  case PILVINEN:
		retval = PILVINEN_WORD;
	  break;
	  case PUOLIPILVINEN_JA_PILVINEN:
		retval = VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE;
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
	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  theFullData = ((*theParameters.theCompleteData[FULL_AREA])[CLOUDINESS_DATA]);
	if(theParameters.theForecastArea & COASTAL_AREA)
	  theCoastalData = ((*theParameters.theCompleteData[COASTAL_AREA])[CLOUDINESS_DATA]);
	if(theParameters.theForecastArea & INLAND_AREA)
	  theInlandData = ((*theParameters.theCompleteData[INLAND_AREA])[CLOUDINESS_DATA]);

	findOutCloudinessPeriods(); 
	joinPeriods();
	findOutCloudinessWeatherEvents();
  }

  Sentence CloudinessForecast::cloudinessChangeSentence(const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;

	const weather_event_id_vector& cloudinessWeatherEvents = 
	  ((theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA) ?
	  theCloudinessWeatherEventsFull : 
	  ((theParameters.theForecastArea & INLAND_AREA) ? theCloudinessWeatherEventsInland :
	   theCloudinessWeatherEventsCoastal));

	for(unsigned int i = 0; i < cloudinessWeatherEvents.size(); i++)
	  {
		NFmiTime weatherEventTimestamp(cloudinessWeatherEvents.at(i).first);

		if(!(weatherEventTimestamp >= thePeriod.localStartTime() &&
			 weatherEventTimestamp <= thePeriod.localEndTime()))
		  {
			continue;
		  }

		weather_event_id trid(cloudinessWeatherEvents.at(i).second);

		if(trid == PILVISTYY)
		  {
			sentence << PILVISTYVAA_WORD;
		  }
		else
		  {
			sentence << SELKENEVAA_WORD;
		  }

		sentence << get_time_phrase(weatherEventTimestamp, true);
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

	get_sub_time_series(theWeatherPeriod1,
						theCloudinessData,
						theInterestingDataPeriod1);

	cloudiness_id cloudinessId1 = get_cloudiness_id(get_mean(theInterestingDataPeriod1));
	
	get_sub_time_series(theWeatherPeriod2,
						theCloudinessData,
						theInterestingDataPeriod2);
	
	cloudiness_id cloudinessId2 = get_cloudiness_id(get_mean(theInterestingDataPeriod2));

	return (abs(cloudinessId1 - cloudinessId2) >= 2);

	/*													
	bool retval  = (mean_period1 <= 5.0 && mean_period2 >= 70.0 ||
					mean_period1 >= 70.0 && mean_period2 <= 5.0);
	
	return retval;
	*/
  }  
  
  void CloudinessForecast::findOutCloudinessWeatherEvents(const weather_result_data_item_vector* theData,
												   weather_event_id_vector& theCloudinessWeatherEvents)
  {
	for(unsigned int i = 3; i < theData->size() - 3; i++)
	  {
		WeatherPeriod firstHalfPeriod(theData->at(0)->thePeriod.localStartTime(),
									  theData->at(i)->thePeriod.localEndTime());
		WeatherPeriod secondHalfPeriod(theData->at(i+1)->thePeriod.localStartTime(),
									   theData->at(theData->size()-1)->thePeriod.localEndTime());

		weather_result_data_item_vector theFirstHalfData;
		weather_result_data_item_vector theSecondHalfData;
		
		get_sub_time_series(firstHalfPeriod,
							*theData,
							theFirstHalfData);
		
		get_sub_time_series(secondHalfPeriod,
							*theData,
							theSecondHalfData);

		float meanCloudinessInTheFirstHalf = get_mean(theFirstHalfData);
		float meanCloudinessInTheSecondHalf = get_mean(theSecondHalfData);
		weather_event_id weatherEventId(MISSING_WEATHER_EVENT);
		unsigned int changeIndex(0);
		
		if(meanCloudinessInTheFirstHalf >= PILVISTYVAA_UPPER_LIMIT && 
		   meanCloudinessInTheSecondHalf <= PILVISTYVAA_LOWER_LIMIT)
		  {
			bool selkeneeReally = true;
			// check that cloudiness stays under the limit for the rest of the forecast period
			for(unsigned int k = i+1; k < theData->size(); k++)
			  if(theData->at(k)->theResult.value() > PILVISTYVAA_LOWER_LIMIT)
				{
				  selkeneeReally = false;
				  break;
				}
			if(!selkeneeReally)
			  continue;

			changeIndex = i+1;
			while(changeIndex-1 >= 0 &&
				  theData->at(changeIndex-1)->theResult.value() <= PILVISTYVAA_LOWER_LIMIT)
			  changeIndex--;

			weatherEventId = SELKENEE;
		  }
		else if(meanCloudinessInTheFirstHalf <= PILVISTYVAA_LOWER_LIMIT && 
				meanCloudinessInTheSecondHalf >= PILVISTYVAA_UPPER_LIMIT)
		  {
			bool pilvistyyReally = true;
			// check that cloudiness stays above the limit for the rest of the forecast period
			for(unsigned int k = i+1; k < theData->size(); k++)
			  if(theData->at(k)->theResult.value() < PILVISTYVAA_UPPER_LIMIT)
				{
				  pilvistyyReally = false;
				  break;
				}
			if(!pilvistyyReally)
			  continue;

			changeIndex = i+1;
			while(changeIndex-1 >= 0 &&
				  theData->at(changeIndex-1)->theResult.value() >= PILVISTYVAA_UPPER_LIMIT)
			  changeIndex--;

			weatherEventId = PILVISTYY;
		  }

		if(weatherEventId != MISSING_WEATHER_EVENT)
		  {
			theCloudinessWeatherEvents.push_back(make_pair(theData->at(changeIndex)->thePeriod.localStartTime(), weatherEventId));
			// note: only one event during the period (original plan was that several events are allowed)
			break;
		  }
	  }

#ifdef LATER
	// check 6-hour periods if there is weatherEvent
	const unsigned int periodLength = 12;
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
		float pearson_coefficient = get_pearson_coefficient(theWholePeriodData, 0, theWholePeriodData.size()-1);
		float meanCloudinessInTheFirstHalf = get_mean(theFirstHalfData);
		float meanCloudinessInTheSecondHalf = get_mean(theSecondHalfData);
		weather_event_id weatherEventId = MISSING_WEATHER_EVENT;

		if(meanCloudinessInTheFirstHalf >= PILVISTYVAA_UPPER_LIMIT && 
		   meanCloudinessInTheSecondHalf <= PILVISTYVAA_LOWER_LIMIT &&
		   pearson_coefficient < -0.65)
		  {
			weatherEventId = SELKENEE;
		  }
		else if(meanCloudinessInTheFirstHalf <= PILVISTYVAA_LOWER_LIMIT && 
				meanCloudinessInTheSecondHalf >= PILVISTYVAA_UPPER_LIMIT && 
				pearson_coefficient >= 0.65)
		  {
			weatherEventId = PILVISTYY;
		  }

		if(weatherEventId != MISSING_WEATHER_EVENT)
		  {
			theCloudinessWeatherEvents.push_back(make_pair(secondHalfPeriod.localStartTime(), weatherEventId));			
		  }

		startIndex += (weatherEventId == MISSING_WEATHER_EVENT ? 1 : (periodLength/2));
	  }
#endif
 }

  void CloudinessForecast::findOutCloudinessWeatherEvents()
  {
	if(theCoastalData)
	  findOutCloudinessWeatherEvents(theCoastalData, theCloudinessWeatherEventsCoastal);
	if(theInlandData)
	  findOutCloudinessWeatherEvents(theInlandData, theCloudinessWeatherEventsInland);
	if(theFullData)
	  findOutCloudinessWeatherEvents(theFullData, theCloudinessWeatherEventsFull);
  }

  void CloudinessForecast::findOutCloudinessPeriods(const weather_result_data_item_vector* theData, 
													cloudiness_period_vector& theCloudinessPeriods)
  {
	if(theData)
	  {
		NFmiTime previousStartTime;
		NFmiTime previousEndTime;
		cloudiness_id previousCloudinessId(MISSING_CLOUDINESS_ID);
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
		if(get_cloudiness_id(theData->at(i)->theResult.value()) >= PUOLIPILVINEN && 
		   get_cloudiness_id(theData->at(i)->theResult.value()) <= PILVINEN &&
		   i != theData->size() - 1)
		  {
			if(index == -1)
			  index = i;
		  }
		else if(get_cloudiness_id(theData->at(i)->theResult.value()) == SELKEAA || 
				get_cloudiness_id(theData->at(i)->theResult.value()) == MELKO_SELKEAA ||
				get_cloudiness_id(theData->at(i)->theResult.value()) == PILVINEN ||
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
			if(clidPrevious == PUOLIPILVINEN && clidCurrent == PILVINEN || 
			   clidPrevious == PILVINEN && clidCurrent == PUOLIPILVINEN)
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
  
  void CloudinessForecast::printOutCloudinessWeatherEvents(std::ostream& theOutput) const
  {
	theOutput << "** CLOUDINESS WEATHER EVENTS **" << endl; 
	bool isWeatherEvents = false;
	if(theCloudinessWeatherEventsCoastal.size() > 0)
	  {
		theOutput << "Coastal weather events: " << endl; 
		print_out_weather_event_vector(theOutput, theCloudinessWeatherEventsCoastal);
		isWeatherEvents = true;
	  }
	if(theCloudinessWeatherEventsInland.size() > 0)
	  {
		theOutput << "Inland weather events: " << endl; 
		print_out_weather_event_vector(theOutput, theCloudinessWeatherEventsInland);
		isWeatherEvents = true;
	  }
	if(theCloudinessWeatherEventsFull.size() > 0)
	  {
		theOutput << "Full area weather events: " << endl; 
		print_out_weather_event_vector(theOutput, theCloudinessWeatherEventsFull);
		isWeatherEvents = true;
	  }

	if(!isWeatherEvents)
		theOutput << "No weather events!" << endl; 
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
												  const bool& theShortForm,
												  const bool& theUseTimeSpecifier) const
  {
	Sentence sentence;

	Sentence cloudinessSentence;

	cloudiness_id coastalCloudinessId = getCloudinessId(thePeriod, theCoastalData);
	cloudiness_id inlandCloudinessId = getCloudinessId(thePeriod, theInlandData);
	cloudiness_id fullAreaCloudinessId = getCloudinessId(thePeriod, theFullData);

	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  {
		if(separateCoastInlandCloudiness(thePeriod))
		  {
			cloudinessSentence << COAST_PHRASE;
			cloudinessSentence << cloudiness_sentence(coastalCloudinessId, theShortForm);
			cloudinessSentence << Delimiter(",");
			cloudinessSentence << INLAND_PHRASE;
			cloudinessSentence << cloudiness_sentence(inlandCloudinessId, theShortForm);
		  }
		else
		  {
			cloudinessSentence << cloudiness_sentence(fullAreaCloudinessId, theShortForm);
		  }
	  }
	else if(theParameters.theForecastArea & INLAND_AREA)
	  {
		cloudinessSentence << cloudiness_sentence(inlandCloudinessId, theShortForm);
	  }
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		cloudinessSentence << cloudiness_sentence(coastalCloudinessId, theShortForm);		
	  }

	if(cloudinessSentence.size() > 0)
	  {
		sentence << cloudinessSentence;
	  }
	
	if(sentence.size() > 0 && !(theParameters.theForecastArea & FULL_AREA))
	  {
		cloudiness_id clid = (theParameters.theForecastArea & INLAND_AREA ? inlandCloudinessId : coastalCloudinessId);
		if(clid == VERRATTAIN_PILVINEN || clid == PILVINEN)
		  sentence << areaSpecificSentence(thePeriod);
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

  cloudiness_id CloudinessForecast::getCloudinessId(const WeatherPeriod& thePeriod) const
  {
	const weather_result_data_item_vector* theCloudinessDataVector = 0;

	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  theCloudinessDataVector = theFullData;
	else if(theParameters.theForecastArea & INLAND_AREA)
	  theCloudinessDataVector = theInlandData;
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  theCloudinessDataVector = theCoastalData;

	return getCloudinessId(thePeriod, theCloudinessDataVector);
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

  void CloudinessForecast::getWeatherEventIdVector(weather_event_id_vector& theCloudinessWeatherEvents) const
  {
	const weather_event_id_vector* vectorToClone = 0;

	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  vectorToClone = &theCloudinessWeatherEventsFull;
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  vectorToClone = &theCloudinessWeatherEventsCoastal;
	else if(theParameters.theForecastArea & INLAND_AREA)
	  vectorToClone = &theCloudinessWeatherEventsInland;

	if(vectorToClone)
	  theCloudinessWeatherEvents = *vectorToClone;
  }

  Sentence CloudinessForecast::areaSpecificSentence(const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;

	// TODO: tsekkaa, että aluuen toisella puolella on selkeää
	/*
	WeatherResult northEastShare(kFloatMissing, 0);
	WeatherResult southEastShare(kFloatMissing, 0);
	WeatherResult southWestShare(kFloatMissing, 0);
	WeatherResult northWestShare(kFloatMissing, 0);

	RangeAcceptor cloudinesslimits;
	cloudinesslimits.lowerLimit(VERRATTAIN_PILVISTA_LOWER_LIMIT);
	AreaTools::getArealDistribution(theParameters.theSources,
									Cloudiness,
									theParameters.theArea,
									thePeriod,
									cloudinesslimits,
									northEastShare,
									southEastShare,
									southWestShare,
									northWestShare);

	float north = northEastShare.value() + northWestShare.value();
	float south = southEastShare.value() + southWestShare.value();
	float east = northEastShare.value() + southEastShare.value();
	float west = northWestShare.value() + southWestShare.value();

	sentence << area_specific_sentence(north,
									   south,
									   east,
									   west,
									   northEastShare.value(),
									   southEastShare.value(),
									   southWestShare.value(),
									   northWestShare.value(),
									   false);

	*/
	return sentence;
  }

}
