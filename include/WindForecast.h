#ifndef TEXTGEN_WIND_FORECAST_H
#define TEXTGEN_WIND_FORECAST_H

#include "WindStoryTools.h"
#include "WindForecastStructs.h"
#include "WeatherForecast.h"
#include "Paragraph.h"

namespace TextGen
{
class WindForecast
{
 public:
  WindForecast(wo_story_params& parameters);

  ~WindForecast() {}
  Paragraph getWindStory(const WeatherPeriod& thePeriod) const;

 private:
  wo_story_params& theParameters;
  mutable short thePreviousDayNumber;
  mutable short thePreviousPartOfTheDay;

  Sentence windDirectionSentenceWithAttribute(
      WindStoryTools::WindDirectionId theWindDirectionId) const;
  Sentence windDirectionSentence(WindStoryTools::WindDirectionId theDirectionId,
                                 bool theBasicForm = false) const;
  Sentence windSpeedIntervalSentence(const WeatherPeriod& thePeriod,
                                     bool theUseAtItsStrongestPhrase = true) const;
  Sentence speedRangeSentence(const WeatherPeriod& thePeriod,
                              const WeatherResult& theMaxSpeed,
                              const WeatherResult& theMeanSpeed,
                              const std::string& theVariable,
                              bool theUseAtItsStrongestPhrase) const;
  Sentence decreasingIncreasingInterval(const WindEventPeriodDataItem& eventPeriodDataItem,
                                        WindEventPeriodDataItem* nextItemWithMissingEvent,
                                        bool firstSentenceInTheStory,
                                        WindEventId eventId) const;
  Sentence getTimePhrase(const WeatherPeriod& thePeriod, bool useAlkaenPhrase) const;
  Sentence getTimePhrase(const TextGenPosixTime& theTime, bool useAlkaenPhrase) const;

  std::vector<WeatherPeriod> getWindSpeedReportingPoints(
      const WindEventPeriodDataItem& eventPeriodDataItem,
      bool firstSentenceInTheStory,
      WindEventId eventId) const;
  bool getSpeedIntervalLimits(const WeatherPeriod& thePeriod,
                              WeatherResult& lowerLimit,
                              WeatherResult& upperLimit) const;
  bool getWindSpeedChangeAttribute(const WeatherPeriod& changePeriod,
                                   std::string& phraseStr,
                                   bool& smallChange,
                                   bool& gradualChange,
                                   bool& fastChange) const;
  std::string getWindDirectionTurntoString(
      WindStoryTools::WindDirectionId theWindDirectionId) const;
  bool samePartOfTheDay(const TextGenPosixTime& time1, const TextGenPosixTime& time2) const;
  Sentence windDirectionChangeSentence(const wo_story_params& theParameters,
                                       const WeatherPeriod& eventPeriod,
                                       WindEventId eventIdPrevious,
                                       WindStoryTools::WindDirectionId& previousWindDirectionId,
                                       const WindEventPeriodDataItem* eventPeriodItemNext) const;

  std::vector<Sentence> windDirectionAndSpeedChangesSentence(
      const wo_story_params& theParameters,
      const WeatherPeriod& eventPeriod,
      bool firstSentenceInTheStory,
      WindEventId previousWindEventId,
      WindEventId currentWindEventId,
      WindStoryTools::WindDirectionId& previousWindDirectionId,
      bool theWindSpeedChangeStarts,
      bool theWindSpeedChangeEnds,
      WeatherPeriod& windSpeedIntervalPeriodPreviousReported,
      bool& appendDecreasingIncreasingInterval) const;
  void getWindDirectionBegEnd(const WeatherPeriod& thePeriod,
                              WindStoryTools::WindDirectionId& theWindDirectionIdBeg,
                              WindStoryTools::WindDirectionId& theWindDirectionIdEnd) const;
  bool windDirectionTurns(const WeatherPeriod& thePeriod) const;
  void getPeriodStartAndEndIndex(const WeatherPeriod& period,
                                 unsigned int& begIndex,
                                 unsigned int& endIndex) const;
  void logWindSpeedPeriod(const WeatherPeriod& period, const Sentence& intervalSentence) const;
  void findOutActualWindSpeedChangePeriod(WindEventPeriodDataItem* currentEventPeriodItem,
                                          WindEventPeriodDataItem* nextEventPeriodItem) const;
  int windSpeedDifference(const WeatherPeriod& thePeriod1, const WeatherPeriod& thePeriod2) const;
};

}  // namespace TextGen

#endif  // TEXTGEN_WIND_FORECAST_H
