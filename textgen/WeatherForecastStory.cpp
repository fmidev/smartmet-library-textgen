#include <iostream>
#include <string>

#include "Delimiter.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "Sentence.h"
#include <calculator/Settings.h>
#include <calculator/WeatherArea.h>
#include <calculator/WeatherPeriod.h>

#include "CloudinessForecast.h"
#include "DebugTextFormatter.h"
#include "FogForecast.h"
#include "PrecipitationForecast.h"
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
using namespace TextGen;
using namespace Settings;

std::ostream& operator<<(std::ostream& theOutput, const WeatherForecastStoryItem& theStoryItem)
{
  theOutput << theStoryItem.getPeriod().localStartTime() << "..."
            << theStoryItem.getPeriod().localEndTime() << " ";
  theOutput << story_part_id_string(theStoryItem.getStoryPartId()) << " - ";
  theOutput << (theStoryItem.isIncluded() ? "included" : "exluded") << endl;

  return theOutput;
}

WeatherForecastStory::WeatherForecastStory(const std::string& var,
                                           const WeatherPeriod& forecastPeriod,
                                           const WeatherArea& weatherArea,
                                           const unsigned short& forecastArea,
                                           const TextGenPosixTime& theForecastTime,
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

  bool specifyPartOfTheDayFlag =
      Settings::optional_bool(theVar + "::specify_part_of_the_day", true);
  int storyItemCounter(0);
  bool moreThanOnePrecipitationForms(false);
  for (unsigned int i = 0; i < theStoryItemVector.size(); i++)
  {
    // when specifyPartOfTheDayFlag is false it means
    // that the period is short and part of the day should not be expressed
    if (!specifyPartOfTheDayFlag)
    {
      // ARE 14.4.2011: checking theIncludeInTheStoryFlag
      if (theStoryItemVector[i]->theIncludeInTheStoryFlag == true &&
          theStoryItemVector[i]->getSentence().size() > 0)
        storyItemCounter++;
    }

    // check wheather more than one precipitation form exists during the forecast period
    if (!moreThanOnePrecipitationForms &&
        theStoryItemVector[i]->theStoryPartId == PRECIPITATION_STORY_PART &&
        theStoryItemVector[i]->theIncludeInTheStoryFlag == true)
    {
      precipitation_form_id precipitationForm = thePrecipitationForecast.getPrecipitationForm(
          theStoryItemVector[i]->thePeriod, theForecastArea);

      moreThanOnePrecipitationForms = !PrecipitationForecast::singleForm(precipitationForm);
    }
  }
  // if more than one item exists, use the phrases "aluksi", "myöhemmin" when the period is short
  theReportTimePhraseFlag = storyItemCounter > 1;
  thePrecipitationForecast.setSinglePrecipitationFormFlag(!moreThanOnePrecipitationForms);
}

WeatherForecastStory::~WeatherForecastStory() { theStoryItemVector.clear(); }
Paragraph WeatherForecastStory::getWeatherForecastStory()
{
  Paragraph paragraph;

  theShortTimePrecipitationReportedFlag = false;
  theCloudinessReportedFlag = false;
  theStorySize = 0;

  thePrecipitationForecast.setDryPeriodTautologyFlag(false);

  for (unsigned int i = 0; i < theStoryItemVector.size(); i++)
  {
    WeatherForecastStoryItem& item = *(theStoryItemVector[i]);

    Sentence storyItemSentence;
    storyItemSentence << item.getSentence();

    if (storyItemSentence.size() > 0 && !item.thePeriodToMergeTo)
    {
      theStorySize += storyItemSentence.size();

      paragraph << storyItemSentence;

      // additional sentences: currently only in precipitation story: like "iltapäivästä alkaen sade
      // voi olla runsasta"
      for (unsigned int k = 0; k < theStoryItemVector[i]->numberOfAdditionalSentences(); k++)
        paragraph << theStoryItemVector[i]->getAdditionalSentence(k);

      // possible fog sentence after
      if (theStoryItemVector[i]->theStoryPartId == CLOUDINESS_STORY_PART)
      {
        Sentence fogSentence;
        fogSentence << theFogForecast.fogSentence(theStoryItemVector[i]->thePeriod);

        theStorySize += fogSentence.size();
        paragraph << fogSentence;
      }
    }
    // reset the today phrase when day changes
    if (i > 0 &&
        theStoryItemVector[i - 1]->thePeriod.localEndTime().GetJulianDay() !=
            theStoryItemVector[i]->thePeriod.localEndTime().GetJulianDay() &&
        get_period_length(theForecastPeriod) > 24)
      const_cast<WeatherHistory&>(theWeatherArea.history())
          .updateTimePhrase("", "", TextGenPosixTime(1970, 1, 1));
  }

  // ARE 19.04.2012: yletv for kaakkois-suomi, empty story was created
  // if story is empty take the whole forecast period and tell story of it
  if (theStorySize == 0)
  {
    Sentence periodPhrase;
    std::vector<Sentence> theAdditionalSentences;
    paragraph << thePrecipitationForecast.precipitationSentence(
        theForecastPeriod, periodPhrase, theAdditionalSentences);
    for (unsigned int i = 0; i < theAdditionalSentences.size(); i++)
      paragraph << theAdditionalSentences[i];
  }

  return paragraph;
}

Paragraph WeatherForecastStory::getWeatherForecastStoryAtSea()
{
  thePrecipitationForecast.setUseIcingPhraseFlag(false);

  Paragraph paragraph;
  std::vector<Sentence> weatherForecastSentences;

  unsigned int forecastPeriodLength = get_period_length(theForecastPeriod);
  unsigned int precipitationAndFogPeriodLength = 0;
  bool inManyPlaces = false;

  thePrecipitationForecast.setDryPeriodTautologyFlag(false);
  theShortTimePrecipitationReportedFlag = false;
  theCloudinessReportedFlag = false;
  theStorySize = 0;

  std::vector<std::pair<story_part_id, unsigned int> > storyItems;

  for (unsigned int i = 0; i < theStoryItemVector.size(); i++)
  {
    WeatherForecastStoryItem* item = theStoryItemVector[i];

    Sentence storyItemSentence;
    storyItemSentence << item->getSentence();

    if (storyItemSentence.empty() || item->thePeriodToMergeTo) continue;

    // report only precipitation
    if (item->theStoryPartId == PRECIPITATION_STORY_PART ||
        item->theStoryPartId == PRECIPITATION_TYPE_CHANGE_STORY_PART)
    {
      PrecipitationForecastStoryItem* precipitationStoryItem =
          static_cast<PrecipitationForecastStoryItem*>(item);

      // if precipitation form is sleet or snow, report it even if it is weak
      // and extent of precipitation area must be over 10 %
      if (((!precipitationStoryItem->weakPrecipitation() ||
            (precipitationStoryItem->theForm & SLEET_FORM ||
             precipitationStoryItem->theForm & SNOW_FORM)) &&
           precipitationStoryItem->theExtent > 10))
      {
        bool mergeDone = false;
        if (storyItems.size() > 0)
        {
          WeatherForecastStoryItem* previousItem = theStoryItemVector[storyItems.back().second];
          if (previousItem->theStoryPartId == PRECIPITATION_STORY_PART &&
              precipitationStoryItem->theStoryPartId == PRECIPITATION_STORY_PART)
          {
            PrecipitationForecastStoryItem* previousPrecipitationStoryItem =
                static_cast<PrecipitationForecastStoryItem*>(previousItem);
            if (previousPrecipitationStoryItem->theExtent >= IN_SOME_PLACES_LOWER_LIMIT &&
                previousPrecipitationStoryItem->theExtent <= IN_MANY_PLACES_UPPER_LIMIT &&
                precipitationStoryItem->theExtent >= IN_SOME_PLACES_LOWER_LIMIT &&
                precipitationStoryItem->theExtent <= IN_MANY_PLACES_UPPER_LIMIT &&
                precipitationStoryItem->thePeriod.localStartTime().DifferenceInHours(
                    previousPrecipitationStoryItem->thePeriod.localEndTime()) <= 2)
            {
              /*
  std::cout << "merge periods: " << as_string(previousPrecipitationStoryItem->thePeriod)
            << " and " << as_string(precipitationStoryItem->thePeriod) << ", "
            << precipitationStoryItem->thePeriod.localStartTime().DifferenceInHours(
                   previousPrecipitationStoryItem->thePeriod.localEndTime())
            << std::endl;
              */
              TextGenPosixTime startTime =
                  previousPrecipitationStoryItem->thePeriod.localStartTime();
              TextGenPosixTime endTime = precipitationStoryItem->thePeriod.localEndTime();
              previousPrecipitationStoryItem->thePeriod = WeatherPeriod(startTime, endTime);
              mergeDone = true;
            }
          }
          /*
WeatherForecastStoryItem*
thePeriodToMergeWith;  // if periods are merged this points to the megreable period
WeatherForecastStoryItem*
thePeriodToMergeTo;  // if periods are merged this points to the merged period

           */
        }
        if (!mergeDone) storyItems.push_back(std::make_pair(item->theStoryPartId, i));
      }
    }

    // possible fog sentence after
    if (item->theStoryPartId == CLOUDINESS_STORY_PART)
    {
      CloudinessForecastStoryItem* cloudinessStoryItem =
          static_cast<CloudinessForecastStoryItem*>(item);
      /*

  FogInfo fi = theFogForecast.fogInfo(cloudinessStoryItem->thePeriod);
  if (get_period_length(fi.period) > 0 && !fi.sentence.empty())
      */
      if (storyItems.size() > 0)
        storyItems.push_back(make_pair(cloudinessStoryItem->theStoryPartId, i));

      //    storyItems.push_back(make_pair(item->theStoryPartId, i));
    }
  }

  //  std::cout << "   Story Items #" << storyItems.size() << std::endl;

  // if precipitation/fog lasts whole period
  if (storyItems.size() > 0)
  {
    WeatherForecastStoryItem* firstItemOfForecast = theStoryItemVector[0];
    WeatherForecastStoryItem* firstItemOfStory = theStoryItemVector[storyItems[0].second];
    // theStorySize is set to get time phrase for the first sentence
    // even if it does not start from the beginning of forecast period
    if (firstItemOfStory->thePeriod.localStartTime() >
        firstItemOfForecast->thePeriod.localStartTime())
      theStorySize += 1;
  }

  WeatherForecastStoryItem* previousPrecipitationPeriod = nullptr;
  WeatherForecastStoryItem* previousCloudinessPeriod = nullptr;
  int previousStoryItemIndex = -1;
  for (unsigned int i = 0; i < storyItems.size(); i++)
  {
    int currentStoryItemIndex = storyItems[i].second;
    WeatherForecastStoryItem* item = theStoryItemVector[currentStoryItemIndex];

    if (storyItems[i].first == CLOUDINESS_STORY_PART)
    {
      // there can be possibly several successive fog sentences -> report them togethermerge them
      if (i < storyItems.size() - 1 && storyItems[i + 1].first == CLOUDINESS_STORY_PART)
      {
        // report clearing up after precipitation
        /*
if (previousCloudinessPeriod)
{
  std::cout << "push cloudine1ss\n ";
  weatherForecastSentences.push_back(previousCloudinessPeriod->getSentence());
}
        */
        FogInfo fogInfo = theFogForecast.fogInfo(item->thePeriod);
        if (fogInfo.id != NO_FOG)
          if (!previousCloudinessPeriod) previousCloudinessPeriod = item;
        continue;
      }
      // report fog periods together
      if (previousCloudinessPeriod)
      {
        Sentence fogSentence;
        FogInfo firstFI = theFogForecast.fogInfo(previousCloudinessPeriod->thePeriod);
        FogInfo secondFI = theFogForecast.fogInfo(item->thePeriod);
        /*
std::cout << "fog period " << as_string(previousCloudinessPeriod->thePeriod) << ", "
          << as_string(item->thePeriod) << ", "
          << get_period_length(previousCloudinessPeriod->thePeriod) +
                 get_period_length(item->thePeriod)
          << "," << forecastPeriodLength << std::endl;
        */
        // take the sentence from longest period
        if (!firstFI.sentence.empty() && !secondFI.sentence.empty())
          fogSentence << firstFI.timePhrase << "ja" << secondFI.timePhrase
                      << (get_period_length(firstFI.period) > get_period_length(secondFI.period)
                              ? firstFI.sentence
                              : secondFI.sentence);

        if (!fogSentence.empty())
        {
          weatherForecastSentences.push_back(fogSentence);
          theStorySize += fogSentence.size();
          /*
  theLogger << "Fog periods: " << as_string(firstFI.period) << " and "
            << as_string(secondFI.period) << std::endl;
          */
        }

        if (firstFI.id == FOG || firstFI.id == FOG_POSSIBLY_DENSE)
          precipitationAndFogPeriodLength += get_period_length(previousCloudinessPeriod->thePeriod);
        if (secondFI.id == FOG || secondFI.id == FOG_POSSIBLY_DENSE)
          precipitationAndFogPeriodLength += get_period_length(item->thePeriod);

        /*
std::cout << "FogSentence0 " << i << "," << as_string(firstFI.period) << ","
          << as_string(secondFI.period) << "," << as_string(fogSentence) << std::endl;
        */
      }
      else
      {
        // report clearing up after precipitation

        if (previousPrecipitationPeriod &&
            get_period_length(previousPrecipitationPeriod->getStoryItemPeriod()) >= 6)
        {
          ;  // weatherForecastSentences.push_back(item->getSentence());
        }

        FogInfo fi = theFogForecast.fogInfo(item->thePeriod);
        Sentence fogSentence;
        fogSentence << fi.timePhrase << fi.sentence;
        if (!fogSentence.empty())
        {
          weatherForecastSentences.push_back(fogSentence);
          theLogger << "Fog period: " << as_string(fi.period) << std::endl;
        }
        theStorySize += fogSentence.size();
        // only extensive fog counted here
        if (fi.id == FOG || fi.id == FOG_POSSIBLY_DENSE)
          precipitationAndFogPeriodLength += get_period_length(item->thePeriod);
        /*
std::cout << "FogSentence0 " << i << "," << as_string(fi.period) << ","
          << as_string(fogSentence) << std::endl;
        */
        /*
std::cout << "fog period " << as_string(item->thePeriod) << ", "
          << get_period_length(item->thePeriod) << "," << forecastPeriodLength << std::endl;
        */
      }
      previousCloudinessPeriod = nullptr;
      previousPrecipitationPeriod = nullptr;
    }
    else
    {
      PrecipitationForecastStoryItem* precipitationItem =
          static_cast<PrecipitationForecastStoryItem*>(item);
      Sentence precipitationSentence;
      precipitationSentence << precipitationItem->getSentence();
      theStorySize += precipitationSentence.size();
      if (!precipitationSentence.empty() || precipitationItem->thePoutaantuuFlag)
      {
        weatherForecastSentences.push_back(precipitationSentence);
        if (!precipitationItem->thePoutaantuuFlag)
        {
          if (!inManyPlaces && precipitationItem->inManyPlaces()) inManyPlaces = true;
          // only continuous precipitation with large extent is counted here
          if (precipitationItem->theType == CONTINUOUS &&
              precipitationItem->theExtent >= IN_MANY_PLACES_UPPER_LIMIT)
            precipitationAndFogPeriodLength += get_period_length(precipitationItem->thePeriod);
          theLogger << "Precipitation period: " << as_string(precipitationItem->thePeriod)
                    << std::endl;
        }
      }

      /*
  std::cout << "precipitation period " << as_string(precipitationItem->thePeriod) << ", "
            << get_period_length(precipitationItem->thePeriod) << ","
            << precipitationSentence.size() << "," << precipitationSentence.empty() << ","
            << paragraph.empty() << ", " << paragraph.size() << std::endl;
      */
      /*
      std::cout << "PrecipitationSentence " << i << "," << as_string(item->thePeriod) << ","
                << as_string(precipitationSentence) << std::endl;
          */
      previousPrecipitationPeriod = item;
    }
    // reset the today phrase when day changes
    if (previousStoryItemIndex > -1 &&
        theStoryItemVector[previousStoryItemIndex]->thePeriod.localEndTime().GetJulianDay() !=
            theStoryItemVector[currentStoryItemIndex]->thePeriod.localEndTime().GetJulianDay() &&
        get_period_length(theForecastPeriod) > 24)
      const_cast<WeatherHistory&>(theWeatherArea.history())
          .updateTimePhrase("", "", TextGenPosixTime(1970, 1, 1));

    previousStoryItemIndex = currentStoryItemIndex;
  }
  if (weatherForecastSentences.empty())
  {
    if (!thePrecipitationForecast.isDryPeriod(theForecastPeriod, theForecastArea))

    {
      Sentence precipitationSentence;
      Sentence periodPhrase;
      std::vector<Sentence> theAdditionalSentences;
      precipitationSentence << thePrecipitationForecast.precipitationSentence(
          theForecastPeriod, periodPhrase, theAdditionalSentences);
      if (!precipitationSentence.empty())
      {
        weatherForecastSentences.push_back(precipitationSentence);
        theStorySize += precipitationSentence.size();
      }
    }
  }

  Sentence visibilitySentence;
  if (weatherForecastSentences.empty())
  {
    visibilitySentence << HYVA_NAKYVYYS_PHRASE;
  }
  else if ((precipitationAndFogPeriodLength / forecastPeriodLength) <= 0.75 && !inManyPlaces)
  {
    weatherForecastSentences.back()
        << Delimiter(COMMA_PUNCTUATION_MARK) << ENIMMAKSEEN_HYVA_NAKYVYYS_PHRASE;
  }
  else if (precipitationAndFogPeriodLength < forecastPeriodLength || inManyPlaces)
  {
    weatherForecastSentences.back()
        << Delimiter(COMMA_PUNCTUATION_MARK) << MUUTEN_HYVA_NAKYVYYS_PHRASE;
  }

  if (!visibilitySentence.empty()) weatherForecastSentences.push_back(visibilitySentence);

  for (auto s : weatherForecastSentences)
    paragraph << s;

  theLogger << "Weather Forecast At Sea: " << std::endl
            << " length of continuous precipitation or extensive fog: "
            << precipitationAndFogPeriodLength << std::endl
            << " forecats period length: " << forecastPeriodLength << std::endl
            << " precipitation of fog in many places: " << (inManyPlaces ? "yes" : "no")
            << std::endl
            << " length of forecast text: " << paragraph.size() << std::endl;
  if (!paragraph.empty()) theLogger << " forecast text: " << as_string(paragraph) << std::endl;

  return paragraph;
}

void WeatherForecastStory::addPrecipitationStoryItems()
{
  vector<WeatherPeriod> precipitationPeriods;

  thePrecipitationForecast.getPrecipitationPeriods(theForecastPeriod, precipitationPeriods);

  PrecipitationForecastStoryItem* previousPrItem = 0;
  WeatherForecastStoryItem* missingStoryItem = 0;
  for (unsigned int i = 0; i < precipitationPeriods.size(); i++)
  {
    float intensity(
        thePrecipitationForecast.getMeanIntensity(precipitationPeriods[i], theForecastArea));
    float extent(
        thePrecipitationForecast.getPrecipitationExtent(precipitationPeriods[i], theForecastArea));
    unsigned int form(
        thePrecipitationForecast.getPrecipitationForm(precipitationPeriods[i], theForecastArea));
    precipitation_type type(
        thePrecipitationForecast.getPrecipitationType(precipitationPeriods[i], theForecastArea));

    bool thunder(
        thePrecipitationForecast.thunderExists(precipitationPeriods[i], theForecastArea, theVar));

    if (get_period_length(precipitationPeriods[i]) <= 1 && extent < 10) continue;

    PrecipitationForecastStoryItem* item =
        new PrecipitationForecastStoryItem(*this,
                                           precipitationPeriods[i],
                                           PRECIPITATION_STORY_PART,
                                           intensity,
                                           extent,
                                           form,
                                           type,
                                           thunder);

    TextGenPosixTime startTimeFull;
    TextGenPosixTime endTimeFull;
    thePrecipitationForecast.getPrecipitationPeriod(
        precipitationPeriods[i].localStartTime(), startTimeFull, endTimeFull);
    // This parameter shows if the precipitation starts
    // before forecast period or continues after
    int precipitationFullDuration = endTimeFull.DifferenceInHours(startTimeFull);
    item->theFullDuration = precipitationFullDuration;

    if (previousPrItem != 0)
    {
      TextGenPosixTime startTime(previousPrItem->thePeriod.localEndTime());
      TextGenPosixTime endTime(precipitationPeriods[i].localStartTime());
      if (endTime.DifferenceInHours(startTime) > 1)
      {
        startTime.ChangeByHours(1);
        endTime.ChangeByHours(-1);

        // placeholder for some other story item between precipitation periods
        missingStoryItem = new WeatherForecastStoryItem(
            *this, WeatherPeriod(startTime, endTime), MISSING_STORY_PART);

        theStoryItemVector.push_back(missingStoryItem);
      }
    }
    // precipitation story item
    theStoryItemVector.push_back(item);

    previousPrItem = item;
  }

  if (theStoryItemVector.size() == 0)
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
    TextGenPosixTime firstPeriodStartTime(theForecastPeriod.localStartTime());
    TextGenPosixTime firstPeriodEndTime(theStoryItemVector[0]->thePeriod.localStartTime());

    WeatherPeriod firstPeriod(theForecastPeriod.localStartTime(),
                              theStoryItemVector[0]->thePeriod.localStartTime());

    if (firstPeriodEndTime.DifferenceInHours(firstPeriodStartTime) > 0)
    {
      firstPeriodEndTime.ChangeByHours(-1);
      WeatherPeriod firstPeriod(firstPeriodStartTime, firstPeriodEndTime);
      missingStoryItem = new WeatherForecastStoryItem(*this, firstPeriod, MISSING_STORY_PART);
      theStoryItemVector.insert(theStoryItemVector.begin(), missingStoryItem);
    }

    TextGenPosixTime lastPeriodStartTime(
        theStoryItemVector[theStoryItemVector.size() - 1]->thePeriod.localEndTime());
    TextGenPosixTime lastPeriodEndTime(theForecastPeriod.localEndTime());
    // chek if the last period is missing
    if (lastPeriodEndTime.DifferenceInHours(lastPeriodStartTime) > 0)
    {
      lastPeriodStartTime.ChangeByHours(1);
      WeatherPeriod lastPeriod(lastPeriodStartTime, lastPeriodEndTime);

      missingStoryItem = new WeatherForecastStoryItem(*this, lastPeriod, MISSING_STORY_PART);
      theStoryItemVector.push_back(missingStoryItem);
    }

    bool emptyPeriodsFound = true;
    while (emptyPeriodsFound)
    {
      emptyPeriodsFound = false;
      // place placeholder in the missing slots
      for (unsigned int i = 1; i < theStoryItemVector.size(); i++)
      {
        TextGenPosixTime middlePeriodStartTime(theStoryItemVector[i - 1]->thePeriod.localEndTime());
        TextGenPosixTime middlePeriodEndTime(theStoryItemVector[i]->thePeriod.localStartTime());

        if (middlePeriodEndTime.DifferenceInHours(middlePeriodStartTime) > 1)
        {
          WeatherPeriod middlePeriod(theStoryItemVector[i - 1]->thePeriod.localEndTime(),
                                     theStoryItemVector[i]->thePeriod.localStartTime());

          middlePeriodStartTime.ChangeByHours(1);
          middlePeriodEndTime.ChangeByHours(-1);
          missingStoryItem = new WeatherForecastStoryItem(
              *this, WeatherPeriod(middlePeriodStartTime, middlePeriodEndTime), MISSING_STORY_PART);
          theStoryItemVector.insert(theStoryItemVector.begin() + i, missingStoryItem);
          emptyPeriodsFound = true;
          break;
        }
      }
    }
  }
}

void WeatherForecastStory::addCloudinessStoryItems()
{  // replace the missing story items with the cloudiness story part
  PrecipitationForecastStoryItem* previousPrecipitationStoryItem = 0;
  for (unsigned int i = 0; i < theStoryItemVector.size(); i++)
  {
    if (theStoryItemVector[i]->theStoryPartId == MISSING_STORY_PART)
    {
      WeatherForecastStoryItem* placeholder = theStoryItemVector[i];

      CloudinessForecastStoryItem* cloudinessStoryItem = new CloudinessForecastStoryItem(
          *this,
          placeholder->thePeriod,
          CLOUDINESS_STORY_PART,
          theCloudinessForecast.getCloudinessId(placeholder->thePeriod),
          previousPrecipitationStoryItem,
          0);

      if (!previousPrecipitationStoryItem)
      {
        cloudinessStoryItem->theReportAboutDryWeatherFlag = true;
      }

      cloudinessStoryItem->thePreviousPrecipitationStoryItem = previousPrecipitationStoryItem;

      if (i < theStoryItemVector.size() - 1 &&
          theStoryItemVector[i + 1]->theStoryPartId == PRECIPITATION_STORY_PART)
        cloudinessStoryItem->theNextPrecipitationStoryItem =
            static_cast<PrecipitationForecastStoryItem*>(theStoryItemVector[i + 1]);

      theStoryItemVector[i] = cloudinessStoryItem;

      delete placeholder;
    }
    else if (theStoryItemVector[i]->theStoryPartId == PRECIPITATION_STORY_PART)
    {
      previousPrecipitationStoryItem =
          static_cast<PrecipitationForecastStoryItem*>(theStoryItemVector[i]);
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
  for (unsigned int i = 0; i < theStoryItemVector.size(); i++)
  {
    if (theStoryItemVector[i]->theStoryPartId == PRECIPITATION_STORY_PART)
    {
      currentPrecipitationStoryItem =
          static_cast<PrecipitationForecastStoryItem*>(theStoryItemVector[i]);
      if (previousPrecipitationStoryItem)
      {
        if (currentPrecipitationStoryItem->thePeriod.localStartTime().DifferenceInHours(
                previousPrecipitationStoryItem->thePeriod.localEndTime()) <= 2)
          previousPrecipitationStoryItem->theReportPoutaantuuFlag = false;

        // if the type is different don't merge, except when thunder exists on both periods
        if (previousPrecipitationStoryItem->theType != currentPrecipitationStoryItem->theType &&
            !(previousPrecipitationStoryItem->theThunder &&
              currentPrecipitationStoryItem->theThunder))
        {
          // if the dry period between precpitation periods is short don't mention it
          if (theStoryItemVector[i - 1] != previousPrecipitationStoryItem &&
              theStoryItemVector[i - 1]->storyItemPeriodLength() <= 1)
          {
            theStoryItemVector[i - 1]->theIncludeInTheStoryFlag = false;
          }
          previousPrecipitationStoryItem = currentPrecipitationStoryItem;
          continue;
        }

        TextGenPosixTime gapPeriodStartTime(
            previousPrecipitationStoryItem->thePeriod.localEndTime());
        gapPeriodStartTime.ChangeByHours(+1);
        TextGenPosixTime gapPeriodEndTime(
            currentPrecipitationStoryItem->thePeriod.localStartTime());
        if (gapPeriodStartTime < gapPeriodEndTime) gapPeriodEndTime.ChangeByHours(-1);
        WeatherPeriod gapPeriod(gapPeriodStartTime, gapPeriodEndTime);

        // merge periods
        if (get_period_length(gapPeriod) <= 3 &&
            (get_period_length(gapPeriod) <=
             currentPrecipitationStoryItem->storyItemPeriodLength() +
                 previousPrecipitationStoryItem->storyItemPeriodLength() + 2))
        {
          // merge two weak precipitation periods
          previousPrecipitationStoryItem->thePeriodToMergeWith = currentPrecipitationStoryItem;
          currentPrecipitationStoryItem->thePeriodToMergeTo = previousPrecipitationStoryItem;
          if (theStoryItemVector[i - 1]->theStoryPartId == CLOUDINESS_STORY_PART)
            theStoryItemVector[i - 1]->theIncludeInTheStoryFlag = false;
        }
      }
      previousPrecipitationStoryItem = currentPrecipitationStoryItem;
    }
  }

  int storyItemCount = theStoryItemVector.size();
  // short precipitation period in the beginning is ignored
  if (storyItemCount > 1)
  {
    WeatherPeriod storyItemPeriod(theStoryItemVector[0]->getStoryItemPeriod());

    if (theStoryItemVector[0]->theStoryPartId == PRECIPITATION_STORY_PART &&
        theStoryItemVector[1]->theStoryPartId == CLOUDINESS_STORY_PART &&
        theStoryItemVector[0]->getPeriodLength() <= 1 &&
        theStoryItemVector[0]->thePeriodToMergeWith == 0)
    {
      theStoryItemVector[0]->theIncludeInTheStoryFlag = false;
    }
    else if (theStoryItemVector[0]->theStoryPartId == CLOUDINESS_STORY_PART &&
             theStoryItemVector[1]->theStoryPartId == PRECIPITATION_STORY_PART &&
             theStoryItemVector[0]->getPeriodLength() <= 1 &&
             theStoryItemVector[0]->thePeriodToMergeWith == 0 &&
             static_cast<PrecipitationForecastStoryItem*>(theStoryItemVector[1])->theIntensity <
                 0.1)
    /* &&
           static_cast<PrecipitationForecastStoryItem*>(theStoryItemVector[1])->theExtent < 40)*/
    {
      theStoryItemVector[0]->theIncludeInTheStoryFlag = false;
    }

    // short precipitation period in the end after coudiness period is not reported
    if (theStoryItemVector[storyItemCount - 1]->theStoryPartId == PRECIPITATION_STORY_PART &&
        theStoryItemVector[storyItemCount - 1]->getPeriodLength() <= 1 &&
        theStoryItemVector[storyItemCount - 1]->thePeriodToMergeTo == 0 &&
        theStoryItemVector[storyItemCount - 2]->theStoryPartId == CLOUDINESS_STORY_PART &&
        theStoryItemVector[storyItemCount - 2]->theIncludeInTheStoryFlag == true)
    {
      theStoryItemVector[storyItemCount - 1]->theIncludeInTheStoryFlag = false;
    }
  }
}

void WeatherForecastStory::mergeCloudinessPeriodsWhenFeasible()
{
  CloudinessForecastStoryItem* previousCloudinessStoryItem = 0;
  CloudinessForecastStoryItem* currentCloudinessStoryItem = 0;
  for (unsigned int i = 0; i < theStoryItemVector.size(); i++)
  {
    if (theStoryItemVector[i]->theStoryPartId == CLOUDINESS_STORY_PART &&
        theStoryItemVector[i]->theIncludeInTheStoryFlag == true)
    {
      currentCloudinessStoryItem = static_cast<CloudinessForecastStoryItem*>(theStoryItemVector[i]);
      if (previousCloudinessStoryItem)
      {
        TextGenPosixTime gapPeriodStartTime(previousCloudinessStoryItem->thePeriod.localEndTime());
        gapPeriodStartTime.ChangeByHours(+1);
        TextGenPosixTime gapPeriodEndTime(currentCloudinessStoryItem->thePeriod.localStartTime());
        if (gapPeriodStartTime < gapPeriodEndTime) gapPeriodEndTime.ChangeByHours(-1);
        WeatherPeriod gapPeriod(gapPeriodStartTime, gapPeriodEndTime);

        // merge periods if the precipitation period in between is short and
        // intensity is weak and precipitation area is small
        if (get_period_length(gapPeriod) <= 1 &&
            currentCloudinessStoryItem->thePreviousPrecipitationStoryItem->theIntensity <= 0.1)
        /* &&
               currentCloudinessStoryItem->thePreviousPrecipitationStoryItem->theExtent <= 40)*/
        {
          // merge two cloudiness periods
          previousCloudinessStoryItem->thePeriodToMergeWith = currentCloudinessStoryItem;
          currentCloudinessStoryItem->thePeriodToMergeTo = previousCloudinessStoryItem;
          currentCloudinessStoryItem->thePreviousPrecipitationStoryItem->theIncludeInTheStoryFlag =
              false;
        }
      }
      previousCloudinessStoryItem = currentCloudinessStoryItem;
    }
  }

  int storyItemCount = theStoryItemVector.size();
  // short cloudiness period in the beginning is ignored
  if (storyItemCount > 1)
  {
    WeatherPeriod storyItemPeriod(theStoryItemVector[0]->getStoryItemPeriod());

    if (theStoryItemVector[0]->theStoryPartId == CLOUDINESS_STORY_PART &&
        theStoryItemVector[1]->theStoryPartId == PRECIPITATION_STORY_PART &&
        theStoryItemVector[0]->getPeriodLength() <= 1 &&
        theStoryItemVector[0]->thePeriodToMergeWith == 0)
    {
      theStoryItemVector[0]->theIncludeInTheStoryFlag = false;
    }
    else if (theStoryItemVector[0]->theStoryPartId == PRECIPITATION_STORY_PART &&
             theStoryItemVector[1]->theStoryPartId == CLOUDINESS_STORY_PART &&
             theStoryItemVector[0]->getPeriodLength() <= 1 &&
             theStoryItemVector[0]->thePeriodToMergeWith == 0 &&
             static_cast<PrecipitationForecastStoryItem*>(theStoryItemVector[0])->theIntensity <
                 0.1)
    /* &&
       static_cast<PrecipitationForecastStoryItem*>(theStoryItemVector[0])->theExtent < 40)*/
    {
      theStoryItemVector[0]->theIncludeInTheStoryFlag = false;
    }

    // short cloudiness period in the end after precipitation period is not reported
    if (theStoryItemVector[storyItemCount - 1]->theStoryPartId == CLOUDINESS_STORY_PART &&
        theStoryItemVector[storyItemCount - 1]->getPeriodLength() <= 1 &&
        theStoryItemVector[storyItemCount - 1]->thePeriodToMergeTo == 0 &&
        theStoryItemVector[storyItemCount - 2]->theStoryPartId == PRECIPITATION_STORY_PART &&
        theStoryItemVector[storyItemCount - 2]->theIncludeInTheStoryFlag == true)
    {
      theStoryItemVector[storyItemCount - 1]->theIncludeInTheStoryFlag = false;
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

  if (theReportTimePhraseFlag)
  {
    if (theStorySize == 0)
      sentence << ALUKSI_WORD;
    else
      sentence << MYOHEMMIN_WORD;
  }

  return sentence;
}

void WeatherForecastStory::logTheStoryItems() const
{
  theLogger << "******** STORY ITEMS ********" << endl;
  for (unsigned int i = 0; i < theStoryItemVector.size(); i++)
  {
    theLogger << *theStoryItemVector[i];
  }
}

WeatherForecastStoryItem::WeatherForecastStoryItem(WeatherForecastStory& weatherForecastStory,
                                                   const WeatherPeriod& period,
                                                   story_part_id storyPartId)
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

  theAdditionalSentences.clear();

  if (theIncludeInTheStoryFlag)
  {
    sentence << getStoryItemSentence();
  }

  return sentence;
}

Sentence WeatherForecastStoryItem::getAdditionalSentence(unsigned int index) const
{
  if (index >= theAdditionalSentences.size())
    return Sentence();
  else
    return theAdditionalSentences[index];
}

WeatherPeriod WeatherForecastStoryItem::getStoryItemPeriod() const
{
  if (thePeriodToMergeWith)
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
                                                          unsigned int theBegIndex,
                                                          unsigned int theEndIndex)
{
  Sentence sentence;

  for (unsigned int i = theBegIndex; i <= theEndIndex; i++)
  {
    if (sentence.size() > 0) sentence << JA_WORD;
    sentence << *(todayVector[i]);
  }
  return sentence;
}

// special treatment, because 06:00 can be aamuyö and morning, depends weather the period starts or
// ends
std::string WeatherForecastStoryItem::checkForAamuyoAndAamuPhrase(
    bool theFromSpecifier, const WeatherPeriod& thePhrasePeriod)
{
  std::string retValue("");

  // 6:00 in the morning or in the evening
  if (is_inside(thePhrasePeriod.localStartTime(), AAMU) &&
      is_inside(thePhrasePeriod.localEndTime(), AAMU) &&
      is_inside(thePhrasePeriod.localStartTime(), AAMUYO) &&
      is_inside(thePhrasePeriod.localEndTime(), AAMUYO))
  {
    if (thePhrasePeriod.localStartTime() ==
        theWeatherForecastStory.theForecastPeriod.localStartTime())
    {
      retValue = (theFromSpecifier ? AAMUSTA_ALKAEN_PHRASE : AAMULLA_WORD);
    }
    else
    {
      retValue = (theFromSpecifier ? AAMUYOSTA_ALKAEN_PHRASE : AAMUYOLLA_WORD);
    }
  }

  return retValue;
}

part_of_the_day_id day_part_id_at_sea(const WeatherPeriod& period)
{
  part_of_the_day_id ret = MISSING_PART_OF_THE_DAY_ID;

  /*
  int startHour = period.localStartTime().GetHour();
  int endHour = period.localEndTime().GetHour();

  bool sameDay = (phrasePeriod.localStartTime().GetDay() == phrasePeriod.localEndTime().GetDay());
    if (startHour >= 21 && endHour <= 6 && !sameDay) sentence << YOLLA_WORD;
  */
  return ret;
}

Sentence WeatherForecastStoryItem::getPeriodPhrase()
{
  Sentence sentence;

  WeatherPeriod phrasePeriod = getStoryItemPeriod();
  int phrasePeriodLength = get_period_length(phrasePeriod);

  // first try the shortest part of the day period
  part_of_the_day_id id = get_part_of_the_day_id_narrow(phrasePeriod);

  // then try longer part of day
  if (id == MISSING_PART_OF_THE_DAY_ID) id = get_part_of_the_day_id_large(phrasePeriod);
  if (id != MISSING_PART_OF_THE_DAY_ID)
  {
    if (id == PAIVA)
      sentence << PAIVALLA_WORD;
    else if (id == YO)
      sentence << YOLLA_WORD;
    else
      sentence << get_time_phrase_from_id(id, theWeatherForecastStory.theVar);
  }
  else
  {
    // TODO:
    if (phrasePeriodLength > 13)
    {
      /*
  std::cout << "KÄÄK: " << theWeatherForecastStory.theWeatherArea.name() << ", "
            << phrasePeriodLength << " -> " << as_string(phrasePeriod) << std::endl;
      */
    }

    int startHour = phrasePeriod.localStartTime().GetHour();
    int endHour = phrasePeriod.localEndTime().GetHour();
    bool sameDay = (phrasePeriod.localStartTime().GetDay() == phrasePeriod.localEndTime().GetDay());
    if (startHour >= 21 && endHour <= 6 && !sameDay)
      sentence << YOLLA_WORD;
    else if (startHour >= 5 && endHour <= 10 && sameDay)
      sentence << AAMULLA_WORD;
    else if (startHour >= 10 && endHour <= 18 && sameDay)
      sentence << PAIVALLA_WORD;
    else if (startHour >= 17 && endHour <= 22 && sameDay)
      sentence << ILLALLA_WORD;
    else if (startHour >= 21 && endHour <= 10 && !sameDay)
      sentence << YOLLA_JA_AAMULLA_PHRASE;
    else if (startHour >= 5 && endHour <= 18 && sameDay)
      sentence << AAMULLA_JA_PAIVALLA_PHRASE;
    else if (startHour >= 10 && endHour <= 22 && sameDay)
      sentence << PAIVALLA_JA_ILLALLA_PHRASE;
    else if (startHour >= 17 && endHour <= 6 && !sameDay)
      sentence << ILLALLA_JA_YOLLA_PHRASE;
  }

  return sentence;
}

Sentence WeatherForecastStoryItem::getPeriodPhrase(bool theFromSpecifier,
                                                   const WeatherPeriod* thePhrasePeriod /*= 0*/,
                                                   bool theStoryUnderConstructionEmpty /*= true*/)
{
  Sentence sentence;

  sentence << getPeriodPhrase();
  if (!sentence.empty())
  {
    //    std::cout << "OUJE2: " << as_string(sentence) << std::endl;
    return sentence;
  }

  WeatherPeriod phrasePeriod(thePhrasePeriod == 0 ? getStoryItemPeriod() : *thePhrasePeriod);
  bool specifyDay = false;
  /*
  if (forecastPeriodLength() > 24 &&
      theWeatherForecastStory.theForecastTime.GetJulianDay() !=
          phrasePeriod.localStartTime().GetJulianDay() &&
      abs(theWeatherForecastStory.theForecastTime.DifferenceInHours(
          phrasePeriod.localStartTime())) >= 21)
  */
  if (forecastPeriodLength() > 24 &&
      theWeatherForecastStory.theForecastTime.GetJulianDay() !=
          phrasePeriod.localStartTime().GetJulianDay() &&
      abs(theWeatherForecastStory.theForecastTime.DifferenceInHours(
          phrasePeriod.localStartTime())) >= 6)
  {
    Sentence todaySentence;
    todaySentence << PeriodPhraseFactory::create("today",
                                                 theWeatherForecastStory.theVar,
                                                 theWeatherForecastStory.theForecastTime,
                                                 phrasePeriod,
                                                 theWeatherForecastStory.theWeatherArea);
    if (todaySentence.size() > 0) specifyDay = true;
  }

  std::string day_phase_phrase("");

  day_phase_phrase = checkForAamuyoAndAamuPhrase(theFromSpecifier, phrasePeriod);
  WeatherHistory& thePhraseHistory =
      const_cast<WeatherArea&>(theWeatherForecastStory.theWeatherArea).history();

  if (!day_phase_phrase.empty())
  {
    if (day_phase_phrase != thePhraseHistory.latestDayPhasePhrase)
    {
      thePhraseHistory.updateDayPhasePhrase(day_phase_phrase);
    }
    sentence << day_phase_phrase;
  }

  if (sentence.size() == 0)
  {
    part_of_the_day_id id;
    get_time_phrase_large(phrasePeriod,
                          specifyDay,
                          theWeatherForecastStory.theVar,
                          day_phase_phrase,
                          theFromSpecifier,
                          id);

    if (day_phase_phrase != thePhraseHistory.latestDayPhasePhrase)
    {
      thePhraseHistory.updateDayPhasePhrase(day_phase_phrase);
    }

    sentence << day_phase_phrase;
  }

  theWeatherForecastStory.theLogger << "PHRASE PERIOD: " << phrasePeriod.localStartTime() << "..."
                                    << phrasePeriod.localEndTime() << endl;
  theWeatherForecastStory.theLogger << "PHRASE: ";
  theWeatherForecastStory.theLogger << sentence;

  return sentence;
}

PrecipitationForecastStoryItem::PrecipitationForecastStoryItem(
    WeatherForecastStory& weatherForecastStory,
    const WeatherPeriod& period,
    story_part_id storyPartId,
    float intensity,
    float extent,
    unsigned int form,
    precipitation_type type,
    bool thunder)
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

bool PrecipitationForecastStoryItem::inManyPlaces() const
{
  return (theExtent > IN_MANY_PLACES_LOWER_LIMIT && theExtent <= IN_MANY_PLACES_UPPER_LIMIT);
}

#ifdef LATER
Sentence PrecipitationForecastStoryItem::getStoryItemSentence()
{
  Sentence sentence;

  const PrecipitationForecast& prForecast = theWeatherForecastStory.thePrecipitationForecast;
  WeatherPeriod forecastPeriod = theWeatherForecastStory.theForecastPeriod;
  WeatherPeriod storyItemPeriod(getStoryItemPeriod());
  Sentence thePeriodPhrase;

  if (storyItemPeriodLength() >= 6)
  {
    if (storyItemPeriod.localStartTime() != forecastPeriod.localStartTime())
    {
      if (storyItemPeriod.localStartTime() > forecastPeriod.localStartTime())
        thePeriodPhrase << getPeriodPhrase(USE_FROM_SPECIFIER);
      if (thePeriodPhrase.size() == 0) thePeriodPhrase << theWeatherForecastStory.getTimePhrase();

      sentence << prForecast.precipitationChangeSentence(
          storyItemPeriod, thePeriodPhrase, SADE_ALKAA, theAdditionalSentences);
    }
    else
    {
      if (storyItemPeriod.localStartTime() > forecastPeriod.localStartTime())
        thePeriodPhrase << getPeriodPhrase(DONT_USE_FROM_SPECIFIER, &storyItemPeriod);
      if (thePeriodPhrase.size() == 0) thePeriodPhrase << theWeatherForecastStory.getTimePhrase();

      sentence << prForecast.precipitationSentence(
          storyItemPeriod, thePeriodPhrase, theAdditionalSentences);
    }

    if (storyItemPeriod.localEndTime() != forecastPeriod.localEndTime() && theReportPoutaantuuFlag)
    {
      WeatherPeriod poutaantuuPeriod(storyItemPeriod.localEndTime(),
                                     storyItemPeriod.localEndTime());
      thePeriodPhrase << getPeriodPhrase(
          DONT_USE_FROM_SPECIFIER, &poutaantuuPeriod, sentence.size() == 0);
      theWeatherForecastStory.theLogger << thePeriodPhrase;
      if (thePeriodPhrase.size() == 0) thePeriodPhrase << theWeatherForecastStory.getTimePhrase();

      if (sentence.size() > 0) sentence << Delimiter(",");
      sentence << prForecast.precipitationChangeSentence(
          storyItemPeriod, thePeriodPhrase, POUTAANTUU, theAdditionalSentences);
    }
    theWeatherForecastStory.theShortTimePrecipitationReportedFlag = false;
  }
  else
  {
    if (thePeriod.localStartTime() > forecastPeriod.localStartTime())
      thePeriodPhrase << getPeriodPhrase(DONT_USE_FROM_SPECIFIER, &storyItemPeriod);
    if (thePeriodPhrase.size() == 0) thePeriodPhrase << theWeatherForecastStory.getTimePhrase();

    if (prForecast.shortTermPrecipitationExists(thePeriod))
    {
      sentence << prForecast.shortTermPrecipitationSentence(thePeriod, thePeriodPhrase);
      theWeatherForecastStory.theShortTimePrecipitationReportedFlag = true;
    }
    else
    {
      sentence << prForecast.precipitationSentence(
          storyItemPeriod, thePeriodPhrase, theAdditionalSentences);
    }
  }

  return sentence;
}
#endif

Sentence PrecipitationForecastStoryItem::getStoryItemSentence()
{
  Sentence sentence;

  const PrecipitationForecast& prForecast = theWeatherForecastStory.thePrecipitationForecast;
  WeatherPeriod forecastPeriod = theWeatherForecastStory.theForecastPeriod;
  WeatherPeriod storyItemPeriod(getStoryItemPeriod());

  float fractionOfForecastPeriod = static_cast<float>(get_period_length(storyItemPeriod)) /
                                   static_cast<float>(get_period_length(forecastPeriod));

  // if story part is more than 70 % of forecast period return empty phrase
  if (fractionOfForecastPeriod >= 0.7) return sentence;

  Sentence thePeriodPhrase;
  thePeriodPhrase << getPeriodPhrase(DONT_USE_FROM_SPECIFIER);

  if (storyItemPeriodLength() >= 6)
  {
    sentence << prForecast.precipitationSentence(
        storyItemPeriod, thePeriodPhrase, theAdditionalSentences);
  }
  else
  {
    if (prForecast.shortTermPrecipitationExists(thePeriod))
    {
      sentence << prForecast.shortTermPrecipitationSentence(thePeriod, thePeriodPhrase);
      theWeatherForecastStory.theShortTimePrecipitationReportedFlag = true;
    }
    else
    {
      sentence << prForecast.precipitationSentence(
          storyItemPeriod, thePeriodPhrase, theAdditionalSentences);
    }
  }

  return sentence;
}
CloudinessForecastStoryItem::CloudinessForecastStoryItem(
    WeatherForecastStory& weatherForecastStory,
    const WeatherPeriod& period,
    story_part_id storyPartId,
    cloudiness_id cloudinessId,
    PrecipitationForecastStoryItem* previousPrecipitationStoryItem,
    PrecipitationForecastStoryItem* nextPrecipitationStoryItem)
    : WeatherForecastStoryItem(weatherForecastStory, period, storyPartId),
      theCloudinessId(cloudinessId),
      thePreviousPrecipitationStoryItem(previousPrecipitationStoryItem),
      theNextPrecipitationStoryItem(nextPrecipitationStoryItem),
      theReportAboutDryWeatherFlag(true)

{
  if (thePreviousPrecipitationStoryItem && thePreviousPrecipitationStoryItem->isIncluded())
  {
    if (thePreviousPrecipitationStoryItem->thePoutaantuuFlag)
    {
      thePreviousPrecipitationStoryItem->theReportPoutaantuuFlag = false;
      theReportAboutDryWeatherFlag = false;
    }
  }
}

Sentence CloudinessForecastStoryItem::cloudinessChangeSentence()
{
  Sentence sentence;

  weather_event_id_vector cloudinessEvents;  // pilvistyy and selkenee

  theWeatherForecastStory.theCloudinessForecast.getWeatherEventIdVector(cloudinessEvents);

  for (unsigned int i = 0; i < cloudinessEvents.size(); i++)
  {
    TextGenPosixTime cloudinessEventTimestamp(cloudinessEvents.at(i).first);
    if (cloudinessEventTimestamp >= thePeriod.localStartTime() &&
        cloudinessEventTimestamp <= thePeriod.localEndTime())
    {
      sentence << theWeatherForecastStory.theCloudinessForecast.cloudinessChangeSentence(
          WeatherPeriod(cloudinessEventTimestamp, cloudinessEventTimestamp));

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
  if (thePeriodToMergeTo) return sentence;

  WeatherPeriod storyItemPeriod(getStoryItemPeriod());
  // if the cloudiness period is max 2 hours and it is in the end of the forecast period and
  // the previous precipitation period is long > 6h -> don't report cloudiness
  if (storyItemPeriod.localEndTime() == theWeatherForecastStory.theForecastPeriod.localEndTime() &&
      storyItemPeriodLength() <= 2)
  {
    if (thePreviousPrecipitationStoryItem && thePreviousPrecipitationStoryItem->isIncluded() &&
        (thePreviousPrecipitationStoryItem->storyItemPeriodLength() >= 6 ||
         get_part_of_the_day_id_narrow(thePreviousPrecipitationStoryItem->getStoryItemPeriod()) ==
             get_part_of_the_day_id_narrow(getStoryItemPeriod())))
      return sentence;
  }

  const CloudinessForecast& clForecast = theWeatherForecastStory.theCloudinessForecast;
  const PrecipitationForecast& prForecast = theWeatherForecastStory.thePrecipitationForecast;

  theSentence.clear();
  theChangeSentence.clear();
  theShortFormSentence.clear();
  thePoutaantuuSentence.clear();

  theChangeSentence << cloudinessChangeSentence();

  if (thePreviousPrecipitationStoryItem && thePreviousPrecipitationStoryItem->isIncluded())
  {
    if (thePreviousPrecipitationStoryItem->thePoutaantuuFlag)
    {
      cloudiness_id cloudinessId = clForecast.getCloudinessId(getStoryItemPeriod());

      WeatherPeriod poutaantuuPeriod(
          thePreviousPrecipitationStoryItem->getStoryItemPeriod().localEndTime(),
          thePreviousPrecipitationStoryItem->getStoryItemPeriod().localEndTime());

      // ARE 22.02.2011: The missing period-phrase added
      Sentence thePeriodPhrase(getPeriodPhrase(USE_FROM_SPECIFIER, &poutaantuuPeriod));
      if (thePeriodPhrase.size() == 0) thePeriodPhrase << theWeatherForecastStory.getTimePhrase();
      thePoutaantuuSentence << prForecast.precipitationPoutaantuuAndCloudiness(thePeriodPhrase,
                                                                               cloudinessId);
      thePreviousPrecipitationStoryItem->theReportPoutaantuuFlag = false;
      theReportAboutDryWeatherFlag = false;
    }
  }

  if (!thePoutaantuuSentence.empty())
  {
    sentence << thePoutaantuuSentence;

    // ARE 10.03.2011: Jos sää on melko selkeä ei enää sanota selkenevää
    if (theChangeSentence.size() > 0 && clForecast.getCloudinessId(storyItemPeriod) > MELKO_SELKEA)
    {
      sentence << Delimiter(COMMA_PUNCTUATION_MARK);
      sentence << theChangeSentence;
    }
  }
  else
  {
    Sentence thePeriodPhrase;
    if (storyItemPeriod.localStartTime() >
        theWeatherForecastStory.theForecastPeriod.localStartTime())
    {
      if (storyItemPeriodLength() >= 6)
      {
        if (theWeatherForecastStory.theStorySize > 0)
          thePeriodPhrase << getPeriodPhrase(USE_FROM_SPECIFIER);
      }
      else
      {
        thePeriodPhrase << getPeriodPhrase(DONT_USE_FROM_SPECIFIER, &storyItemPeriod);
      }
    }
    if (thePeriodPhrase.size() == 0) thePeriodPhrase << theWeatherForecastStory.getTimePhrase();

    if (theChangeSentence.size() > 0)
    {
      WeatherPeriod clPeriod(storyItemPeriod.localStartTime(), theCloudinessChangeTimestamp);
      sentence << clForecast.cloudinessSentence(
          clPeriod,
          theReportAboutDryWeatherFlag && !prForecast.getDryPeriodTautologyFlag(),
          thePeriodPhrase,
          DONT_USE_SHORT_FORM);
    }
    else
    {
      sentence << clForecast.cloudinessSentence(
          storyItemPeriod,
          theReportAboutDryWeatherFlag && !prForecast.getDryPeriodTautologyFlag(),
          thePeriodPhrase,
          DONT_USE_SHORT_FORM);
    }
    prForecast.setDryPeriodTautologyFlag(theReportAboutDryWeatherFlag);

    // ARE 10.03.2011: Jos sää on melko selkeä ei enää sanota selkenevää
    if (theChangeSentence.size() > 0 &&
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
