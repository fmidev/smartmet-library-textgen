// ======================================================================
/*!
 * \file
 * \brief Implementation of CloudinessForecast class
 */
// ======================================================================

#include "CloudinessForecast.h"
#include "AreaTools.h"
#include "CloudinessStory.h"
#include "CloudinessStoryTools.h"
#include "Delimiter.h"
#include "MessageLogger.h"
#include "NightAndDayPeriodGenerator.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "PrecipitationPeriodTools.h"
#include "PrecipitationStoryTools.h"
#include "SeasonTools.h"
#include "Sentence.h"
#include "SubMaskExtractor.h"
#include "ValueAcceptor.h"
#include "WeatherStory.h"
#include "WeekdayTools.h"
#include <calculator/GridForecaster.h>
#include <calculator/HourPeriodGenerator.h>
#include <calculator/MathTools.h>
#include <calculator/NullPeriodGenerator.h>
#include <calculator/RangeAcceptor.h>
#include <calculator/Settings.h>
#include <calculator/TimeTools.h>
#include <calculator/WeatherPeriodTools.h>
#include <calculator/WeatherResult.h>
#include <macgyver/Exception.h>
#include <math.h>

#include <boost/lexical_cast.hpp>
#include <map>
#include <vector>

namespace TextGen
{
using namespace Settings;
using namespace TextGen;
using namespace AreaTools;

using namespace std;

#define HUOMENNA_SISAMAASSA_PUOLIPILVISESTA_PILVISEEN_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] saa vaihtelee puolipilvisesta pilviseen"
#define SISAMAASSA_PUOLIPILVISESTA_PILVISEEN_COMPOSITE_PHRASE \
  "[sisamaassa] saa vaihtelee puolipilvisesta pilviseen"
#define HUOMENNA_PUOLIPILVISESTA_PILVISEEN_COMPOSITE_PHRASE \
  "[huomenna] saa vaihtelee puolipilvisesta pilviseen"

#define HUOMENNA_SISAMAASSA_PUOLIPILVISESTA_PILVISEEN_JA_POUTAINEN_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] saa vaihtelee puolipilvisesta pilviseen ja on poutainen"
#define SISAMAASSA_PUOLIPILVISESTA_PILVISEEN_JA_POUTAINEN_COMPOSITE_PHRASE \
  "[sisamaassa] saa vaihtelee puolipilvisesta pilviseen ja on poutainen"
#define HUOMENNA_PUOLIPILVISESTA_PILVISEEN_JA_POUTAINEN_COMPOSITE_PHRASE \
  "[huomenna] saa vaihtelee puolipilvisesta pilviseen ja on poutainen"

#define HUOMENNA_SISAMAASSA_SAA_ON_SELKEA_COMPOSITE_PHRASE "[huomenna] [sisamaassa] saa on [selkea]"
#define SISAMAASSA_SAA_ON_SELKEA_COMPOSITE_PHRASE "[sisamaassa] saa on [selkea]"
#define HUOMENNA_SAA_ON_SELKEA_COMPOSITE_PHRASE "[huomenna] saa on [selkea]"

#define HUOMENNA_SISAMAASSA_SAA_ON_SELKEA_JA_POUTAINEN_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] saa on [selkea] ja poutainen"
#define SISAMAASSA_SAA_ON_SELKEA_JA_POUTAINEN_COMPOSITE_PHRASE \
  "[sisamaassa] saa on [selkea] ja poutainen"
#define HUOMENNA_SAA_ON_SELKEA_JA_POUTAINEN_COMPOSITE_PHRASE \
  "[huomenna] saa on [selkea] ja poutainen"
#define SAA_ON_SELKEA_JA_POUTAINEN_COMPOSITE_PHRASE "saa on [selkea] ja poutainen"

#define ILTAPAIVASTA_ALKAEN_PILVISTYVAA_COMPOSITE_PHRASE "[iltapaivasta alkaen] [pilvistyvaa]"

#define PUOLIPILVISESTA_PILVISEEN_JA_POUTAINEN_PHRASE \
  "saa vaihtelee puolipilvisesta pilviseen ja on poutainen"

const char* cloudiness_string(const cloudiness_id& theCloudinessId)
{
  const char* retval = "";

  switch (theCloudinessId)
  {
    case SELKEA:
      retval = SELKEA_WORD;
      break;
    case MELKO_SELKEA:
      retval = MELKO_SELKEA_PHRASE;
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

Sentence cloudiness_sentence(const cloudiness_id& theCloudinessId, const bool& theShortForm)
{
  Sentence sentence;
  Sentence cloudinessSentence;

  switch (theCloudinessId)
  {
    case PUOLIPILVINEN_JA_PILVINEN:
      cloudinessSentence << VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE;
      break;
    case SELKEA:
      cloudinessSentence << SELKEA_WORD;
      break;
    case MELKO_SELKEA:
      cloudinessSentence << MELKO_SELKEA_PHRASE;
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

  if (!cloudinessSentence.empty() && !theShortForm)
  {
    if (theCloudinessId == PUOLIPILVINEN_JA_PILVINEN)
    {
      sentence << SAA_WORD;
    }
    else
    {
      sentence << SAA_WORD << ON_WORD;
    }
  }

  sentence << cloudinessSentence;

  return sentence;
}

Sentence cloudiness_sentence(const cloudiness_id& theCloudinessId,
                             const bool& thePoutainenFlag,
                             const Sentence& thePeriodPhrase,
                             const std::string& theAreaString,
                             const bool& theShortForm)
{
  Sentence sentence;
  Sentence cloudinessSentence;
  bool periodPhraseEmpty(thePeriodPhrase.empty());
  bool areaPhraseEmpty(theAreaString.empty() || theAreaString == EMPTY_STRING);

  if (thePeriodPhrase.empty() && theAreaString == EMPTY_STRING)
  {
    if (thePoutainenFlag)
    {
      if (theCloudinessId == PUOLIPILVINEN_JA_PILVINEN)
        sentence << PUOLIPILVISESTA_PILVISEEN_JA_POUTAINEN_PHRASE;
      else if (theCloudinessId != SELKEA)
        sentence << SAA_ON_SELKEA_JA_POUTAINEN_COMPOSITE_PHRASE
                 << cloudiness_string(theCloudinessId);
      else
        sentence << cloudiness_sentence(theCloudinessId, theShortForm);
    }
    else
    {
      sentence << cloudiness_sentence(theCloudinessId, theShortForm);
    }

    return sentence;
  }

  cloudinessSentence << cloudiness_sentence(theCloudinessId, true);

  if (theShortForm)
  {
    sentence << cloudinessSentence;
  }
  else
  {
    if (theCloudinessId == PUOLIPILVINEN_JA_PILVINEN)
    {
      if ((periodPhraseEmpty && !areaPhraseEmpty) || (!periodPhraseEmpty && areaPhraseEmpty))
      {
        if (periodPhraseEmpty)
        {
          if (thePoutainenFlag)
            sentence << SISAMAASSA_PUOLIPILVISESTA_PILVISEEN_JA_POUTAINEN_COMPOSITE_PHRASE
                     << theAreaString;
          else
            sentence << SISAMAASSA_PUOLIPILVISESTA_PILVISEEN_COMPOSITE_PHRASE << theAreaString;
        }
        else
        {
          if (thePoutainenFlag)
            sentence << HUOMENNA_PUOLIPILVISESTA_PILVISEEN_JA_POUTAINEN_COMPOSITE_PHRASE
                     << thePeriodPhrase;
          else
            sentence << HUOMENNA_PUOLIPILVISESTA_PILVISEEN_COMPOSITE_PHRASE << thePeriodPhrase;
        }
      }
      else
      {
        if (thePoutainenFlag)
          sentence << HUOMENNA_SISAMAASSA_PUOLIPILVISESTA_PILVISEEN_JA_POUTAINEN_COMPOSITE_PHRASE
                   << thePeriodPhrase << theAreaString;
        else
          sentence << HUOMENNA_SISAMAASSA_PUOLIPILVISESTA_PILVISEEN_COMPOSITE_PHRASE
                   << thePeriodPhrase << theAreaString;
      }
    }
    else
    {
      if ((periodPhraseEmpty && !areaPhraseEmpty) || (!periodPhraseEmpty && areaPhraseEmpty))
      {
        if (periodPhraseEmpty)
        {
          if (thePoutainenFlag && theCloudinessId != SELKEA)

            sentence << SISAMAASSA_SAA_ON_SELKEA_JA_POUTAINEN_COMPOSITE_PHRASE << theAreaString
                     << cloudinessSentence;
          else
            sentence << SISAMAASSA_SAA_ON_SELKEA_COMPOSITE_PHRASE << theAreaString
                     << cloudinessSentence;
        }
        else
        {
          if (thePoutainenFlag && theCloudinessId != SELKEA)
            sentence << HUOMENNA_SAA_ON_SELKEA_JA_POUTAINEN_COMPOSITE_PHRASE << thePeriodPhrase
                     << cloudinessSentence;
          else
            sentence << HUOMENNA_SAA_ON_SELKEA_COMPOSITE_PHRASE << thePeriodPhrase
                     << cloudinessSentence;
        }
      }
      else
      {
        if (thePoutainenFlag && theCloudinessId != SELKEA)
          sentence << HUOMENNA_SISAMAASSA_SAA_ON_SELKEA_JA_POUTAINEN_COMPOSITE_PHRASE
                   << thePeriodPhrase << theAreaString << cloudinessSentence;
        else
          sentence << HUOMENNA_SISAMAASSA_SAA_ON_SELKEA_COMPOSITE_PHRASE << thePeriodPhrase
                   << theAreaString << cloudinessSentence;
      }
    }
  }
  return sentence;
}

std::ostream& operator<<(std::ostream& theOutput,
                         const CloudinessDataItemData& theCloudinessDataItemData)
{
  string cloudinessIdStr(cloudiness_string(theCloudinessDataItemData.theId));

  string weatherEventIdStr = weather_event_string(theCloudinessDataItemData.theWeatherEventId);

  theOutput << "    " << cloudinessIdStr << ": ";
  theOutput << "min=" << theCloudinessDataItemData.theMin << " ";
  theOutput << "mean=" << theCloudinessDataItemData.theMean << " ";
  theOutput << "max=" << theCloudinessDataItemData.theMax << " ";
  theOutput << "std.dev=" << theCloudinessDataItemData.theStandardDeviation << '\n';
  theOutput << "    weather event: " << weatherEventIdStr << '\n';
  theOutput << "    pearson coefficient: " << theCloudinessDataItemData.thePearsonCoefficient
            << '\n';

  return theOutput;
}

std::ostream& operator<<(std::ostream& theOutput, const CloudinessDataItem& theCloudinessDataItem)
{
  if (theCloudinessDataItem.theCoastalData)
  {
    theOutput << "  Coastal\n";
    theOutput << *theCloudinessDataItem.theCoastalData;
  }
  if (theCloudinessDataItem.theInlandData)
  {
    theOutput << "  Inland\n";
    theOutput << *theCloudinessDataItem.theInlandData;
  }
  if (theCloudinessDataItem.theFullData)
  {
    theOutput << "  Full area\n";
    theOutput << *theCloudinessDataItem.theFullData;
  }
  return theOutput;
}

CloudinessForecast::CloudinessForecast(wf_story_params& parameters) : itsParameters(parameters)
{
  if (itsParameters.theForecastArea & INLAND_AREA && itsParameters.theForecastArea & COASTAL_AREA)
    itsFullData = ((*itsParameters.theCompleteData[FULL_AREA])[CLOUDINESS_DATA].get());
  if (itsParameters.theForecastArea & COASTAL_AREA)
    itsCoastalData = ((*itsParameters.theCompleteData[COASTAL_AREA])[CLOUDINESS_DATA].get());
  if (itsParameters.theForecastArea & INLAND_AREA)
    itsInlandData = ((*itsParameters.theCompleteData[INLAND_AREA])[CLOUDINESS_DATA].get());

  findOutCloudinessPeriods();
  joinPeriods();
  findOutCloudinessWeatherEvents();
}

Sentence CloudinessForecast::cloudinessChangeSentence(const WeatherPeriod& thePeriod) const
{
  Sentence sentence;

  const weather_event_id_vector& cloudinessWeatherEvents =
      ((itsParameters.theForecastArea & INLAND_AREA && itsParameters.theForecastArea & COASTAL_AREA)
           ? itsCloudinessWeatherEventsFull
           : ((itsParameters.theForecastArea & INLAND_AREA) ? itsCloudinessWeatherEventsInland
                                                            : itsCloudinessWeatherEventsCoastal));

  for (const auto& cloudinessWeatherEvent : cloudinessWeatherEvents)
  {
    TextGenPosixTime weatherEventTimestamp(cloudinessWeatherEvent.first);

    if (!(weatherEventTimestamp >= thePeriod.localStartTime() &&
          weatherEventTimestamp <= thePeriod.localEndTime()))
    {
      continue;
    }

    weather_event_id trid = cloudinessWeatherEvent.second;
    std::string timePhrase =
        get_time_phrase(weatherEventTimestamp, itsParameters.theVariable, true);
    if (timePhrase.empty())
      timePhrase = EMPTY_STRING;

    sentence << ILTAPAIVASTA_ALKAEN_PILVISTYVAA_COMPOSITE_PHRASE << timePhrase
             << (trid == PILVISTYY ? PILVISTYVAA_WORD : SELKENEVAA_WORD);
  }

  return sentence;
}

float CloudinessForecast::getMeanCloudiness(
    const WeatherPeriod& theWeatherPeriod,
    const weather_result_data_item_vector& theDataVector) const
{
  float cloudinessSum = 0;
  unsigned int count = 0;
  for (const auto& i : theDataVector)
  {
    if (i->thePeriod.localStartTime() >= theWeatherPeriod.localStartTime() &&
        i->thePeriod.localStartTime() <= theWeatherPeriod.localEndTime() &&
        i->thePeriod.localEndTime() >= theWeatherPeriod.localStartTime() &&
        i->thePeriod.localEndTime() <= theWeatherPeriod.localEndTime())
    {
      cloudinessSum += i->theResult.value();
      count++;
    }
  }
  return (count == 0 ? 0.0 : (cloudinessSum / count));
}

bool CloudinessForecast::separateCoastInlandCloudiness(const WeatherPeriod& theWeatherPeriod) const
{
  if (itsParameters.theCoastalAndInlandTogetherFlag)
    return false;

  weather_result_data_item_vector theInterestingDataCoastal;
  weather_result_data_item_vector theInterestingDataInland;

  get_sub_time_series(theWeatherPeriod, *itsCoastalData, theInterestingDataCoastal);

  float mean_coastal = get_mean(theInterestingDataCoastal);

  get_sub_time_series(theWeatherPeriod, *itsInlandData, theInterestingDataInland);

  float mean_inland = get_mean(theInterestingDataInland);

  bool retval = ((mean_inland <= 5.0 && mean_coastal >= 70.0) ||
                 (mean_inland >= 70.0 && mean_coastal <= 5.0));

  return retval;
}

bool CloudinessForecast::separateWeatherPeriodCloudiness(
    const WeatherPeriod& theWeatherPeriod1,
    const WeatherPeriod& theWeatherPeriod2,
    const weather_result_data_item_vector& theCloudinessData) const
{
  weather_result_data_item_vector theInterestingDataPeriod1;
  weather_result_data_item_vector theInterestingDataPeriod2;

  get_sub_time_series(theWeatherPeriod1, theCloudinessData, theInterestingDataPeriod1);

  cloudiness_id cloudinessId1 = getCloudinessId(get_mean(theInterestingDataPeriod1));

  get_sub_time_series(theWeatherPeriod2, theCloudinessData, theInterestingDataPeriod2);

  cloudiness_id cloudinessId2 = getCloudinessId(get_mean(theInterestingDataPeriod2));

  // difference must be at least two grades
  return (abs(cloudinessId1 - cloudinessId2) >= 2);
}

void CloudinessForecast::findOutCloudinessWeatherEvents(
    const weather_result_data_item_vector* theData,
    weather_event_id_vector& theCloudinessWeatherEvents)
{
  for (unsigned int i = 3; i < theData->size() - 3; i++)
  {
    WeatherPeriod firstHalfPeriod(theData->at(0)->thePeriod.localStartTime(),
                                  theData->at(i)->thePeriod.localEndTime());
    WeatherPeriod secondHalfPeriod(theData->at(i + 1)->thePeriod.localStartTime(),
                                   theData->at(theData->size() - 1)->thePeriod.localEndTime());

    weather_result_data_item_vector theFirstHalfData;
    weather_result_data_item_vector theSecondHalfData;

    get_sub_time_series(firstHalfPeriod, *theData, theFirstHalfData);

    get_sub_time_series(secondHalfPeriod, *theData, theSecondHalfData);

    float meanCloudinessInTheFirstHalf = get_mean(theFirstHalfData);
    float meanCloudinessInTheSecondHalf = get_mean(theSecondHalfData);
    weather_event_id weatherEventId = MISSING_WEATHER_EVENT;
    unsigned int changeIndex = 0;

    if (meanCloudinessInTheFirstHalf >= PILVISTYVAA_UPPER_LIMIT &&
        meanCloudinessInTheSecondHalf <= PILVISTYVAA_LOWER_LIMIT)
    {
      bool selkeneeReally = true;
      // check that cloudiness stays under the limit for the rest of the forecast period
      for (unsigned int k = i + 1; k < theData->size(); k++)
        if (theData->at(k)->theResult.value() > PILVISTYVAA_LOWER_LIMIT)
        {
          selkeneeReally = false;
          break;
        }
      if (!selkeneeReally)
        continue;

      changeIndex = i + 1;
      while (changeIndex >= 1 &&
             theData->at(changeIndex - 1)->theResult.value() <= PILVISTYVAA_LOWER_LIMIT)
        changeIndex--;

      weatherEventId = SELKENEE;
    }
    else if (meanCloudinessInTheFirstHalf <= PILVISTYVAA_LOWER_LIMIT &&
             meanCloudinessInTheSecondHalf >= PILVISTYVAA_UPPER_LIMIT)
    {
      bool pilvistyyReally = true;
      // check that cloudiness stays above the limit for the rest of the forecast period
      for (unsigned int k = i + 1; k < theData->size(); k++)
        if (theData->at(k)->theResult.value() < PILVISTYVAA_UPPER_LIMIT)
        {
          pilvistyyReally = false;
          break;
        }
      if (!pilvistyyReally)
        continue;

      changeIndex = i + 1;
      while (changeIndex >= 1 &&
             theData->at(changeIndex - 1)->theResult.value() >= PILVISTYVAA_UPPER_LIMIT)
        changeIndex--;

      weatherEventId = PILVISTYY;
    }

    if (weatherEventId != MISSING_WEATHER_EVENT)
    {
      theCloudinessWeatherEvents.emplace_back(theData->at(changeIndex)->thePeriod.localStartTime(),
                                              weatherEventId);
      // Note: only one event (pilvistyy/selkenee) during the period.
      // The Original plan was that several events are allowed
      break;
    }
  }
}

void CloudinessForecast::findOutCloudinessWeatherEvents()
{
  if (itsCoastalData)
    findOutCloudinessWeatherEvents(itsCoastalData, itsCloudinessWeatherEventsCoastal);
  if (itsInlandData)
    findOutCloudinessWeatherEvents(itsInlandData, itsCloudinessWeatherEventsInland);
  if (itsFullData)
    findOutCloudinessWeatherEvents(itsFullData, itsCloudinessWeatherEventsFull);
}

void CloudinessForecast::findOutCloudinessPeriods(const weather_result_data_item_vector* theData,
                                                  cloudiness_period_vector& theCloudinessPeriods)
{
  if (theData)
  {
    TextGenPosixTime previousStartTime;
    TextGenPosixTime previousEndTime;
    cloudiness_id previousCloudinessId = MISSING_CLOUDINESS_ID;
    for (unsigned int i = 0; i < theData->size(); i++)
    {
      if (i == 0)
      {
        previousStartTime = theData->at(i)->thePeriod.localStartTime();
        previousEndTime = theData->at(i)->thePeriod.localEndTime();
        previousCloudinessId = getCloudinessId(theData->at(i)->theResult.value());
      }
      else
      {
        if (previousCloudinessId != getCloudinessId(theData->at(i)->theResult.value()))
        {
          pair<WeatherPeriod, cloudiness_id> item =
              make_pair(WeatherPeriod(previousStartTime, previousEndTime), previousCloudinessId);
          theCloudinessPeriods.push_back(item);
          previousStartTime = theData->at(i)->thePeriod.localStartTime();
          previousEndTime = theData->at(i)->thePeriod.localEndTime();
          previousCloudinessId = getCloudinessId(theData->at(i)->theResult.value());
        }
        else if (i == theData->size() - 1)
        {
          pair<WeatherPeriod, cloudiness_id> item =
              make_pair(WeatherPeriod(previousStartTime, theData->at(i)->thePeriod.localEndTime()),
                        previousCloudinessId);
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
  findOutCloudinessPeriods(itsCoastalData, itsCloudinessPeriodsCoastal);
  findOutCloudinessPeriods(itsInlandData, itsCloudinessPeriodsInland);
  findOutCloudinessPeriods(itsFullData, itsCloudinessPeriodsFull);
}

void CloudinessForecast::joinPuolipilvisestaPilviseen(
    const weather_result_data_item_vector* theData,
    vector<int>& theCloudinessPuolipilvisestaPilviseen) const
{
  if (!theData)
    return;

  int index = -1;
  for (unsigned int i = 0; i < theData->size(); i++)
  {
    if (getCloudinessId(theData->at(i)->theResult.value()) >= PUOLIPILVINEN &&
        getCloudinessId(theData->at(i)->theResult.value()) <= PILVINEN && i != theData->size() - 1)
    {
      if (index == -1)
        index = i;
    }
    else if (getCloudinessId(theData->at(i)->theResult.value()) == SELKEA ||
             getCloudinessId(theData->at(i)->theResult.value()) == MELKO_SELKEA ||
             getCloudinessId(theData->at(i)->theResult.value()) == PILVINEN ||
             i == theData->size() - 1)
    {
      if (index != -1 && i - 1 - index > 1)
      {
        for (int k = index; k < static_cast<int>(i); k++)
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
  if (theCloudinessPeriodsSource.empty())
    return;

  vector<int> theCloudinessPuolipilvisestaPilviseen;

  int periodStartIndex = 0;
  cloudiness_id clidPrevious = theCloudinessPeriodsSource.at(0).second;

  for (unsigned int i = 1; i < theCloudinessPeriodsSource.size(); i++)
  {
    bool lastPeriod = (i == theCloudinessPeriodsSource.size() - 1);
    cloudiness_id clidCurrent = theCloudinessPeriodsSource.at(i).second;

    if (abs(clidPrevious - clidCurrent) >= 2 || lastPeriod)
    {
      // check if "puolipilvisesta pilviseen"
      /*
      if((clidPrevious == PUOLIPILVINEN && clidCurrent == PILVINEN) ||
         (clidPrevious == PILVINEN && clidCurrent == PUOLIPILVINEN))
        {
        }
      */

      TextGenPosixTime startTime(
          theCloudinessPeriodsSource.at(periodStartIndex).first.localStartTime());
      TextGenPosixTime endTime(
          theCloudinessPeriodsSource.at(lastPeriod ? i : i - 1).first.localEndTime());

      weather_result_data_item_vector thePeriodCloudiness;

      get_sub_time_series(
          WeatherPeriod(startTime, endTime), *theCloudinessDataSource, thePeriodCloudiness);

      float min;
      float max;
      float mean;
      float stddev;

      get_min_max(thePeriodCloudiness, min, max);
      mean = get_mean(thePeriodCloudiness);
      stddev = get_standard_deviation(thePeriodCloudiness);

      cloudiness_id actual_clid = getCloudinessId(min, mean, max, stddev);

      pair<WeatherPeriod, cloudiness_id> item =
          make_pair(WeatherPeriod(startTime, endTime), actual_clid);

      theCloudinessPeriodsDestination.push_back(item);

      clidPrevious = clidCurrent;
      periodStartIndex = i;
    }
  }
}

void CloudinessForecast::joinPeriods()
{
  joinPeriods(itsCoastalData, itsCloudinessPeriodsCoastal, itsCloudinessPeriodsCoastalJoined);
  joinPeriods(itsInlandData, itsCloudinessPeriodsInland, itsCloudinessPeriodsInlandJoined);
  joinPeriods(itsFullData, itsCloudinessPeriodsFull, itsCloudinessPeriodsFullJoined);
}

void CloudinessForecast::printOutCloudinessData(std::ostream& theOutput) const
{
  theOutput << "** CLOUDINESS DATA **\n";
  if (itsCoastalData)
  {
    theOutput << "Coastal cloudiness: \n";
    printOutCloudinessData(theOutput, itsCoastalData);
  }
  if (itsInlandData)
  {
    theOutput << "Inland cloudiness: \n";
    printOutCloudinessData(theOutput, itsInlandData);
  }
  if (itsFullData)
  {
    theOutput << "Full area cloudiness: \n";
    printOutCloudinessData(theOutput, itsFullData);
  }
}

void CloudinessForecast::printOutCloudinessData(
    std::ostream& theOutput, const weather_result_data_item_vector* theDataVector) const
{
  for (const auto& i : *theDataVector)
  {
    theOutput << i->thePeriod.localStartTime() << "..." << i->thePeriod.localEndTime() << ": "
              << i->theResult.value() << '\n';
  }
}

void CloudinessForecast::printOutCloudinessPeriods(
    std::ostream& theOutput, const cloudiness_period_vector& theCloudinessPeriods) const
{
  for (const auto& theCloudinessPeriod : theCloudinessPeriods)
  {
    WeatherPeriod period(theCloudinessPeriod.first.localStartTime(),
                         theCloudinessPeriod.first.localEndTime());
    cloudiness_id clid = theCloudinessPeriod.second;
    theOutput << period.localStartTime() << "..." << period.localEndTime() << ": "
              << cloudiness_string(clid) << '\n';
  }
}

void CloudinessForecast::printOutCloudinessWeatherEvents(std::ostream& theOutput) const
{
  theOutput << "** CLOUDINESS WEATHER EVENTS **\n";
  bool isWeatherEvents = false;
  if (!itsCloudinessWeatherEventsCoastal.empty())
  {
    theOutput << "Coastal weather events: \n";
    print_out_weather_event_vector(theOutput, itsCloudinessWeatherEventsCoastal);
    isWeatherEvents = true;
  }
  if (!itsCloudinessWeatherEventsInland.empty())
  {
    theOutput << "Inland weather events: \n";
    print_out_weather_event_vector(theOutput, itsCloudinessWeatherEventsInland);
    isWeatherEvents = true;
  }
  if (!itsCloudinessWeatherEventsFull.empty())
  {
    theOutput << "Full area weather events: \n";
    print_out_weather_event_vector(theOutput, itsCloudinessWeatherEventsFull);
    isWeatherEvents = true;
  }

  if (!isWeatherEvents)
    theOutput << "No weather events!\n";
}

void CloudinessForecast::printOutCloudinessPeriods(std::ostream& theOutput) const
{
  theOutput << "** CLOUDINESS PERIODS **\n";

  if (itsCoastalData)
  {
    theOutput << "Coastal cloudiness: \n";
    printOutCloudinessPeriods(theOutput, itsCloudinessPeriodsCoastal);
    theOutput << "Coastal cloudiness joined: \n";
    printOutCloudinessPeriods(theOutput, itsCloudinessPeriodsCoastalJoined);
    vector<int> theCloudinessPuolipilvisestaPilviseen;
    joinPuolipilvisestaPilviseen(itsCoastalData, theCloudinessPuolipilvisestaPilviseen);
  }
  if (itsInlandData)
  {
    theOutput << "Inland cloudiness: \n";
    printOutCloudinessPeriods(theOutput, itsCloudinessPeriodsInland);
    theOutput << "Inland cloudiness joined: \n";
    printOutCloudinessPeriods(theOutput, itsCloudinessPeriodsInlandJoined);
    vector<int> theCloudinessPuolipilvisestaPilviseen;
    joinPuolipilvisestaPilviseen(itsInlandData, theCloudinessPuolipilvisestaPilviseen);
  }
  if (itsFullData)
  {
    theOutput << "Full area cloudiness: \n";
    printOutCloudinessPeriods(theOutput, itsCloudinessPeriodsFull);
    theOutput << "Full area cloudiness joined: \n";
    printOutCloudinessPeriods(theOutput, itsCloudinessPeriodsFullJoined);
    vector<int> theCloudinessPuolipilvisestaPilviseen;
    joinPuolipilvisestaPilviseen(itsFullData, theCloudinessPuolipilvisestaPilviseen);
  }
}

void CloudinessForecast::getWeatherPeriodCloudiness(
    const WeatherPeriod& thePeriod,
    const cloudiness_period_vector& theSourceCloudinessPeriods,
    cloudiness_period_vector& theWeatherPeriodCloudiness) const
{
  for (const auto& theSourceCloudinessPeriod : theSourceCloudinessPeriods)
  {
    if (thePeriod.localStartTime() >= theSourceCloudinessPeriod.first.localStartTime() &&
        thePeriod.localEndTime() <= theSourceCloudinessPeriod.first.localEndTime())
    {
      const TextGenPosixTime& startTime = thePeriod.localStartTime();
      const TextGenPosixTime& endTime = thePeriod.localEndTime();
      cloudiness_id clid = theSourceCloudinessPeriod.second;
      pair<WeatherPeriod, cloudiness_id> item = make_pair(WeatherPeriod(startTime, endTime), clid);
      theWeatherPeriodCloudiness.push_back(item);
    }
    else if (thePeriod.localStartTime() >= theSourceCloudinessPeriod.first.localStartTime() &&
             thePeriod.localStartTime() < theSourceCloudinessPeriod.first.localEndTime() &&
             thePeriod.localEndTime() > theSourceCloudinessPeriod.first.localEndTime())
    {
      const TextGenPosixTime& startTime = thePeriod.localStartTime();
      const TextGenPosixTime& endTime = theSourceCloudinessPeriod.first.localEndTime();
      cloudiness_id clid = theSourceCloudinessPeriod.second;
      pair<WeatherPeriod, cloudiness_id> item = make_pair(WeatherPeriod(startTime, endTime), clid);
      theWeatherPeriodCloudiness.push_back(item);
    }
    else if (thePeriod.localStartTime() < theSourceCloudinessPeriod.first.localStartTime() &&
             thePeriod.localEndTime() > theSourceCloudinessPeriod.first.localStartTime() &&
             thePeriod.localEndTime() <= theSourceCloudinessPeriod.first.localEndTime())
    {
      TextGenPosixTime startTime = theSourceCloudinessPeriod.first.localStartTime();
      TextGenPosixTime endTime = thePeriod.localEndTime();
      cloudiness_id clid = theSourceCloudinessPeriod.second;
      pair<WeatherPeriod, cloudiness_id> item = make_pair(WeatherPeriod(startTime, endTime), clid);
      theWeatherPeriodCloudiness.push_back(item);
    }
    else if (thePeriod.localStartTime() < theSourceCloudinessPeriod.first.localStartTime() &&
             thePeriod.localEndTime() > theSourceCloudinessPeriod.first.localEndTime())
    {
      TextGenPosixTime startTime = theSourceCloudinessPeriod.first.localStartTime();
      TextGenPosixTime endTime = theSourceCloudinessPeriod.first.localEndTime();
      cloudiness_id clid = theSourceCloudinessPeriod.second;
      pair<WeatherPeriod, cloudiness_id> item = make_pair(WeatherPeriod(startTime, endTime), clid);
      theWeatherPeriodCloudiness.push_back(item);
    }
  }
}

Sentence CloudinessForecast::cloudinessSentence(const WeatherPeriod& thePeriod,
                                                const bool& theShortForm) const
{
  Sentence sentence;

  Sentence cloudinessSentence;

  cloudiness_id coastalCloudinessId = getCloudinessId(thePeriod, itsCoastalData);
  cloudiness_id inlandCloudinessId = getCloudinessId(thePeriod, itsInlandData);
  cloudiness_id fullAreaCloudinessId = getCloudinessId(thePeriod, itsFullData);

  if (itsParameters.theForecastArea & INLAND_AREA && itsParameters.theForecastArea & COASTAL_AREA)
  {
    if (separateCoastInlandCloudiness(thePeriod))
    {
      cloudinessSentence << COAST_PHRASE;
      cloudinessSentence << cloudiness_sentence(coastalCloudinessId, theShortForm);
      cloudinessSentence << Delimiter(COMMA_PUNCTUATION_MARK);
      cloudinessSentence << INLAND_PHRASE;
      cloudinessSentence << cloudiness_sentence(inlandCloudinessId, theShortForm);
    }
    else
    {
      cloudinessSentence << cloudiness_sentence(fullAreaCloudinessId, theShortForm);
    }
  }
  else if (itsParameters.theForecastArea & INLAND_AREA)
  {
    cloudinessSentence << cloudiness_sentence(inlandCloudinessId, theShortForm);
  }
  else if (itsParameters.theForecastArea & COASTAL_AREA)
  {
    cloudinessSentence << cloudiness_sentence(coastalCloudinessId, theShortForm);
  }

  if (!cloudinessSentence.empty())
  {
    sentence << cloudinessSentence;
  }

  /*
  if(sentence.size() > 0 && !(theParameters.theForecastArea & FULL_AREA))
    {
          cloudiness_id clid = (theParameters.theForecastArea & INLAND_AREA ? inlandCloudinessId :
  coastalCloudinessId);
          if(clid == VERRATTAIN_PILVINEN || clid == PILVINEN)
            sentence << areaSpecificSentence(thePeriod);
    }
  */

  return sentence;
}

Sentence CloudinessForecast::cloudinessSentence(const WeatherPeriod& thePeriod,
                                                const bool& thePoutainenFlag,
                                                const Sentence& thePeriodPhrase,
                                                const bool& theShortForm) const
{
  Sentence sentence;

  Sentence cloudinessSentence;

  cloudiness_id coastalCloudinessId = getCloudinessId(thePeriod, itsCoastalData);
  cloudiness_id inlandCloudinessId = getCloudinessId(thePeriod, itsInlandData);
  cloudiness_id fullAreaCloudinessId = getCloudinessId(thePeriod, itsFullData);

  if (itsParameters.theForecastArea & INLAND_AREA && itsParameters.theForecastArea & COASTAL_AREA)
  {
    if (separateCoastInlandCloudiness(thePeriod))
    {
      cloudinessSentence << COAST_PHRASE;
      cloudinessSentence << cloudiness_sentence(
          coastalCloudinessId, thePoutainenFlag, thePeriodPhrase, COAST_PHRASE, theShortForm);
      cloudinessSentence << Delimiter(COMMA_PUNCTUATION_MARK);
      cloudinessSentence << INLAND_PHRASE;
      cloudinessSentence << cloudiness_sentence(
          inlandCloudinessId, thePoutainenFlag, thePeriodPhrase, INLAND_PHRASE, theShortForm);
    }
    else
    {
      cloudinessSentence << cloudiness_sentence(
          fullAreaCloudinessId, thePoutainenFlag, thePeriodPhrase, EMPTY_STRING, theShortForm);
    }
  }
  else if (itsParameters.theForecastArea & INLAND_AREA)
  {
    cloudinessSentence << cloudiness_sentence(
        inlandCloudinessId, thePoutainenFlag, thePeriodPhrase, EMPTY_STRING, theShortForm);
  }
  else if (itsParameters.theForecastArea & COASTAL_AREA)
  {
    cloudinessSentence << cloudiness_sentence(
        coastalCloudinessId, thePoutainenFlag, thePeriodPhrase, EMPTY_STRING, theShortForm);
  }

  if (!cloudinessSentence.empty())
  {
    sentence << cloudinessSentence;
  }

  /*
  if(sentence.size() > 0 && !(theParameters.theForecastArea & FULL_AREA))
    {
          cloudiness_id clid = (theParameters.theForecastArea & INLAND_AREA ? inlandCloudinessId :
  coastalCloudinessId);
          if(clid == VERRATTAIN_PILVINEN || clid == PILVINEN)
            sentence << areaSpecificSentence(thePeriod);
    }
  */

  return sentence;
}

cloudiness_id CloudinessForecast::getCloudinessPeriodId(
    const TextGenPosixTime& theObservationTime,
    const cloudiness_period_vector& theCloudinessPeriodVector) const
{
  for (const auto& i : theCloudinessPeriodVector)
  {
    if (theObservationTime >= i.first.localStartTime() &&
        theObservationTime <= i.first.localEndTime())
      return i.second;
  }

  return MISSING_CLOUDINESS_ID;
}

cloudiness_id CloudinessForecast::getCloudinessId(const WeatherPeriod& thePeriod) const
{
  const weather_result_data_item_vector* theCloudinessDataVector = nullptr;

  if (itsParameters.theForecastArea & INLAND_AREA && itsParameters.theForecastArea & COASTAL_AREA)
    theCloudinessDataVector = itsFullData;
  else if (itsParameters.theForecastArea & INLAND_AREA)
    theCloudinessDataVector = itsInlandData;
  else if (itsParameters.theForecastArea & COASTAL_AREA)
    theCloudinessDataVector = itsCoastalData;

  return getCloudinessId(thePeriod, theCloudinessDataVector);
}

cloudiness_id CloudinessForecast::getCloudinessId(
    const WeatherPeriod& thePeriod, const weather_result_data_item_vector* theCloudinessData) const
{
  if (!theCloudinessData)
    return MISSING_CLOUDINESS_ID;

  weather_result_data_item_vector thePeriodCloudiness;

  get_sub_time_series(thePeriod, *theCloudinessData, thePeriodCloudiness);

  float min;
  float max;
  float mean;
  float stddev;
  get_min_max(thePeriodCloudiness, min, max);
  mean = get_mean(thePeriodCloudiness);
  stddev = get_standard_deviation(thePeriodCloudiness);
  cloudiness_id clid = getCloudinessId(min, mean, max, stddev);

  return clid;
}

void CloudinessForecast::getWeatherEventIdVector(
    weather_event_id_vector& theCloudinessWeatherEvents) const
{
  const weather_event_id_vector* vectorToClone = nullptr;

  if (itsParameters.theForecastArea & INLAND_AREA && itsParameters.theForecastArea & COASTAL_AREA)
    vectorToClone = &itsCloudinessWeatherEventsFull;
  else if (itsParameters.theForecastArea & COASTAL_AREA)
    vectorToClone = &itsCloudinessWeatherEventsCoastal;
  else if (itsParameters.theForecastArea & INLAND_AREA)
    vectorToClone = &itsCloudinessWeatherEventsInland;

  if (vectorToClone)
    theCloudinessWeatherEvents = *vectorToClone;
}

Sentence CloudinessForecast::areaSpecificSentence(const WeatherPeriod& /*thePeriod*/) const
{
  Sentence sentence;

  // TODO: tsekkaa, etta aluuen toisella puolella on selkeaa
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

cloudiness_id CloudinessForecast::getCloudinessId(const float& theMin,
                                                  const float& theMean,
                                                  const float& theMax,
                                                  const float& /*theStandardDeviation*/) const
{
  if (theMean == -1)
    return MISSING_CLOUDINESS_ID;

  if (theMin > itsParameters.theAlmostClearSkyUpperLimit &&
      theMin <= itsParameters.thePartlyCloudySkyUpperLimit &&
      theMax > itsParameters.theMostlyCloudySkyUpperLimit)
    return PUOLIPILVINEN_JA_PILVINEN;

  if (theMean <= itsParameters.theClearSkyUpperLimit)
    return SELKEA;

  if (theMean <= itsParameters.theAlmostClearSkyUpperLimit)
    return MELKO_SELKEA;

  if (theMean <= itsParameters.thePartlyCloudySkyUpperLimit)
    return PUOLIPILVINEN;

  if (theMean <= itsParameters.theMostlyCloudySkyUpperLimit)
    return VERRATTAIN_PILVINEN;

  return PILVINEN;
}

cloudiness_id CloudinessForecast::getCloudinessId(const float& theCloudiness) const
{
  if (theCloudiness < 0)
    return MISSING_CLOUDINESS_ID;

  if (theCloudiness <= itsParameters.theClearSkyUpperLimit)
    return SELKEA;

  if (theCloudiness <= itsParameters.theAlmostClearSkyUpperLimit)
    return MELKO_SELKEA;

  if (theCloudiness <= itsParameters.thePartlyCloudySkyUpperLimit)
    return PUOLIPILVINEN;

  if (theCloudiness <= itsParameters.theMostlyCloudySkyUpperLimit)
    return VERRATTAIN_PILVINEN;

  return PILVINEN;
}
}  // namespace TextGen
