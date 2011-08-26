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

 std::ostream& operator<<(std::ostream & theOutput,
						  const WeatherForecastStoryItem& theStoryItem)
  {
	theOutput << theStoryItem.getPeriod().localStartTime() << "..." << theStoryItem.getPeriod().localEndTime() << " ";
	theOutput << story_part_id_string(theStoryItem.getStoryPartId()) << " - ";
	theOutput << (theStoryItem.isIncluded() ? "included" : "exluded") << endl;

	return theOutput;
  }


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
	int storyItemCounter(0);
	bool moreThanOnePrecipitationForms(false);
	for(unsigned int i = 0; i < theStoryItemVector.size(); i++)
	  {
		if(!specifyPartOfTheDayFlag) // this indicates that the period is short
		  {
			// ARE 14.4.2011: checking theIncludeInTheStoryFlag
			if(theStoryItemVector[i]->theIncludeInTheStoryFlag == true &&
			   theStoryItemVector[i]->getSentence().size() > 0)
			  storyItemCounter++;
		  }

		// check wheather more than one precipitation form exists during the forecast period
		if(!moreThanOnePrecipitationForms &&
		   theStoryItemVector[i]->theStoryPartId == PRECIPITATION_STORY_PART && 
		   theStoryItemVector[i]->theIncludeInTheStoryFlag == true)
		  {
			precipitation_form_id precipitationForm = 
			  thePrecipitationForecast.getPrecipitationForm(theStoryItemVector[i]->thePeriod,
															theForecastArea);

			moreThanOnePrecipitationForms = !PrecipitationForecast::singleForm(precipitationForm);
		  }
	  }
	// if more than one item exists, use the phrases "aluksi", "myöhemmin" when the period is short
	theReportTimePhraseFlag = storyItemCounter > 1;
	thePrecipitationForecast.setSinglePrecipitationFormFlag(!moreThanOnePrecipitationForms);
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

	thePrecipitationForecast.setDryPeriodTautologyFlag(false);

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

		bool thunder(thePrecipitationForecast.thunderExists(precipitationPeriods[i],
															theForecastArea));

		if(get_period_length(precipitationPeriods[i]) <= 1 && extent < 10)
		  continue;

		PrecipitationForecastStoryItem* item = new PrecipitationForecastStoryItem(*this,
																				  precipitationPeriods[i],
																				  PRECIPITATION_STORY_PART,
																				  intensity,
																				  extent,
																				  form, 
																				  type,
																				  thunder);

		NFmiTime startTimeFull;
		NFmiTime endTimeFull;
		thePrecipitationForecast.getPrecipitationPeriod(precipitationPeriods[i].localStartTime(), 
														startTimeFull, endTimeFull);
		// This parameter shows if the precipitation starts 
		// before forecast period or continues after
		int precipitationFullDuration = endTimeFull.DifferenceInHours(startTimeFull);
		item->theFullDuration = precipitationFullDuration;

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
  {	  	// replace the missing story items with the cloudiness story part
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
			previousPrecipitationStoryItem = static_cast<PrecipitationForecastStoryItem*>(theStoryItemVector[i]);
		  }
		  
		// one hour period in the beginning is ignored
		/*
		if(i == 0)
		  {
			if(theStoryItemVector[i]->storyItemPeriodLength() <= 1 && theStoryItemVector.size() > 1)
			  {
				theStoryItemVector[i]->theIncludeInTheStoryFlag = false;
			  }
		  }
		*/
	  }

	// short cloudiness period in the end after precipitation period is not reported
	/*
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
	*/
  }


  void WeatherForecastStory::mergePrecipitationPeriodsWhenFeasible()
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
				
				if(currentPrecipitationStoryItem->thePeriod.localStartTime().DifferenceInHours(previousPrecipitationStoryItem->thePeriod.localEndTime()) <= 2)
				  previousPrecipitationStoryItem->theReportPoutaantuuFlag = false;

				// if the type is different don't merge, except when thunder exists on both periods
				if(previousPrecipitationStoryItem->theType != currentPrecipitationStoryItem->theType &&
				   !(previousPrecipitationStoryItem->theThunder && currentPrecipitationStoryItem->theThunder))
				  {
					// if the dry period between precpitation periods is short don't mention it
					if(theStoryItemVector[i-1] != previousPrecipitationStoryItem && 
					   theStoryItemVector[i-1]->storyItemPeriodLength() <= 1)
					  {
						theStoryItemVector[i-1]->theIncludeInTheStoryFlag = false;
					  }
					continue;
				  }

				NFmiTime gapPeriodStartTime(previousPrecipitationStoryItem->thePeriod.localEndTime());
				NFmiTime gapPeriodEndTime(currentPrecipitationStoryItem->thePeriod.localStartTime());
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
					if(theStoryItemVector[i-1]->theStoryPartId == CLOUDINESS_STORY_PART)
					  theStoryItemVector[i-1]->theIncludeInTheStoryFlag = false;
				  }
			  }
			previousPrecipitationStoryItem = currentPrecipitationStoryItem;
		  }
	  }
  }

  void WeatherForecastStory::mergeCloudinessPeriodsWhenFeasible()
  {
  	CloudinessForecastStoryItem* previousCloudinessStoryItem = 0;
	CloudinessForecastStoryItem* currentCloudinessStoryItem = 0;
	for(unsigned int i = 0; i < theStoryItemVector.size(); i++)
	  {
		if(theStoryItemVector[i]->theStoryPartId == CLOUDINESS_STORY_PART)
		  {
			currentCloudinessStoryItem = static_cast<CloudinessForecastStoryItem*>(theStoryItemVector[i]);
			if(previousCloudinessStoryItem)
			  {
				NFmiTime gapPeriodStartTime(previousCloudinessStoryItem->thePeriod.localEndTime());
				NFmiTime gapPeriodEndTime(currentCloudinessStoryItem->thePeriod.localStartTime());
				WeatherPeriod gapPeriod(gapPeriodStartTime, gapPeriodEndTime);

				// merge periods if the precipitation period in between is short and
				// intensity is weak and precipitation area is small
				if(get_period_length(gapPeriod) <= 2 &&
				   currentCloudinessStoryItem->thePreviousPrecipitationStoryItem->theIntensity <= 0.07 &&
				   currentCloudinessStoryItem->thePreviousPrecipitationStoryItem->theExtent <= 40)
				  {
					// merge two weak cloudiness periods
					previousCloudinessStoryItem->thePeriodToMergeWith = currentCloudinessStoryItem;
					currentCloudinessStoryItem->thePeriodToMergeTo = previousCloudinessStoryItem;
					currentCloudinessStoryItem->thePreviousPrecipitationStoryItem->theIncludeInTheStoryFlag = false;
				  }
			  }
			previousCloudinessStoryItem = currentCloudinessStoryItem;
		  }
	  }

	int storyItemCount = theStoryItemVector.size();
	// short cloudiness period in the start is ignored
	if(storyItemCount > 1)
	  {
		WeatherPeriod storyItemPeriod(theStoryItemVector[0]->getStoryItemPeriod());
		/*
		cout << "story part 0: " << story_part_id_string(theStoryItemVector[0]->theStoryPartId) << endl;
		cout << "story part 1: " << story_part_id_string(theStoryItemVector[1]->theStoryPartId) << endl;
		cout << "story item period length: " << theStoryItemVector[0]->storyItemPeriodLength() << endl;
		cout << "theStoryItemVector[0]->thePeriodToMergeWith == 0: " << 
		  (theStoryItemVector[0]->thePeriodToMergeWith == 0 ? "true" : "false") << endl;
		cout << "period item length: " << storyItemPeriod.localStartTime() << "..." << storyItemPeriod.localEndTime() << endl;
		cout << "period length: " 
			 << theStoryItemVector[0]->getPeriod().localStartTime() 
			 << "..." 
			 << theStoryItemVector[0]->getPeriod().localEndTime() 
			 << endl;
		*/
		if(theStoryItemVector[0]->theStoryPartId == CLOUDINESS_STORY_PART &&
		   theStoryItemVector[1]->theStoryPartId == PRECIPITATION_STORY_PART &&
		   theStoryItemVector[0]->getPeriodLength() <= 1 &&
		   theStoryItemVector[0]->thePeriodToMergeWith == 0)
		  {
			theStoryItemVector[0]->theIncludeInTheStoryFlag = false;
		  }
		else if(theStoryItemVector[0]->theStoryPartId == PRECIPITATION_STORY_PART &&
				theStoryItemVector[1]->theStoryPartId == CLOUDINESS_STORY_PART &&
				theStoryItemVector[0]->getPeriodLength() <= 1 &&
				theStoryItemVector[0]->thePeriodToMergeWith == 0 &&
				static_cast<PrecipitationForecastStoryItem*>(theStoryItemVector[0])->theIntensity < 0.1 &&
				static_cast<PrecipitationForecastStoryItem*>(theStoryItemVector[0])->theExtent < 40)
		  {
			theStoryItemVector[0]->theIncludeInTheStoryFlag = false;
		  }
		
		/*
		cout << "theStoryItemVector[storyItemCount-1]: " << story_part_id_string(theStoryItemVector[storyItemCount-1]->theStoryPartId) << endl;
		cout << "theStoryItemVector[storyItemCount-2]: " << story_part_id_string(theStoryItemVector[storyItemCount-2]->theStoryPartId) << endl;
		cout << "story item period length (storyItemCount-1): " << theStoryItemVector[storyItemCount-1]->storyItemPeriodLength() << endl;
		cout << "theStoryItemVector[storyItemCount-1]->thePeriodToMergeTo == 0: " << 
		  (theStoryItemVector[storyItemCount-1]->thePeriodToMergeWith == 0 ? "true" : "false") << endl;
		cout << "theStoryItemVector[storyItemCount-2]->theIncludeInTheStoryFlag: " <<
		  (theStoryItemVector[storyItemCount-2]->theIncludeInTheStoryFlag ? "true" : "false") << endl;
		*/

		// short cloudiness period in the end after precipitation period is not reported
		if(theStoryItemVector[storyItemCount-1]->theStoryPartId == CLOUDINESS_STORY_PART &&
		   theStoryItemVector[storyItemCount-1]->getPeriodLength() <= 1 &&
		   theStoryItemVector[storyItemCount-1]->thePeriodToMergeTo == 0 &&
		   theStoryItemVector[storyItemCount-2]->theStoryPartId == PRECIPITATION_STORY_PART &&
		   theStoryItemVector[storyItemCount-2]->theIncludeInTheStoryFlag == true)
		  {
			theStoryItemVector[storyItemCount-1]->theIncludeInTheStoryFlag = false;
		  }
	  }
 }
  
  void WeatherForecastStory::mergePeriodsWhenFeasible()
  {
	mergePrecipitationPeriodsWhenFeasible();
	mergeCloudinessPeriodsWhenFeasible();
  }

  Sentence WeatherForecastStory::getTimePhrase()
  {
	Sentence sentence;

	if(theReportTimePhraseFlag)
	  {
		if(theStorySize == 0)
		  sentence << ALUKSI_WORD;
		else
		  sentence << MYOHEMMIN_WORD;
	  }

	return sentence;
  }



	const void WeatherForecastStory::logTheStoryItems() const
	{
	  theLogger << "******** STORY ITEMS ********" << endl;
	  for(unsigned int i = 0; i < theStoryItemVector.size(); i++)
		{
		  theLogger << *theStoryItemVector[i];
		}
	}


  WeatherForecastStoryItem::WeatherForecastStoryItem(WeatherForecastStory& weatherForecastStory,
													 const WeatherPeriod& period, 
													 const story_part_id& storyPartId)
	: theWeatherForecastStory(weatherForecastStory),
	  thePeriod(period),
	  theStoryPartId(storyPartId),
	  theIncludeInTheStoryFlag(true),
	  thePeriodToMergeWith(0),
	  thePeriodToMergeTo(0)
  {
  }
	
	
  Sentence WeatherForecastStoryItem::getSentence()
  {
	Sentence sentence;

	if(theIncludeInTheStoryFlag)
	  {
		sentence << getStoryItemSentence();
	  }

	return sentence;
  }

  WeatherPeriod WeatherForecastStoryItem::getStoryItemPeriod() const 
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

  // special treatment, because 06:00 can be aamuyö and morning, depends weather the period starts or ends
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

  Sentence WeatherForecastStoryItem::getPeriodPhrase(const bool& theFromSpecifier,
													 const WeatherPeriod* thePhrasePeriod /*= 0*/,
													 const bool& theStoryUnderConstructionEmpty /*= true*/)
  {
	Sentence sentence;

	if(theWeatherForecastStory.theStorySize == 0 && theStoryUnderConstructionEmpty)
	  return sentence;

	WeatherPeriod phrasePeriod(thePhrasePeriod == 0 ? getStoryItemPeriod() : *thePhrasePeriod);
	bool specifyDay = false;

	if(forecastPeriodLength() > 24 &&
	   theWeatherForecastStory.theForecastTime.GetJulianDay() != phrasePeriod.localStartTime().GetJulianDay() &&
	   abs(theWeatherForecastStory.theForecastTime.DifferenceInHours(phrasePeriod.localStartTime())) >= 21)
	  {
		Sentence todaySentence;
		todaySentence <<  PeriodPhraseFactory::create("today",
													  theWeatherForecastStory.theVar,
													  theWeatherForecastStory.theForecastTime,
													  phrasePeriod,
													  theWeatherForecastStory.theWeatherArea);
		if(todaySentence.size() > 0)
		  specifyDay = true;
	  }

	sentence << checkForAamuyoAndAamuPhrase(theFromSpecifier,
											phrasePeriod);
	if(sentence.size() == 0)
	  {
		std::string time_phrase;
		sentence << get_time_phrase_large(phrasePeriod,
										  specifyDay,
										  theWeatherForecastStory.theVar,
										  time_phrase,
										  theFromSpecifier);
	  }

	theWeatherForecastStory.theLogger << "PHRASE PERIOD: " 
									  << phrasePeriod.localStartTime() 
									  << "..."
									  << phrasePeriod.localEndTime()
									  << endl;
	theWeatherForecastStory.theLogger << "PHRASE: ";
	theWeatherForecastStory.theLogger << sentence;

	return sentence;
  }

  PrecipitationForecastStoryItem::PrecipitationForecastStoryItem(WeatherForecastStory& weatherForecastStory,
																 const WeatherPeriod& period, 
																 const story_part_id& storyPartId,
																 const float& intensity,
																 const float& extent,
																 const unsigned int& form,
																 const precipitation_type& type,
																 const bool& thunder)
	: WeatherForecastStoryItem(weatherForecastStory, period, storyPartId),
	  theIntensity(intensity),
	  theExtent(extent),
	  theForm(form),
	  theType(type),
	  theThunder(thunder),
	  theSadeJatkuuFlag(false),
	  thePoutaantuuFlag(intensity > WEAK_PRECIPITATION_LIMIT_WATER),
	  theReportPoutaantuuFlag(intensity > WEAK_PRECIPITATION_LIMIT_WATER),
	  theFullDuration(period.localEndTime().DifferenceInHours(period.localStartTime()))
  {
  }

  bool PrecipitationForecastStoryItem::weakPrecipitation() const 
  { 
	return theIntensity <= WEAK_PRECIPITATION_LIMIT_WATER; 
  }

	
  Sentence PrecipitationForecastStoryItem::getStoryItemSentence()
  { 
	Sentence sentence;

	// thePeriodToMergeWith handles the whole stuff
	if(thePeriodToMergeTo)
	  return sentence;

	const PrecipitationForecast& prForecast = theWeatherForecastStory.thePrecipitationForecast;
	WeatherPeriod forecastPeriod = theWeatherForecastStory.theForecastPeriod;
	WeatherPeriod storyItemPeriod(getStoryItemPeriod());
	Sentence thePeriodPhrase;

	if(storyItemPeriodLength() >= 6)
	  {
		if(storyItemPeriod.localStartTime() != forecastPeriod.localStartTime())
		  {
			if(storyItemPeriod.localStartTime() > forecastPeriod.localStartTime())
			  thePeriodPhrase << getPeriodPhrase(USE_FROM_SPECIFIER);
			if(thePeriodPhrase.size() == 0)
			  thePeriodPhrase << theWeatherForecastStory.getTimePhrase();
			
			sentence << prForecast.precipitationChangeSentence(thePeriod, thePeriodPhrase, SADE_ALKAA);
		  }
		else
		  {			
			if(storyItemPeriod.localStartTime() > forecastPeriod.localStartTime())
			  thePeriodPhrase << getPeriodPhrase(DONT_USE_FROM_SPECIFIER, &storyItemPeriod);
			if(thePeriodPhrase.size() == 0)
			  thePeriodPhrase << theWeatherForecastStory.getTimePhrase();

			sentence << prForecast.precipitationSentence(thePeriod, thePeriodPhrase);
		  }
		  
		if(storyItemPeriod.localEndTime() != forecastPeriod.localEndTime() && 
		   theReportPoutaantuuFlag)
		  {
			WeatherPeriod poutaantuuPeriod(storyItemPeriod.localEndTime(), storyItemPeriod.localEndTime());
			thePeriodPhrase << getPeriodPhrase(DONT_USE_FROM_SPECIFIER, &poutaantuuPeriod, sentence.size() == 0);
			theWeatherForecastStory.theLogger << thePeriodPhrase;
			if(thePeriodPhrase.size() == 0)
			  thePeriodPhrase << theWeatherForecastStory.getTimePhrase();
			
			if(sentence.size() > 0)
			  sentence << Delimiter(",");
			sentence << prForecast.precipitationChangeSentence(thePeriod, thePeriodPhrase, POUTAANTUU);
		  }
		theWeatherForecastStory.theShortTimePrecipitationReportedFlag = false;
	  }
	else
	  {
		if(thePeriod.localStartTime() > forecastPeriod.localStartTime())
		  thePeriodPhrase << getPeriodPhrase(DONT_USE_FROM_SPECIFIER, &storyItemPeriod);
		if(thePeriodPhrase.size() == 0)
		  thePeriodPhrase << theWeatherForecastStory.getTimePhrase();

		if(prForecast.shortTermPrecipitationExists(thePeriod))
		  {
			sentence << prForecast.shortTermPrecipitationSentence(thePeriod, thePeriodPhrase);
			theWeatherForecastStory.theShortTimePrecipitationReportedFlag = true;
		  }
		else
		  {
			sentence << prForecast.precipitationSentence(thePeriod, thePeriodPhrase);
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

	// thePeriodToMergeWith handles the whole stuff
	if(thePeriodToMergeTo)
	  return sentence;

	// precipitation periods aroud are merged
	//	if(thePreviousPrecipitationStoryItem && thePreviousPrecipitationStoryItem->thePeriodToMergeWith)
	//return sentence;

	WeatherPeriod storyItemPeriod(getStoryItemPeriod());
	// if the cloudiness period is max 2 hours and it is in the end of the forecast period and
	// the previous precipitation period is long > 6h -> don't report cloudiness
	if(storyItemPeriod.localEndTime() == theWeatherForecastStory.theForecastPeriod.localEndTime() &&
	   storyItemPeriodLength() <= 2 &&
	   thePreviousPrecipitationStoryItem && thePreviousPrecipitationStoryItem->storyItemPeriodLength() >= 6)
	  return sentence;

	const CloudinessForecast& clForecast = theWeatherForecastStory.theCloudinessForecast;
	const PrecipitationForecast& prForecast = theWeatherForecastStory.thePrecipitationForecast;

	theSentence.clear();
	theChangeSentence.clear();
	theShortFormSentence.clear();
	thePoutaantuuSentence.clear();

	theChangeSentence << cloudinessChangeSentence();

	if(thePreviousPrecipitationStoryItem)
	  {
		if(thePreviousPrecipitationStoryItem->thePoutaantuuFlag)
		  {
			cloudiness_id cloudinessId = clForecast.getCloudinessId(getStoryItemPeriod());

			WeatherPeriod poutaantuuPeriod(thePreviousPrecipitationStoryItem->getStoryItemPeriod().localEndTime(),
										   thePreviousPrecipitationStoryItem->getStoryItemPeriod().localEndTime());

			// ARE 22.02.2011: The missing period-phrase added
			Sentence thePeriodPhrase(getPeriodPhrase(USE_FROM_SPECIFIER, &poutaantuuPeriod));
			if(thePeriodPhrase.size() == 0)
			  thePeriodPhrase << theWeatherForecastStory.getTimePhrase();
			thePoutaantuuSentence << prForecast.precipitationPoutaantuuAndCloudiness(thePeriodPhrase,
																					 cloudinessId);
			thePreviousPrecipitationStoryItem->theReportPoutaantuuFlag = false;
			theReportAboutDryWeatherFlag = false;
		  }
	  }

	if(!thePoutaantuuSentence.empty())
	  {
		sentence << thePoutaantuuSentence;

		// ARE 10.03.2011: Jos sää on melko selkeä ei enää sanota selkenevää
		if(theChangeSentence.size() > 0 &&
		   clForecast.getCloudinessId(storyItemPeriod) > MELKO_SELKEA)
		  {
			sentence << Delimiter(COMMA_PUNCTUATION_MARK);
			sentence << theChangeSentence;
		  }
	  }
	else
	  {
		Sentence thePeriodPhrase;
		if(storyItemPeriod.localStartTime() > theWeatherForecastStory.theForecastPeriod.localStartTime())
		  {
			if(storyItemPeriodLength() >= 6)
			  {
				if(theWeatherForecastStory.theStorySize > 0)
				  thePeriodPhrase << getPeriodPhrase(USE_FROM_SPECIFIER);
			  }
			else
			  {
				thePeriodPhrase << getPeriodPhrase(DONT_USE_FROM_SPECIFIER, &storyItemPeriod);
			  }
		  }
		if(thePeriodPhrase.size() == 0)
		  thePeriodPhrase << theWeatherForecastStory.getTimePhrase();

		if(theChangeSentence.size() > 0)
		  {
			WeatherPeriod clPeriod(storyItemPeriod.localStartTime(), theCloudinessChangeTimestamp);
			sentence << 
			  clForecast.cloudinessSentence(clPeriod, 
											theReportAboutDryWeatherFlag && !prForecast.getDryPeriodTautologyFlag(),
											thePeriodPhrase, 
											DONT_USE_SHORT_FORM);
		  }
		else
		  {
			sentence << 
			  clForecast.cloudinessSentence(storyItemPeriod,
											theReportAboutDryWeatherFlag && !prForecast.getDryPeriodTautologyFlag(),
											thePeriodPhrase, 
											DONT_USE_SHORT_FORM);
		  }
		prForecast.setDryPeriodTautologyFlag(theReportAboutDryWeatherFlag);

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
