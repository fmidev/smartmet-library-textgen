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

#define USE_FROM_SPECIFIER true
#define DONT_USE_FROM_SPECIFIER false
#define USE_SHORT_FORM true
#define DONT_USE_SHORT_FORM false
#define USE_TIME_SPECIFIER true
#define DONT_USE_TIME_SPECIFIER false

namespace TextGen
{
using namespace std;
using namespace TextGen;
using namespace Settings;

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
                << as_string(theStoryItem.thePeriodToMergeWith->thePeriod) << std::endl;
    else
      theOutput << " " << std::endl;
  }
  else
    theOutput << "exluded" << std::endl;

  return theOutput;
}

namespace
{
std::vector<unsigned int> get_story_item_indexes(
    const std::vector<WeatherForecastStoryItem*>& storyItems)
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
      theStorySize(0),
      theShortTimePrecipitationReportedFlag(false),
      theReportTimePhraseFlag(false),
      theCloudinessReportedFlag(false),
      thePeriodPhraseGenerator(var)
{
  addPrecipitationStoryItems();
  addCloudinessStoryItems();
  mergePeriodsWhenFeasible();

  bool specifyPartOfTheDayFlag =
      Settings::optional_bool(theVar + "::specify_part_of_the_day", true);
  int storyItemCounter(0);
  bool moreThanOnePrecipitationForms(false);
  for (auto & i : theStoryItemVector)
  {
    // when specifyPartOfTheDayFlag is false it means
    // that the period is short and part of the day should not be expressed
    if (!specifyPartOfTheDayFlag)
    {
      // ARE 14.4.2011: checking theIncludeInTheStoryFlag
      if (i->theIncludeInTheStoryFlag &&
          !i->getSentence().empty())
        storyItemCounter++;
    }

    // check wheather more than one precipitation form exists during the forecast period
    if (!moreThanOnePrecipitationForms &&
        i->theStoryPartId == PRECIPITATION_STORY_PART &&
        i->theIncludeInTheStoryFlag)
    {
      precipitation_form_id precipitationForm = thePrecipitationForecast.getPrecipitationForm(
          i->thePeriod, theParameters.theForecastArea);

      moreThanOnePrecipitationForms = !PrecipitationForecast::singleForm(precipitationForm);
    }
  }
  // if more than one item exists, use the phrases "aluksi", "myˆhemmin" when the period is short
  theReportTimePhraseFlag = storyItemCounter > 1;
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
  theLogger << "Start processing weather forcast at land" << std::endl;
  thePeriodPhraseGenerator.reset();

  Paragraph paragraph;

  theShortTimePrecipitationReportedFlag = false;
  theCloudinessReportedFlag = false;
  theStorySize = 0;

  thePrecipitationForecast.setDryPeriodTautologyFlag(false);

  for (auto & i : theStoryItemVector)
  {
    WeatherForecastStoryItem& item = *i;

    Sentence storyItemSentence;
    storyItemSentence << item.getSentence();

    if (!storyItemSentence.empty())
    {
      theStorySize += storyItemSentence.size();

      paragraph << storyItemSentence;

      // additional sentences: currently only in precipitation story: like "iltap‰iv‰st‰ alkaen sade
      // voi olla runsasta"
      for (unsigned int k = 0; k < i->numberOfAdditionalSentences(); k++)
      {
        std::pair<WeatherPeriod, Sentence> additionalSentence(
            i->getAdditionalSentence(k));
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
    for (auto & theAdditionalSentence : theAdditionalSentences)
    {
      paragraph << theAdditionalSentence.second;
    }
  }

  return paragraph;
}

Paragraph WeatherForecastStory::getWeatherForecastStoryAtSea()
{
  theLogger << "Start processing weather forcast at sea" << std::endl;

  thePrecipitationForecast.setUseIcingPhraseFlag(false);
  thePeriodPhraseGenerator.reset();

  Paragraph paragraph;
  std::vector<Sentence> weatherForecastSentences;

  unsigned int forecastPeriodLength = get_period_length(theForecastPeriod);
  unsigned int precipitationPeriodLength = 0;
  unsigned int precipitationAndFogPeriodLength = 0;
  bool inManyPlaces = false;
  bool badVisibility = false;
  unsigned int badVisibilityPeriodLength = 0;
  unsigned int badVisibilityPeriodLengthWater = 0;
  bool snowPrecipitationFormInvolved = false;

  thePrecipitationForecast.setDryPeriodTautologyFlag(false);
  theShortTimePrecipitationReportedFlag = false;
  theCloudinessReportedFlag = false;
  theStorySize = 0;

  std::vector<std::pair<story_part_id, unsigned int>> storyItems;

  for (unsigned int i = 0; i < theStoryItemVector.size(); i++)
  {
    WeatherForecastStoryItem* item = theStoryItemVector[i];
    theLogger << "Story item: " << as_string(item->getStoryItemPeriod())
              << (item->thePeriodToMergeWith ? " WITH " : " ") << std::endl;

    Sentence storyItemSentence;
    storyItemSentence << item->getSentence();

    if (storyItemSentence.empty())
      continue;

    // report only precipitation
    if (item->theStoryPartId == PRECIPITATION_STORY_PART ||
        item->theStoryPartId == PRECIPITATION_TYPE_CHANGE_STORY_PART)
    {
      theLogger << "Precipitation story item: " << as_string(item->getStoryItemPeriod())
                << std::endl;

      auto* precipitationStoryItem = static_cast<PrecipitationForecastStoryItem*>(item);

      bool sleetOrSnow = (precipitationStoryItem->theForm & SLEET_FORM ||
                          precipitationStoryItem->theForm & SNOW_FORM);

      bool weakPrecipitation = precipitationStoryItem->isWeakPrecipitation(theParameters);
      float precipitationExtent = precipitationStoryItem->precipitationExtent();
      WeatherPeriod precipitationPeriod = precipitationStoryItem->getStoryItemPeriod();

      theLogger << "Precipitation period (At Sea): " << as_string(precipitationPeriod) << ": "
                << (weakPrecipitation ? "weak!!, " : "not weak, ")
                << " extent: " << precipitationExtent << (sleetOrSnow ? ", sleet or snow" : "")
                << std::endl;

      // if precipitation form is sleet or snow, report it even if it is weak
      // and extent of precipitation area must be over 10 %
      if ((sleetOrSnow || !weakPrecipitation) && precipitationExtent > 10)
      {
        bool mergeDone = false;
        if (!storyItems.empty())
        {
          WeatherForecastStoryItem* previousItem = theStoryItemVector[storyItems.back().second];
          if (previousItem->theStoryPartId == PRECIPITATION_STORY_PART &&
              precipitationStoryItem->theStoryPartId == PRECIPITATION_STORY_PART)
          {
            auto* previousPrecipitationStoryItem =
                static_cast<PrecipitationForecastStoryItem*>(previousItem);
            if (previousPrecipitationStoryItem->precipitationExtent() >=
                    IN_SOME_PLACES_LOWER_LIMIT &&
                previousPrecipitationStoryItem->precipitationExtent() <=
                    IN_MANY_PLACES_UPPER_LIMIT &&
                precipitationExtent >= IN_SOME_PLACES_LOWER_LIMIT &&
                precipitationExtent <= IN_MANY_PLACES_UPPER_LIMIT &&
                precipitationPeriod.localStartTime().DifferenceInHours(
                    previousPrecipitationStoryItem->getStoryItemPeriod().localEndTime()) <= 2)
            {
              theLogger << "Merging periods  "
                        << as_string(previousPrecipitationStoryItem->getStoryItemPeriod())
                        << " and " << as_string(precipitationPeriod)
                        << " because they are close to each other" << std::endl;
              TextGenPosixTime startTime =
                  previousPrecipitationStoryItem->getStoryItemPeriod().localStartTime();
              TextGenPosixTime endTime = precipitationPeriod.localEndTime();
              previousPrecipitationStoryItem->thePeriod = WeatherPeriod(startTime, endTime);
              mergeDone = true;
            }
          }
        }
        if (!mergeDone)
          storyItems.emplace_back(item->theStoryPartId, i);
      }
    }

    // possible fog sentence after
    if (item->theStoryPartId == CLOUDINESS_STORY_PART)
    {
      theLogger << "Cloudiness story item: " << as_string(item->getStoryItemPeriod()) << std::endl;
      FogInfo fogInfo = theFogForecast.fogInfo(item->getStoryItemPeriod());

      auto* cloudinessStoryItem = static_cast<CloudinessForecastStoryItem*>(item);

      FogInfo fi = theFogForecast.fogInfo(item->getStoryItemPeriod());

      // if (storyItems.size() > 0)
      if (fi.id != NO_FOG)
      {
        storyItems.emplace_back(cloudinessStoryItem->theStoryPartId, i);
      }
    }
  }

  // if precipitation/fog lasts whole period
  if (!storyItems.empty())
  {
    WeatherForecastStoryItem* firstItemOfForecast = theStoryItemVector[0];
    WeatherForecastStoryItem* firstItemOfStory = theStoryItemVector[storyItems[0].second];
    // theStorySize is set in order to obtain time phrase for the first sentence
    // even if it does not start from the beginning of forecast period
    if (firstItemOfStory->getStoryItemPeriod().localStartTime() >
        firstItemOfForecast->getStoryItemPeriod().localStartTime())
      theStorySize += 1;
  }

  // if the successive periods of precipitation have
  // same type, form and extent report them together
  if (storyItems.size() > 1)
  {
    unsigned int firstIndex = 0;
    for (unsigned int i = 1; i < storyItems.size(); i++)
    {
      if (storyItems[firstIndex].first != CLOUDINESS_STORY_PART &&
          storyItems[i].first != CLOUDINESS_STORY_PART)
      {
        // successive precipitation periods
        auto* previousItem = static_cast<PrecipitationForecastStoryItem*>(
            theStoryItemVector[storyItems[firstIndex].second]);
        auto* currentItem =
            static_cast<PrecipitationForecastStoryItem*>(theStoryItemVector[storyItems[i].second]);
        if (previousItem->theType == currentItem->theType &&
            previousItem->theForm == currentItem->theForm)
        {
          // mark as missing
          storyItems[i].first = MISSING_STORY_PART;
          // merge periods
          previousItem->getStoryItemPeriod() =
              WeatherPeriod(previousItem->getStoryItemPeriod().localStartTime(),
                            currentItem->getStoryItemPeriod().localEndTime());
          // new the extent and intensity is time weighted average of both periods
          float previousPeriodLength = get_period_length(previousItem->getStoryItemPeriod());
          float currentPeriodLength = get_period_length(currentItem->getStoryItemPeriod());
          previousItem->theIntensity = (((previousItem->theIntensity * previousPeriodLength) +
                                         (currentItem->theIntensity * currentPeriodLength)) /
                                        (previousPeriodLength + currentPeriodLength));
          previousItem->theExtent = (((previousItem->theExtent * previousPeriodLength) +
                                      (currentItem->theExtent * currentPeriodLength)) /
                                     (previousPeriodLength + currentPeriodLength));
        }
      }
      else
        firstIndex = i;
    }
  }

  WeatherForecastStoryItem* previousCloudinessPeriod = nullptr;
  //  int previousStoryItemIndex = -1;
  for (unsigned int i = 0; i < storyItems.size(); i++)
  {
    if (storyItems[i].first == MISSING_STORY_PART)
      continue;
    int currentStoryItemIndex = storyItems[i].second;
    WeatherForecastStoryItem* item = theStoryItemVector[currentStoryItemIndex];

    if (storyItems[i].first == CLOUDINESS_STORY_PART)
    {
      // there can possibly be several successive fog sentences -> report them togethermerge them
      if (i < storyItems.size() - 1 && storyItems[i + 1].first == CLOUDINESS_STORY_PART)
      {
        FogInfo fogInfo = theFogForecast.fogInfo(item->getStoryItemPeriod());

        if (fogInfo.id != NO_FOG)
          if (!previousCloudinessPeriod)
            previousCloudinessPeriod = item;
        continue;
      }
      // report fog periods together
      if (previousCloudinessPeriod)
      {
        Sentence fogSentence;
        FogInfo firstFI = theFogForecast.fogInfo(previousCloudinessPeriod->getStoryItemPeriod());
        FogInfo secondFI = theFogForecast.fogInfo(item->getStoryItemPeriod());

        // take the sentence from longest period
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
          weatherForecastSentences.push_back(fogSentence);
          theStorySize += fogSentence.size();

          if (!firstFI.sentence.empty() && !secondFI.sentence.empty())
            theLogger << "Fog periods: " << as_string(firstFI.period) << " and "
                      << as_string(secondFI.period) << " -> " << as_string(fogSentence)
                      << std::endl;
          else if (!firstFI.sentence.empty())
            theLogger << "Fog period (first): " << as_string(firstFI.period) << " -> "
                      << as_string(fogSentence) << std::endl;
          else if (!secondFI.sentence.empty())
            theLogger << "Fog period (second): " << as_string(secondFI.period) << " -> "
                      << as_string(fogSentence) << std::endl;
        }

        if (firstFI.id == FOG || firstFI.id == FOG_POSSIBLY_DENSE)
          precipitationAndFogPeriodLength += get_period_length(previousCloudinessPeriod->thePeriod);
        if (secondFI.id == FOG || secondFI.id == FOG_POSSIBLY_DENSE)
          precipitationAndFogPeriodLength += get_period_length(item->thePeriod);
      }
      else
      {
        FogInfo fi = theFogForecast.fogInfo(item->thePeriod);
        Sentence fogSentence;
        fogSentence << fi.timePhrase << fi.sentence;
        if (!fogSentence.empty())
        {
          weatherForecastSentences.push_back(fogSentence);
          theLogger << "Fog period: " << as_string(fi.period) << " -> " << as_string(fogSentence)
                    << std::endl;
          theStorySize += fogSentence.size();
          // only extensive fog counted here
          if (fi.id == FOG || fi.id == FOG_POSSIBLY_DENSE)
            precipitationAndFogPeriodLength += get_period_length(item->thePeriod);
        }
      }
      previousCloudinessPeriod = nullptr;
    }
    else
    {
      auto* precipitationItem = static_cast<PrecipitationForecastStoryItem*>(item);

      float precipitationExtent = precipitationItem->precipitationExtent();

      if (precipitationExtent <= 10)
      {
        theLogger << "Precipitation extent at period " << as_string(precipitationItem->thePeriod)
                  << " is less than 10% (" << precipitationExtent << ") -> dont report"
                  << std::endl;
        continue;
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
          // sateen olomuoto = LUMI, r‰nt‰, j‰‰t‰v‰, sateen tyyppi kuuro tai jatkuva (eli kaikki) ja
          // sateen intensiteetti > heikko ainakin 3 tuntia laajuus > ja monin paikoin
          badVisibilityPeriodLength += (moderatePrecipitationHours + heavyPrecipitationHours);
          badVisibility = true;
        }
        else if (((preciptationForm & WATER_FORM) || (preciptationForm & SLEET_FORM) ||
                  (preciptationForm & DRIZZLE_FORM)) &&
                 precipitationItem->theType == SHOWERS &&
                 precipitationExtent >= theParameters.theInManyPlacesLowerLimit)
        {
          // sateen olomuoto = VESI, r‰nt‰, tihku, sateen tyyppi KUURO ja sateen intensiteetti >=
          // rankka ainakin kolme tuntia ja laajuus > monin paikoin
          // Elokuussa 2017 Kiira-myrskyn j‰lkeen p‰‰tettiin, ett‰ wesisateella ei raportoida huonoa
          // n‰kyvyytt‰
          //          badVisibilityPeriodLengthWater += heavyPrecipitationHours;
          //          badVisibility = true;
        }
      }

      Sentence precipitationSentence;
      precipitationSentence << precipitationItem->getSentence();
      theStorySize += precipitationSentence.size();
      theLogger << "Precipitation story item: "
                << as_string(precipitationItem->getStoryItemPeriod())
                << (precipitationSentence.empty() ? ", empty sentence" : " not empty sentence")
                << ", "
                << (precipitationItem->thePoutaantuuFlag ? "poutaantuu = true"
                                                         : "poutaantuu = false")
                << std::endl;

      if (!precipitationSentence.empty() || precipitationItem->thePoutaantuuFlag)
      {
        weatherForecastSentences.push_back(precipitationSentence);
        if (!inManyPlaces)
        {
          inManyPlaces = (precipitationExtent > theParameters.theInManyPlacesLowerLimit &&
                          precipitationExtent <= theParameters.theInManyPlacesUpperLimit);
        }
        // only continuous precipitation with large extent is counted here
        if (precipitationItem->theType == CONTINUOUS &&
            precipitationExtent >= IN_MANY_PLACES_UPPER_LIMIT)
          precipitationAndFogPeriodLength += get_period_length(precipitationItem->thePeriod);
        precipitationPeriodLength += get_period_length(precipitationItem->thePeriod);
        snowPrecipitationFormInvolved = (precipitationItem->precipitationForm() & SNOW_FORM);
        theLogger << "Precipitation period: " << as_string(precipitationItem->thePeriod) << " -> "
                  << as_string(precipitationSentence) << std::endl;
      }
    }
    // reset the today phrase when day changes
    /*
if (previousStoryItemIndex > -1 &&
    theStoryItemVector[previousStoryItemIndex]->thePeriod.localEndTime().GetJulianDay() !=
        theStoryItemVector[currentStoryItemIndex]->thePeriod.localEndTime().GetJulianDay() &&
    get_period_length(theForecastPeriod) > 24)
  const_cast<WeatherHistory&>(theWeatherArea.history())
      .updateTimePhrase("", "", TextGenPosixTime(1970, 1, 1));
    */

    //    previousStoryItemIndex = currentStoryItemIndex;
  }
#ifdef LATER
  // turha?: jos ei yksitt‰isi‰ sade/sumuperiodeja, ei reportoida koko ennustejaksoa kerralla
  if (weatherForecastSentences.empty())
  {
    if (!thePrecipitationForecast.isDryPeriod(theForecastPeriod, theParameters.theForecastArea))

    {
      Sentence precipitationSentence;
      Sentence periodPhrase;
      std::vector<std::pair<WeatherPeriod, Sentence>> theAdditionalSentences;
      precipitationSentence << thePrecipitationForecast.precipitationSentence(
          theForecastPeriod, periodPhrase, theAdditionalSentences);

      if (!precipitationSentence.empty())
      {
        weatherForecastSentences.push_back(precipitationSentence);
        theStorySize += precipitationSentence.size();
      }
    }
  }
#endif

  Sentence visibilitySentence;
  if (weatherForecastSentences.empty())
  {
    visibilitySentence << HYVA_NAKYVYYS_PHRASE;
  }
  else if (!theParameters.theShortTextModeFlag && precipitationPeriodLength != 0)
  {
    float precipitationPeriodShare =
        (static_cast<float>(precipitationPeriodLength) / static_cast<float>(forecastPeriodLength));

    float heavyRainPeriodShare = (static_cast<float>(badVisibilityPeriodLengthWater) /
                                  static_cast<float>(forecastPeriodLength));

    // sateen kesto > 50 % jakson pituudesta, huono n‰kyvyys v‰hint‰‰n 3h (lumi, r‰nt‰ j‰‰t‰v‰)
    // tai yli 50% (vesi, r‰nt‰, tihku)
    if (badVisibility && precipitationPeriodShare > 0.50 &&
        (badVisibilityPeriodLength >= 3 || heavyRainPeriodShare > 0.50))
    {
      Sentence badVisibilitySentence;
      badVisibilitySentence << HUONO_NAKYVYYS_PHRASE;
      weatherForecastSentences.push_back(badVisibilitySentence);
    }
    else if (precipitationPeriodShare <= 0.50 && !inManyPlaces)
    {
      weatherForecastSentences.back()
          << Delimiter(COMMA_PUNCTUATION_MARK) << ENIMMAKSEEN_HYVA_NAKYVYYS_PHRASE;
    }
    else if (!snowPrecipitationFormInvolved &&
             (static_cast<float>(precipitationAndFogPeriodLength) <
                  static_cast<float>(forecastPeriodLength) ||
              inManyPlaces))
    {
      weatherForecastSentences.back()
          << Delimiter(COMMA_PUNCTUATION_MARK) << MUUTEN_HYVA_NAKYVYYS_PHRASE;
    }
  }

  if (!visibilitySentence.empty())
    weatherForecastSentences.push_back(visibilitySentence);

  for (const auto& s : weatherForecastSentences)
    paragraph << s;

  theLogger << "Weather Forecast At Sea: " << std::endl
            << " length of continuous precipitation or extensive fog: "
            << precipitationAndFogPeriodLength << std::endl
            << " forecats period length: " << forecastPeriodLength << std::endl
            << " precipitation period length: " << precipitationPeriodLength << std::endl
            << " bad visibility period length: " << badVisibilityPeriodLength << std::endl
            << " bad visibility period length for water: " << badVisibilityPeriodLengthWater
            << std::endl
            << " precipitation or fog in many places: " << (inManyPlaces ? "yes" : "no")
            << std::endl
            << " length of forecast text: " << paragraph.size() << std::endl;
  if (!paragraph.empty())
    theLogger << " forecast text: " << as_string(paragraph) << std::endl;

  return paragraph;
}

void WeatherForecastStory::addPrecipitationStoryItems()
{
  vector<WeatherPeriod> precipitationPeriods;

  thePrecipitationForecast.getPrecipitationPeriods(theForecastPeriod, precipitationPeriods);

  PrecipitationForecastStoryItem* previousPrItem = nullptr;
  WeatherForecastStoryItem* missingStoryItem = nullptr;
  for (auto & precipitationPeriod : precipitationPeriods)
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

    auto* item = new PrecipitationForecastStoryItem(*this,
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
        missingStoryItem = new WeatherForecastStoryItem(
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
    auto* item =
        new CloudinessForecastStoryItem(*this,
                                        cloudinessPeriod,
                                        CLOUDINESS_STORY_PART,
                                        theCloudinessForecast.getCloudinessId(cloudinessPeriod),
                                        nullptr,
                                        nullptr);

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
  PrecipitationForecastStoryItem* previousPrecipitationStoryItem = nullptr;
  for (unsigned int i = 0; i < theStoryItemVector.size(); i++)
  {
    if (theStoryItemVector[i]->theStoryPartId == MISSING_STORY_PART)
    {
      WeatherForecastStoryItem* placeholder = theStoryItemVector[i];

      auto* cloudinessStoryItem = new CloudinessForecastStoryItem(
          *this,
          placeholder->thePeriod,
          CLOUDINESS_STORY_PART,
          theCloudinessForecast.getCloudinessId(placeholder->thePeriod),
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
            static_cast<PrecipitationForecastStoryItem*>(theStoryItemVector[i + 1]);

      theStoryItemVector[i] = cloudinessStoryItem;

      delete placeholder;
    }
    else if (theStoryItemVector[i]->theStoryPartId == PRECIPITATION_STORY_PART)
    {
      previousPrecipitationStoryItem =
          static_cast<PrecipitationForecastStoryItem*>(theStoryItemVector[i]);
    }
  }
}

void WeatherForecastStory::mergePrecipitationPeriodsWhenFeasible()
{
  PrecipitationForecastStoryItem* previousPrecipitationStoryItem = nullptr;
  PrecipitationForecastStoryItem* currentPrecipitationStoryItem = nullptr;
  std::vector<unsigned int> indexes = get_story_item_indexes(theStoryItemVector);

  for (unsigned int i = 0; i < indexes.size(); i++)
  {
    WeatherForecastStoryItem* currentStoryItem = theStoryItemVector[indexes[i]];
    WeatherForecastStoryItem* previousStoryItem =
        (i > 0 ? theStoryItemVector[indexes[i - 1]] : nullptr);
    if (!currentStoryItem->theIncludeInTheStoryFlag)
      continue;

    if (currentStoryItem->theStoryPartId == PRECIPITATION_STORY_PART)
    {
      currentPrecipitationStoryItem =
          static_cast<PrecipitationForecastStoryItem*>(currentStoryItem);
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
          if (previousStoryItem != previousPrecipitationStoryItem &&
              previousStoryItem->storyItemPeriodLength() <= 1)
            previousStoryItem->theIncludeInTheStoryFlag = false;
          previousPrecipitationStoryItem = currentPrecipitationStoryItem;
          continue;
        }

        TextGenPosixTime gapPeriodStartTime(
            previousPrecipitationStoryItem->thePeriod.localEndTime());
        gapPeriodStartTime.ChangeByHours(+1);
        TextGenPosixTime gapPeriodEndTime(
            currentPrecipitationStoryItem->thePeriod.localStartTime());
        if (gapPeriodStartTime < gapPeriodEndTime)
          gapPeriodEndTime.ChangeByHours(-1);
        WeatherPeriod gapPeriod(gapPeriodStartTime, gapPeriodEndTime);

        // merge periods
        if (get_period_length(gapPeriod) <= 3 &&
            (get_period_length(gapPeriod) <=
             currentPrecipitationStoryItem->storyItemPeriodLength() +
                 previousPrecipitationStoryItem->storyItemPeriodLength() + 2))
        {
          // merge two precipitation periods
          previousPrecipitationStoryItem->thePeriodToMergeWith = currentPrecipitationStoryItem;
          currentPrecipitationStoryItem->theIncludeInTheStoryFlag = false;
          if (previousStoryItem->theStoryPartId == CLOUDINESS_STORY_PART)
            previousStoryItem->theIncludeInTheStoryFlag = false;
        }
      }
      previousPrecipitationStoryItem = currentPrecipitationStoryItem;
    }
  }

  indexes = get_story_item_indexes(theStoryItemVector);
  int storyItemCount = indexes.size();
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
    // short precipitation period in the end after coudiness period is not reported
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
    WeatherForecastStoryItem* currentStoryItem = theStoryItemVector[indexe];
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
  theLogger << "******** STORY ITEMS BEFORE MERGE ********" << endl;
  for (auto & i : theStoryItemVector)
    theLogger << as_string(i->getStoryItemPeriod()) << std::endl;

  mergeCloudinessPeriodsWhenFeasible();
  mergePrecipitationPeriodsWhenFeasible();

  theLogger << "******** STORY ITEMS AFTER MERGE ********" << endl;
  for (auto & i : theStoryItemVector)
  {
    if (!i->theIncludeInTheStoryFlag)
      continue;

    if (i->thePeriodToMergeWith)
      theLogger << as_string(i->getStoryItemPeriod())
                << "  merged periods: " << as_string(i->thePeriod) << " and "
                << as_string(i->thePeriodToMergeWith->thePeriod) << std::endl;
    else
      theLogger << as_string(i->getStoryItemPeriod()) << std::endl;
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

  return sentence;
}

void WeatherForecastStory::logTheStoryItems() const
{
  theLogger << "******** STORY ITEMS ********" << endl;
  for (auto *i : theStoryItemVector)
  {
    theLogger << *i;
  }
}

WeatherForecastStoryItem::WeatherForecastStoryItem(WeatherForecastStory& weatherForecastStory,
                                                   const WeatherPeriod& period,
                                                   story_part_id storyPartId)
    : theWeatherForecastStory(weatherForecastStory),
      thePeriod(period),
      theStoryPartId(storyPartId),
      theIncludeInTheStoryFlag(true),
      thePeriodToMergeWith(nullptr)
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

// special treatment, because 06:00 can be aamuyˆ and morning, depends weather the period starts
// or
// ends
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

  return WeatherPeriod(starttime, endtime);
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
    }
    break;
    case AAMUYO:
    case KESKIYO_JA_AAMUYO:
    case AAMUYO_JA_AAMU:
    {
      dayNum = period.localEndTime().GetWeekday();
    }
    break;
    default:
      break;
  };
  if (dayNum > -1)
    dayNumber += (to_string(dayNum) + "-");

  return dayNum;
}

void check_percentage(const WeatherPeriod& period,
                      const std::string& part_of_day,
                      part_of_the_day_id  /*id*/,
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
      if (dayNum == -1 || dayExists(dayNum))
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

Sentence WeatherForecastStoryItem::getPeriodPhrase(bool theFromSpecifier,
                                                   const WeatherPeriod* thePhrasePeriod /*= 0*/,
                                                   bool  /*theStoryUnderConstructionEmpty*/ /*= true*/)
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

  return false;
}

Sentence PrecipitationForecastStoryItem::getStoryItemSentence()
{
  Sentence sentence;

  if (!theIncludeInTheStoryFlag)
    return sentence;

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
      WeatherPeriod poutaantuuPeriod(storyItemPeriod.localEndTime(),
                                     storyItemPeriod.localEndTime());
      thePeriodPhrase << getPeriodPhrase(
          DONT_USE_FROM_SPECIFIER, &poutaantuuPeriod, sentence.empty());
      theWeatherForecastStory.theLogger << thePeriodPhrase;
      if (thePeriodPhrase.empty())
        thePeriodPhrase << theWeatherForecastStory.getTimePhrase();

      if (!sentence.empty())
        sentence << Delimiter(",");
      sentence << prForecast.precipitationChangeSentence(
          storyItemPeriod, thePeriodPhrase, POUTAANTUU, theAdditionalSentences);
    }
    theWeatherForecastStory.theShortTimePrecipitationReportedFlag = false;
  }
  else
  {
    if (thePeriod.localStartTime() > forecastPeriod.localStartTime())
    {
      thePeriodPhrase << getPeriodPhrase(DONT_USE_FROM_SPECIFIER, &storyItemPeriod);
    }
    if (thePeriodPhrase.empty())
    {
      thePeriodPhrase << theWeatherForecastStory.getTimePhrase();
    }

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

  for (auto & cloudinessEvent : cloudinessEvents)
  {
    TextGenPosixTime cloudinessEventTimestamp(cloudinessEvent.first);
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

  if (!theIncludeInTheStoryFlag)
    return sentence;

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
      if (thePeriodPhrase.empty())
        thePeriodPhrase << theWeatherForecastStory.getTimePhrase();
      thePoutaantuuSentence << prForecast.precipitationPoutaantuuAndCloudiness(thePeriodPhrase,
                                                                               cloudinessId);
      thePreviousPrecipitationStoryItem->theReportPoutaantuuFlag = false;
      theReportAboutDryWeatherFlag = false;
    }
  }

  if (!thePoutaantuuSentence.empty())
  {
    sentence << thePoutaantuuSentence;

    // ARE 10.03.2011: Jos s‰‰ on melko selke‰ ei en‰‰ sanota selkenev‰‰
    if (!theChangeSentence.empty() && clForecast.getCloudinessId(storyItemPeriod) > MELKO_SELKEA)
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
    if (thePeriodPhrase.empty())
      thePeriodPhrase << theWeatherForecastStory.getTimePhrase();

    if (!theChangeSentence.empty())
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

    // ARE 10.03.2011: Jos s‰‰ on melko selke‰ ei en‰‰ sanota selkenev‰‰
    if (!theChangeSentence.empty() &&
        clForecast.getCloudinessId(getStoryItemPeriod()) > MELKO_SELKEA)
    {
      sentence << Delimiter(COMMA_PUNCTUATION_MARK);
      sentence << theChangeSentence;
    }
  }
  theWeatherForecastStory.theCloudinessReportedFlag = true;

  return sentence;
}
}  // namespace TextGen
