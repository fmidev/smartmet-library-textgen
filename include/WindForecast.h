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
  mutable part_of_the_day_id thePreviousPartOfTheDay;

  std::vector<Sentence> constructWindSentence(
      const WindEventPeriodDataItem* windSpeedItem,
      const WindEventPeriodDataItem* nextWindSpeedItem,
      std::vector<WeatherPeriod> windDirectionReportingPoints,
      WindDirectionInfo& thePreviousWindDirection,
      bool firstSentence) const;

  Sentence windDirectionSentenceWithAttribute(
      WindStoryTools::WindDirectionId theWindDirectionId) const;
  Sentence windSpeedIntervalSentence(const WeatherPeriod& thePeriod,
                                     bool theUseAtItsStrongestPhrase = true) const;
  Sentence windSpeedIntervalSentence(const WeatherPeriod& thePeriodLowerLimit,
                                     const WeatherPeriod& thePeriodUpperLimit,
                                     bool theUseAtItsStrongestPhrase = true) const;
  Sentence speedRangeSentence(const WeatherPeriod& thePeriod,
                              const WeatherResult& theMaxSpeed,
                              const WeatherResult& theMeanSpeed,
                              const std::string& theVariable,
                              bool theUseAtItsStrongestPhrase) const;
  Sentence getTimePhrase(const WeatherPeriod& thePeriod, bool useAlkaenPhrase) const;
  Sentence getTimePhrase(const TextGenPosixTime& theTime, bool useAlkaenPhrase) const;

  std::vector<WeatherPeriod> getWindSpeedReportingPoints(
      const WindEventPeriodDataItem& eventPeriodDataItem,
      bool firstSentenceInTheStory,
      WindEventId eventId) const;
  bool getWindSpeedChangeAttribute(const WeatherPeriod& changePeriod,
                                   std::string& phraseStr,
                                   bool& smallChange,
                                   bool& gradualChange,
                                   bool& fastChange) const;
  std::vector<Sentence> reportDirectionChanges(const WeatherPeriod& thePeriod,
                                               std::vector<WeatherPeriod>& theDirectionPeriods,
                                               WindDirectionInfo& thePreviousWindDirection,
                                               std::string& thePreviousTimePhrase,
                                               bool lastPeriod) const;
};

}  // namespace TextGen

#endif  // TEXTGEN_WIND_FORECAST_H
