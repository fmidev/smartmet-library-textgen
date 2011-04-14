#include <iostream>
#include <string>

#include "WeatherPeriod.h"
#include "WeatherArea.h"
#include "MessageLogger.h"
#include "Sentence.h"
#include "Paragraph.h"
#include "Delimiter.h"
#include "Settings.h"
#include "PeriodPhraseFactory.h"

#include "PrecipitationForecast.h"
#include "CloudinessForecast.h"
#include "FogForecast.h"
#include "ThunderForecast.h"
#include "WeatherForecastStory.h"

#define USE_FROM_SPECIFIER true
#define DONT_USE_FROM_SPECIFIER false
#define USE_SHORT_FORM true
#define DONT_USE_SHORT_FORM false
#define USE_TIME_SPECIFIER true
#define DONT_USE_TIME_SPECIFIER false


namespace TextGen
{
  using namespace boost;
  using namespace std;
  using namespace WeatherAnalysis;
  using namespace Settings;

  WeatherForecastStory::WeatherForecastStory(const std::string& var,
											 const WeatherPeriod& forecastPeriod,
											 const WeatherArea& weatherArea,
											 const unsigned short& forecastArea,
											 const NFmiTime& theForecastTime,
											 PrecipitationForecast& precipitationForecast,
											 const CloudinessForecast& cloudinessForecast,
											 const FogForecast& fogForecast,
											 const ThunderForecast& thunderForecast,
											 MessageLogger& logger)
	: theVar(var),
	  theForecastPeriod(forecastPeriod),
	  theWeatherArea(weatherArea),
	  theForecastArea(forecastArea),
	  theForecastTime(theForecastTime),
	  thePrecipitationForecast(precipitationForecast),
	  theCloudinessForecast(cloudinessForecast),
	  theFogForecast(fogForecast),
	  theThunderForecast(thunderForecast),
	  theLogger(logger),
	  theStorySize(0),
	  theShortTimePrecipitationReportedFlag(false),
	  theReportTimePhraseFlag(false),
	  theCloudinessReportedFlag(false)
  {
	addPrecipitationStoryItems();
	addCloudinessStoryItems();
	mergePeriodsWhenFeasible();

	bool specifyPartOfTheDayFlag = Settings::optional_bool(theVar + "::specify_part_of_the_day", true);
	if(!specifyPartOfTheDayFlag)
	  {
		int counter(0);
		for(unsigned int i = 0; i < theStoryItemVector.size(); i++)
		  {
			// ARE 14.4.2011: checking theIncludeInTheStoryFlag
			if(theStoryItemVector[i]->theIncludeInTheStoryFlag == true &&
			   theStoryItemVector[i]->getSentence().size() > 0)
			  counter++;
		  }
		theReportTimePhraseFlag = counter > 1;
	  }
  }
	  
  WeatherForecastStory::~WeatherForecastStory()
  {
	theStoryItemVector.clear();
  }


  Paragraph WeatherForecastStory::getWeatherForecastStory()
  {
	Paragraph paragraph;

	theShortTimePrecipitationReportedFlag = false;
	theCloudinessReportedFlag = false;
	theStorySize = 0;

	thePrecipitationForecast.dryPeriodTautologyFlag(false);

	for(unsigned int i = 0; i < theStoryItemVector.size(); i++)
	  {
		Sentence storyItemSentence;
		storyItemSentence << theStoryItemVector[i]->getSentence();
		if(storyItemSentence.size() > 0)
		  {
			theStorySize += storyItemSentence.size();

			paragraph << storyItemSentence;
			// possible fog sentence after 
			if( theStoryItemVector[i]->theStoryPartId == CLOUDINESS_STORY_PART)
			  {
				paragraph << theFogForecast.fogSentence(theStoryItemVector[i]->thePeriod);
			  }
		  }
		// reset the today phrase when day changes
		if(i > 0 && theStoryItemVector[i-1]->thePeriod.localEndTime().GetJulianDay() !=
		   theStoryItemVector[i]->thePeriod.localEndTime().GetJulianDay() &&
		   get_period_length(theForecastPeriod) > 24)
		  const_cast<WeatherHistory&>(theWeatherArea.history()).updateTimePhrase("", NFmiTime(1970,1,1));
	  }

	return paragraph;
  }

  void WeatherForecastStory::addPrecipitationStoryItems() 
  {
	vector<WeatherPeriod> precipitationPeriods;

	thePrecipitationForecast.getPrecipitationPeriods(theForecastPeriod, precipitationPeriods);

	PrecipitationForecastStoryItem* previousPrItem = 0;
	WeatherForecastStoryItem* missingStoryItem = 0;
	for(unsigned int i = 0; i < precipitationPeriods.size(); i++)
	  {
		float intensity(thePrecipitationForecast.getMeanIntensity(precipitationPeriods[i], 
																  theForecastArea));
		float extent(thePrecipitationForecast.getPrecipitationExtent(precipitationPeriods[i], 
																	 theForecastArea));
		unsigned int form(thePrecipitationForecast.getPrecipitationForm(precipitationPeriods[i], 
																		theForecastArea));
		precipitation_type type(thePrecipitationForecast.getPrecipitationType(precipitationPeriods[i], 
																			  theForecastArea));
		if(get_period_length(precipitationPeriods[i]) <= 1 && extent < 10)
		  continue;

		PrecipitationForecastStoryItem* item = new PrecipitationForecastStoryItem(*this,
																				  precipitationPeriods[i],
																				  PRECIPITATION_STORY_PART,
																				  intensity,
																				  extent,
																				  form, 
																				  type);


		NFmiTime startTimeFull;
		NFmiTime endTimeFull;
		thePrecipitationForecast.getPrecipitationPeriod(precipitationPeriods[i].localStartTime(), 
														startTimeFull, endTimeFull);
		// This parameter shows if the precipitation starts 
		// before forecast period or continues after
		int precipitationFullDuration = endTimeFull.DifferenceInHours(startTimeFull);
		item->theFullDuration = precipitationFullDuration;
		if(precipitationFullDuration >= 6)
		  {
			item->theSentence << thePrecipitationForecast.precipitationSentence(precipitationPeriods[i]);
		  }
		else
		  {
			item->theSentence << thePrecipitationForecast.shortTermPrecipitationSentence(precipitationPeriods[i]);
		  }
		if(previousPrItem != 0)
		  {
			NFmiTime startTime(previousPrItem->thePeriod.localEndTime());
			NFmiTime endTime(precipitationPeriods[i].localStartTime());
			if(endTime.DifferenceInHours(startTime) > 1)
			  {
				startTime.ChangeByHours(1);
				endTime.ChangeByHours(-1);

				// placeholder for some other story item between precipitation periods
				missingStoryItem = new WeatherForecastStoryItem(*this,
																WeatherPeriod(startTime, endTime),
																MISSING_STORY_PART);

				theStoryItemVector.push_back(missingStoryItem);
			  }
		  }
		// precipitation story item
		theStoryItemVector.push_back(item);
		  
		previousPrItem = item;
	  }


	if(theStoryItemVector.size() == 0)
	  {
		WeatherPeriod cloudinessPeriod(theForecastPeriod);
		CloudinessForecastStoryItem* item =
		  new CloudinessForecastStoryItem(*this,
										  cloudinessPeriod, 
										  CLOUDINESS_STORY_PART,
										  theCloudinessForecast.getCloudinessId(cloudinessPeriod),
										  0,
										  0);
			  
		item->theReportAboutDryWeatherFlag = true;
		  
		// cloudiness story item
		theStoryItemVector.push_back(item);
	  }
	else
	  {
		// check if first period is missing
		NFmiTime firstPeriodStartTime(theForecastPeriod.localStartTime());
		NFmiTime firstPeriodEndTime(theStoryItemVector[0]->thePeriod.localStartTime());

		WeatherPeriod firstPeriod(theForecastPeriod.localStartTime(),
								  theStoryItemVector[0]->thePeriod.localStartTime());

		if(firstPeriodEndTime.DifferenceInHours(firstPeriodStartTime) > 0)
		  {
			firstPeriodEndTime.ChangeByHours(-1);
			WeatherPeriod firstPeriod(firstPeriodStartTime, firstPeriodEndTime);
			missingStoryItem = new WeatherForecastStoryItem(*this,
															firstPeriod,
															MISSING_STORY_PART);
			theStoryItemVector.insert(theStoryItemVector.begin(),
									  missingStoryItem);
		  }

		NFmiTime lastPeriodStartTime(theStoryItemVector[theStoryItemVector.size()-1]->thePeriod.localEndTime());
		NFmiTime lastPeriodEndTime(theForecastPeriod.localEndTime());
		// chek if the last period is missing
		if(lastPeriodEndTime.DifferenceInHours(lastPeriodStartTime) > 0)
		  {
			lastPeriodStartTime.ChangeByHours(1);
			WeatherPeriod lastPeriod(lastPeriodStartTime, lastPeriodEndTime);

			missingStoryItem = new WeatherForecastStoryItem(*this,
															lastPeriod,
															MISSING_STORY_PART);
			theStoryItemVector.push_back(missingStoryItem);
		  }

		bool emptyPeriodsFound = true;
		while(emptyPeriodsFound)
		  {
			emptyPeriodsFound = false;
			// place placeholder in the missing slots
			for(unsigned int i = 1; i < theStoryItemVector.size(); i++)
			  {
				NFmiTime middlePeriodStartTime(theStoryItemVector[i-1]->thePeriod.localEndTime());
				NFmiTime middlePeriodEndTime(theStoryItemVector[i]->thePeriod.localStartTime());

				if(middlePeriodEndTime.DifferenceInHours(middlePeriodStartTime) > 1)
				  {
					WeatherPeriod middlePeriod(theStoryItemVector[i-1]->thePeriod.localEndTime(),
											   theStoryItemVector[i]->thePeriod.localStartTime());

					middlePeriodStartTime.ChangeByHours(1);
					middlePeriodEndTime.ChangeByHours(-1);
					missingStoryItem = new WeatherForecastStoryItem(*this,
																	WeatherPeriod(middlePeriodStartTime,
																				  middlePeriodEndTime),
																	MISSING_STORY_PART);
					theStoryItemVector.insert(theStoryItemVector.begin()+i, missingStoryItem);
					emptyPeriodsFound = true;
					break;
				  }
			  }
		  }
	  }
  }

  void WeatherForecastStory::addCloudinessStoryItems() 
  {	  
	// replace the missing story items with the cloudiness story part
	PrecipitationForecastStoryItem* previousPrecipitationStoryItem = 0;
	for(unsigned int i = 0; i < theStoryItemVector.size(); i++)
	  {
		if(theStoryItemVector[i]->theStoryPartId == MISSING_STORY_PART)
		  {
			WeatherForecastStoryItem* placeholder = theStoryItemVector[i];
		  
			CloudinessForecastStoryItem* cloudinessStoryItem =
			  new CloudinessForecastStoryItem(*this,
											  placeholder->thePeriod,
											  CLOUDINESS_STORY_PART,
											  theCloudinessForecast.getCloudinessId(placeholder->thePeriod),
											  previousPrecipitationStoryItem,
											  0);
			  
			if(!previousPrecipitationStoryItem)
			  {
				cloudinessStoryItem->theReportAboutDryWeatherFlag = true;
			  }

			cloudinessStoryItem->thePreviousPrecipitationStoryItem = previousPrecipitationStoryItem;

			if(i < theStoryItemVector.size() - 1 &&
			   theStoryItemVector[i+1]->theStoryPartId == PRECIPITATION_STORY_PART)
			  cloudinessStoryItem->theNextPrecipitationStoryItem = 
				static_cast<PrecipitationForecastStoryItem*>(theStoryItemVector[i+1]);
			  
			theStoryItemVector[i] = cloudinessStoryItem;		  
			  
			delete placeholder;
		  }
		else if(theStoryItemVector[i]->theStoryPartId == PRECIPITATION_STORY_PART)
		  {
			previousPrecipitationStoryItem = 	static_cast<PrecipitationForecastStoryItem*>(theStoryItemVector[i]);
		  }
		  
		// one hour period in the beginning is ignored
		if(i == 0)
		  {
			if(theStoryItemVector[i]->storyItemPeriodLength() <= 1 && theStoryItemVector.size() > 1)
			  {
				theStoryItemVector[i]->theIncludeInTheStoryFlag = false;
			  }
		  }
	  }

	// short cloudiness period in the end after precipitation period is not reported
	int storyItemCount = theStoryItemVector.size();
	if(storyItemCount > 1)
	  {
		if(theStoryItemVector[storyItemCount-1]->theStoryPartId == CLOUDINESS_STORY_PART &&
		   theStoryItemVector[storyItemCount-1]->storyItemPeriodLength() <= 1 &&
		   theStoryItemVector[storyItemCount-2]->theStoryPartId == PRECIPITATION_STORY_PART &&
		   theStoryItemVector[storyItemCount-2]->theIncludeInTheStoryFlag == true)
		  {
			theStoryItemVector[storyItemCount-1]->theIncludeInTheStoryFlag = false;
		  }
	  }
  }

  void WeatherForecastStory::mergePeriodsWhenFeasible()
  {
	PrecipitationForecastStoryItem* previousPrecipitationStoryItem = 0;
	PrecipitationForecastStoryItem* currentPrecipitationStoryItem = 0;
	for(unsigned int i = 0; i < theStoryItemVector.size(); i++)
	  {
		if(theStoryItemVector[i]->theStoryPartId == PRECIPITATION_STORY_PART)
		  {
			currentPrecipitationStoryItem = static_cast<PrecipitationForecastStoryItem*>(theStoryItemVector[i]);
			if(previousPrecipitationStoryItem)
			  {
				// if the type is different don't merge
				if(previousPrecipitationStoryItem->theType != currentPrecipitationStoryItem->theType)
				  {
					// if the period between precpitation periods is short don't mention it
					if(theStoryItemVector[i-1] != previousPrecipitationStoryItem && 
					   theStoryItemVector[i-1]->storyItemPeriodLength() <= 1)
					  {
						theStoryItemVector[i-1]->theIncludeInTheStoryFlag = false;
					  }
					continue;
				  }

				NFmiTime gapPeriodStartTime(previousPrecipitationStoryItem->thePeriod.localEndTime());
				gapPeriodStartTime.ChangeByHours(1);
				NFmiTime gapPeriodEndTime(currentPrecipitationStoryItem->thePeriod.localStartTime());
				gapPeriodEndTime.ChangeByHours(-1);
				WeatherPeriod gapPeriod(gapPeriodStartTime, gapPeriodEndTime);

				// merge periods
				if(get_period_length(gapPeriod) <= 3 &&
				   (get_period_length(gapPeriod) <= 
					currentPrecipitationStoryItem->storyItemPeriodLength() +
					previousPrecipitationStoryItem->storyItemPeriodLength() + 1))
				  {
					// merge two weak precipitation periods
					previousPrecipitationStoryItem->thePeriodToMergeWith = currentPrecipitationStoryItem;
					currentPrecipitationStoryItem->thePeriodToMergeTo = previousPrecipitationStoryItem;
				  }
			  }
			previousPrecipitationStoryItem = currentPrecipitationStoryItem;
		  }
	  }
  }


  Sentence WeatherForecastStory::getTimePhrase()
  {
	Sentence sentence;

	if(theReportTimePhraseFlag)
	  if(theStorySize == 0)
		sentence << ALUKSI_WORD;
	  else
		sentence << MYOHEMMIN_WORD;

	return sentence;
  }



  WeatherForecastStoryItem::WeatherForecastStoryItem(WeatherForecastStory& weatherForecastStory,
													 const WeatherPeriod& period, 
													 const story_part_id& storyPartId)
	: theWeatherForecastStory(weatherForecastStory),
	  thePeriod(period),
	  theStoryPartId(storyPartId),
	  theIncludeInTheStoryFlag(true)
  {
  }
	
	
  Sentence WeatherForecastStoryItem::getSentence()
  {
	Sentence sentence;

	if(theIncludeInTheStoryFlag)
	  {
		// aluksi - myöhemmin phrase
		if(theWeatherForecastStory.theReportTimePhraseFlag)
		  {
			sentence << theWeatherForecastStory.getTimePhrase();
		  }

		sentence << getStoryItemSentence();
	  }

	return sentence;
  }

  // if periods are merged this must be overwritten
  WeatherPeriod WeatherForecastStoryItem::getStoryItemPeriod() const 
  { 
	return thePeriod; 
  } 

  unsigned int WeatherForecastStoryItem::getPeriodLength() 
  { 
	return thePeriod.localEndTime().DifferenceInHours(thePeriod.localStartTime()); 
  }

  int WeatherForecastStoryItem::forecastPeriodLength() const 
  { 
	return get_period_length(theWeatherForecastStory.theForecastPeriod); 
  }

  int WeatherForecastStoryItem::storyItemPeriodLength() const 
  { 
	return get_period_length(getStoryItemPeriod()); 
  }


  Sentence WeatherForecastStoryItem::getTodayVectorSentence(const vector<Sentence*>& todayVector,
															const unsigned int& theBegIndex, 
															const unsigned int& theEndIndex)
  {
	Sentence sentence;

	for(unsigned int i = theBegIndex; i <= theEndIndex; i++)
	  {
		if(sentence.size() > 0)
		  sentence << JA_WORD;
		sentence << *(todayVector[i]);
	  }
	return sentence;
  }

  Sentence WeatherForecastStoryItem::checkForAamuyoAndAamuPhrase(const bool& theFromSpecifier,
																 const WeatherPeriod& thePhrasePeriod)
  {
	Sentence sentence;

	// 6:00 in the morning or in the evening
	if(is_inside(thePhrasePeriod.localStartTime(), AAMU) && 
	   is_inside(thePhrasePeriod.localEndTime(), AAMU) &&
	   is_inside(thePhrasePeriod.localStartTime(), AAMUYO) && 
	   is_inside(thePhrasePeriod.localEndTime(), AAMUYO))
	  {
		if(thePhrasePeriod.localStartTime() == theWeatherForecastStory.theForecastPeriod.localStartTime())
		  {
			sentence << (theFromSpecifier ? AAMUSTA_ALKAEN_PHRASE : AAMULLA_WORD);
		  }
		else
		  {
			sentence << (theFromSpecifier ? AAMUYOSTA_ALKAEN_PHRASE : AAMUYOLLA_WORD);
		  }
	  }

	return sentence;
  }

  Sentence WeatherForecastStoryItem::checkForExtendedPeriodPhrase(const WeatherPeriod& thePhrasePeriod)
  {
	Sentence sentence;

	sentence << get_time_phrase_large(thePhrasePeriod, theWeatherForecastStory.theVar);

	if(sentence.size() == 0)
	  {		
		NFmiTime startTime(thePhrasePeriod.localStartTime());
		NFmiTime endTime(thePhrasePeriod.localEndTime());
		startTime.ChangeByHours(1); // one hour forwards in the beginning

		sentence << get_time_phrase_large(WeatherPeriod(startTime, endTime), theWeatherForecastStory.theVar);

		if(sentence.size() == 0)
		  {
			endTime.ChangeByHours(-1); // one hour backwards in the end
			sentence << get_time_phrase_large(WeatherPeriod(startTime, endTime), theWeatherForecastStory.theVar);
		  }
	  }

	if(sentence.size() > 0)
	  {
		theWeatherForecastStory.theLogger << "Extended period phrase(";
		theWeatherForecastStory.theLogger << thePhrasePeriod.localStartTime() 
										  << "..." 
										  << thePhrasePeriod.localEndTime()
										  << "): ";
		theWeatherForecastStory.theLogger << sentence;
	  }

	return sentence;
  }


  Sentence WeatherForecastStoryItem::getPeriodPhrase(const bool& theFromSpecifier,
													 const WeatherPeriod* thePhrasePeriod /*= 0*/)
  {
	Sentence sentence;


	if(theWeatherForecastStory.theStorySize == 0)
	  return sentence;

	WeatherPeriod phrasePeriod(thePhrasePeriod == 0 ? getStoryItemPeriod() : *thePhrasePeriod);

	vector<Sentence*> todayVector;

	int julianDayOfTheFirstDay = get_today_vector(theWeatherForecastStory.theVar,
												  theWeatherForecastStory.theWeatherArea,
												  phrasePeriod,
												  theWeatherForecastStory.theForecastTime,
												  todayVector);


	bool firstTodayPeriodWritten = false;

	// TODOcheck this
	if(julianDayOfTheFirstDay == -1 && 
	   forecastPeriodLength() > 24 &&
	   theWeatherForecastStory.theForecastTime.GetJulianDay() != phrasePeriod.localStartTime().GetJulianDay())
	  {
		sentence <<  PeriodPhraseFactory::create("today",
												 theWeatherForecastStory.theVar,
												 theWeatherForecastStory.theForecastTime,
												 phrasePeriod,
												 theWeatherForecastStory.theWeatherArea);
	  }
	else
	  {
		if(todayVector.size() > 0 &&
		   (julianDayOfTheFirstDay == getStoryItemPeriod().localStartTime().GetJulianDay() || 
			theWeatherForecastStory.theForecastTime.GetJulianDay() != phrasePeriod.localStartTime().GetJulianDay())
		   && forecastPeriodLength() > 24)
		  {		
			sentence << *(todayVector[0]);
			firstTodayPeriodWritten = sentence.size() > 0;
		  }
	  }


	if(theFromSpecifier)
	  {
		Sentence timeSentence;

		timeSentence << checkForAamuyoAndAamuPhrase(theFromSpecifier,
													phrasePeriod);
		if(timeSentence.size() > 0)
		  {
			sentence << timeSentence;
		  }
		else
		  {
			// if the period fits into some day part (aamupäivä, yö,...) use that phrase
			Sentence extendedPeriodSentence;
			extendedPeriodSentence << checkForExtendedPeriodPhrase(phrasePeriod);
						
			if(extendedPeriodSentence.size() > 0)
			  sentence << extendedPeriodSentence;
			else
			  sentence << get_time_phrase(phrasePeriod.localStartTime(), theWeatherForecastStory.theVar, true);
		  }
		
		if(todayVector.size() > 0)
		  {
			if(forecastPeriodLength() > 24)
			  {
				Sentence todaySentence;
				todaySentence << getTodayVectorSentence(todayVector, firstTodayPeriodWritten ? 1 : 0, todayVector.size() - 1);
				if(todaySentence.size() > 0)
				  sentence << JA_WORD;
				sentence << todaySentence;
			  }
		  }
	  }
	else
	  {
		if(todayVector.size() > 0)
		  {
			if(forecastPeriodLength() > 24)
			  {
				Sentence todaySentence;
				todaySentence << getTodayVectorSentence(todayVector, firstTodayPeriodWritten ? 1 : 0, todayVector.size() - 1);
				if(todaySentence.size() > 0)
				  sentence << JA_WORD;
				sentence << todaySentence;
			  }
		  }

		Sentence timeSentence;
		timeSentence << checkForAamuyoAndAamuPhrase(theFromSpecifier,
													phrasePeriod);
		if(timeSentence.size() > 0)
		  {
			sentence << timeSentence;
		  }
		else
		  {
			// if the period fits into some day part (aamupäivä, yö,...) use that phrase
			sentence << checkForExtendedPeriodPhrase(phrasePeriod);
		  }
	  }

	todayVector.clear();

	return sentence;
  }




  PrecipitationForecastStoryItem::PrecipitationForecastStoryItem(WeatherForecastStory& weatherForecastStory,
																 const WeatherPeriod& period, 
																 const story_part_id& storyPartId,
																 const float& intensity,
																 const float& extent,
																 unsigned int& form,
																 precipitation_type& type)
	: WeatherForecastStoryItem(weatherForecastStory, period, storyPartId),
	  theIntensity(intensity),
	  theExtent(extent),
	  theForm(form),
	  theType(type),
	  theSadeJatkuuFlag(false),
	  thePoutaantuuFlag(intensity > WEAK_PRECIPITATION_LIMIT_WATER),
	  theReportPoutaantuuFlag(intensity > WEAK_PRECIPITATION_LIMIT_WATER),
	  theFullDuration(period.localEndTime().DifferenceInHours(period.localStartTime())),
	  thePeriodToMergeWith(0),
	  thePeriodToMergeTo(0)
  {
  }

  bool PrecipitationForecastStoryItem::weakPrecipitation() const 
  { 
	return theIntensity <= WEAK_PRECIPITATION_LIMIT_WATER; 
  }

  WeatherPeriod PrecipitationForecastStoryItem::getStoryItemPeriod() const 
  {
	if(thePeriodToMergeWith)
	  {
		WeatherPeriod period(thePeriod.localStartTime(),
							 thePeriodToMergeWith->getStoryItemPeriod().localEndTime());
		return period;
	  }
	else
	  {
		return thePeriod;
	  }
  } 

	
  Sentence PrecipitationForecastStoryItem::getStoryItemSentence()
  { 
	Sentence sentence;

	// the thePeriodToMergeWith handles the whole stuff
	if(thePeriodToMergeTo)
	  return sentence;

	PrecipitationForecast& prForecast = theWeatherForecastStory.thePrecipitationForecast;
	WeatherPeriod forecastPeriod = theWeatherForecastStory.theForecastPeriod;
	WeatherPeriod storyItemPeriod(getStoryItemPeriod());

	if(storyItemPeriodLength() >= 6)
	  {
		if(storyItemPeriod.localStartTime() != forecastPeriod.localStartTime())
		  {
			if(storyItemPeriod.localStartTime() > forecastPeriod.localStartTime())
			  sentence << getPeriodPhrase(USE_FROM_SPECIFIER);
			sentence << prForecast.precipitationChangeSentence(thePeriod, SADE_ALKAA);
		  }
		else
		  {
			if(storyItemPeriod.localStartTime() > forecastPeriod.localStartTime())
			  sentence << getPeriodPhrase(DONT_USE_FROM_SPECIFIER);
			sentence << prForecast.precipitationSentence(thePeriod);
		  }
		  
		if(storyItemPeriod.localEndTime() != forecastPeriod.localEndTime() && 
		   theReportPoutaantuuFlag)
		  {
			WeatherPeriod poutaantuuPeriod(storyItemPeriod.localEndTime(), storyItemPeriod.localEndTime());
			sentence << getPeriodPhrase(DONT_USE_FROM_SPECIFIER, &poutaantuuPeriod);
			sentence << prForecast.precipitationChangeSentence(thePeriod, POUTAANTUU);
		  }
		theWeatherForecastStory.theShortTimePrecipitationReportedFlag = false;
	  }
	else
	  {
		if(thePeriod.localStartTime() > forecastPeriod.localStartTime())
		  sentence << getPeriodPhrase(DONT_USE_FROM_SPECIFIER);
		if(prForecast.shortTermPrecipitationExists(thePeriod))
		  {
			sentence << prForecast.shortTermPrecipitationSentence(thePeriod);
			theWeatherForecastStory.theShortTimePrecipitationReportedFlag = true;
		  }
		else
		  {
			sentence << prForecast.precipitationSentence(thePeriod);
		  }
	  }
	return sentence;
  }





  CloudinessForecastStoryItem::CloudinessForecastStoryItem(WeatherForecastStory& weatherForecastStory,
														   const WeatherPeriod& period, 
														   const story_part_id& storyPartId,
														   const cloudiness_id& cloudinessId,
														   PrecipitationForecastStoryItem* previousPrecipitationStoryItem,
														   PrecipitationForecastStoryItem* nextPrecipitationStoryItem)
	: WeatherForecastStoryItem(weatherForecastStory, period, storyPartId),
	  theCloudinessId(cloudinessId),
	  thePreviousPrecipitationStoryItem(previousPrecipitationStoryItem),
	  theNextPrecipitationStoryItem(nextPrecipitationStoryItem),
	  theReportAboutDryWeatherFlag(true)
		
  {
	if(thePreviousPrecipitationStoryItem)
	  {
		if(thePreviousPrecipitationStoryItem->thePoutaantuuFlag)
		  {
			thePreviousPrecipitationStoryItem->theReportPoutaantuuFlag = false;
			theReportAboutDryWeatherFlag = false;
		  }
	  }
  }


  Sentence CloudinessForecastStoryItem::cloudinessChangeSentence()
  {
	Sentence sentence;

	weather_event_id_vector cloudinessEvents; // pilvistyy and selkenee

	theWeatherForecastStory.theCloudinessForecast.getWeatherEventIdVector(cloudinessEvents);
	  
	for(unsigned int i = 0; i < cloudinessEvents.size(); i++)
	  {
		NFmiTime cloudinessEventTimestamp(cloudinessEvents.at(i).first);
		if(cloudinessEventTimestamp >= thePeriod.localStartTime() &&
		   cloudinessEventTimestamp <= thePeriod.localEndTime())
		  {
			sentence << theWeatherForecastStory.theCloudinessForecast.cloudinessChangeSentence(WeatherPeriod(cloudinessEventTimestamp,
																											 cloudinessEventTimestamp));

			theCloudinessChangeTimestamp = cloudinessEventTimestamp;

			theWeatherForecastStory.theLogger << "CLOUDINESS CHANGE: " << endl;
			theWeatherForecastStory.theLogger << cloudinessEventTimestamp;
			theWeatherForecastStory.theLogger << ": ";
			theWeatherForecastStory.theLogger << sentence;

			break;
		  }
	  }

	return sentence;
  }

  Sentence CloudinessForecastStoryItem::getStoryItemSentence()
  {
	Sentence sentence;

	// precipitation periods aroud are merged
	if(thePreviousPrecipitationStoryItem && thePreviousPrecipitationStoryItem->thePeriodToMergeWith)
	  return sentence;

	// if the cloudiness period is max 2 hours and it is in the end of the forecast period and
	// the previous precipitation period is long > 6h -> don't report cloudiness
	if(thePeriod.localEndTime() == theWeatherForecastStory.theForecastPeriod.localEndTime() &&
	   storyItemPeriodLength() <= 2 &&
	   thePreviousPrecipitationStoryItem && thePreviousPrecipitationStoryItem->storyItemPeriodLength() >= 6)
	  return sentence;

	const CloudinessForecast& clForecast = theWeatherForecastStory.theCloudinessForecast;
	PrecipitationForecast& prForecast = theWeatherForecastStory.thePrecipitationForecast;

	theSentence.clear();
	theChangeSentence.clear();
	theShortFormSentence.clear();
	thePoutaantuuSentence.clear();

	theChangeSentence << cloudinessChangeSentence();
	if(theChangeSentence.size() > 0)
	  {
		WeatherPeriod clPeriod(thePeriod.localStartTime(), theCloudinessChangeTimestamp);
		theSentence << 
		  clForecast.cloudinessSentence(clPeriod, DONT_USE_SHORT_FORM, USE_TIME_SPECIFIER);
		theShortFormSentence << 
		  clForecast.cloudinessSentence(clPeriod, USE_SHORT_FORM, USE_TIME_SPECIFIER);
	  }
	else
	  {
		theSentence << 
		  clForecast.cloudinessSentence(thePeriod, DONT_USE_SHORT_FORM, USE_TIME_SPECIFIER);
		theShortFormSentence << 
		  clForecast.cloudinessSentence(thePeriod, USE_SHORT_FORM, USE_TIME_SPECIFIER);
	  }

	if(thePreviousPrecipitationStoryItem)
	  {
		if(thePreviousPrecipitationStoryItem->thePoutaantuuFlag)
		  {
			WeatherPeriod poutaantuuPeriod(thePreviousPrecipitationStoryItem->getStoryItemPeriod().localEndTime(),
										   thePreviousPrecipitationStoryItem->getStoryItemPeriod().localEndTime());

			// ARE 22.02.2011: The missing period-phrase added
			thePoutaantuuSentence << 
			  getPeriodPhrase(DONT_USE_FROM_SPECIFIER, &poutaantuuPeriod) <<
			  prForecast.precipitationChangeSentence(poutaantuuPeriod, POUTAANTUU);
			thePreviousPrecipitationStoryItem->theReportPoutaantuuFlag = false;
			theReportAboutDryWeatherFlag = false;
		  }
	  }



	if(!thePoutaantuuSentence.empty())
	  {
		sentence << thePoutaantuuSentence;
		sentence << JA_WORD;
		if(clForecast.getCloudinessId(getStoryItemPeriod()) != PUOLIPILVINEN_JA_PILVINEN)
		  sentence << ON_WORD;

		sentence << theShortFormSentence;

		prForecast.dryPeriodTautologyFlag(true);

		// ARE 10.03.2011: Jos sää on melko selkeä ei enää sanota selkenevää
		if(theChangeSentence.size() > 0 &&
		   clForecast.getCloudinessId(getStoryItemPeriod()) > MELKO_SELKEA)
		  {
			sentence << Delimiter(COMMA_PUNCTUATION_MARK);
			sentence << theChangeSentence;
		  }
	  }
	else
	  {
		if(thePeriod.localStartTime() > theWeatherForecastStory.theForecastPeriod.localStartTime())
		  {
			if(storyItemPeriodLength() >= 6)
			  {
				if(theWeatherForecastStory.theStorySize > 0)
				  sentence << getPeriodPhrase(USE_FROM_SPECIFIER);
			  }
			else
			  {
				sentence << getPeriodPhrase(DONT_USE_FROM_SPECIFIER);
			  }
		  }

		sentence << theSentence;

		if(theReportAboutDryWeatherFlag)
		  {
			sentence << JA_WORD;
			  
			if(clForecast.getCloudinessId(getStoryItemPeriod()) == PUOLIPILVINEN_JA_PILVINEN)
			  sentence << ON_WORD;
			  
			sentence << POUTAINEN_WORD;
			prForecast.dryPeriodTautologyFlag(true);			  
		  }

		// ARE 10.03.2011: Jos sää on melko selkeä ei enää sanota selkenevää
		if(theChangeSentence.size() > 0 &&
		   clForecast.getCloudinessId(getStoryItemPeriod()) > MELKO_SELKEA)
		  {
			sentence << Delimiter(COMMA_PUNCTUATION_MARK);
			sentence << theChangeSentence;
		  }
	  }
	theWeatherForecastStory.theCloudinessReportedFlag = true;

	return sentence;
  }

}
