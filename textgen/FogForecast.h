#pragma once

#include "PrecipitationForecast.h"
#include "Sentence.h"
#include "WeatherForecast.h"

namespace TextGen
{

// Use this to enable or disable dense fogs in actual output. We use
// this so as not to destroy the original algorithm in case we want
// dense fogs back later on.

#define ENABLE_DENSE_FOG 0

enum fog_type_id
{
  FOG,
  FOG_POSSIBLY_DENSE,
  FOG_IN_SOME_PLACES,
  FOG_IN_SOME_PLACES_POSSIBLY_DENSE,
  FOG_IN_MANY_PLACES,
  FOG_IN_MANY_PLACES_POSSIBLY_DENSE,
  NO_FOG
};

using weather_period_fog_intensity_pair = std::pair<WeatherPeriod, FogIntensityDataItem>;
using weather_period_fog_type_intensity_pair = std::pair<WeatherPeriod, fog_type_id>;
using fog_period_vector = std::vector<weather_period_fog_intensity_pair>;
using fog_type_period_vector = std::vector<weather_period_fog_type_intensity_pair>;

struct FogInfo
{
  FogInfo() : period(TextGenPosixTime(), TextGenPosixTime()) {}
  fog_type_id id{NO_FOG};
  WeatherPeriod period;
  Sentence sentence;
  Sentence timePhrase;
};

class FogForecast
{
 public:
  FogForecast(wf_story_params& parameters, bool visibilityForecast = false);

  ~FogForecast() = default;
  Sentence fogSentence(const WeatherPeriod& thePeriod) const;
  FogInfo fogInfo(const WeatherPeriod& thePeriod) const;

  void printOutFogPeriods(std::ostream& theOutput) const;
  void printOutFogTypePeriods(std::ostream& theOutput) const;
  void printOutFogData(std::ostream& theOutput) const;

 private:
  void findOutFogTypePeriods(const fog_period_vector& theFogPeriods,
                             fog_type_period_vector& theFogTypePeriods) const;
  void findOutFogPeriods(const weather_result_data_item_vector* theModerateFogData,
                         const weather_result_data_item_vector* theDenseFogData,
                         fog_period_vector& theFogPeriods);
  void printOutFogPeriods(std::ostream& theOutput, const fog_period_vector& theFogPeriods) const;
  void printOutFogTypePeriods(std::ostream& theOutput,
                              const fog_type_period_vector& theFogTypePeriods) const;
  void printOutFogData(std::ostream& theOutput,
                       const std::string& theLinePrefix,
                       const weather_result_data_item_vector& theFogData) const;
  Sentence getFogPhrase(const fog_type_id& theFogTypeId);
  void findOutFogPeriods();
  void findOutFogTypePeriods();
  Sentence fogSentence(const WeatherPeriod& thePeriod,
                       const fog_type_period_vector& theFogTypePeriods,
                       const std::string& theAreaString) const;
  FogInfo fogInfo(const WeatherPeriod& thePeriod,
                  const fog_type_period_vector& theFogTypePeriods,
                  const std::string& theAreaString) const;

  Sentence getFogPhrase(const fog_type_id& theFogTypeId) const;
  Sentence areaSpecificSentence(const WeatherPeriod& thePeriod) const;
  float getMean(const fog_period_vector& theFogPeriods,
                const WeatherPeriod& theWeatherPeriod) const;
  WeatherPeriod getActualFogPeriod(const WeatherPeriod& theForecastPeriod,
                                   const WeatherPeriod& theFogPeriod,
                                   bool& theFogPeriodOkFlag) const;
  bool getFogPeriodAndId(const WeatherPeriod& theForecastPeriod,
                         const fog_type_period_vector& theFogTypePeriods,
                         WeatherPeriod& theResultPeriod,
                         fog_type_id& theFogTypeId) const;
  Sentence constructFogSentence(const std::string& theDayPhasePhrase,
                                const std::string& theAreaString,
                                const std::string& theInPlacesString,
                                bool thePossiblyDenseFlag) const;

  wf_story_params& theParameters;

  const weather_result_data_item_vector* theCoastalModerateFogData = nullptr;
  const weather_result_data_item_vector* theInlandModerateFogData = nullptr;
  const weather_result_data_item_vector* theFullAreaModerateFogData = nullptr;
  const weather_result_data_item_vector* theCoastalDenseFogData = nullptr;
  const weather_result_data_item_vector* theInlandDenseFogData = nullptr;
  const weather_result_data_item_vector* theFullAreaDenseFogData = nullptr;

  fog_period_vector theCoastalFog;
  fog_period_vector theInlandFog;
  fog_period_vector theFullAreaFog;

  fog_type_period_vector theCoastalFogType;
  fog_type_period_vector theInlandFogType;
  fog_type_period_vector theFullAreaFogType;

  bool theVisibityForecastAtSea;
};
}  // namespace TextGen
