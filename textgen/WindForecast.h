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
  int lowerLimit;
  int upperLimit;
  int peakWind;
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
  boost::optional<WindDirectionInfo> directionChange;     // wind direction changes nearby
  // interval start time (so that we can
  // report them together)
  bool useAlkaenPhrase;
  bool skip;  // flag to indicate if interval is not reported (too close to previous)
  interval_sentence_info()
      : period(TextGenPosixTime(), TextGenPosixTime()), useAlkaenPhrase(false), skip(false)
  {
  }
};

// type of parameter in composite sentence
enum SentenceParameterType
{
  TIME_PERIOD,
  WIND_DIRECTION,
  CHANGE_TYPE,
  CHANGE_SPEED,
  WIND_SPEED_INTERVAL
};

// Sentence is constructed by populating the sentence with parameters.
// Directon changes are checked in between?
struct sentence_info
{
  Sentence sentence;
  WeatherPeriod period;     // period when wind strenghtens/weakens or wind direction
                            // stays the
                            // same??
  std::string changeType;   // e.g. voimistuvaa,heikenevää, alkaa voimistua,
                            // alkaa heiketä
  std::string changeSpeed;  // e.g. nopeasti, vähitellen
  bool useWindBasicForm;    // basic form: 'tuuli', partitive form: 'tuulta'
  bool firstSentence;
  boost::optional<WindDirectionInfo> directionChange;  // wind direction change nearby
                                                       // sentence start time (so that we can
                                                       // report them together)

  std::vector<interval_sentence_info> intervalSentences;
  std::vector<SentenceParameterType> sentenceParameterTypes;  // order of
                                                              // sentence
                                                              // parameters

  sentence_info()
      : period(TextGenPosixTime(), TextGenPosixTime()),
        changeType(""),
        changeSpeed(""),
        useWindBasicForm(false),
        firstSentence(false)
  {
  }
};

struct sentence_parameter
{
  Sentence sentence;
  TimePhraseInfo tpi;
};

struct paragraph_info
{
  Sentence sentence;
  std::vector<sentence_parameter> sentenceParameters;
};

typedef std::vector<sentence_info> WindSpeedSentenceInfo;
typedef std::vector<paragraph_info> ParagraphInfoVector;

class WindForecast
{
 public:
  WindForecast(wo_story_params& parameters);

  ~WindForecast() {}
  Paragraph getWindStory(const WeatherPeriod& thePeriod) const;

 private:
  wo_story_params& theParameters;

  std::vector<Sentence> constructWindSentence(
      const WindEventPeriodDataItem* windSpeedItem,
      const WindEventPeriodDataItem* nextWindSpeedItem,
      std::vector<WeatherPeriod> windDirectionReportingPoints,
      WindDirectionInfo& thePreviousWindDirection,
      TimePhraseInfo& thePreviousTimePhrase,
      bool firstSentence) const;

  void constructWindSentence2(const WindEventPeriodDataItem* windSpeedItem,
                              const WindEventPeriodDataItem* nextWindSpeedItem,
                              const WindDirectionPeriodInfo& firstDirectionPeriodInfo,
                              WindDirectionInfo& thePreviousWindDirection,
                              WindSpeedSentenceInfo& sentenceInfoVector) const;

  Sentence windDirectionSentenceWithAttribute(
      WindStoryTools::WindDirectionId theWindDirectionId) const;
  Sentence windSpeedIntervalSentence(const WeatherPeriod& thePeriod,
                                     bool theUseAtItsStrongestPhrase = true) const;
  Sentence windSpeedIntervalSentence(const WeatherPeriod& thePeriodLowerLimit,
                                     const WeatherPeriod& thePeriodUpperLimit,
                                     bool theUseAtItsStrongestPhrase = true) const;
  Sentence speedRangeSentence(const WeatherPeriod& thePeriod,
                              bool theUseAtItsStrongestPhrase) const;
  Sentence speedRangeSentence2(const WeatherPeriod& thePeriod,
                               TimePhraseInfo& tpi,
                               bool theUseAtItsStrongestPhrase) const;

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
  std::vector<Sentence> reportDirectionChanges(std::vector<WeatherPeriod>& theDirectionPeriods,
                                               WindDirectionInfo& thePreviousWindDirection,
                                               const TimePhraseInfo& thePreviousTimePhraseInfo,
                                               bool theTuuliBasicForm) const;
  std::vector<sentence_parameter> reportWindDirectionChanges2(
      const std::vector<WindDirectionInfo>& directionChanges, TimePhraseInfo& timePhraseInfo) const;

  std::vector<Sentence> reportDirectionChanges(const WeatherPeriod& thePeriod,
                                               std::vector<WeatherPeriod>& theDirectionPeriods,
                                               WindDirectionInfo& thePreviousWindDirection,
                                               TimePhraseInfo& thePreviousTimePhrase,
                                               bool lastPeriod,
                                               bool voimistuvaa) const;
  void injectWindDirections(const wo_story_params& theParameters,
                            WindSpeedSentenceInfo& sentenceInfoVector,
                            const std::vector<WeatherPeriod>& windDirectionPeriods) const;
  void checkWindSpeedIntervals(WindSpeedSentenceInfo& sentenceInfoVector,
                               const std::vector<WeatherPeriod>& windDirectionPeriods) const;
  Paragraph constructParagraph(
      const WeatherPeriod& thePeriod,
      const WindSpeedSentenceInfo& sentenceInfoVector,
      const std::vector<WindDirectionPeriodInfo>& directionInfoVector) const;
  interval_info windSpeedIntervalInfo(const WeatherPeriod& thePeriod) const;
  Sentence reportWindDirectionChanges(const std::vector<WindDirectionInfo>& directionChanges,
                                      TimePhraseInfo& timePhraseInfo) const;
  void checkTimePhrases(WindSpeedSentenceInfo& sentenceInfoVector) const;
  void checkTimePhrases2(WindSpeedSentenceInfo& sentenceInfoVector) const;
  void checkWindDirections(WindSpeedSentenceInfo& sentenceInfoVector) const;

  WeatherPeriod negotiateNewPeriod(part_of_the_day_id& partOfTheDay,
                                   const WeatherPeriod& period) const;
  bool negotiateNewPeriod2(part_of_the_day_id& partOfTheDay,
                           WeatherPeriod& period1,
                           WeatherPeriod& period2,
                           const WeatherPeriod& notEarlierThanPeriod,
                           const WeatherPeriod& notLaterThanPeriod) const;
  WeatherPeriod getNewPeriod(part_of_the_day_id& partOfTheDay,
                             const WeatherPeriod& period,
                             const WeatherPeriod& guardPeriod,
                             unsigned int& hours,
                             bool backwards) const;

  Sentence windSpeedIntervalSentence2(const WeatherPeriod& thePeriod,
                                      TimePhraseInfo& tpi,
                                      bool theUseAtItsStrongestPhrase = true) const;
  ParagraphInfoVector getParagraphInfo(
      const WeatherPeriod& thePeriod,
      const WindSpeedSentenceInfo& sentenceInfoVector,
      const std::vector<WindDirectionPeriodInfo>& directionInfoVector) const;
};

}  // namespace TextGen