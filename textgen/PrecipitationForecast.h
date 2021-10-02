#pragma once

#include "WeatherForecast.h"

namespace TextGen
{
// Declaration
class InPlacesPhrase
{
 public:
  enum PhraseType
  {
    NONEXISTENT_PHRASE,
    IN_SOME_PLACES_PHRASE,
    IN_MANY_PLACES_PHRASE
  };

  InPlacesPhrase();
  void preventTautology(bool preventTautologyFlag)
  {
    thePreventTautologyFlag = preventTautologyFlag;
  }
  Sentence getInPlacesPhrase(PhraseType thePhraseType, bool useOllaVerbFlag);

 private:
  PhraseType thePreviousPhrase;
  bool thePreventTautologyFlag;
};

class PrecipitationForecast
{
 public:
  PrecipitationForecast(wf_story_params& parameters);
  ~PrecipitationForecast();

  Sentence precipitationChangeSentence(
      const WeatherPeriod& thePeriod,
      const Sentence& thePeriodPhrase,
      weather_event_id theWeatherEvent,
      std::vector<std::pair<WeatherPeriod, Sentence>>& theAdditionalSentences) const;
  Sentence precipitationPoutaantuuAndCloudiness(const Sentence& thePeriodPhrase,
                                                cloudiness_id theCloudinessId) const;

  Sentence precipitationSentence(
      const WeatherPeriod& thePeriod,
      const Sentence& thePeriodPhrase,
      std::vector<std::pair<WeatherPeriod, Sentence>>& theAdditionalSentences) const;
  bool shortTermPrecipitationExists(const WeatherPeriod& thePeriod) const;
  Sentence shortTermPrecipitationSentence(const WeatherPeriod& thePeriod,
                                          const Sentence& thePeriodPhrase) const;
  bool isDryPeriod(const WeatherPeriod& theWeatherPeriod,
                   const unsigned short theForecastAreaId) const;
  bool isMostlyDryPeriod(const WeatherPeriod& theWeatherPeriod,
                         const unsigned short theForecastArea) const;
  bool getDryPeriods(const WeatherPeriod& theSourcePeriod,
                     std::vector<WeatherPeriod>& theDryPeriods) const;
  bool getPrecipitationPeriods(const WeatherPeriod& theSourcePeriod,
                               std::vector<WeatherPeriod>& thePrecipitationPeriods) const;

  void getWeatherEventIdVector(weather_event_id_vector& thePrecipitationWeatherEvents) const;

  void printOutPrecipitationVector(
      std::ostream& theOutput, const precipitation_data_vector& thePrecipitationDataVector) const;
  void printOutPrecipitationData(std::ostream& theOutput) const;
  void printOutPrecipitationPeriods(std::ostream& theOutput, bool isPoint) const;
  void printOutPrecipitationWeatherEvents(std::ostream& theOutput) const;
  void printOutPrecipitationDistribution(std::ostream& theOutput) const;
  void setUseOllaVerbFlag(const bool theFlag = true) const { theUseOllaVerbFlag = theFlag; }
  void setDryPeriodTautologyFlag(const bool theFlag) const { theDryPeriodTautologyFlag = theFlag; }
  bool getDryPeriodTautologyFlag() const { return theDryPeriodTautologyFlag; }
  void setSinglePrecipitationFormFlag(const bool theFlag) const
  {
    theSinglePrecipitationFormFlag = theFlag;
  }
  float getPrecipitationExtent(const WeatherPeriod& thePeriod,
                               const unsigned short theForecastArea) const;
  precipitation_type getPrecipitationType(const WeatherPeriod& thePeriod,
                                          const unsigned short theForecastArea) const;
  precipitation_form_id getPrecipitationForm(const WeatherPeriod& thePeriod,
                                             const unsigned short theForecastArea) const;
  float getMaxIntensity(const WeatherPeriod& thePeriod, const unsigned short theForecastArea) const;
  float getMeanIntensity(const WeatherPeriod& thePeriod,
                         const unsigned short theForecastArea) const;
  bool getPrecipitationPeriod(const TextGenPosixTime& theTimestamp,
                              TextGenPosixTime& theStartTime,
                              TextGenPosixTime& theEndTime) const;
  static bool singleForm(precipitation_form_id thePrecipitationForm);
  Sentence getThunderSentence(const WeatherPeriod& thePeriod,
                              unsigned short theForecastAreaId,
                              const std::string& theVariable) const;
  bool thunderExists(const WeatherPeriod& thePeriod,
                     unsigned short theForecastArea,
                     const std::string& theVariable) const;
  void setUseIcingPhraseFlag(bool theFlag) const { theUseIcingPhraseFlag = theFlag; }
  unsigned int getPrecipitationHours(precipitation_intesity_id intensityId,
                                     const WeatherPeriod& period) const;

 private:
  std::string getTimePhrase(part_of_the_day_id thePartOfTheDayId,
                            time_phrase_format theTimePhraseFormat) const;
  WeatherPeriod getHeavyPrecipitationPeriod(const WeatherPeriod& thePeriod,
                                            const precipitation_data_vector& theDataVector) const;

  Sentence parseFinalSentence(std::map<std::string, Sentence>& theCompositePhraseElements,
                              const Sentence& thePeriodPhrase,
                              const std::string& theAreaPhrase) const;
  bool getIntensityFormExtent(const WeatherPeriod& theWeatherPeriod,
                              const unsigned short theForecastArea,
                              float& theIntensity,
                              precipitation_form_id& theForm,
                              float& theExtent) const;

  AreaTools::Rect getPrecipitationRect(const TextGenPosixTime& theTimestamp,
                                       float theLowerLimit,
                                       float theUpperLimit) const;
  NFmiPoint getPrecipitationRepresentativePoint(const WeatherPeriod& thePeriod) const;
  AreaTools::direction_id getPrecipitationArrivalDirection(const WeatherPeriod& thePeriod) const;
  AreaTools::direction_id getPrecipitationLeavingDirection(const WeatherPeriod& thePeriod) const;
  precipitation_traverse_id getPrecipitationTraverseId(const WeatherPeriod& thePeriod) const;
  const precipitation_data_vector& getPrecipitationDataVector(
      const unsigned short theForecastArea) const;
  precipitation_form_transformation_id getPrecipitationTransformationId(
      const WeatherPeriod& thePeriod, const unsigned short theForecastArea) const;
  Sentence areaSpecificSentence(const WeatherPeriod& thePeriod) const;
  void joinPrecipitationPeriods(std::vector<WeatherPeriod>& thePrecipitationPeriodVector);
  bool printOutPrecipitationPeriods(std::ostream& theOutput,
                                    const std::vector<WeatherPeriod>& thePrecipitationPeriods,
                                    const precipitation_data_vector& theDataVector,
                                    bool isPoint) const;
  void printOutPrecipitationDistribution(std::ostream& theOutput,
                                         const precipitation_data_vector& theDataVector) const;
  void printOutPrecipitationDistribution(std::ostream& theOutput, const WeatherPeriod& thePeriod);
  void getPrecipitationDistribution(const WeatherPeriod& thePeriod,
                                    float& theNorthPercentage,
                                    float& theSouthPercentage,
                                    float& theEastPercentage,
                                    float& theWestPercentage,
                                    float& theNorthEastPercentage,
                                    float& theSouthEastPercentage,
                                    float& theSouthWestPercentage,
                                    float& theNorthWestPercentage) const;
  void cleanUpPrecipitationWeatherEvents(weather_event_id_vector& thePrecipitationWeatherEvents);
  void removeRedundantWeatherEvents(weather_event_id_vector& thePrecipitationWeatherEvents,
                                    const std::vector<int>& theRemoveIndexes);
  void removeDuplicatePrecipitationWeatherEvents(
      weather_event_id_vector& thePrecipitationWeatherEvents);
  float getMin(const precipitation_data_vector& theData,
               weather_result_data_id theDataId,
               const WeatherPeriod& theWeatherPeriod) const;
  float getMax(const precipitation_data_vector& theData,
               weather_result_data_id theDataId,
               const WeatherPeriod& theWeatherPeriod) const;
  float getMean(const precipitation_data_vector& theData,
                weather_result_data_id theDataId,
                const WeatherPeriod& theWeatherPeriod) const;

  unsigned int getPrecipitationCategory(float thePrecipitation, unsigned int theType) const;
  bool separateCoastInlandPrecipitation(const WeatherPeriod& theWeatherPeriod) const;
  void gatherPrecipitationData();
  void fillInPrecipitationDataVector(AreaTools::forecast_area_id theAreaId);
  void findOutPrecipitationPeriods();
  void findOutPrecipitationPeriods(AreaTools::forecast_area_id theAreaId);

  void findOutPrecipitationWeatherEvents();
  void findOutPrecipitationWeatherEvents(const std::vector<WeatherPeriod>& thePrecipitationPeriods,
                                         unsigned short theForecastArea,
                                         weather_event_id_vector& thePrecipitationWeatherEvents);

  void waterAndSnowShowersPhrase(float thePrecipitationIntensity,
                                 float thePrecipitationIntensityAbsoluteMax,
                                 float theWaterDrizzleSleetShare,
                                 bool theCanBeFreezingFlag,
                                 std::map<std::string, Sentence>& theCompositePhraseElements) const;

  void mostlyDryWeatherPhrase(bool theIsShowersFlag,
                              const WeatherPeriod& thePeriod,
                              const char* thePhrase,
                              std::map<std::string, Sentence>& theCompositePhraseElements) const;

  unsigned int getPrecipitationTypeChange(const precipitation_data_vector& theData,
                                          const WeatherPeriod& thePeriod) const;
  void getTransformationPhraseElements(
      const WeatherPeriod& thePeriod,
      float thePrecipitationExtent,
      precipitation_type thePrecipitationType,
      precipitation_form_transformation_id theTransformId,
      std::map<std::string, Sentence>& theCompositePhraseElements) const;
  void getPrecipitationPhraseElements(
      const WeatherPeriod& thePeriod,
      precipitation_form_id thePrecipitationForm,
      float thePrecipitationIntensity,
      float thePrecipitationIntensityAbsoluteMax,
      float thePrecipitationExtent,
      float thePrecipitationFormWater,
      float thePrecipitationFormDrizzle,
      float thePrecipitationFormSleet,
      float thePrecipitationFormSnow,
      float thePrecipitationFormFreezingRain,
      float thePrecipitationFormFreezingDrizzle,
      precipitation_type thePrecipitationType,
      const TextGenPosixTime& theTypeChangeTime,
      std::map<std::string, Sentence>& theCompositePhraseElements) const;
  std::string precipitationSentenceString(const WeatherPeriod& thePeriod,
                                          precipitation_form_id thePrecipitationForm,
                                          float thePrecipitationIntensity,
                                          float thePrecipitationIntensityAbsoluteMax,
                                          float thePrecipitationExtent,
                                          float thePrecipitationFormWater,
                                          float thePrecipitationFormDrizzle,
                                          float thePrecipitationFormSleet,
                                          float thePrecipitationFormSnow,
                                          float thePrecipitationFormFreezing,
                                          precipitation_type thePrecipitationType,
                                          const TextGenPosixTime& theTypeChangeTime) const;
  void selectPrecipitationSentence(
      const WeatherPeriod& thePeriod,
      precipitation_form_id thePrecipitationForm,
      float thePrecipitationIntensity,
      float thePrecipitationIntensityAbsoluteMax,
      float thePrecipitationExtent,
      float thePrecipitationFormWater,
      float thePrecipitationFormDrizzle,
      float thePrecipitationFormSleet,
      float thePrecipitationFormSnow,
      float thePrecipitationFormFreezingRain,
      float thePrecipitationFormFreezingDrizzle,
      precipitation_type thePrecipitationType,
      const TextGenPosixTime& theTypeChangeTime,
      precipitation_form_transformation_id theTransformationId,
      std::map<std::string, Sentence>& theCompositePhraseElements) const;

  Sentence constructPrecipitationSentence(
      const WeatherPeriod& thePeriod,
      const Sentence& thePeriodPhrase,
      unsigned short theForecastAreaId,
      const std::string& theAreaPhrase,
      std::vector<std::pair<WeatherPeriod, Sentence>>& theAdditionalSentences) const;

  void calculatePrecipitationParameters(const WeatherPeriod& thePeriod,
                                        const precipitation_data_vector& theDataVector,
                                        float& thePrecipitationIntensity,
                                        float& thePrecipitationAbsoluteMaxIntensity,
                                        float& thePrecipitationExtent,
                                        float& thePrecipitationFormWater,
                                        float& thePrecipitationFormDrizzle,
                                        float& thePrecipitationFormSleet,
                                        float& thePrecipitationFormSnow,
                                        float& thePrecipitationFormFreezingRain,
                                        float& thePrecipitationFormFreezingDrizzle) const;

  float getStat(const precipitation_data_vector& theData,
                weather_result_data_id theDataId,
                const WeatherPeriod& theWeatherPeriod,
                stat_func_id theStatFunc) const;

  bool reportPrecipitationFormsSeparately(precipitation_form_id form1,
                                          precipitation_form_id form2) const;

  precipitation_form_id getPoutaantuuPrecipitationForm() const;

  precipitation_data_vector theCoastalData;
  precipitation_data_vector theInlandData;
  precipitation_data_vector theFullData;

  weather_event_id_vector thePrecipitationWeatherEventsCoastal;
  weather_event_id_vector thePrecipitationWeatherEventsInland;
  weather_event_id_vector thePrecipitationWeatherEventsFull;

  std::vector<WeatherPeriod> thePrecipitationPeriodsCoastal;
  std::vector<WeatherPeriod> thePrecipitationPeriodsInland;
  std::vector<WeatherPeriod> thePrecipitationPeriodsFull;

  wf_story_params& theParameters;

  mutable bool theUseOllaVerbFlag;
  mutable bool theDryPeriodTautologyFlag;
  mutable bool theSinglePrecipitationFormFlag;
  mutable precipitation_form_id thePrecipitationFormBeforeDryPeriod;
  mutable precipitation_type theCheckHeavyIntensityFlag;
  mutable bool theUseIcingPhraseFlag;
};

}  // namespace TextGen
