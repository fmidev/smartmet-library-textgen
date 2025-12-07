#pragma once

#include "Sentence.h"
#include "WeatherForecast.h"
#include "WindStoryTools.h"

namespace TextGen
{
using namespace WindStoryTools;

struct WindDataItemUnit;
struct WindDataItemsByArea;
struct WindSpeedPeriodDataItem;
struct WindDirectionPeriodDataItem;
struct WindEventPeriodDataItem;

using value_distribution_data_vector = std::vector<std::pair<float, WeatherResult>>;
using wind_data_item_vector = std::vector<WindDataItemsByArea*>;
using wind_speed_period_data_item_vector = std::vector<WindSpeedPeriodDataItem*>;
using wind_direction_period_data_item_vector = std::vector<WindDirectionPeriodDataItem*>;
using wind_event_period_data_item_vector = std::vector<WindEventPeriodDataItem*>;
using wind_event_period = std::pair<WindEventId, WeatherPeriod>;
using wind_event_period_vector = std::vector<wind_event_period>;

struct index_vectors
{
  // contains all indexes to
  std::vector<unsigned int> theOriginalWindDataIndexes;
  std::vector<unsigned int> theEqualizedWindSpeedIndexesForMaxWind;
  std::vector<unsigned int> theEqualizedWindSpeedIndexesForMedianWind;
  std::vector<unsigned int> theEqualizedWindSpeedIndexesForTopWind;
  std::vector<unsigned int> theEqualizedWindSpeedIndexesForCalcWind;
  std::vector<unsigned int> theEqualizedWindDirectionIndexes;
};

struct wo_story_params
{
  wo_story_params(const std::string& var,
                  const WeatherArea& area,
                  const WeatherPeriod& forecastPeriod,
                  const TextGenPosixTime& forecastTime,
                  const AnalysisSources& sources,
                  MessageLogger& log)
      : theVar(var),
        theArea(area),
        theForecastPeriod(forecastPeriod),
        theForecastTime(forecastTime),
        theSources(sources),
        theDataPeriod(forecastPeriod),
        theLog(log)
  {
  }

  const std::string& theVar;
  const WeatherArea& theArea;
  const WeatherPeriod& theForecastPeriod;
  const TextGenPosixTime& theForecastTime;
  const AnalysisSources& theSources;
  // currently the same as forecast period, but could be longer in both ends
  WeatherPeriod theDataPeriod;
  MessageLogger& theLog;
  split_method theSplitMethod = NO_SPLITTING;

  double theWindSpeedMaxError = 2;
  double theWindDirectionMaxError = 10;
  double theWindSpeedThreshold = 4;
  double theWindSpeedWarningThreshold = 11;
  double theWindDirectionThreshold = 25;
  double theWindDirectionMinSpeed = 6.5;
  double theWindCalcTopShare = 80;
  double theWindCalcTopShareWeak = 80;
  double theWindSpeedTopCoverage = 98;
  double theGustyWindTopWindDifference = 5;
  std::string theRangeSeparator = "-";
  int theMinIntervalSize = 2;
  int theMaxIntervalSize = 5;
  int theContextualMaxIntervalSize = 5;
  std::string theMetersPerSecondFormat = "SI";
  bool theAlkaenPhraseUsed = false;
  bool theWeakTopWind = false;  // if top wind strays under 10 m/s the whole period
  bool theWeekdaysUsed = true;

  // contains raw data
  wind_data_item_vector theWindDataVector;
  wind_speed_period_data_item_vector theWindSpeedVector;
  wind_direction_period_data_item_vector theWindDirectionVector;
  wind_event_period_data_item_vector theWindSpeedEventPeriodVector;
  std::vector<WeatherPeriod> theWindDirectionPeriods;

  std::map<WeatherArea::Type, index_vectors*> indexes;

  std::vector<unsigned int>& originalWindDataIndexes(WeatherArea::Type type)
  {
    return indexes[type]->theOriginalWindDataIndexes;
  }
  std::vector<unsigned int>& equalizedWSIndexesMaxWind(WeatherArea::Type type)
  {
    return indexes[type]->theEqualizedWindSpeedIndexesForMaxWind;
  }
  std::vector<unsigned int>& equalizedWSIndexesMedian(WeatherArea::Type type)
  {
    return indexes[type]->theEqualizedWindSpeedIndexesForMedianWind;
  }
  std::vector<unsigned int>& equalizedWSIndexesTopWind(WeatherArea::Type type)
  {
    return indexes[type]->theEqualizedWindSpeedIndexesForTopWind;
  }
  std::vector<unsigned int>& equalizedWSIndexesCalcWind(WeatherArea::Type type)
  {
    return indexes[type]->theEqualizedWindSpeedIndexesForCalcWind;
  }
  std::vector<unsigned int>& equalizedWDIndexes(WeatherArea::Type type)
  {
    return indexes[type]->theEqualizedWindDirectionIndexes;
  }

  // If the area is split this contains e.g. inland coast, full, eastern, western areas
  std::vector<WeatherArea> theWeatherAreas;

  std::string areaName() { return (theArea.isNamed() ? theArea.name() : ""); }
};

struct WindDataItemUnit
{
  WindDataItemUnit(WeatherPeriod period,
                   const WeatherResult& windSpeedMin,
                   const WeatherResult& windSpeedMax,
                   const WeatherResult& windSpeedMean,
                   const WeatherResult& windSpeedMedian,
                   const WeatherResult& windSpeedTop,
                   const WeatherResult& windDirection,
                   const WeatherResult& gustSpeed)
      : thePeriod(std::move(period)),
        theWindSpeedMin(windSpeedMin),
        theWindSpeedMax(windSpeedMax),
        theWindSpeedMean(windSpeedMean),
        theWindSpeedMedian(windSpeedMedian),
        theWindSpeedTop(windSpeedTop),
        theWindSpeedCalc(WeatherResult(kFloatMissing, kFloatMissing)),
        theWindDirection(windDirection),
        theGustSpeed(gustSpeed),
        theCorrectedWindDirection(windDirection),
        theEqualizedMedianWind(windSpeedMedian),
        theEqualizedMaxWind(windSpeedMax),
        theEqualizedTopWind(windSpeedTop),
        theEqualizedCalcWind(WeatherResult(kFloatMissing, kFloatMissing)),
        theEqualizedWindDirection(theWindDirection)
  {
  }

  float getTopWindSpeedShare(float theLowerLimit, float theUpperLimit) const;
  float getWindSpeedShare(float theLowerLimit, float theUpperLimit) const;
  float getWindDirectionShare(WindStoryTools::WindDirectionId windDirectionId,
                              double theWindDirectionMinSpeed,
                              WindStoryTools::CompassType compass_type =
                                  WindStoryTools::CompassType::sixteen_directions) const;

  bool operator==(const WindDataItemUnit& dataItemUnit) const
  {
    return thePeriod == dataItemUnit.thePeriod;
  }

  WeatherPeriod thePeriod;
  WeatherResult theWindSpeedMin;
  WeatherResult theWindSpeedMax;
  WeatherResult theWindSpeedMean;
  WeatherResult theWindSpeedMedian;
  WeatherResult theWindSpeedTop;
  WeatherResult theWindSpeedCalc;
  WeatherResult theWindDirection;
  WeatherResult theGustSpeed;
  // if wind is varying and wind speed is high >= 7 m/s, we store corrected
  // wind direction here and use it in calculations
  WeatherResult theCorrectedWindDirection;
  WeatherResult theEqualizedMedianWind;
  WeatherResult theEqualizedMaxWind;
  WeatherResult theEqualizedTopWind;
  WeatherResult theEqualizedCalcWind;
  WeatherResult theEqualizedWindDirection;
  value_distribution_data_vector theWindSpeedDistribution;
  value_distribution_data_vector theWindSpeedDistributionTop;
  value_distribution_data_vector theWindDirectionDistribution16;
  value_distribution_data_vector theWindDirectionDistribution8;
};

// contains WindDataItemUnit structs for different areas (coastal, inland, full area)
struct WindDataItemsByArea
{
  WindDataItemsByArea() = default;
  ~WindDataItemsByArea()
  {
    std::map<WeatherArea::Type, WindDataItemUnit*>::iterator it;
    for (it = theDataItems.begin(); it != theDataItems.end(); it++)
      delete it->second;
  }

  void addItem(const WeatherPeriod& period,
               const WeatherResult& windSpeedMin,
               const WeatherResult& windSpeedMax,
               const WeatherResult& windSpeedMean,
               const WeatherResult& windSpeedMedian,
               const WeatherResult& windSpeedTop,
               const WeatherResult& windDirection,
               const WeatherResult& gustSpeed,
               const WeatherArea::Type& type)
  {
    auto* dataItem = new WindDataItemUnit(period,
                                          windSpeedMin,
                                          windSpeedMax,
                                          windSpeedMean,
                                          windSpeedMedian,
                                          windSpeedTop,
                                          windDirection,
                                          gustSpeed);
    theDataItems.insert(std::make_pair(type, dataItem));
  }

  const WindDataItemUnit& operator()(const WeatherArea::Type& type = WeatherArea::Full) const
  {
    return *(theDataItems.find(type)->second);
  }

  WindDataItemUnit& getDataItem(const WeatherArea::Type& type = WeatherArea::Full) const
  {
    return *(theDataItems.find(type)->second);
  }

 private:
  std::map<WeatherArea::Type, WindDataItemUnit*> theDataItems;
};

struct WindSpeedPeriodDataItem
{
  WindSpeedPeriodDataItem(WeatherPeriod period, const WindStoryTools::WindSpeedId& windSpeedId)
      : thePeriod(std::move(period)), theWindSpeedId(windSpeedId)
  {
  }
  WeatherPeriod thePeriod;
  WindStoryTools::WindSpeedId theWindSpeedId;
};

struct WindDirectionPeriodDataItem
{
  WindDirectionPeriodDataItem(WeatherPeriod period, WindStoryTools::WindDirectionId windDirection)
      : thePeriod(std::move(period)), theWindDirection(windDirection)
  {
  }
  WeatherPeriod thePeriod;
  WindStoryTools::WindDirectionId theWindDirection;
};

struct WindEventPeriodDataItem
{
  WindEventPeriodDataItem(const WeatherPeriod& period,
                          WindEventId windEvent,
                          const WindDataItemUnit& periodBeginDataItem,
                          const WindDataItemUnit& periodEndDataItem)
      : thePeriod(period),
        theWindSpeedChangePeriod(period),
        theWindEvent(windEvent),
        thePeriodBeginDataItem(periodBeginDataItem),
        thePeriodEndDataItem(periodEndDataItem)
  {
  }

  WeatherPeriod thePeriod;
  WeatherPeriod theWindSpeedChangePeriod;  // can be shorter than thePeriod
  WindEventId theWindEvent;
  const WindDataItemUnit& thePeriodBeginDataItem;
  const WindDataItemUnit& thePeriodEndDataItem;
  bool theSuccessiveEventFlag;  // if there is a long MISSING_WIND_EVENT period between two
  // strenghtening/weakening period, theSuccessiveEventFlag is set true to the latter
  // strenghtening/weakening period, so that we can use phrase 'voimistuu/heikkenee edelleen'
};

struct WindDirectionInfo
{
  WeatherPeriod period;
  WeatherResult direction;
  WindDirectionId id = MISSING_WIND_DIRECTION_ID;

  WindDirectionInfo()
      : period(WeatherPeriod(TextGenPosixTime(), TextGenPosixTime())),
        direction(WeatherResult(kFloatMissing, kFloatMissing))

  {
  }
  WindDirectionInfo(WeatherPeriod p, const WeatherResult& d, WindDirectionId i)
      : period(std::move(p)), direction(d), id(i)
  {
  }

  TextGenPosixTime startTime() const { return period.localStartTime(); }
  TextGenPosixTime endTime() const { return period.localEndTime(); }
  bool empty() const { return (id == MISSING_WIND_DIRECTION_ID); }
};

struct TimePhraseInfo
{
  TextGenPosixTime starttime;
  TextGenPosixTime endtime;
  short day_number = -1;
  part_of_the_day_id part_of_the_day = MISSING_PART_OF_THE_DAY_ID;

  TimePhraseInfo() = default;
  TimePhraseInfo(const TextGenPosixTime& st,
                 const TextGenPosixTime& et,
                 short d,
                 part_of_the_day_id pd)
      : starttime(st), endtime(et), day_number(d), part_of_the_day(pd)
  {
  }
  bool empty() const { return part_of_the_day == MISSING_PART_OF_THE_DAY_ID; }
};

// in WindForecast.cpp
std::string get_wind_event_string(WindEventId theWindEventId);
bool wind_speed_differ_enough(wo_story_params& theParameter, const WeatherPeriod& thePeriod);
bool wind_direction_differ_enough(WeatherResult theWindDirection1,
                                  WeatherResult theWindDirection2,
                                  float theWindDirectionThreshold);
WindDirectionInfo get_wind_direction(const wo_story_params& theParameters,
                                     const TextGenPosixTime& pointOfTime,
                                     const WindDirectionInfo* thePreviousWindDirection = nullptr);
WindDirectionInfo get_wind_direction(const wo_story_params& theParameters,
                                     const WeatherPeriod& period,
                                     const WindDirectionInfo* thePreviousWindDirection = nullptr);

bool is_weak_period(const wo_story_params& theParameters, const WeatherPeriod& thePeriod);

unsigned int get_peak_wind(const WeatherPeriod& thePeriod, const wo_story_params& theParameters);
float get_top_wind(const WeatherPeriod& thePeriod, const wo_story_params& theParameters);

std::ostream& operator<<(std::ostream& theOutput,
                         const WindEventPeriodDataItem& theWindEventPeriodDataItem);

}  // namespace TextGen
