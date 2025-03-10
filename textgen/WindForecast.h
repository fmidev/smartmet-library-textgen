#pragma once

#include "Paragraph.h"
#include "WeatherForecast.h"
#include "WindForecastStructs.h"
#include "WindStoryTools.h"

namespace TextGen
{
struct WindDirectionPeriodInfo
{
  WindDirectionPeriodInfo() : period(TextGenPosixTime(), TextGenPosixTime()) {}
  WindDirectionPeriodInfo(const WeatherPeriod& p, const WindDirectionInfo& i) : period(p), info(i)
  {
  }
  WeatherPeriod period;
  WindDirectionInfo info;
};

struct interval_info
{
  int lowerLimit = 0;
  int upperLimit = 0;
  int peakWind = 0;
  TextGenPosixTime startTime;
  TextGenPosixTime endTime;
  TextGenPosixTime peakWindTime;
};

struct interval_sentence_info
{
  Sentence sentence;
  WeatherPeriod period;
  interval_info intervalInfo;
  std::vector<WindDirectionInfo> directionChangesBefore;  // direction changes before interval
  std::vector<WindDirectionInfo> directionChangesAfter;   // direction changes after interval
  std::optional<WindDirectionInfo> directionChange;       // wind direction changes nearby
  // interval start time (so that we can
  // report them together)
  bool useAlkaenPhrase{false};
  bool skip{false};  // flag to indicate if interval is not reported (too close to previous)
  interval_sentence_info() : period(TextGenPosixTime(), TextGenPosixTime()) {}
};

// type of parameter in composite sentence
enum SentenceParameterType
{
  TIME_PERIOD,
  WIND_DIRECTION,
  CHANGE_TYPE,
  CHANGE_SPEED,
  WIND_SPEED_INTERVAL,
  VOID_TYPE
};

// Sentence is constructed by populating the sentence with parameters.
// Directon changes are checked in between
struct sentence_info
{
  Sentence sentence;
  WeatherPeriod period;          // period when wind strenghtens/weakens or wind direction
                                 // stays the
                                 // same??
  std::string changeType;        // e.g. voimistuvaa,heikenevää, alkaa voimistua,
                                 // alkaa heiketä
  std::string changeSpeed;       // e.g. nopeasti, vähitellen
  bool useWindBasicForm{false};  // basic form: 'tuuli', partitive form: 'tuulta'
  bool firstSentence{false};
  bool skip{false};
  interval_info intervalInfo;                        // for first period
  std::optional<WindDirectionInfo> directionChange;  // wind direction change nearby
                                                     // sentence start time (so that we can
                                                     // report them together)

  std::vector<interval_sentence_info> intervalSentences;
  std::vector<SentenceParameterType> sentenceParameterTypes;  // order of
                                                              // sentence
                                                              // parameters

  sentence_info() : period(TextGenPosixTime(), TextGenPosixTime()) {}
};

struct sentence_parameter
{
  sentence_parameter(SentenceParameterType t = VOID_TYPE) : type(t) {}
  SentenceParameterType type;
  Sentence sentence;
  TimePhraseInfo tpi;
};

struct paragraph_info
{
  Sentence sentence;
  std::vector<sentence_parameter> sentenceParameters;
};

using WindSpeedSentenceInfo = std::vector<sentence_info>;
using ParagraphInfoVector = std::vector<paragraph_info>;

class WindForecast
{
 public:
  WindForecast(wo_story_params& parameters);

  ~WindForecast() = default;
  Paragraph getWindStory(const WeatherPeriod& thePeriod) const;

 private:
  wo_story_params& theParameters;

  void constructWindSentence(const WindEventPeriodDataItem* windSpeedItem,
                             const WindEventPeriodDataItem* nextWindSpeedItem,
                             const WindDirectionPeriodInfo& firstDirectionPeriodInfo,
                             WindDirectionInfo& thePreviousWindDirection,
                             WindSpeedSentenceInfo& sentenceInfoVector) const;

  Sentence windDirectionSentenceWithAttribute(
      WindStoryTools::WindDirectionId theWindDirectionId) const;
  Sentence getTimePhrase(const WeatherPeriod& thePeriod,
                         TimePhraseInfo& timePhraseInfo,
                         bool useAlkaenPhrase) const;
  Sentence getTimePhrase(const TextGenPosixTime& theTime,
                         TimePhraseInfo& timePhraseInfo,
                         bool useAlkaenPhrase) const;

  std::vector<WeatherPeriod> getWindSpeedReportingPeriods(
      const WindEventPeriodDataItem& eventPeriodDataItem, bool firstSentenceInTheStory) const;
  bool getWindSpeedChangeAttribute(const WeatherPeriod& changePeriod,
                                   std::string& phraseStr,
                                   bool& smallChange,
                                   bool& gradualChange,
                                   bool& fastChange) const;
  std::vector<sentence_parameter> reportWindDirectionChanges(
      const std::vector<WindDirectionInfo>& directionChanges,
      TimePhraseInfo& timePhraseInfo,
      bool startWithComma = true) const;
  void injectWindDirections(WindSpeedSentenceInfo& sentenceInfoVector) const;
  void checkWindSpeedIntervals(WindSpeedSentenceInfo& sentenceInfoVector) const;
  interval_info windSpeedIntervalInfo(const WeatherPeriod& thePeriod) const;
  void checkTimePhrases(WindSpeedSentenceInfo& sentenceInfoVector) const;
  void checkWindDirections(WindSpeedSentenceInfo& sentenceInfoVector) const;

  bool negotiateNewPeriod(part_of_the_day_id& partOfTheDay,
                          WeatherPeriod& period1,
                          WeatherPeriod& period2,
                          const WeatherPeriod& notEarlierThanPeriod,
                          const WeatherPeriod& notLaterThanPeriod) const;
  WeatherPeriod getNewPeriod(part_of_the_day_id& partOfTheDay,
                             const WeatherPeriod& period,
                             const WeatherPeriod& guardPeriod,
                             unsigned int& hours,
                             bool backwards) const;

  Sentence windSpeedIntervalSentence(const WeatherPeriod& thePeriod,
                                     TimePhraseInfo& timePhraseInfo,
                                     bool theUseAtItsStrongestPhrase = true) const;
  Sentence windSpeedIntervalSentence(const WeatherPeriod& thePeriod,
                                     interval_info intervalInfo,
                                     TimePhraseInfo& timePhraseInfo,
                                     bool theUseAtItsStrongestPhrase) const;

  ParagraphInfoVector getParagraphInfo(const WeatherPeriod& thePeriod,
                                       const WindSpeedSentenceInfo& sentenceInfoVector) const;
};

}  // namespace TextGen
