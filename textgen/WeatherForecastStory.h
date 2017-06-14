#pragma once

class MessageLogger;

namespace TextGen
{
class Sentence;
class Paragraph;
class WeatherForecastStory;

class WeatherForecastStoryItem
{
 public:
  WeatherForecastStoryItem(WeatherForecastStory& weatherForecastStory,
                           const WeatherPeriod& period,
                           story_part_id storyPartId);

  virtual ~WeatherForecastStoryItem() {}
  Sentence getSentence();
  // if periods are merged this must be overwritten
  virtual WeatherPeriod getStoryItemPeriod() const;
  virtual Sentence getStoryItemSentence() { return Sentence(); }
  unsigned int getPeriodLength();
  int forecastPeriodLength() const;
  int storyItemPeriodLength() const;
  Sentence getTodayVectorSentence(const std::vector<Sentence*>& todayVector,
                                  unsigned int theBegIndex,
                                  unsigned int theEndIndex);
  Sentence getPeriodPhrase();
  Sentence getPeriodPhrase(bool theFromSpecifier,
                           const WeatherPeriod* thePhrasePeriod = 0,
                           bool theStoryUnderConstructionEmpty = true);
  std::string checkForAamuyoAndAamuPhrase(bool theFromSpecifier,
                                          const WeatherPeriod& thePhrasePeriod);
  Sentence checkForExtendedPeriodPhrase(const WeatherPeriod& thePhrasePeriod);

  story_part_id getStoryPartId() const { return theStoryPartId; }
  const WeatherPeriod& getPeriod() const { return thePeriod; }
  bool isIncluded() const { return theIncludeInTheStoryFlag; }
  unsigned int numberOfAdditionalSentences() { return theAdditionalSentences.size(); }
  Sentence getAdditionalSentence(unsigned int index) const;

 protected:
  std::vector<Sentence> theAdditionalSentences;
  WeatherForecastStory& theWeatherForecastStory;
  WeatherPeriod thePeriod;
  story_part_id theStoryPartId;
  // can be used to control if this item is included in the story
  bool theIncludeInTheStoryFlag;
  Sentence theSentence;
  WeatherForecastStoryItem*
      thePeriodToMergeWith;  // if periods are merged this points to the megreable period
  WeatherForecastStoryItem*
      thePeriodToMergeTo;  // if periods are merged this points to the merged period

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

  Sentence getStoryItemSentence();

 private:
  float theIntensity;
  float theExtent;
  unsigned int theForm;
  precipitation_type theType;
  // this flag indicates wheather thunder exists or not during the period
  bool theThunder;
  // this flag indicates if there has been a gap between precipitation periods
  bool theSadeJatkuuFlag;
  // this flag indicates whether we tell about
  // ending precipitation period (poutaantuu)
  bool thePoutaantuuFlag;
  // if precipitation has been weak we dont report ending of it
  bool theReportPoutaantuuFlag;
  int theFullDuration;  // includes precipitation period beyond the forecast period

  friend class WeatherForecastStory;
  friend class CloudinessForecastStoryItem;
};

class CloudinessForecastStoryItem : public WeatherForecastStoryItem
{
 public:
  CloudinessForecastStoryItem(WeatherForecastStory& weatherForecastStory,
                              const WeatherPeriod& period,
                              story_part_id storyPartId,
                              cloudiness_id cloudinessId,
                              PrecipitationForecastStoryItem* previousPrecipitationStoryItem,
                              PrecipitationForecastStoryItem* nextPrecipitationStoryItem);

  Sentence getStoryItemSentence();
  Sentence cloudinessChangeSentence();

 private:
  cloudiness_id theCloudinessId;
  PrecipitationForecastStoryItem* thePreviousPrecipitationStoryItem;
  PrecipitationForecastStoryItem* theNextPrecipitationStoryItem;
  bool theReportAboutDryWeatherFlag;
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
                       const wf_story_params& parameters,
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
  const std::vector<WeatherForecastStoryItem*>& getStoryItemVector() const
  {
    return theStoryItemVector;
  }
  void logTheStoryItems() const;

 private:
  void addPrecipitationStoryItems();
  void addCloudinessStoryItems();
  void mergePeriodsWhenFeasible();
  Sentence getTimePhrase();
  void mergePrecipitationPeriodsWhenFeasible();
  void mergeCloudinessPeriodsWhenFeasible();

  const std::string theVar;
  const WeatherPeriod& theForecastPeriod;
  const WeatherArea& theWeatherArea;
  const wf_story_params& theParameters;
  const TextGenPosixTime& theForecastTime;
  const PrecipitationForecast& thePrecipitationForecast;
  const CloudinessForecast& theCloudinessForecast;
  const FogForecast& theFogForecast;
  const ThunderForecast& theThunderForecast;
  MessageLogger& theLogger;
  int theStorySize;
  bool theShortTimePrecipitationReportedFlag;
  bool theReportTimePhraseFlag;
  bool theCloudinessReportedFlag;

  std::vector<WeatherForecastStoryItem*> theStoryItemVector;

  friend class PrecipitationForecastStoryItem;
  friend class CloudinessForecastStoryItem;
  friend class WeatherForecastStoryItem;
};
}
