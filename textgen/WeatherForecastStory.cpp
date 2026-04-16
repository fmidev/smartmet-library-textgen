#include <iostream>
#include <string>

#include "Delimiter.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "Sentence.h"
#include <boost/algorithm/string/classification.hpp>
#include <boost/range/algorithm/remove_if.hpp>

#include <calculator/Settings.h>
#include <calculator/WeatherArea.h>
#include <calculator/WeatherPeriod.h>
#include <macgyver/StringConversion.h>

#include "CloudinessForecast.h"
#include "FogForecast.h"
#include "PrecipitationForecast.h"
#include "ThunderForecast.h"
#include "WeatherForecastStory.h"

using namespace std;

#define USE_FROM_SPECIFIER true
#define DONT_USE_FROM_SPECIFIER false
#define USE_SHORT_FORM true
#define DONT_USE_SHORT_FORM false
#define USE_TIME_SPECIFIER true
#define DONT_USE_TIME_SPECIFIER false

namespace TextGen
{
using namespace TextGen;
using namespace Settings;

namespace
{

std::ostream& operator<<(std::ostream& theOutput, const WeatherForecastStoryItem& theStoryItem)
{
  theOutput << theStoryItem.getPeriod().localStartTime() << "..."
            << theStoryItem.getPeriod().localEndTime() << " ";
  theOutput << story_part_id_string(theStoryItem.getStoryPartId()) << " - ";
  if (theStoryItem.isIncluded())
  {
    theOutput << "included";
    if (theStoryItem.thePeriodToMergeWith)
      theOutput << " - merged with period "
                << as_string(theStoryItem.thePeriodToMergeWith->thePeriod) << '\n';
    else
      theOutput << " \n";
  }
  else
    theOutput << "exluded\n";

  return theOutput;
}

std::vector<unsigned int> get_story_item_indexes(
    const std::vector<std::shared_ptr<WeatherForecastStoryItem>>& storyItems)
{
  std::vector<unsigned int> indexes;

  for (unsigned int i = 0; i < storyItems.size(); i++)
    if (storyItems.at(i)->theIncludeInTheStoryFlag)
      indexes.push_back(i);

  return indexes;
}
}  // namespace

WeatherForecastStory::WeatherForecastStory(const std::string& var,
                                           const WeatherPeriod& forecastPeriod,
                                           const WeatherArea& weatherArea,
                                           wf_story_params& parameters,
                                           const TextGenPosixTime& forecastTime,
                                           PrecipitationForecast& precipitationForecast,
                                           const CloudinessForecast& cloudinessForecast,
                                           const FogForecast& fogForecast,
                                           const ThunderForecast& thunderForecast,
                                           MessageLogger& logger)
    : theVar(var),
      theForecastPeriod(forecastPeriod),
      theWeatherArea(weatherArea),
      theParameters(parameters),
      theForecastTime(forecastTime),
      thePrecipitationForecast(precipitationForecast),
      theCloudinessForecast(cloudinessForecast),
      theFogForecast(fogForecast),
      theThunderForecast(thunderForecast),
      theLogger(logger),
      thePeriodPhraseGenerator(var)
{
  addPrecipitationStoryItems();
  addCloudinessStoryItems();
  mergePeriodsWhenFeasible();

  bool specifyPartOfTheDayFlag =
      Settings::optional_bool(theVar + "::specify_part_of_the_day", true);
  bool useInTheBeginningPhrase =
      Settings::optional_bool(theVar + "::use_in_the_beginning_phrase", true);
  int storyItemCounter(0);
  int storyItemCounterTotal(0);
  bool moreThanOnePrecipitationForms(false);
  for (auto& i : theStoryItemVector)
  {
    // when specifyPartOfTheDayFlag is false it means
    // that the period is short and part of the day should not be expressed
    if (!specifyPartOfTheDayFlag)
    {
      // ARE 14.4.2011: checking theIncludeInTheStoryFlag
      if (i->theIncludeInTheStoryFlag && !i->getSentence().empty())
        storyItemCounter++;
    }

    if (!i->getSentence().empty())
      storyItemCounterTotal++;

    // check wheather more than one precipitation form exists during the forecast period
    if (!moreThanOnePrecipitationForms && i->theStoryPartId == PRECIPITATION_STORY_PART &&
        i->theIncludeInTheStoryFlag)
    {
      precipitation_form_id precipitationForm = thePrecipitationForecast.getPrecipitationForm(
          i->thePeriod, theParameters.theForecastArea);

      moreThanOnePrecipitationForms = !PrecipitationForecast::singleForm(precipitationForm);
    }
  }
  // if more than one item exists, use the phrases "aluksi", "myöhemmin" when the period is short
  theReportTimePhraseFlag = (storyItemCounter > 1);
  theAddAluksiWord = (useInTheBeginningPhrase && storyItemCounterTotal > 1);
  thePrecipitationForecast.setSinglePrecipitationFormFlag(!moreThanOnePrecipitationForms);
}

WeatherForecastStory::~WeatherForecastStory()
{
  theStoryItemVector.clear();
}

Sentence WeatherForecastStory::getPeriodPhrase(const WeatherPeriod& period)
{
  return thePeriodPhraseGenerator.getPeriodPhrase(period);
}

Paragraph WeatherForecastStory::getWeatherForecastStory()
{
  theLogger << "Start processing weather forcast at land\n";
  thePeriodPhraseGenerator.reset();

  Paragraph paragraph;

  theShortTimePrecipitationReportedFlag = false;
  theCloudinessReportedFlag = false;
  theStorySize = 0;

  thePrecipitationForecast.setDryPeriodTautologyFlag(false);

  for (auto& i : theStoryItemVector)
  {
    WeatherForecastStoryItem& item = *i;

    Sentence storyItemSentence;
    storyItemSentence << item.getSentence();

    if (!storyItemSentence.empty())
    {
      theStorySize += storyItemSentence.size();

      paragraph << storyItemSentence;

      // additional sentences: currently only in precipitation story: like "iltapäivästä alkaen sade
      // voi olla runsasta"
      for (unsigned int k = 0; k < i->numberOfAdditionalSentences(); k++)
      {
        std::pair<WeatherPeriod, Sentence> additionalSentence(i->getAdditionalSentence(k));
        paragraph << additionalSentence.second;
      }

      // possible fog sentence after
      if (i->theStoryPartId == CLOUDINESS_STORY_PART)
      {
        Sentence fogSentence;
        fogSentence << theFogForecast.fogSentence(i->thePeriod);

        theStorySize += fogSentence.size();
        paragraph << fogSentence;
      }
    }
    // reset the today phrase when day changes
    /*
if (i > 0 &&
    theStoryItemVector[i - 1]->thePeriod.localEndTime().GetJulianDay() !=
        theStoryItemVector[i]->thePeriod.localEndTime().GetJulianDay() &&
    get_period_length(theForecastPeriod) > 24)
  const_cast<WeatherHistory&>(theWeatherArea.history())
      .updateTimePhrase("", "", TextGenPosixTime(1970, 1, 1));
    */
  }

  // ARE 19.04.2012: yletv for kaakkois-suomi, empty story was created
  // if story is empty take the whole forecast period and tell story of it
  if (theStorySize == 0)
  {
    Sentence periodPhrase;
    std::vector<std::pair<WeatherPeriod, Sentence>> theAdditionalSentences;
    paragraph << thePrecipitationForecast.precipitationSentence(
        theForecastPeriod, periodPhrase, theAdditionalSentences);
    for (auto& theAdditionalSentence : theAdditionalSentences)
    {
      paragraph << theAdditionalSentence.second;
    }
  }

  return paragraph;
}

void WeatherForecastStory::collectPrecipitationSeaStoryItem(unsigned int i,
                                                            SeaStoryItems& storyItems)
{
  WeatherForecastStoryItem* item = theStoryItemVector[i].get();
  auto* precipitationStoryItem = static_cast<PrecipitationForecastStoryItem*>(item);
  bool sleetOrSnow =
      (precipitationStoryItem->theForm & SLEET_FORM || precipitationStoryItem->theForm & SNOW_FORM);
  bool weakPrecipitation = precipitationStoryItem->isWeakPrecipitation(theParameters);
  float precipitationExtent = precipitationStoryItem->precipitationExtent();
  WeatherPeriod precipitationPeriod = precipitationStoryItem->getStoryItemPeriod();

  theLogger << "Precipitation period (At Sea): " << as_string(precipitationPeriod) << ": "
            << (weakPrecipitation ? "weak!!, " : "not weak, ") << " extent: " << precipitationExtent
            << (sleetOrSnow ? ", sleet or snow" : "") << '\n';

  if (!(sleetOrSnow || !weakPrecipitation) || precipitationExtent <= 10)
    return;

  if (storyItems.empty())
  {
    storyItems.emplace_back(item->theStoryPartId, i);
    return;
  }

  WeatherForecastStoryItem* previousItem = theStoryItemVector[storyItems.back().second].get();
  if (previousItem->theStoryPartId != PRECIPITATION_STORY_PART ||
      precipitationStoryItem->theStoryPartId != PRECIPITATION_STORY_PART)
  {
    storyItems.emplace_back(item->theStoryPartId, i);
    return;
  }

  auto* previousPr = static_cast<PrecipitationForecastStoryItem*>(previousItem);
  bool canMerge = previousPr->precipitationExtent() >= IN_SOME_PLACES_LOWER_LIMIT &&
                  previousPr->precipitationExtent() <= IN_MANY_PLACES_UPPER_LIMIT &&
                  precipitationExtent >= IN_SOME_PLACES_LOWER_LIMIT &&
                  precipitationExtent <= IN_MANY_PLACES_UPPER_LIMIT &&
                  precipitationPeriod.localStartTime().DifferenceInHours(
                      previousPr->getStoryItemPeriod().localEndTime()) <= 2;
  if (canMerge)
  {
    theLogger << "Merging periods  " << as_string(previousPr->getStoryItemPeriod()) << " and "
              << as_string(precipitationPeriod) << " because they are close to each other\n";
    previousPr->thePeriod = WeatherPeriod(previousPr->getStoryItemPeriod().localStartTime(),
                                          precipitationPeriod.localEndTime());
  }
  else
  {
    storyItems.emplace_back(item->theStoryPartId, i);
  }
}

WeatherForecastStory::SeaStoryItems WeatherForecastStory::collectSeaStoryItems()
{
  SeaStoryItems storyItems;

  for (unsigned int i = 0; i < theStoryItemVector.size(); i++)
  {
    WeatherForecastStoryItem* item = theStoryItemVector[i].get();
    theLogger << "Story item: " << as_string(item->getStoryItemPeriod())
              << (item->thePeriodToMergeWith ? " WITH " : " ") << '\n';

    Sentence storyItemSentence;
    storyItemSentence << item->getSentence();
    if (storyItemSentence.empty())
      continue;

    if (item->theStoryPartId == PRECIPITATION_STORY_PART ||
        item->theStoryPartId == PRECIPITATION_TYPE_CHANGE_STORY_PART)
    {
      theLogger << "Precipitation story item: " << as_string(item->getStoryItemPeriod()) << '\n';
      collectPrecipitationSeaStoryItem(i, storyItems);
    }
    else if (item->theStoryPartId == CLOUDINESS_STORY_PART)
    {
      theLogger << "Cloudiness story item: " << as_string(item->getStoryItemPeriod()) << '\n';
      auto* cloudinessStoryItem = static_cast<CloudinessForecastStoryItem*>(item);
      FogInfo fi = theFogForecast.fogInfo(item->getStoryItemPeriod());
      if (fi.id != NO_FOG)
        storyItems.emplace_back(cloudinessStoryItem->theStoryPartId, i);
    }
  }

  return storyItems;
}

void WeatherForecastStory::mergeSuccessiveSeaPrecipitationPeriods(SeaStoryItems& storyItems)
{
  if (storyItems.size() <= 1)
    return;

  unsigned int firstIndex = 0;
  for (unsigned int i = 1; i < storyItems.size(); i++)
  {
    if (storyItems[firstIndex].first != CLOUDINESS_STORY_PART &&
        storyItems[i].first != CLOUDINESS_STORY_PART)
    {
      auto* previousItem = static_cast<PrecipitationForecastStoryItem*>(
          theStoryItemVector[storyItems[firstIndex].second].get());
      auto* currentItem = static_cast<PrecipitationForecastStoryItem*>(
          theStoryItemVector[storyItems[i].second].get());
      if (previousItem->theType == currentItem->theType &&
          previousItem->theForm == currentItem->theForm)
      {
        storyItems[i].first = MISSING_STORY_PART;
        float prevLen = get_period_length(previousItem->getStoryItemPeriod());
        float currLen = get_period_length(currentItem->getStoryItemPeriod());
        previousItem->getStoryItemPeriod() =
            WeatherPeriod(previousItem->getStoryItemPeriod().localStartTime(),
                          currentItem->getStoryItemPeriod().localEndTime());
        previousItem->theIntensity =
            ((previousItem->theIntensity * prevLen) + (currentItem->theIntensity * currLen)) /
            (prevLen + currLen);
        previousItem->theExtent =
            ((previousItem->theExtent * prevLen) + (currentItem->theExtent * currLen)) /
            (prevLen + currLen);
      }
    }
    else
      firstIndex = i;
  }
}

void WeatherForecastStory::processMergedFogItems(WeatherForecastStoryItem* first,
                                                 WeatherForecastStoryItem* second,
                                                 std::vector<Sentence>& sentences,
                                                 SeaStoryStats& stats)
{
  FogInfo firstFI = theFogForecast.fogInfo(first->getStoryItemPeriod());
  FogInfo secondFI = theFogForecast.fogInfo(second->getStoryItemPeriod());

  Sentence fogSentence;
  if (!firstFI.sentence.empty() && !secondFI.sentence.empty())
    fogSentence << firstFI.timePhrase << "ja" << secondFI.timePhrase
                << (get_period_length(firstFI.period) > get_period_length(secondFI.period)
                        ? firstFI.sentence
                        : secondFI.sentence);
  else if (!firstFI.sentence.empty())
    fogSentence << firstFI.timePhrase << firstFI.sentence;
  else if (!secondFI.sentence.empty())
    fogSentence << secondFI.timePhrase << secondFI.sentence;

  if (!fogSentence.empty())
  {
    sentences.push_back(fogSentence);
    theStorySize += fogSentence.size();
    if (!firstFI.sentence.empty() && !secondFI.sentence.empty())
      theLogger << "Fog periods: " << as_string(firstFI.period) << " and "
                << as_string(secondFI.period) << " -> " << as_string(fogSentence) << '\n';
    else if (!firstFI.sentence.empty())
      theLogger << "Fog period (first): " << as_string(firstFI.period) << " -> "
                << as_string(fogSentence) << '\n';
    else if (!secondFI.sentence.empty())
      theLogger << "Fog period (second): " << as_string(secondFI.period) << " -> "
                << as_string(fogSentence) << '\n';
  }

  if (firstFI.id == FOG || firstFI.id == FOG_POSSIBLY_DENSE)
    stats.precipitationAndFogPeriodLength += get_period_length(first->thePeriod);
  if (secondFI.id == FOG || secondFI.id == FOG_POSSIBLY_DENSE)
    stats.precipitationAndFogPeriodLength += get_period_length(second->thePeriod);
}

void WeatherForecastStory::processSingleFogItem(WeatherForecastStoryItem* item,
                                                std::vector<Sentence>& sentences,
                                                SeaStoryStats& stats)
{
  FogInfo fi = theFogForecast.fogInfo(item->thePeriod);
  Sentence fogSentence;
  fogSentence << fi.timePhrase << fi.sentence;
  if (!fogSentence.empty())
  {
    sentences.push_back(fogSentence);
    theLogger << "Fog period: " << as_string(fi.period) << " -> " << as_string(fogSentence) << '\n';
    theStorySize += fogSentence.size();
    if (fi.id == FOG || fi.id == FOG_POSSIBLY_DENSE)
      stats.precipitationAndFogPeriodLength += get_period_length(item->thePeriod);
  }
}

void WeatherForecastStory::processFogItem(unsigned int i,
                                          const SeaStoryItems& storyItems,
                                          WeatherForecastStoryItem*& previousCloudinessPeriod,
                                          std::vector<Sentence>& sentences,
                                          SeaStoryStats& stats)
{
  WeatherForecastStoryItem* item = theStoryItemVector[storyItems[i].second].get();

  // accumulate successive fog items
  if (i < storyItems.size() - 1 && storyItems[i + 1].first == CLOUDINESS_STORY_PART)
  {
    FogInfo fogInfo = theFogForecast.fogInfo(item->getStoryItemPeriod());
    if (fogInfo.id != NO_FOG && !previousCloudinessPeriod)
      previousCloudinessPeriod = item;
    return;
  }

  if (previousCloudinessPeriod)
    processMergedFogItems(previousCloudinessPeriod, item, sentences, stats);
  else
    processSingleFogItem(item, sentences, stats);

  previousCloudinessPeriod = nullptr;
}

void WeatherForecastStory::processPrecipitationItem(WeatherForecastStoryItem* item,
                                                    std::vector<Sentence>& sentences,
                                                    SeaStoryStats& stats)
{
  auto* precipitationItem = static_cast<PrecipitationForecastStoryItem*>(item);
  float precipitationExtent = precipitationItem->precipitationExtent();

  if (precipitationExtent <= 10)
  {
    theLogger << "Precipitation extent at period " << as_string(precipitationItem->thePeriod)
              << " is less than 10% (" << precipitationExtent << ") -> dont report" << '\n';
    return;
  }

  bool weakPrecipitation = precipitationItem->isWeakPrecipitation(theParameters);

  if (!weakPrecipitation)
  {
    unsigned int moderatePrecipitationHours = thePrecipitationForecast.getPrecipitationHours(
        MODERATE_PRECIPITATION, precipitationItem->thePeriod);
    unsigned int heavyPrecipitationHours = thePrecipitationForecast.getPrecipitationHours(
        HEAVY_PRECIPITATION, precipitationItem->thePeriod);
    unsigned int preciptationForm = precipitationItem->precipitationForm();

    if (((preciptationForm & SNOW_FORM) || (preciptationForm & SLEET_FORM) ||
         (preciptationForm & FREEZING_FORM)) &&
        precipitationExtent >= theParameters.theInManyPlacesLowerLimit)
    {
      // sateen olomuoto = LUMI, räntä, jäätävä, sateen tyyppi kuuro tai jatkuva (eli kaikki) ja
      // sateen intensiteetti > heikko ainakin 3 tuntia laajuus > ja monin paikoin
      stats.badVisibilityPeriodLength += (moderatePrecipitationHours + heavyPrecipitationHours);
      stats.badVisibility = true;
    }
    else if (((preciptationForm & WATER_FORM) || (preciptationForm & SLEET_FORM) ||
              (preciptationForm & DRIZZLE_FORM)) &&
             precipitationItem->theType == SHOWERS &&
             precipitationExtent >= theParameters.theInManyPlacesLowerLimit)
    {
      // sateen olomuoto = VESI, räntä, tihku, sateen tyyppi KUURO ja sateen intensiteetti >=
      // rankka ainakin kolme tuntia ja laajuus > monin paikoin
      // Elokuussa 2017 Kiira-myrskyn jälkeen päätettiin, että wesisateella ei raportoida huonoa
      // näkyvyyttä
      //          stats.badVisibilityPeriodLengthWater += heavyPrecipitationHours;
      //          stats.badVisibility = true;
    }
  }

  Sentence precipitationSentence;
  precipitationSentence << precipitationItem->getSentence();
  theStorySize += precipitationSentence.size();
  theLogger << "Precipitation story item: " << as_string(precipitationItem->getStoryItemPeriod())
            << (precipitationSentence.empty() ? ", empty sentence" : " not empty sentence") << ", "
            << (precipitationItem->thePoutaantuuFlag ? "poutaantuu = true" : "poutaantuu = false")
            << '\n';

  if (!precipitationSentence.empty() || precipitationItem->thePoutaantuuFlag)
  {
    sentences.push_back(precipitationSentence);
    if (!stats.inManyPlaces)
      stats.inManyPlaces = (precipitationExtent > theParameters.theInManyPlacesLowerLimit &&
                            precipitationExtent <= theParameters.theInManyPlacesUpperLimit);
    if (precipitationItem->theType == CONTINUOUS &&
        precipitationExtent >= IN_MANY_PLACES_UPPER_LIMIT)
      stats.precipitationAndFogPeriodLength += get_period_length(precipitationItem->thePeriod);
    stats.precipitationPeriodLength += get_period_length(precipitationItem->thePeriod);
    stats.snowPrecipitationFormInvolved = (precipitationItem->precipitationForm() & SNOW_FORM);
    theLogger << "Precipitation period: " << as_string(precipitationItem->thePeriod) << " -> "
              << as_string(precipitationSentence) << '\n';
  }
}

void WeatherForecastStory::appendVisibilitySentence(std::vector<Sentence>& sentences,
                                                    const SeaStoryStats& stats)
{
  if (sentences.empty())
  {
    Sentence s;
    s << HYVA_NAKYVYYS_PHRASE;
    sentences.push_back(s);
    return;
  }

  if (theParameters.theShortTextModeFlag || stats.precipitationPeriodLength == 0)
    return;

  float precipitationPeriodShare = static_cast<float>(stats.precipitationPeriodLength) /
                                   static_cast<float>(stats.forecastPeriodLength);
  float heavyRainPeriodShare = static_cast<float>(stats.badVisibilityPeriodLengthWater) /
                               static_cast<float>(stats.forecastPeriodLength);

  if (stats.badVisibility && precipitationPeriodShare > 0.50 &&
      (stats.badVisibilityPeriodLength >= 3 || heavyRainPeriodShare > 0.50))
  {
    Sentence s;
    s << HUONO_NAKYVYYS_PHRASE;
    sentences.push_back(s);
  }
  else if (precipitationPeriodShare <= 0.50 && !stats.inManyPlaces)
  {
    sentences.back() << Delimiter(COMMA_PUNCTUATION_MARK) << ENIMMAKSEEN_HYVA_NAKYVYYS_PHRASE;
  }
  else if (!stats.snowPrecipitationFormInvolved &&
           (static_cast<float>(stats.precipitationAndFogPeriodLength) <
                static_cast<float>(stats.forecastPeriodLength) ||
            stats.inManyPlaces))
  {
    sentences.back() << Delimiter(COMMA_PUNCTUATION_MARK) << MUUTEN_HYVA_NAKYVYYS_PHRASE;
  }
}

Paragraph WeatherForecastStory::getWeatherForecastStoryAtSea()
{
  theLogger << "Start processing weather forcast at sea\n";

  thePrecipitationForecast.setUseIcingPhraseFlag(false);
  thePeriodPhraseGenerator.reset();
  thePrecipitationForecast.setDryPeriodTautologyFlag(false);
  theShortTimePrecipitationReportedFlag = false;
  theCloudinessReportedFlag = false;
  theStorySize = 0;

  SeaStoryStats stats;
  stats.forecastPeriodLength = get_period_length(theForecastPeriod);

  SeaStoryItems storyItems = collectSeaStoryItems();

  // if precipitation/fog lasts whole period
  if (!storyItems.empty())
  {
    WeatherForecastStoryItem* firstItemOfForecast = theStoryItemVector[0].get();
    WeatherForecastStoryItem* firstItemOfStory = theStoryItemVector[storyItems[0].second].get();
    if (firstItemOfStory->getStoryItemPeriod().localStartTime() >
        firstItemOfForecast->getStoryItemPeriod().localStartTime())
      theStorySize += 1;
  }

  mergeSuccessiveSeaPrecipitationPeriods(storyItems);

  Paragraph paragraph;
  std::vector<Sentence> weatherForecastSentences;
  WeatherForecastStoryItem* previousCloudinessPeriod = nullptr;

  for (unsigned int i = 0; i < storyItems.size(); i++)
  {
    if (storyItems[i].first == MISSING_STORY_PART)
      continue;

    WeatherForecastStoryItem* item = theStoryItemVector[storyItems[i].second].get();

    if (storyItems[i].first == CLOUDINESS_STORY_PART)
      processFogItem(i, storyItems, previousCloudinessPeriod, weatherForecastSentences, stats);
    else
      processPrecipitationItem(item, weatherForecastSentences, stats);
  }

  appendVisibilitySentence(weatherForecastSentences, stats);

  for (const auto& s : weatherForecastSentences)
    paragraph << s;

  theLogger << "Weather Forecast At Sea: \n"
            << " length of continuous precipitation or extensive fog: "
            << stats.precipitationAndFogPeriodLength << '\n'
            << " forecats period length: " << stats.forecastPeriodLength << '\n'
            << " precipitation period length: " << stats.precipitationPeriodLength << '\n'
            << " bad visibility period length: " << stats.badVisibilityPeriodLength << '\n'
            << " bad visibility period length for water: " << stats.badVisibilityPeriodLengthWater
            << '\n'
            << " precipitation or fog in many places: " << (stats.inManyPlaces ? "yes" : "no")
            << '\n'
            << " length of forecast text: " << paragraph.size() << '\n';
  if (!paragraph.empty())
    theLogger << " forecast text: " << as_string(paragraph) << '\n';

  return paragraph;
}

void WeatherForecastStory::addPrecipitationStoryItems()
{
  vector<WeatherPeriod> precipitationPeriods;

  thePrecipitationForecast.getPrecipitationPeriods(theForecastPeriod, precipitationPeriods);

  std::shared_ptr<PrecipitationForecastStoryItem> previousPrItem;
  std::shared_ptr<WeatherForecastStoryItem> missingStoryItem;
  for (auto& precipitationPeriod : precipitationPeriods)
  {
    float intensity(thePrecipitationForecast.getMeanIntensity(precipitationPeriod,
                                                              theParameters.theForecastArea));
    float extent(thePrecipitationForecast.getPrecipitationExtent(precipitationPeriod,
                                                                 theParameters.theForecastArea));
    unsigned int form(thePrecipitationForecast.getPrecipitationForm(precipitationPeriod,
                                                                    theParameters.theForecastArea));
    precipitation_type type(thePrecipitationForecast.getPrecipitationType(
        precipitationPeriod, theParameters.theForecastArea));

    bool thunder(thePrecipitationForecast.thunderExists(
        precipitationPeriod, theParameters.theForecastArea, theVar));

    if (get_period_length(precipitationPeriod) <= 1 && extent < 10)
      continue;

    auto item = std::make_shared<PrecipitationForecastStoryItem>(*this,
                                                                 precipitationPeriod,
                                                                 PRECIPITATION_STORY_PART,
                                                                 intensity,
                                                                 extent,
                                                                 form,
                                                                 type,
                                                                 thunder);

    TextGenPosixTime startTimeFull;
    TextGenPosixTime endTimeFull;
    thePrecipitationForecast.getPrecipitationPeriod(
        precipitationPeriod.localStartTime(), startTimeFull, endTimeFull);
    // This parameter shows if the precipitation starts
    // before forecast period or continues after
    int precipitationFullDuration = endTimeFull.DifferenceInHours(startTimeFull);
    item->theFullDuration = precipitationFullDuration;

    if (previousPrItem != nullptr)
    {
      TextGenPosixTime startTime(previousPrItem->thePeriod.localEndTime());
      TextGenPosixTime endTime(precipitationPeriod.localStartTime());
      if (endTime.DifferenceInHours(startTime) > 1)
      {
        startTime.ChangeByHours(1);
        endTime.ChangeByHours(-1);

        // placeholder for some other story item between precipitation periods
        missingStoryItem = std::make_shared<WeatherForecastStoryItem>(
            *this, WeatherPeriod(startTime, endTime), MISSING_STORY_PART);

        theStoryItemVector.push_back(missingStoryItem);
      }
    }
    // precipitation story item
    theStoryItemVector.push_back(item);

    previousPrItem = item;
  }

  if (theStoryItemVector.empty())
  {
    WeatherPeriod cloudinessPeriod(theForecastPeriod);
    auto item = std::make_shared<CloudinessForecastStoryItem>(
        *this, cloudinessPeriod, CLOUDINESS_STORY_PART, nullptr, nullptr);

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
      missingStoryItem =
          std::make_shared<WeatherForecastStoryItem>(*this, firstPeriod, MISSING_STORY_PART);
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

      missingStoryItem =
          std::make_shared<WeatherForecastStoryItem>(*this, lastPeriod, MISSING_STORY_PART);
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
          missingStoryItem = std::make_shared<WeatherForecastStoryItem>(
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
  PrecipitationForecastStoryItem* previousPrecipitationStoryItem = nullptr;
  for (unsigned int i = 0; i < theStoryItemVector.size(); i++)
  {
    if (theStoryItemVector[i]->theStoryPartId == MISSING_STORY_PART)
    {
      WeatherForecastStoryItem* placeholder = theStoryItemVector[i].get();

      auto cloudinessStoryItem =
          std::make_shared<CloudinessForecastStoryItem>(*this,
                                                        placeholder->thePeriod,
                                                        CLOUDINESS_STORY_PART,
                                                        previousPrecipitationStoryItem,
                                                        nullptr);

      if (!previousPrecipitationStoryItem)
      {
        cloudinessStoryItem->theReportAboutDryWeatherFlag = true;
      }

      cloudinessStoryItem->thePreviousPrecipitationStoryItem = previousPrecipitationStoryItem;

      if (i < theStoryItemVector.size() - 1 &&
          theStoryItemVector[i + 1]->theStoryPartId == PRECIPITATION_STORY_PART)
        cloudinessStoryItem->theNextPrecipitationStoryItem =
            static_cast<PrecipitationForecastStoryItem*>(theStoryItemVector[i + 1].get());

      theStoryItemVector[i] = cloudinessStoryItem;

      // delete placeholder;
    }
    else if (theStoryItemVector[i]->theStoryPartId == PRECIPITATION_STORY_PART)
    {
      previousPrecipitationStoryItem =
          static_cast<PrecipitationForecastStoryItem*>(theStoryItemVector[i].get());
    }
  }
}

// Try to merge two consecutive precipitation items; returns true if types differ (skip merge)
bool WeatherForecastStory::tryMergePrecipitationItems(PrecipitationForecastStoryItem* prev,
                                                      PrecipitationForecastStoryItem* curr,
                                                      WeatherForecastStoryItem* prevStoryItem)
{
  if (curr->thePeriod.localStartTime().DifferenceInHours(prev->thePeriod.localEndTime()) <= 2)
    prev->theReportPoutaantuuFlag = false;

  // if the type is different don't merge, except when thunder exists on both periods
  bool typesDiffer = (prev->theType != curr->theType);
  bool bothHaveThunder = (prev->theThunder && curr->theThunder);
  if (typesDiffer && !bothHaveThunder)
  {
    // if the dry period between precipitation periods is short don't mention it
    if (prevStoryItem != prev && prevStoryItem->storyItemPeriodLength() <= 1)
      prevStoryItem->theIncludeInTheStoryFlag = false;
    return true;  // types differ, don't merge
  }

  TextGenPosixTime gapStart(prev->thePeriod.localEndTime());
  gapStart.ChangeByHours(+1);
  TextGenPosixTime gapEnd(curr->thePeriod.localStartTime());
  if (gapStart < gapEnd)
    gapEnd.ChangeByHours(-1);
  WeatherPeriod gapPeriod(gapStart, gapEnd);

  if (get_period_length(gapPeriod) <= 3 &&
      get_period_length(gapPeriod) <=
          curr->storyItemPeriodLength() + prev->storyItemPeriodLength() + 2)
  {
    prev->thePeriodToMergeWith = curr;
    curr->theIncludeInTheStoryFlag = false;
    if (prevStoryItem->theStoryPartId == CLOUDINESS_STORY_PART)
      prevStoryItem->theIncludeInTheStoryFlag = false;
  }
  return false;  // types compatible, merge attempted
}

void WeatherForecastStory::mergePrecipitationPeriodsWhenFeasible()
{
  PrecipitationForecastStoryItem* previousPrecipitationStoryItem = nullptr;
  std::vector<unsigned int> indexes = get_story_item_indexes(theStoryItemVector);

  for (unsigned int i = 0; i < indexes.size(); i++)
  {
    WeatherForecastStoryItem* currentStoryItem = theStoryItemVector[indexes[i]].get();
    WeatherForecastStoryItem* previousStoryItem =
        (i > 0 ? theStoryItemVector[indexes[i - 1]].get() : nullptr);

    if (!currentStoryItem->theIncludeInTheStoryFlag)
      continue;
    if (currentStoryItem->theStoryPartId != PRECIPITATION_STORY_PART)
      continue;

    auto* currentPrecipitationStoryItem =
        static_cast<PrecipitationForecastStoryItem*>(currentStoryItem);

    if (previousPrecipitationStoryItem)
    {
      bool typesDiffer = tryMergePrecipitationItems(
          previousPrecipitationStoryItem, currentPrecipitationStoryItem, previousStoryItem);
      if (typesDiffer)
      {
        previousPrecipitationStoryItem = currentPrecipitationStoryItem;
        continue;
      }
    }
    previousPrecipitationStoryItem = currentPrecipitationStoryItem;
  }

  indexes = get_story_item_indexes(theStoryItemVector);
  int storyItemCount = static_cast<int>(indexes.size());
  if (storyItemCount > 1)
  {
    unsigned int firstIndex = indexes[0];
    unsigned int secondIndex = indexes[1];
    // short precipitation period in the beginning is ignored
    if (theStoryItemVector[firstIndex]->theStoryPartId == PRECIPITATION_STORY_PART &&
        theStoryItemVector[secondIndex]->theStoryPartId == CLOUDINESS_STORY_PART &&
        theStoryItemVector[firstIndex]->getPeriodLength() <= 1 &&
        theStoryItemVector[firstIndex]->thePeriodToMergeWith == nullptr)
      theStoryItemVector[firstIndex]->theIncludeInTheStoryFlag = false;

    unsigned int lastIndex = indexes[storyItemCount - 1];
    unsigned int beforeLastIndex = indexes[storyItemCount - 2];
    // short precipitation period in the end after cloudiness period is not reported
    if (theStoryItemVector[lastIndex]->theStoryPartId == PRECIPITATION_STORY_PART &&
        theStoryItemVector[lastIndex]->getPeriodLength() <= 1 &&
        theStoryItemVector[beforeLastIndex]->theStoryPartId == CLOUDINESS_STORY_PART &&
        theStoryItemVector[beforeLastIndex]->theIncludeInTheStoryFlag)
      theStoryItemVector[lastIndex]->theIncludeInTheStoryFlag = false;
  }
}

void WeatherForecastStory::mergeCloudinessPeriodsWhenFeasible()
{
  CloudinessForecastStoryItem* previousCloudinessStoryItem = nullptr;
  CloudinessForecastStoryItem* currentCloudinessStoryItem = nullptr;
  std::vector<unsigned int> indexes = get_story_item_indexes(theStoryItemVector);

  for (unsigned int indexe : indexes)
  {
    WeatherForecastStoryItem* currentStoryItem = theStoryItemVector[indexe].get();
    if (currentStoryItem->theStoryPartId == CLOUDINESS_STORY_PART &&
        currentStoryItem->theIncludeInTheStoryFlag)
    {
      currentCloudinessStoryItem = static_cast<CloudinessForecastStoryItem*>(currentStoryItem);
      if (previousCloudinessStoryItem)
      {
        TextGenPosixTime gapPeriodStartTime(previousCloudinessStoryItem->thePeriod.localEndTime());
        gapPeriodStartTime.ChangeByHours(+1);
        TextGenPosixTime gapPeriodEndTime(currentCloudinessStoryItem->thePeriod.localStartTime());
        if (gapPeriodStartTime < gapPeriodEndTime)
          gapPeriodEndTime.ChangeByHours(-1);
        WeatherPeriod gapPeriod(gapPeriodStartTime, gapPeriodEndTime);

        // merge periods if the precipitation period in between is short and
        // intensity is weak and precipitation area is small
        if (get_period_length(gapPeriod) <= 1 &&
            currentCloudinessStoryItem->thePreviousPrecipitationStoryItem->theIntensity <= 0.1)
        {
          // merge two cloudiness periods
          previousCloudinessStoryItem->thePeriodToMergeWith = currentCloudinessStoryItem;
          currentCloudinessStoryItem->theIncludeInTheStoryFlag = false;
          currentCloudinessStoryItem->thePreviousPrecipitationStoryItem->theIncludeInTheStoryFlag =
              false;
        }
      }
      previousCloudinessStoryItem = currentCloudinessStoryItem;
    }
  }

  indexes = get_story_item_indexes(theStoryItemVector);

  int storyItemCount = indexes.size();
  if (storyItemCount > 1)
  {
    unsigned int firstIndex = indexes[0];
    unsigned int secondIndex = indexes[1];
    // short cloudiness period in the beginning is ignored
    if (theStoryItemVector[firstIndex]->theStoryPartId == CLOUDINESS_STORY_PART &&
        theStoryItemVector[secondIndex]->theStoryPartId == PRECIPITATION_STORY_PART &&
        theStoryItemVector[firstIndex]->getPeriodLength() <= 1 &&
        theStoryItemVector[firstIndex]->thePeriodToMergeWith == nullptr)
      theStoryItemVector[firstIndex]->theIncludeInTheStoryFlag = false;

    unsigned int lastIndex = indexes[storyItemCount - 1];
    unsigned int beforeLastIndex = indexes[storyItemCount - 2];
    // short cloudiness period in the end after precipitation period is not reported
    if (theStoryItemVector[lastIndex]->theStoryPartId == CLOUDINESS_STORY_PART &&
        theStoryItemVector[lastIndex]->getPeriodLength() <= 1 &&
        theStoryItemVector[beforeLastIndex]->theStoryPartId == PRECIPITATION_STORY_PART &&
        theStoryItemVector[beforeLastIndex]->theIncludeInTheStoryFlag)
      theStoryItemVector[lastIndex]->theIncludeInTheStoryFlag = false;
  }
}

void WeatherForecastStory::mergePeriodsWhenFeasible()
{
  theLogger << "******** STORY ITEMS BEFORE MERGE ********\n";
  for (auto& i : theStoryItemVector)
    theLogger << as_string(i->getStoryItemPeriod()) << '\n';

  mergeCloudinessPeriodsWhenFeasible();
  mergePrecipitationPeriodsWhenFeasible();

  theLogger << "******** STORY ITEMS AFTER MERGE ********\n";
  for (auto& i : theStoryItemVector)
  {
    if (!i->theIncludeInTheStoryFlag)
      continue;

    if (i->thePeriodToMergeWith)
      theLogger << as_string(i->getStoryItemPeriod())
                << "  merged periods: " << as_string(i->thePeriod) << " and "
                << as_string(i->thePeriodToMergeWith->thePeriod) << '\n';
    else
      theLogger << as_string(i->getStoryItemPeriod()) << '\n';
  }
}

Sentence WeatherForecastStory::getTimePhrase() const
{
  Sentence sentence;

  if (theReportTimePhraseFlag)
  {
    if (theStorySize == 0)
      sentence << ALUKSI_WORD;
    else
      sentence << MYOHEMMIN_WORD;
  }
  else if (theAddAluksiWord && theStorySize == 0)
  {
    sentence << ALUKSI_WORD;
  }

  return sentence;
}

void WeatherForecastStory::logTheStoryItems() const
{
  theLogger << "******** STORY ITEMS ********\n";
  for (const auto& i : theStoryItemVector)
  {
    theLogger << *i;
  }
}

WeatherForecastStoryItem::WeatherForecastStoryItem(WeatherForecastStory& weatherForecastStory,
                                                   WeatherPeriod period,
                                                   story_part_id storyPartId)
    : theWeatherForecastStory(weatherForecastStory),
      thePeriod(std::move(period)),
      theStoryPartId(storyPartId),
      theIncludeInTheStoryFlag(true)
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

std::pair<WeatherPeriod, Sentence> WeatherForecastStoryItem::getAdditionalSentence(
    unsigned int index) const
{
  if (index >= theAdditionalSentences.size())
    return make_pair(WeatherPeriod(TextGenPosixTime(), TextGenPosixTime()), Sentence());
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

  return thePeriod;
}

unsigned int WeatherForecastStoryItem::getPeriodLength() const
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
    if (!sentence.empty())
      sentence << JA_WORD;
    sentence << *(todayVector[i]);
  }
  return sentence;
}

// special treatment, because 06:00 can be aamuyö and morning, depends weather the period starts
// or ends

std::string WeatherForecastStoryItem::checkForAamuyoAndAamuPhrase(
    bool theFromSpecifier, const WeatherPeriod& thePhrasePeriod)
{
  std::string retValue;

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

WeatherPeriod get_part_of_day_period(const TextGenPosixTime& t, const std::string& part_of_day)
{
  TextGenPosixTime starttime(t);
  starttime.SetMin(0);
  starttime.SetSec(0);
  TextGenPosixTime endtime(t);
  endtime.SetMin(0);
  endtime.SetSec(0);

  if (part_of_day == YOLLA_WORD)
  {
    starttime.SetHour(21);
    endtime.SetHour(6);
    endtime.ChangeByDays(1);
  }
  else if (part_of_day == AAMULLA_WORD)
  {
    starttime.SetHour(5);
    endtime.SetHour(10);
  }
  else if (part_of_day == PAIVALLA_WORD)
  {
    starttime.SetHour(10);
    endtime.SetHour(18);
  }
  else if (part_of_day == ILLALLA_WORD)
  {
    starttime.SetHour(17);
    endtime.SetHour(22);
  }
  if (part_of_day == YOLLA_JA_AAMULLA_PHRASE)
  {
    starttime.SetHour(21);
    endtime.SetHour(10);
    endtime.ChangeByDays(1);
  }
  else if (part_of_day == AAMULLA_JA_PAIVALLA_PHRASE)
  {
    starttime.SetHour(5);
    endtime.SetHour(18);
  }
  else if (part_of_day == PAIVALLA_JA_ILLALLA_PHRASE)
  {
    starttime.SetHour(10);
    endtime.SetHour(22);
  }
  else if (part_of_day == ILLALLA_JA_YOLLA_PHRASE)
  {
    starttime.SetHour(17);
    endtime.SetHour(6);
    endtime.ChangeByDays(1);
  }

  return {starttime, endtime};
}

int get_day_number(part_of_the_day_id id, const WeatherPeriod& period, std::string& dayNumber)
{
  int dayNum = -1;

  switch (id)
  {
    case AAMU:
    case AAMUPAIVA:
    case KESKIPAIVA:
    case ILTA:
    case ILTAPAIVA:
    case ILTAYO:
    case AAMU_JA_AAMUPAIVA:
    case ILTAPAIVA_JA_ILTA:
    case ILTA_JA_ILTAYO:
    case ILTAYO_JA_KESKIYO:
    case PAIVA:
    {
      dayNum = period.localStartTime().GetWeekday();
      break;
    }
    case AAMUYO:
    case KESKIYO_JA_AAMUYO:
    case AAMUYO_JA_AAMU:
    {
      dayNum = period.localEndTime().GetWeekday();
      break;
    }
    default:
      break;
  }
  if (dayNum > -1)
    dayNumber += (std::to_string(dayNum) + "-");

  return dayNum;
}

void check_percentage(const WeatherPeriod& period,
                      const std::string& part_of_day,
                      part_of_the_day_id /*id*/,
                      float& topPercentage,
                      std::string& phrase)
{
  bool ok;
  WeatherPeriod parOfDayPeriod = get_part_of_day_period(period.localStartTime(), part_of_day);
  WeatherPeriod intersectionPeriod = get_intersection_period(parOfDayPeriod, period, ok);
  if (ok)
  {
    float percentage = static_cast<float>(get_period_length(intersectionPeriod)) /
                       static_cast<float>(get_period_length(period));
    if (percentage > topPercentage)
    {
      topPercentage = percentage;
      phrase = part_of_day;
    }
  }
}

bool PeriodPhraseGenerator::dayExists(int n) const
{
  return (dayNumbers.find(n) != dayNumbers.end());
}

bool PeriodPhraseGenerator::phraseExists(const WeatherPeriod& period) const
{
  return (periodPhrases.find(period) != periodPhrases.end());
}

Sentence PeriodPhraseGenerator::getPeriodPhrase(const WeatherPeriod& period)
{
  if (periodPhrases.find(period) != periodPhrases.end())
  {
    return periodPhrases.at(period);
  }

  Sentence periodPhrase;
  part_of_the_day_id id = get_part_of_the_day_id_large(period);

  if (id != MISSING_PART_OF_THE_DAY_ID)
  {
    if (id == YO)
      periodPhrase << YOLLA_WORD;
    else
    {
      std::string dayNumber;
      int dayNum = get_day_number(id, period, dayNumber);
      if (dayNum == -1 || dayExists(dayNum) ||
          false == Settings::optional_bool(itsVar + "::weekdays", true))
        dayNumber = "";
      else
        dayNumbers.insert(dayNum);

      if (id == PAIVA)
        periodPhrase << (dayNumber + PAIVALLA_WORD);
      else
      {
        std::string phrase = get_time_phrase_from_id(id, itsVar, get_period_length(period) >= 6);
        if (!phrase.empty())
          periodPhrase << (dayNumber + phrase);
      }
    }
  }
  else
  {
    float topPercentage = 0.0;
    std::string phrase;
    check_percentage(period, YOLLA_WORD, YO, topPercentage, phrase);
    check_percentage(period, AAMULLA_WORD, AAMU, topPercentage, phrase);
    check_percentage(period, PAIVALLA_WORD, PAIVA, topPercentage, phrase);
    check_percentage(period, ILLALLA_WORD, ILTA, topPercentage, phrase);
    check_percentage(
        period, YOLLA_JA_AAMULLA_PHRASE, MISSING_PART_OF_THE_DAY_ID, topPercentage, phrase);
    check_percentage(
        period, AAMULLA_JA_PAIVALLA_PHRASE, MISSING_PART_OF_THE_DAY_ID, topPercentage, phrase);
    check_percentage(
        period, PAIVALLA_JA_ILLALLA_PHRASE, MISSING_PART_OF_THE_DAY_ID, topPercentage, phrase);
    check_percentage(
        period, ILLALLA_JA_YOLLA_PHRASE, MISSING_PART_OF_THE_DAY_ID, topPercentage, phrase);

    if (topPercentage >= 0.85)
      periodPhrase << phrase;
  }

  periodPhrases.insert(make_pair(period, periodPhrase));

  return periodPhrase;
}

void PeriodPhraseGenerator::reset()
{
  dayNumbers.clear();
  periodPhrases.clear();
}

Sentence WeatherForecastStoryItem::getPeriodPhrase()
{
  return theWeatherForecastStory.getPeriodPhrase(getStoryItemPeriod());
}

Sentence WeatherForecastStoryItem::getPeriodPhrase(
    bool theFromSpecifier,
    const WeatherPeriod* thePhrasePeriod /*= 0*/,
    bool /*theStoryUnderConstructionEmpty*/ /*= true*/)
{
  Sentence sentence;

  sentence << getPeriodPhrase();

  WeatherPeriod phrasePeriod(thePhrasePeriod == nullptr ? getStoryItemPeriod() : *thePhrasePeriod);
  if (!sentence.empty())
    return sentence;

  bool specifyDay = false;

  std::string day_phase_phrase;

  day_phase_phrase = checkForAamuyoAndAamuPhrase(theFromSpecifier, phrasePeriod);

  if (!day_phase_phrase.empty())
  {
    sentence << day_phase_phrase;
  }

  if (sentence.empty())
  {
    part_of_the_day_id id;
    get_time_phrase_large(phrasePeriod,
                          specifyDay,
                          theWeatherForecastStory.theVar,
                          day_phase_phrase,
                          theFromSpecifier,
                          id);

    sentence << day_phase_phrase;
  }

  theWeatherForecastStory.theLogger << "PHRASE PERIOD: " << phrasePeriod.localStartTime() << "..."
                                    << phrasePeriod.localEndTime() << '\n';
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
      thePoutaantuuFlag(intensity > WEAK_PRECIPITATION_LIMIT_WATER),
      theReportPoutaantuuFlag(intensity > WEAK_PRECIPITATION_LIMIT_WATER),
      theFullDuration(period.localEndTime().DifferenceInHours(period.localStartTime()))
{
}

float PrecipitationForecastStoryItem::precipitationExtent() const
{
  if (thePeriodToMergeWith)
  {
    auto* mergeWith = static_cast<PrecipitationForecastStoryItem*>(thePeriodToMergeWith);

    float periodLen = get_period_length(thePeriod);
    float mergeWithPeriodLen = get_period_length(mergeWith->thePeriod);

    float extent = ((theExtent * periodLen) + (mergeWithPeriodLen * mergeWith->theExtent)) /
                   (periodLen + mergeWithPeriodLen);

    return extent;
  }

  return theExtent;
}

unsigned int PrecipitationForecastStoryItem::precipitationForm() const
{
  if (thePeriodToMergeWith)
  {
    auto* mergeWith = static_cast<PrecipitationForecastStoryItem*>(thePeriodToMergeWith);

    float periodLen = get_period_length(thePeriod);
    float mergeWithPeriodLen = get_period_length(mergeWith->thePeriod);

    unsigned int form = (periodLen > mergeWithPeriodLen ? theForm : mergeWith->theForm);

    return form;
  }

  return theForm;
}

bool PrecipitationForecastStoryItem::isWeakPrecipitation(const wf_story_params& theParameters) const
{
  if (thePeriodToMergeWith)
  {
    auto* mergeWith = static_cast<PrecipitationForecastStoryItem*>(thePeriodToMergeWith);

    float periodLen = get_period_length(thePeriod);
    float mergeWithPeriodLen = get_period_length(mergeWith->thePeriod);

    unsigned int form = (periodLen > mergeWithPeriodLen ? theForm : mergeWith->theForm);
    float intensity =
        ((theIntensity * periodLen) + (mergeWithPeriodLen * mergeWith->theIntensity)) /
        (periodLen + mergeWithPeriodLen);

    return (get_precipitation_intensity_id(form, intensity, theParameters) == WEAK_PRECIPITATION);
  }

  return (get_precipitation_intensity_id(theForm, theIntensity, theParameters) ==
          WEAK_PRECIPITATION);
}

Sentence PrecipitationForecastStoryItem::getLongPeriodPrecipitationSentence(
    const PrecipitationForecast& prForecast,
    const WeatherPeriod& forecastPeriod,
    const WeatherPeriod& storyItemPeriod)
{
  Sentence sentence;
  Sentence thePeriodPhrase;

  if (storyItemPeriod.localStartTime() != forecastPeriod.localStartTime())
  {
    if (storyItemPeriod.localStartTime() > forecastPeriod.localStartTime())
      thePeriodPhrase << getPeriodPhrase(USE_FROM_SPECIFIER);
    if (thePeriodPhrase.empty())
      thePeriodPhrase << theWeatherForecastStory.getTimePhrase();

    sentence << prForecast.precipitationChangeSentence(
        storyItemPeriod, thePeriodPhrase, SADE_ALKAA, theAdditionalSentences);
  }
  else
  {
    if (storyItemPeriod.localStartTime() > forecastPeriod.localStartTime())
      thePeriodPhrase << getPeriodPhrase(DONT_USE_FROM_SPECIFIER, &storyItemPeriod);
    if (thePeriodPhrase.empty())
      thePeriodPhrase << theWeatherForecastStory.getTimePhrase();

    sentence << prForecast.precipitationSentence(
        storyItemPeriod, thePeriodPhrase, theAdditionalSentences);
  }

  if (storyItemPeriod.localEndTime() != forecastPeriod.localEndTime() && theReportPoutaantuuFlag)
  {
    WeatherPeriod poutaantuuPeriod(storyItemPeriod.localEndTime(), storyItemPeriod.localEndTime());
    Sentence poutaantuuPhrase;
    poutaantuuPhrase << getPeriodPhrase(
        DONT_USE_FROM_SPECIFIER, &poutaantuuPeriod, sentence.empty());
    theWeatherForecastStory.theLogger << poutaantuuPhrase;
    if (poutaantuuPhrase.empty())
      poutaantuuPhrase << theWeatherForecastStory.getTimePhrase();

    if (!sentence.empty())
      sentence << Delimiter(",");
    sentence << prForecast.precipitationChangeSentence(
        storyItemPeriod, poutaantuuPhrase, POUTAANTUU, theAdditionalSentences);
  }

  theWeatherForecastStory.theShortTimePrecipitationReportedFlag = false;
  return sentence;
}

Sentence PrecipitationForecastStoryItem::getShortPeriodPrecipitationSentence(
    const PrecipitationForecast& prForecast,
    const WeatherPeriod& forecastPeriod,
    const WeatherPeriod& storyItemPeriod)
{
  Sentence sentence;
  Sentence thePeriodPhrase;

  if (thePeriod.localStartTime() > forecastPeriod.localStartTime())
    thePeriodPhrase << getPeriodPhrase(DONT_USE_FROM_SPECIFIER, &storyItemPeriod);
  if (thePeriodPhrase.empty())
    thePeriodPhrase << theWeatherForecastStory.getTimePhrase();

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

  return sentence;
}

Sentence PrecipitationForecastStoryItem::getStoryItemSentence()
{
  if (!theIncludeInTheStoryFlag)
    return {};

  const PrecipitationForecast& prForecast = theWeatherForecastStory.thePrecipitationForecast;
  WeatherPeriod forecastPeriod = theWeatherForecastStory.theForecastPeriod;
  WeatherPeriod storyItemPeriod(getStoryItemPeriod());

  if (storyItemPeriodLength() >= 6)
    return getLongPeriodPrecipitationSentence(prForecast, forecastPeriod, storyItemPeriod);

  return getShortPeriodPrecipitationSentence(prForecast, forecastPeriod, storyItemPeriod);
}

CloudinessForecastStoryItem::CloudinessForecastStoryItem(
    WeatherForecastStory& weatherForecastStory,
    const WeatherPeriod& period,
    story_part_id storyPartId,
    PrecipitationForecastStoryItem* previousPrecipitationStoryItem,
    PrecipitationForecastStoryItem* nextPrecipitationStoryItem)
    : WeatherForecastStoryItem(weatherForecastStory, period, storyPartId),
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

  for (auto& cloudinessEvent : cloudinessEvents)
  {
    TextGenPosixTime cloudinessEventTimestamp(cloudinessEvent.first);
    if (cloudinessEventTimestamp >= thePeriod.localStartTime() &&
        cloudinessEventTimestamp <= thePeriod.localEndTime())
    {
      sentence << theWeatherForecastStory.theCloudinessForecast.cloudinessChangeSentence(
          WeatherPeriod(cloudinessEventTimestamp, cloudinessEventTimestamp));

      theCloudinessChangeTimestamp = cloudinessEventTimestamp;

      theWeatherForecastStory.theLogger << "CLOUDINESS CHANGE: \n";
      theWeatherForecastStory.theLogger << cloudinessEventTimestamp;
      theWeatherForecastStory.theLogger << ": ";
      theWeatherForecastStory.theLogger << sentence;

      break;
    }
  }

  return sentence;
}

bool CloudinessForecastStoryItem::shouldSkipShortEndPeriod(
    const WeatherPeriod& storyItemPeriod) const
{
  if (storyItemPeriod.localEndTime() != theWeatherForecastStory.theForecastPeriod.localEndTime())
    return false;
  if (storyItemPeriodLength() > 2)
    return false;
  if (!thePreviousPrecipitationStoryItem || !thePreviousPrecipitationStoryItem->isIncluded())
    return false;
  return (thePreviousPrecipitationStoryItem->storyItemPeriodLength() >= 6 ||
          get_part_of_the_day_id_narrow(thePreviousPrecipitationStoryItem->getStoryItemPeriod()) ==
              get_part_of_the_day_id_narrow(getStoryItemPeriod()));
}

void CloudinessForecastStoryItem::buildPoutaantuuSentence(const CloudinessForecast& clForecast,
                                                          const PrecipitationForecast& prForecast)
{
  if (!thePreviousPrecipitationStoryItem || !thePreviousPrecipitationStoryItem->isIncluded())
    return;
  if (!thePreviousPrecipitationStoryItem->thePoutaantuuFlag)
    return;

  cloudiness_id cloudinessId = clForecast.getCloudinessId(getStoryItemPeriod());
  WeatherPeriod poutaantuuPeriod(
      thePreviousPrecipitationStoryItem->getStoryItemPeriod().localEndTime(),
      thePreviousPrecipitationStoryItem->getStoryItemPeriod().localEndTime());

  // ARE 22.02.2011: The missing period-phrase added
  Sentence thePeriodPhrase(getPeriodPhrase(USE_FROM_SPECIFIER, &poutaantuuPeriod));
  if (thePeriodPhrase.empty())
    thePeriodPhrase << theWeatherForecastStory.getTimePhrase();
  thePoutaantuuSentence << prForecast.precipitationPoutaantuuAndCloudiness(thePeriodPhrase,
                                                                           cloudinessId);
  thePreviousPrecipitationStoryItem->theReportPoutaantuuFlag = false;
  theReportAboutDryWeatherFlag = false;
}

Sentence CloudinessForecastStoryItem::buildCloudinessSentence(
    const CloudinessForecast& clForecast,
    const PrecipitationForecast& prForecast,
    const WeatherPeriod& storyItemPeriod)
{
  Sentence sentence;
  Sentence thePeriodPhrase;

  if (storyItemPeriod.localStartTime() > theWeatherForecastStory.theForecastPeriod.localStartTime())
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
  if (thePeriodPhrase.empty())
    thePeriodPhrase << theWeatherForecastStory.getTimePhrase();

  bool reportDry = theReportAboutDryWeatherFlag && !prForecast.getDryPeriodTautologyFlag();

  if (!theChangeSentence.empty())
  {
    WeatherPeriod clPeriod(storyItemPeriod.localStartTime(), theCloudinessChangeTimestamp);
    sentence << clForecast.cloudinessSentence(
        clPeriod, reportDry, thePeriodPhrase, DONT_USE_SHORT_FORM);
  }
  else
  {
    sentence << clForecast.cloudinessSentence(
        storyItemPeriod, reportDry, thePeriodPhrase, DONT_USE_SHORT_FORM);
  }
  prForecast.setDryPeriodTautologyFlag(theReportAboutDryWeatherFlag);

  // ARE 10.03.2011: Jos sää on melko selkeä ei enää sanota selkenevää
  if (!theChangeSentence.empty() && clForecast.getCloudinessId(getStoryItemPeriod()) > MELKO_SELKEA)
  {
    sentence << Delimiter(COMMA_PUNCTUATION_MARK);
    sentence << theChangeSentence;
  }
  return sentence;
}

Sentence CloudinessForecastStoryItem::getStoryItemSentence()
{
  if (!theIncludeInTheStoryFlag)
    return {};

  WeatherPeriod storyItemPeriod(getStoryItemPeriod());
  // if the cloudiness period is max 2 hours and it is in the end of the forecast period and
  // the previous precipitation period is long > 6h -> don't report cloudiness
  if (shouldSkipShortEndPeriod(storyItemPeriod))
    return {};

  const CloudinessForecast& clForecast = theWeatherForecastStory.theCloudinessForecast;
  const PrecipitationForecast& prForecast = theWeatherForecastStory.thePrecipitationForecast;

  theSentence.clear();
  theChangeSentence.clear();
  theShortFormSentence.clear();
  thePoutaantuuSentence.clear();

  theChangeSentence << cloudinessChangeSentence();
  buildPoutaantuuSentence(clForecast, prForecast);

  Sentence sentence;
  if (!thePoutaantuuSentence.empty())
  {
    sentence << thePoutaantuuSentence;
    // ARE 10.03.2011: Jos sää on melko selkeä ei enää sanota selkenevää
    if (!theChangeSentence.empty() && clForecast.getCloudinessId(storyItemPeriod) > MELKO_SELKEA)
    {
      sentence << Delimiter(COMMA_PUNCTUATION_MARK);
      sentence << theChangeSentence;
    }
  }
  else
  {
    sentence << buildCloudinessSentence(clForecast, prForecast, storyItemPeriod);
  }
  theWeatherForecastStory.theCloudinessReportedFlag = true;

  return sentence;
}
}  // namespace TextGen
