#pragma once

#include <map>
#include <memory>
#include <set>

class MessageLogger;

namespace TextGen
{
class Sentence;
class Paragraph;
class WeatherForecastStory;

class PeriodPhraseGenerator
{
 public:
  PeriodPhraseGenerator(const std::string& var) : itsVar(var) {}

  bool dayExists(int n) const;
  bool phraseExists(const WeatherPeriod& period) const;
  Sentence getPeriodPhrase(const WeatherPeriod& period);
  void reset();

 private:
  std::set<int> dayNumbers;
  std::map<WeatherPeriod, Sentence> periodPhrases;
  const std::string& itsVar;
};

class WeatherForecastStoryItem
{
 public:
  WeatherForecastStoryItem(WeatherForecastStory& weatherForecastStory,
                           WeatherPeriod period,
                           story_part_id storyPartId);

  virtual ~WeatherForecastStoryItem() = default;
  Sentence getSentence();
  // if periods are merged this must be overwritten
  virtual WeatherPeriod getStoryItemPeriod() const;
  virtual Sentence getStoryItemSentence() { return {}; }
  unsigned int getPeriodLength() const;
  int forecastPeriodLength() const;
  int storyItemPeriodLength() const;
  Sentence getTodayVectorSentence(const std::vector<Sentence*>& todayVector,
                                  unsigned int theBegIndex,
                                  unsigned int theEndIndex);
  Sentence getPeriodPhrase();
  Sentence getPeriodPhrase(bool theFromSpecifier,
                           const WeatherPeriod* thePhrasePeriod = nullptr,
                           bool theStoryUnderConstructionEmpty = true);
  std::string checkForAamuyoAndAamuPhrase(bool theFromSpecifier,
                                          const WeatherPeriod& thePhrasePeriod);
  Sentence checkForExtendedPeriodPhrase(const WeatherPeriod& thePhrasePeriod);

  story_part_id getStoryPartId() const { return theStoryPartId; }
  const WeatherPeriod& getPeriod() const { return thePeriod; }
  bool isIncluded() const { return theIncludeInTheStoryFlag; }
  unsigned int numberOfAdditionalSentences() const { return theAdditionalSentences.size(); }
  std::pair<WeatherPeriod, Sentence> getAdditionalSentence(unsigned int index) const;

  // protected:
  std::vector<std::pair<WeatherPeriod, Sentence>> theAdditionalSentences;
  WeatherForecastStory& theWeatherForecastStory;
  WeatherPeriod thePeriod;
  story_part_id theStoryPartId;
  // can be used to control if this item is included in the story
  bool theIncludeInTheStoryFlag = false;
  Sentence theSentence;

  // if periods are merged this points to the megreable period
  WeatherForecastStoryItem* thePeriodToMergeWith = nullptr;

  friend class WeatherForecastStory;

 private:
};

class PrecipitationForecastStoryItem : public WeatherForecastStoryItem
{
 public:
  PrecipitationForecastStoryItem(WeatherForecastStory& weatherForecastStory,
                                 const WeatherPeriod& period,
                                 story_part_id storyPartId,
                                 float intensity,
                                 float extent,
                                 unsigned int form,
                                 precipitation_type type,
                                 bool thunder);

  Sentence getStoryItemSentence() override;
  bool isWeakPrecipitation(const wf_story_params& theParameters) const;
  float precipitationExtent() const;
  unsigned int precipitationForm() const;

 private:
  float theIntensity = 0;
  float theExtent = 0;
  unsigned int theForm = 0;
  precipitation_type theType;

  // this flag indicates wheather thunder exists or not during the period
  bool theThunder = false;

  // this flag indicates whether we tell about
  // ending precipitation period (poutaantuu)
  bool thePoutaantuuFlag = false;

  // if precipitation has been weak we dont report ending of it
  bool theReportPoutaantuuFlag = false;

  int theFullDuration = 0;  // includes precipitation period beyond the forecast period

  friend class WeatherForecastStory;
  friend class CloudinessForecastStoryItem;
};

class CloudinessForecastStoryItem : public WeatherForecastStoryItem
{
 public:
  CloudinessForecastStoryItem(WeatherForecastStory& weatherForecastStory,
                              const WeatherPeriod& period,
                              story_part_id storyPartId,
                              PrecipitationForecastStoryItem* previousPrecipitationStoryItem,
                              PrecipitationForecastStoryItem* nextPrecipitationStoryItem);

  Sentence getStoryItemSentence() override;
  Sentence cloudinessChangeSentence();

 private:
  PrecipitationForecastStoryItem* thePreviousPrecipitationStoryItem;
  PrecipitationForecastStoryItem* theNextPrecipitationStoryItem;
  bool theReportAboutDryWeatherFlag = false;
  Sentence thePoutaantuuSentence;
  Sentence theShortFormSentence;
  Sentence theChangeSentence;
  TextGenPosixTime theCloudinessChangeTimestamp;

  friend class WeatherForecastStory;
};

class WeatherForecastStory
{
 public:
  WeatherForecastStory(const std::string& var,
                       const TextGen::WeatherPeriod& forecastPeriod,
                       const TextGen::WeatherArea& weatherArea,
                       wf_story_params& parameters,
                       const TextGenPosixTime& forecastTime,
                       PrecipitationForecast& precipitationForecast,
                       const CloudinessForecast& cloudinessForecast,
                       const FogForecast& fogForecast,
                       const ThunderForecast& thunderForecast,
                       MessageLogger& logger);

  ~WeatherForecastStory();

  Paragraph getWeatherForecastStory();
  Paragraph getWeatherForecastStoryAtSea();
  const WeatherPeriod& getStoryPeriod() const { return theForecastPeriod; }
  const std::vector<std::shared_ptr<WeatherForecastStoryItem>>& getStoryItemVector() const
  {
    return theStoryItemVector;
  }
  Sentence getPeriodPhrase(const WeatherPeriod& period);

  void logTheStoryItems() const;

 private:
  void addPrecipitationStoryItems();
  void addCloudinessStoryItems();
  void mergePeriodsWhenFeasible();
  Sentence getTimePhrase() const;
  void mergePrecipitationPeriodsWhenFeasible();
  void mergeCloudinessPeriodsWhenFeasible();

  const std::string theVar;
  const WeatherPeriod& theForecastPeriod;
  const WeatherArea& theWeatherArea;
  wf_story_params& theParameters;
  const TextGenPosixTime& theForecastTime;
  const PrecipitationForecast& thePrecipitationForecast;
  const CloudinessForecast& theCloudinessForecast;
  const FogForecast& theFogForecast;
  const ThunderForecast& theThunderForecast;
  MessageLogger& theLogger;
  int theStorySize = 0;
  bool theShortTimePrecipitationReportedFlag = false;
  bool theReportTimePhraseFlag = false;
  bool theCloudinessReportedFlag = false;
  bool theAddAluksiWord = false;
  PeriodPhraseGenerator thePeriodPhraseGenerator;

  std::vector<std::shared_ptr<WeatherForecastStoryItem>> theStoryItemVector;

  friend class PrecipitationForecastStoryItem;
  friend class CloudinessForecastStoryItem;
  friend class WeatherForecastStoryItem;
};
}  // namespace TextGen
