#include "Delimiter.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PositiveValueAcceptor.h"
#include "Sentence.h"
#include "SubMaskExtractor.h"
#include "UnitFactory.h"
#include "WeatherForecast.h"
#include "WindForecast.h"
#include "WindStory.h"
#include "WindStoryTools.h"
#include <calculator/GridForecaster.h>
#include <calculator/ParameterAnalyzer.h>
#include <calculator/RangeAcceptor.h>
#include <calculator/Settings.h>
#include <calculator/WeatherResult.h>
#include <calculator/WeatherResultTools.h>
#include <macgyver/StringConversion.h>
#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <random>

using namespace TextGen;
using namespace TextGen::WindStoryTools;
using namespace std;

namespace TextGen
{
// Define named constants for magic numbers
constexpr int INITIAL_MERGE_THRESHOLD_HOURS = 3;
constexpr int SHORT_PERIOD_THRESHOLD_HOURS = 2;
constexpr int MISSING_EVENT_THRESHOLD_HOURS = 6;

std::size_t generateUniqueID()
{
  static std::random_device rd;
  static std::mt19937_64 generator(rd());
  static std::uniform_int_distribution<std::size_t> distribution;

  return distribution(generator);
}

bool find_forecast_period_bounds(const wo_story_params& storyParams,
                                 unsigned int& firstIndex,
                                 unsigned int& lastIndex)
{
  firstIndex = UINT_MAX;
  lastIndex = UINT_MAX;
  for (unsigned int i = 0; i < storyParams.theWindDataVector.size(); i++)
  {
    const WindDataItemUnit& windDataItem =
        storyParams.theWindDataVector[i]->getDataItem(storyParams.theArea.type());
    if (is_inside(windDataItem.thePeriod, storyParams.theForecastPeriod))
    {
      if (firstIndex == UINT_MAX)
        firstIndex = i;
      lastIndex = i;
    }
  }
  return firstIndex != UINT_MAX;
}

std::vector<TextGenPosixTime> build_direction_change_times(wo_story_params& storyParams,
                                                           unsigned int firstIndex,
                                                           unsigned int lastIndex)
{
  WindDataItemUnit previousWindDataItem =
      storyParams.theWindDataVector[firstIndex]->getDataItem(storyParams.theArea.type());
  WindDirectionInfo previousWD = get_wind_direction(storyParams, previousWindDataItem.thePeriod);

  std::vector<TextGenPosixTime> changeTimes;
  changeTimes.push_back(previousWindDataItem.thePeriod.localStartTime());

  for (unsigned int index = firstIndex + 1; index < lastIndex; index++)
  {
    WindDataItemUnit currentWindDataItem =
        storyParams.theWindDataVector[index]->getDataItem(storyParams.theArea.type());
    WindDirectionInfo currentWD = get_wind_direction(storyParams, currentWindDataItem.thePeriod);

    if (currentWD.id == previousWD.id)
      continue;
    if (currentWD.id != VAIHTELEVA && previousWD.id != VAIHTELEVA &&
        !wind_direction_differ_enough(
            previousWD.direction, currentWD.direction, storyParams.theWindDirectionThreshold))
      continue;

    changeTimes.push_back(currentWindDataItem.thePeriod.localStartTime());
    previousWindDataItem = currentWindDataItem;
    previousWD = currentWD;
  }
  return changeTimes;
}

std::vector<WeatherPeriod> build_direction_periods_from_change_times(
    wo_story_params& storyParams, const std::vector<TextGenPosixTime>& changeTimes)
{
  std::vector<WeatherPeriod> directionPeriods;
  WeatherPeriod previousPeriod(changeTimes[0], changeTimes[0]);

  for (unsigned int i = 0; i < changeTimes.size(); i++)
  {
    WeatherPeriod newPeriod(changeTimes[0], changeTimes[0]);
    if (i < changeTimes.size() - 1)
    {
      auto endTime = changeTimes[i + 1];
      endTime.ChangeByHours(-1);
      newPeriod = WeatherPeriod(changeTimes[i], endTime);
    }
    else
    {
      newPeriod = WeatherPeriod(changeTimes[i], storyParams.theForecastPeriod.localEndTime());
    }

    // if direction id is the same merge previous and new period
    if (!directionPeriods.empty() && get_wind_direction(storyParams, newPeriod).id ==
                                         get_wind_direction(storyParams, previousPeriod).id)
    {
      directionPeriods.back() =
          WeatherPeriod(previousPeriod.localStartTime(), newPeriod.localEndTime());
    }
    else
    {
      directionPeriods.push_back(newPeriod);
    }

    previousPeriod = directionPeriods.back();
  }

  // short period (< 3h) in the beginning is merged with the next
  if (directionPeriods.size() > 1 &&
      get_period_length(directionPeriods.front()) < INITIAL_MERGE_THRESHOLD_HOURS)
  {
    directionPeriods.front() = WeatherPeriod(directionPeriods.front().localStartTime(),
                                             directionPeriods[1].localEndTime());
    directionPeriods.erase(directionPeriods.begin() + 1);
  }

  return directionPeriods;
}

std::vector<WeatherPeriod> remove_short_and_varying_periods(
    wo_story_params& storyParams, const std::vector<WeatherPeriod>& directionPeriods)
{
  // remove short periods (max 2 hours) around varying wind, except the last period
  // and remove short period of different direction in the middle
  std::set<unsigned int> indexesToRemove;
  for (unsigned int i = 1; i < directionPeriods.size() - 1; i++)
  {
    if (indexesToRemove.find(i) != indexesToRemove.end())
      continue;
    const WeatherPeriod& prevP = directionPeriods[i - 1];
    const WeatherPeriod& currP = directionPeriods[i];
    const WeatherPeriod& nextP = directionPeriods[i + 1];
    WindDirectionInfo prevId = get_wind_direction(storyParams, prevP);
    WindDirectionInfo currId = get_wind_direction(storyParams, currP);
    WindDirectionInfo nextId = get_wind_direction(storyParams, nextP);
    if (currId.id == VAIHTELEVA)
    {
      if (i != 1 && get_period_length(prevP) <= SHORT_PERIOD_THRESHOLD_HOURS)
        indexesToRemove.insert(i - 1);
      if (i + 1 != directionPeriods.size() - 1 &&
          get_period_length(nextP) <= SHORT_PERIOD_THRESHOLD_HOURS)
        indexesToRemove.insert(i + 1);
    }
    else if (prevId.id == nextId.id)
    {
      int prevLen = get_period_length(prevP);
      int currLen = get_period_length(currP);
      int nextLen = get_period_length(nextP);
      if (currLen < 2 && currLen < prevLen && currLen < nextLen)
        indexesToRemove.insert(i);
    }
  }

  std::vector<WeatherPeriod> cleanedPeriods;
  for (unsigned int i = 0; i < directionPeriods.size(); i++)
  {
    if (i == 0 && get_period_length(directionPeriods[i]) < 1)
      continue;
    if (indexesToRemove.find(i) == indexesToRemove.end())
      cleanedPeriods.push_back(directionPeriods[i]);
  }
  return cleanedPeriods;
}

void merge_and_filter_direction_periods(wo_story_params& storyParams,
                                        std::vector<WeatherPeriod>& cleanedPeriods)
{
  // merge successive periods with same direction
  std::set<unsigned int> indexesToRemove;
  for (unsigned int i = 0; i < cleanedPeriods.size() - 1; i++)
  {
    for (unsigned int j = i + 1; j < cleanedPeriods.size(); j++)
    {
      if (indexesToRemove.find(i) != indexesToRemove.end())
        continue;
      WindDirectionInfo currentId = get_wind_direction(storyParams, cleanedPeriods[i]);
      WindDirectionInfo nextId = get_wind_direction(storyParams, cleanedPeriods[j]);
      if (currentId.id == nextId.id)
      {
        cleanedPeriods[i] =
            WeatherPeriod(cleanedPeriods[i].localStartTime(), cleanedPeriods[j].localEndTime());
        indexesToRemove.insert(j);
      }
      else
      {
        break;
      }
    }
  }

  for (unsigned int i = 0; i < cleanedPeriods.size(); i++)
  {
    if (indexesToRemove.find(i) != indexesToRemove.end())
      continue;

    const WeatherPeriod& period = cleanedPeriods[i];
    WindDirectionId id = get_wind_direction(storyParams, period).id;

    // dont report short varying wind if it is not the first/last one
    if (i < cleanedPeriods.size() - 1 && id == VAIHTELEVA &&
        get_period_length(period) < SHORT_PERIOD_THRESHOLD_HOURS && i != 0)
      continue;

    storyParams.theWindDirectionPeriods.push_back(period);
  }
}

void find_out_wind_direction_periods(wo_story_params& storyParams)
{
  unsigned int firstIndex = UINT_MAX;
  unsigned int lastIndex = UINT_MAX;

  if (!find_forecast_period_bounds(storyParams, firstIndex, lastIndex))
    return;

  std::vector<TextGenPosixTime> changeTimes =
      build_direction_change_times(storyParams, firstIndex, lastIndex);

  if (changeTimes.empty())
    return;

  std::vector<WeatherPeriod> directionPeriods =
      build_direction_periods_from_change_times(storyParams, changeTimes);

  std::vector<WeatherPeriod> cleanedPeriods =
      remove_short_and_varying_periods(storyParams, directionPeriods);

  merge_and_filter_direction_periods(storyParams, cleanedPeriods);

  storyParams.theLog << "** ORIGINAL DIRECTION PERIODS **\n";
  for (const WeatherPeriod& p : storyParams.theWindDirectionPeriods)
  {
    WindDirectionInfo wdi = get_wind_direction(storyParams, p);
    storyParams.theLog << as_string(p) << " -> " << wind_direction_string(wdi.id) << ", "
                       << as_string(wdi.direction) << '\n';
  }
}

const WindDataItemUnit& get_data_item(const wo_story_params& storyParams,
                                      const TextGenPosixTime& theTime)
{
  unsigned int retIndex = 0;

  WeatherArea::Type areaType(storyParams.theArea.type());
  for (unsigned int i = 0; i < storyParams.theWindDataVector.size(); i++)
  {
    const WindDataItemUnit& dataItem = (*storyParams.theWindDataVector[i])(areaType);

    if (dataItem.thePeriod.localStartTime() == theTime)
    {
      retIndex = i;
      break;
    }
  }

  return (*storyParams.theWindDataVector[retIndex])(areaType);
}

float calculate_weighted_wind_speed(const wo_story_params& storyParams,
                                    float topWind,
                                    float medianWind)
{
  // weights are read from config file
  double share = (storyParams.theWeakTopWind ? storyParams.theWindCalcTopShareWeak
                                             : storyParams.theWindCalcTopShare);
  float topWindWeight = (share / 100.0);
  float medianWindWeight = 1.0 - topWindWeight;

  return ((topWind * topWindWeight) + (medianWind * medianWindWeight));
}

float calculate_weighted_wind_speed(const wo_story_params& storyParams,
                                    const WindDataItemUnit& dataItem)
{
  return calculate_weighted_wind_speed(
      storyParams, dataItem.theWindSpeedTop.value(), dataItem.theWindSpeedMedian.value());
}

bool wind_event_period_sort(const WindEventPeriodDataItem* first,
                            const WindEventPeriodDataItem* second)
{
  return (first->thePeriod.localStartTime() < second->thePeriod.localStartTime());
}

bool wind_direction_item_sort(pair<float, WeatherResult> firstItem,
                              pair<float, WeatherResult> secondItem)
{
  return firstItem.second.value() > secondItem.second.value();
}

std::string get_area_type_string(const WeatherArea::Type& theAreaType)
{
  std::string retval;

  switch (theAreaType)
  {
    case WeatherArea::Full:
      retval = "full";
      break;
    case WeatherArea::Land:
      retval = "land";
      break;
    case WeatherArea::Coast:
      retval = "coast";
      break;
    case WeatherArea::Inland:
      retval = "inland";
      break;
    case WeatherArea::Northern:
      retval = "northern";
      break;
    case WeatherArea::Southern:
      retval = "southern";
      break;
    case WeatherArea::Eastern:
      retval = "eastern";
      break;
    case WeatherArea::Western:
      retval = "western";
      break;
    default:
      break;
  }

  return retval;
}

std::string get_area_name_string(const WeatherArea& theArea)
{
  std::string retval(theArea.isNamed() ? theArea.name() : "");
  retval += "_";
  retval += get_area_type_string(theArea.type());

  return retval;
}

std::string get_direction_abbreviation(
    float direction, WindStoryTools::CompassType compass_type = sixteen_directions)
{
  direction = std::fabs(direction);
  while (direction > 360.0f)
    direction -= 360.0f;

  if (compass_type == sixteen_directions)
  {
    // Each of the 16 sectors spans 22.5°. Offset by half a sector (11.25°) so that
    // the boundary at 348.75° / 0° / 11.25° maps to index 0 ("N").
    static const std::array<const char*, 16> dirs = {"N",
                                                     "n-ne",
                                                     "NE",
                                                     "ne-e",
                                                     "E",
                                                     "e-se",
                                                     "SE",
                                                     "se-s",
                                                     "S",
                                                     "s-sw",
                                                     "SW",
                                                     "sw-w",
                                                     "W",
                                                     "w-nw",
                                                     "NW",
                                                     "nw-n"};
    return dirs[static_cast<int>((direction + 11.25f) / 22.5f) % 16];
  }

  // 8-direction compass: each sector spans 45°, offset by 22.5°.
  static const std::array<const char*, 8> dirs = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};
  return dirs[static_cast<int>((direction + 22.5f) / 45.0f) % 8];
}

void printDataItem(std::ostream& theOutput,
                   const WindDataItemUnit& theWindDataItem,
                   double theWindDirectionMinSpeed)
{
  theOutput << theWindDataItem.thePeriod.localStartTime() << " ... "
            << theWindDataItem.thePeriod.localEndTime() << ": min: " << fixed << setprecision(4)
            << theWindDataItem.theWindSpeedMin.value() << "; maximi: " << fixed << setprecision(4)
            << theWindDataItem.theWindSpeedMax.value() << "; tasoitettu maximi: " << fixed
            << setprecision(4) << theWindDataItem.theEqualizedMaxWind.value()
            << "; mediaani: " << fixed << setprecision(4)
            << theWindDataItem.theWindSpeedMedian.value() << "; tasoitettu mediaani: " << fixed
            << setprecision(4) << theWindDataItem.theEqualizedMedianWind.value()
            << "; ka,k-hajonta: (" << fixed << setprecision(4)
            << theWindDataItem.theWindSpeedMean.value() << ", " << fixed << setprecision(4)
            << theWindDataItem.theWindSpeedMean.error() << ")"
            << "; huipputuuli: " << fixed << setprecision(4)
            << theWindDataItem.theWindSpeedTop.value() << "; tasoitettu huipputuuli: " << fixed
            << setprecision(4) << theWindDataItem.theEqualizedTopWind.value()
            << "; laskennallinen tuuli: " << fixed << setprecision(4)
            << theWindDataItem.theWindSpeedCalc.value() << "; tasoitettu laskenn: " << fixed
            << setprecision(4) << theWindDataItem.theEqualizedCalcWind.value()
            << "; suunta: " << fixed << setprecision(4)
            << theWindDataItem.theCorrectedWindDirection.value() << "; suunnan k-hajonta: " << fixed
            << setprecision(4) << theWindDataItem.theCorrectedWindDirection.error()
            << "; tasoitettu suunta: " << fixed << setprecision(4)
            << theWindDataItem.theEqualizedWindDirection.value() << " ("
            << wind_direction_string(wind_direction_id(theWindDataItem.theEqualizedWindDirection,
                                                       theWindDataItem.theEqualizedTopWind,
                                                       "",
                                                       theWindDirectionMinSpeed))
            << ") " << theWindDataItem.theEqualizedWindDirection.value() << ", "
            << theWindDataItem.theEqualizedWindDirection.error() << ", "
            << theWindDataItem.theEqualizedTopWind.value() << "; puuska: " << fixed
            << setprecision(4) << theWindDataItem.theGustSpeed.value();
}

std::ostream& operator<<(std::ostream& theOutput,
                         const WindSpeedPeriodDataItem& theWindSpeedPeriodDataItem)
{
  theOutput << theWindSpeedPeriodDataItem.thePeriod.localStartTime() << " ... "
            << theWindSpeedPeriodDataItem.thePeriod.localEndTime() << ": "
            << wind_speed_string(theWindSpeedPeriodDataItem.theWindSpeedId) << '\n';

  return theOutput;
}

std::ostream& operator<<(std::ostream& theOutput,
                         const WindDirectionPeriodDataItem& theWindDirectionPeriodDataItem)
{
  theOutput << theWindDirectionPeriodDataItem.thePeriod.localStartTime() << " ... "
            << theWindDirectionPeriodDataItem.thePeriod.localEndTime() << ": "
            << wind_direction_string(theWindDirectionPeriodDataItem.theWindDirection) << '\n';

  return theOutput;
}

std::ostream& operator<<(std::ostream& theOutput,
                         const WindEventPeriodDataItem& theWindEventPeriodDataItem)
{
  theOutput << '\n'
            << "thePeriod: " << as_string(theWindEventPeriodDataItem.thePeriod) << '\n'
            << "theWindSpeedChangePeriod: "
            << as_string(theWindEventPeriodDataItem.theWindSpeedChangePeriod) << '\n'
            << "theWindEvent: " << get_wind_event_string(theWindEventPeriodDataItem.theWindEvent)
            << '\n'
            << '\n';

  return theOutput;
}

void print_wiki_table(const WeatherArea& theArea,
                      const string& theVar,
                      const wind_data_item_vector& theWindDataItemVector)
{
  std::string filename("./" + get_area_name_string(theArea) + "_rawdata.txt");

  ofstream output_file(filename.c_str(), ios::out);

  if (output_file.fail())
  {
    throw std::runtime_error("wind_overview failed to open '" + filename + "' for writing");
  }

  output_file << "|| aika || min || max || ka, k-hajonta || maksimituuli || puuska || suunta || "
                 "suunnan k-hajonta || fraasi ||"
              << '\n';

  for (auto* i : theWindDataItemVector)
  {
    const WindDataItemUnit& theWindDataItem = (*i)(theArea.type());
    output_file << "| " << theWindDataItem.thePeriod.localEndTime() << " | " << fixed
                << setprecision(4) << theWindDataItem.theWindSpeedMin.value() << " | " << fixed
                << setprecision(4) << theWindDataItem.theWindSpeedMax.value() << " | (" << fixed
                << setprecision(4) << theWindDataItem.theWindSpeedMean.value() << ", " << fixed
                << setprecision(4) << theWindDataItem.theWindSpeedMean.error() << ")"
                << " | " << fixed << setprecision(4) << theWindDataItem.theWindSpeedTop.value()
                << " | " << fixed << setprecision(4) << theWindDataItem.theGustSpeed.value()
                << " | " << fixed << setprecision(4)
                << theWindDataItem.theCorrectedWindDirection.value() << " | " << fixed
                << setprecision(4) << theWindDataItem.theWindDirection.error() << " | ";

    output_file << directed16_speed_string(theWindDataItem.theWindSpeedMean,
                                           theWindDataItem.theCorrectedWindDirection,
                                           theVar)
                << " |\n";
  }
}

namespace
{
void write_csv_row(std::ostream& out,
                   const WindDataItemUnit& item,
                   const bitset<14>& cols,
                   const std::string& theVar)
{
  if (cols.test(13))
    out << item.thePeriod.localEndTime();
  if (cols.test(12))
    out << ", " << fixed << setprecision(4) << item.theWindSpeedMin.value();
  if (cols.test(11))
    out << ", " << fixed << setprecision(4) << item.theWindSpeedMax.value();
  if (cols.test(10))
    out << ", " << fixed << setprecision(4) << item.theWindSpeedMedian.value();
  if (cols.test(9))
    out << ", " << fixed << setprecision(4) << item.theEqualizedMedianWind.value();
  if (cols.test(8))
    out << ", " << fixed << setprecision(4) << item.theWindSpeedMean.value();
  if (cols.test(7))
    out << ", " << fixed << setprecision(4) << item.theWindSpeedMean.error();
  if (cols.test(6))
    out << ", " << fixed << setprecision(4) << item.theGustSpeed.value();
  if (cols.test(5))
    out << ", " << fixed << setprecision(4) << item.theWindSpeedTop.value();
  if (cols.test(4))
    out << ", " << fixed << setprecision(4) << item.theEqualizedTopWind.value();
  if (cols.test(3))
    out << ", " << fixed << setprecision(4) << item.theCorrectedWindDirection.value();
  if (cols.test(2))
    out << ", " << fixed << setprecision(4) << item.theEqualizedWindDirection.value();
  if (cols.test(1))
    out << ", " << fixed << setprecision(4) << item.theWindDirection.error();
  if (cols.test(0))
    out << ", "
        << directed16_speed_string(item.theWindSpeedMean, item.theCorrectedWindDirection, theVar);
  out << '\n';
}

void write_html_row(std::ostream& out,
                    const WindDataItemUnit& item,
                    const bitset<14>& cols,
                    const std::string& theVar)
{
  if (cols.test(13))
    out << "<td style=\"white-space: nowrap;\">" << item.thePeriod.localEndTime() << "</td>\n";
  if (cols.test(12))
    out << "<td>" << fixed << setprecision(2) << item.theWindSpeedMin.value() << "</td>\n";
  if (cols.test(11))
    out << "<td>" << fixed << setprecision(2) << item.theWindSpeedMax.value() << "</td>\n";
  if (cols.test(10))
    out << "<td>" << fixed << setprecision(2) << item.theWindSpeedMedian.value() << "</td>\n";
  if (cols.test(9))
    out << "<td>" << fixed << setprecision(2) << item.theEqualizedMedianWind.value() << "</td>\n";
  if (cols.test(8))
    out << "<td>" << fixed << setprecision(2) << item.theWindSpeedMean.value() << "</td>\n";
  if (cols.test(7))
    out << "<td>" << fixed << setprecision(2) << item.theWindSpeedMean.error() << "</td>\n";
  if (cols.test(6))
    out << "<td>" << fixed << setprecision(2) << item.theGustSpeed.value() << "</td>\n";
  if (cols.test(5))
    out << "<td>" << fixed << setprecision(2) << item.theWindSpeedTop.value() << "</td>\n";
  if (cols.test(4))
    out << "<td>" << fixed << setprecision(2) << item.theEqualizedTopWind.value() << "</td>\n";
  if (cols.test(3))
    out << "<td>" << fixed << setprecision(2) << item.theCorrectedWindDirection.value()
        << "</td>\n";
  if (cols.test(2))
    out << "<td>" << fixed << setprecision(2) << item.theEqualizedWindDirection.value()
        << "</td>\n";
  if (cols.test(1))
    out << "<td>" << fixed << setprecision(2) << item.theWindDirection.error() << "</td>\n";
  if (cols.test(0))
    out << "<td style=\"white-space: nowrap;\">"
        << directed16_speed_string(item.theWindSpeedMean, item.theCorrectedWindDirection, theVar)
        << "</td>\n";
}
}  // namespace

void print_csv_table(const WeatherArea& theArea,
                     const std::string& fileIdentifierString,
                     const std::string& theVar,
                     const wind_data_item_vector& theWindDataItemVector,
                     const vector<unsigned int>& theIndexVector,
                     const bitset<14>& theColumnSelectionBitset)
{
  std::string filename("./" + get_area_name_string(theArea) + fileIdentifierString + ".csv");

  ofstream output_file(filename.c_str(), ios::out);

  if (output_file.fail())
  {
    throw std::runtime_error("wind_overview failed to open '" + filename + "' for writing");
  }

  constexpr std::array<const char*, 14> column_names = {{"aika",
                                                         ", min",
                                                         ", max",
                                                         ", mediaani",
                                                         ", tasoitettu mediaani",
                                                         ", ka",
                                                         ", nopeuden k-hajonta",
                                                         ", puuska",
                                                         ", maksimituuli",
                                                         ", tasoitettu maksimituuli",
                                                         ", suunta",
                                                         ", tasoitettu suunta",
                                                         ", suunnan k-hajonta",
                                                         ", fraasi"}};

  const unsigned int number_of_columns = 14;
  for (unsigned int i = 0; i < number_of_columns; i++)
    if (theColumnSelectionBitset.test(number_of_columns - i - 1))
      output_file << column_names[i];
  output_file << '\n';

  for (unsigned int index : theIndexVector)
    write_csv_row(output_file,
                  (*theWindDataItemVector[index])(theArea.type()),
                  theColumnSelectionBitset,
                  theVar);
}

void print_windspeed_distribution(const WeatherArea& theArea,
                                  const std::string& fileIdentifierString,
                                  const std::string& /*theVar*/,
                                  const wind_data_item_vector& theWindDataItemVector,
                                  const vector<unsigned int>& theIndexVector)
{
  std::string filename("./" + get_area_name_string(theArea) + fileIdentifierString + ".csv");

  ofstream output_file(filename.c_str(), ios::out);

  if (output_file.fail())
  {
    throw std::runtime_error("wind_overview failed to open '" + filename + "' for writing");
  }

  const WindDataItemUnit& firstWindDataItem = (*theWindDataItemVector[0])(theArea.type());

  unsigned int numberOfWindSpeedCategories = firstWindDataItem.theWindSpeedDistribution.size();

  for (unsigned int i = 0; i < numberOfWindSpeedCategories; i++)
  {
    if (i == numberOfWindSpeedCategories - 1)
      output_file << ", > " << i + 1 << " m/s";
    else
      output_file << ", " << i + 1 << " m/s";
  }
  output_file << '\n';

  for (unsigned int index : theIndexVector)
  {
    const WindDataItemUnit& theWindDataItem = (*theWindDataItemVector[index])(theArea.type());

    output_file << theWindDataItem.thePeriod.localEndTime();

    for (unsigned int k = 0; k < numberOfWindSpeedCategories; k++)
    {
      output_file << ", ";
      output_file << fixed << setprecision(2) << theWindDataItem.getWindSpeedShare(k, k + 1);
    }
    output_file << '\n';
  }
}

void print_winddirection_distribution(const WeatherArea& theArea,
                                      const std::string& fileIdentifierString,
                                      wo_story_params& storyParams)
{
  std::string filename("./" + get_area_name_string(storyParams.theArea) + fileIdentifierString +
                       ".csv");

  ofstream output_file(filename.c_str(), ios::out);

  if (output_file.fail())
  {
    throw std::runtime_error("wind_overview failed to open '" + filename + "' for writing");
  }

  const vector<unsigned int>& theIndexVector =
      storyParams.originalWindDataIndexes(storyParams.theArea.type());

  for (unsigned int i = POHJOINEN; i <= POHJOINEN_LUODE; i++)
  {
    output_file << ", ";
    output_file << wind_direction_string(static_cast<WindDirectionId>(i));
  }
  output_file << ", ";
  output_file << "keskihajonta";
  output_file << '\n';

  for (unsigned int index : theIndexVector)
  {
    const WindDataItemUnit& theWindDataItem =
        (*storyParams.theWindDataVector[index])(theArea.type());

    output_file << theWindDataItem.thePeriod.localEndTime();

    for (unsigned int i = POHJOINEN; i <= POHJOINEN_LUODE; i++)
    {
      output_file << ", ";
      output_file << fixed << setprecision(2)
                  << theWindDataItem.getWindDirectionShare(static_cast<WindDirectionId>(i),
                                                           storyParams.theWindDirectionMinSpeed);
    }
    output_file << ", ";
    output_file << fixed << setprecision(2) << theWindDataItem.theWindDirection.error();
    output_file << '\n';
  }
}

void print_html_table(const WeatherArea::Type& theAreaType,
                      const std::string& fileIdentifierString,
                      const std::string& theVar,
                      const wind_data_item_vector& theWindDataItemVector,
                      const vector<unsigned int>& theIndexVector,
                      const bitset<14>& theColumnSelectionBitset)
{
  std::string filename("./" + get_area_type_string(theAreaType) + fileIdentifierString + ".html");

  ofstream output_file(filename.c_str(), ios::out);

  if (output_file.fail())
  {
    throw std::runtime_error("wind_overview failed to open '" + filename + "' for writing");
  }

  output_file << "<h1>" << get_area_type_string(theAreaType) << "</h1>\n";
  output_file << "<table border=\"1\">\n";

  constexpr std::array<const char*, 14> column_names = {{"aika",
                                                         "min",
                                                         "max",
                                                         "mediaani",
                                                         "tasoitettu mediaani",
                                                         "ka",
                                                         "nopeuden k-hajonta",
                                                         "puuska",
                                                         "maksimituuli",
                                                         "tasoitettu maksimituuli",
                                                         "suunta",
                                                         "tasoitettu suunta",
                                                         "suunnan k-hajonta",
                                                         "fraasi"}};

  output_file << "<tr>\n";
  const unsigned int number_of_columns = 14;
  for (unsigned int i = 0; i < number_of_columns; i++)
    if (theColumnSelectionBitset.test(number_of_columns - i - 1))
    {
      output_file << "<td>";
      output_file << column_names[i];
      output_file << "</td>";
    }
  output_file << "</tr>\n";

  for (unsigned int index : theIndexVector)
  {
    output_file << "<tr>\n";
    write_html_row(output_file,
                   (*theWindDataItemVector[index])(theAreaType),
                   theColumnSelectionBitset,
                   theVar);
    output_file << "</tr>\n";
  }

  output_file << "</table>";
}

void save_raw_data(wo_story_params& storyParams, const string& id_str = "_original")
{
  bitset<14> columnSelectionBitset;

  // aika == 13
  // min, == 12
  // max == 11
  // mediaani == 10
  // tasoitettu mediaani == 9
  // ka == 8
  // nopeuden k-hajonta == 7
  // puuska == 6
  // maksimituuli == 5
  // tasoitettu maksimituuli == 4
  // suunta == 3
  // tasoitettu suunta == 2
  // suunnan k-hajonta == 1
  // fraasi == 0

  for (unsigned int i = 0; i < storyParams.theWeatherAreas.size(); i++)
  {
    const WeatherArea& weatherArea = storyParams.theWeatherAreas[i];

    columnSelectionBitset.set();
    print_csv_table(weatherArea,
                    id_str,
                    storyParams.theVar,
                    storyParams.theWindDataVector,
                    storyParams.originalWindDataIndexes(storyParams.theArea.type()),
                    columnSelectionBitset);

    print_windspeed_distribution(weatherArea,
                                 "_windspeed_distribution",
                                 storyParams.theVar,
                                 storyParams.theWindDataVector,
                                 storyParams.originalWindDataIndexes(storyParams.theArea.type()));

    print_winddirection_distribution(weatherArea, "_winddirection_distribution", storyParams);
  }
}

std::string get_csv_data(wo_story_params& storyParams, const std::string& param)
{
  std::stringstream csv_data;
  if (param.empty())
    csv_data << '\n'
             << "time,min,max,eq-max,median,eq-median,top-wind,eq-top-wind,wind-calc,gust,"
                "direction,direction-sdev,eq-direction"
             << '\n';
  else if (param == "windspeed")
    csv_data << "\ntime,median,eq-median,top-wind,eq-top-wind,wind-calc\n";
  else if (param == "winddirection")
    csv_data << "\ntime,direction,eq-direction\n";

  WeatherArea::Type areaType(storyParams.theWeatherAreas[0].type());

  for (auto& i : storyParams.theWindDataVector)
  {
    const WindDataItemUnit& windDataItem = (*i)(areaType);

    csv_data << windDataItem.thePeriod.localStartTime();
    if (param.empty())
    {
      csv_data << ", " << fixed << setprecision(2) << windDataItem.theWindSpeedMin.value() << ", "
               << fixed << setprecision(2) << windDataItem.theWindSpeedMax.value() << ", " << fixed
               << setprecision(2) << windDataItem.theEqualizedMaxWind.value() << ", " << fixed
               << setprecision(2) << windDataItem.theWindSpeedMedian.value() << ", " << fixed
               << setprecision(2) << windDataItem.theEqualizedMedianWind.value() << ", " << fixed
               << setprecision(2) << windDataItem.theWindSpeedMean.value() << ", " << fixed
               << setprecision(2) << windDataItem.theWindSpeedMean.error() << ", " << fixed
               << setprecision(2) << windDataItem.theWindSpeedTop.value() << ", " << fixed
               << setprecision(2) << windDataItem.theEqualizedTopWind.value() << ", " << fixed
               << setprecision(2) << windDataItem.theEqualizedCalcWind.value() << ", " << fixed
               << setprecision(2) << windDataItem.theGustSpeed.value() << ", " << fixed
               << setprecision(2) << windDataItem.theCorrectedWindDirection.value() << ", " << fixed
               << setprecision(2) << windDataItem.theCorrectedWindDirection.error() << ", " << fixed
               << setprecision(2) << windDataItem.theEqualizedWindDirection.value();
    }
    else if (param == "windspeed")
    {
      csv_data << ", " << fixed << setprecision(2) << windDataItem.theWindSpeedMedian.value()
               << ", " << fixed << setprecision(2) << windDataItem.theEqualizedMedianWind.value()
               << ", " << fixed << setprecision(2) << windDataItem.theWindSpeedTop.value() << ", "
               << fixed << setprecision(2) << windDataItem.theEqualizedTopWind.value() << ", "
               << fixed << setprecision(2) << windDataItem.theEqualizedCalcWind.value();
    }
    else if (param == "winddirection")
    {
      csv_data << ", " << fixed << setprecision(2) << windDataItem.theCorrectedWindDirection.value()
               << ", " << fixed << setprecision(2)
               << windDataItem.theEqualizedWindDirection.value();
    }

    csv_data << '\n';
  }

  return csv_data.str();
}

std::string get_html_rawdata(wo_story_params& storyParams)
{
  std::stringstream html_data;

  html_data << "<h5>Summary data</h5>\n";
  html_data << "<table border=\"1\">\n";

  html_data << '\n'
            << "<tr>\n"
            << "<td>time</td><td>min</td><td>max</td><td>eq-max</td><td>median</td><td "
               "BGCOLOR=lightgreen>eq-median</td><td>mean</td><td>sdev</td><td>top-wind</td><td "
               "BGCOLOR=lightgreen>eq-top-wind</td><td "
               "BGCOLOR=gold>wind-calc</td><td>gust</td><td>direction</td><td>direction-sdev</"
               "td><td BGCOLOR=lightblue>eq-direction</td>"
            << '\n'
            << "</tr>\n";

  WeatherArea::Type areaType(storyParams.theWeatherAreas[0].type());

  for (auto& i : storyParams.theWindDataVector)
  {
    const WindDataItemUnit& windDataItem = (*i)(areaType);

    html_data
        << "<tr>"
        << "<td>" << Fmi::to_simple_string(windDataItem.thePeriod.localStartTime().GetDateTime())
        << "</td>"
        << "<td>" << fixed << setprecision(2) << windDataItem.theWindSpeedMin.value() << "</td>"
        << "<td>" << fixed << setprecision(2) << windDataItem.theWindSpeedMax.value() << "</td>"
        << "<td>" << fixed << setprecision(2) << windDataItem.theEqualizedMaxWind.value() << "</td>"
        << "<td>" << fixed << setprecision(2) << windDataItem.theWindSpeedMedian.value() << "</td>"
        << "<td BGCOLOR=lightgreen>" << fixed << setprecision(2)
        << windDataItem.theEqualizedMedianWind.value() << "</td>"
        << "<td>" << fixed << setprecision(2) << windDataItem.theWindSpeedMean.value() << "</td>"
        << "<td>" << fixed << setprecision(2) << windDataItem.theWindSpeedMean.error() << "</td>"
        << "<td>" << fixed << setprecision(2) << windDataItem.theWindSpeedTop.value() << "</td>"
        << "<td BGCOLOR=lightgreen>" << fixed << setprecision(2)
        << windDataItem.theEqualizedTopWind.value() << "</td>"
        << "<td BGCOLOR=gold>" << fixed << setprecision(2) << windDataItem.theWindSpeedCalc.value()
        << "</td>"
        << "<td>" << fixed << setprecision(2) << windDataItem.theGustSpeed.value() << "</td>"
        << "<td>" << fixed << setprecision(2) << windDataItem.theCorrectedWindDirection.value()
        << "</td>"
        << "<td>" << fixed << setprecision(2) << windDataItem.theCorrectedWindDirection.error()
        << "</td>"
        << "<td BGCOLOR=lightblue>" << fixed << setprecision(2)
        << windDataItem.theEqualizedWindDirection.value() << "</td>\n"
        << "</tr>\n";
  }

  html_data << "</table>\n";

  return html_data.str();
}

std::string get_html_winddirection_distribution(
    wo_story_params& storyParams, WindStoryTools::CompassType compass_type = sixteen_directions)
{
  std::stringstream html_data;

  html_data << "<h5>Wind direction distribution - "
            << (compass_type == sixteen_directions ? "16-compass" : "8-compass") << "</h5>\n";
  html_data << R"(<font face="Serif" size="2" color="darkblue">\n)";
  html_data << "<p>\n";

  if (compass_type == sixteen_directions)
  {
    html_data << "N=348.75-11.25 / n-ne=11.25-33.75 / NE=33.75-56.25 / "
                 "ne-e=56.25-78.75 / "
                 "E=78.75-101.25 / e-se=101.25-123.75 / SE=123.75-146.25 / "
                 "se-s=146.25-168.75</br>"
              << '\n';
    html_data << "S=168.75-191.25 / s-sw=191.25-213.75 / SW=213.75-236.25 / "
                 "sw-w=236.25-258.75 / "
                 "W=258.75-281.25 / w-nw=281.25-303.75 / NW=303.75-326.25 / "
                 "nw-n=326.25-348.75</br>"
              << '\n';
  }
  else
  {
    html_data << "N=337.50-22.50 / NE=22.20-67.50 / E=67.50-112.50 SE=112.50-157.50 / "
                 "S=157.50-202.50</br>"
              << '\n';
    html_data << "SW=202.50-247.50 W=247.50-292.50 NW=292.50-337.50</br>\n";
  }

  html_data << "</p>\n";
  html_data << "</font>\n";
  html_data << "<table border=\"1\">\n";

  if (compass_type == sixteen_directions)
  {
    html_data << '\n'
              << "<tr>\n"
              << "<td>time</td><td>N</td><td>n-ne</td><td>NE</td><td>ne-e</td><td>E</"
                 "td><td>e-se</td><td>SE</td><td>se-s</td><td>S</td><td>s-sw</td><td>SW</"
                 "td><td>sw-w</td><td>W</td><td>w-nw</td><td>NW</"
                 "td><td>nw-n</td><td>mean</td><td>sdev</td>"
              << '\n'
              << "</tr>\n";
  }
  else
  {
    html_data << '\n'
              << "<tr>\n"
              << "<td>time</td><td>N</td><td>NE</td><td>E</td><td>SE</td><td>S</td><td>SW</"
                 "td><td>W</td><td>NW</td><td>mean</td><td>sdev</td>"
              << '\n'
              << "</tr>\n";
  }

  const wind_data_item_vector& theWindDataItemVector(storyParams.theWindDataVector);
  const vector<unsigned int>& theIndexVector(
      storyParams.originalWindDataIndexes(storyParams.theArea.type()));

  const double share_limit = (compass_type == sixteen_directions ? 10.0 : 20.0);

  for (unsigned int index : theIndexVector)
  {
    const WindDataItemUnit& theWindDataItem =
        (*theWindDataItemVector[index])(storyParams.theArea.type());

    html_data << "<tr>\n"
              << "<td>"
              << Fmi::to_simple_string(theWindDataItem.thePeriod.localEndTime().GetDateTime())
              << "</td>";

    // Which index is the mean at?
    float mean = theWindDataItem.theCorrectedWindDirection.value();
    unsigned int steps = (compass_type == sixteen_directions ? 16 : 8);
    float step = 360.0 / steps;
    unsigned int mean_idx = static_cast<int>((mean + step / 2) / step) % steps + POHJOINEN;

    for (unsigned int i = POHJOINEN; i <= POHJOINEN_LUODE;
         i += (compass_type == sixteen_directions ? 1 : 2))
    {
      double share = theWindDataItem.getWindDirectionShare(
          static_cast<WindDirectionId>(i), storyParams.theWindDirectionMinSpeed, compass_type);
      std::string cell_effect = "<td>";
      if (share >= share_limit)
        cell_effect = "<td bgcolor=\"#FF9A9A\">";
      else if (share > 0.0)
        cell_effect = "<td bgcolor=lightgreen>";
      html_data << cell_effect;
      if (i == mean_idx)
        html_data << "<b>";
      html_data << fixed << setprecision(2) << share;
      if (i == mean_idx)
        html_data << "</b>";
      html_data << "</td>";
    }

    html_data << "<td>" << fixed << setprecision(2) << mean << "("
              << get_direction_abbreviation(mean, compass_type) << ")</td>\n"
              << "<td>" << fixed << setprecision(2) << theWindDataItem.theWindDirection.error()
              << "</td>\n"
              << "</tr>\n";
  }

  html_data << "</table>\n";

  return html_data.str();
}

namespace
{
using WindShareFn = double (*)(const WindDataItemUnit&, unsigned int);

double mean_wind_share(const WindDataItemUnit& item, unsigned int k)
{
  return item.getWindSpeedShare((k == 0 ? 0.0 : k - 0.5), (k == 0 ? 0.5 : k + 0.5));
}

double top_wind_share(const WindDataItemUnit& item, unsigned int k)
{
  return item.getTopWindSpeedShare((k == 0 ? 0.0 : k - 0.5), (k == 0 ? 0.5 : k + 0.5));
}

std::pair<unsigned int, unsigned int> find_wind_speed_category_bounds(
    const wind_data_item_vector& windData,
    const vector<unsigned int>& indexVector,
    WeatherArea::Type areaType,
    unsigned int numCategories,
    WindShareFn shareOf)
{
  unsigned int startIdx = 33;
  unsigned int endIdx = 0;
  for (unsigned int index : indexVector)
  {
    const WindDataItemUnit& item = (*windData[index])(areaType);
    for (unsigned int k = 0; k < numCategories; k++)
    {
      if (shareOf(item, k) > 0.0)
      {
        startIdx = std::min(k, startIdx);
        endIdx = std::max(k, endIdx);
      }
    }
  }
  startIdx = (startIdx <= 2 ? 0 : startIdx - 2);
  endIdx = (endIdx + 3 >= 33 ? 33 : endIdx + 3);
  return {startIdx, endIdx};
}

void append_html_wind_data_row(std::stringstream& out,
                               const WindDataItemUnit& item,
                               unsigned int startIdx,
                               unsigned int endIdx,
                               WindShareFn shareOf)
{
  auto median = std::lround(item.theWindSpeedMedian.value());
  auto mean = std::lround(item.theWindSpeedMean.value());

  out << '\n'
      << "<tr>\n<td>" << Fmi::to_simple_string(item.thePeriod.localEndTime().GetDateTime())
      << "</td>";

  for (unsigned int k = startIdx; k < endIdx; k++)
  {
    double share = shareOf(item, k);
    if (share > 10.0)
      out << "<td BGCOLOR=\"#FF9A9A\">";
    else if (share > 0.0)
      out << "<td BGCOLOR=lightgreen>";
    else
      out << "<td>";
    if (k == static_cast<unsigned int>(mean))
      out << "<b>";
    if (k == static_cast<unsigned int>(median))
      out << "<u>";
    out << fixed << setprecision(2) << share;
    if (k == static_cast<unsigned int>(median))
      out << "</u>";
    if (k == static_cast<unsigned int>(mean))
      out << "</b>";
    out << "</td>";
  }
  out << "\n</tr>\n";
}
}  // namespace

std::string get_html_windspeed_distribution(wo_story_params& storyParams, const std::string& type)
{
  if (storyParams.theWindDataVector.empty())
    return "";

  const wind_data_item_vector& windData(storyParams.theWindDataVector);
  const vector<unsigned int>& indexVector(
      storyParams.originalWindDataIndexes(storyParams.theArea.type()));
  WeatherArea::Type areaType = storyParams.theArea.type();

  bool isMean = (type == "mean");
  WindShareFn shareOf = isMean ? mean_wind_share : top_wind_share;
  const WindDataItemUnit& firstItem = (*windData[0])(areaType);
  unsigned int numCategories = isMean ? firstItem.theWindSpeedDistribution.size()
                                      : firstItem.theWindSpeedDistributionTop.size();

  auto [startIdx, endIdx] =
      find_wind_speed_category_bounds(windData, indexVector, areaType, numCategories, shareOf);

  std::stringstream html;
  html << "<h5>Wind speed distribution - " + type + " wind </h5>\n"
       << "<b>Mean</b> <u>Median</u><br>"
       << "<table border=\"1\">\n";

  // Header row
  html << "\n<tr>\n<td>time</td>";
  for (unsigned int k = startIdx; k < endIdx; k++)
    html << "<td>" << k << "m/s</td>";
  html << "\n</tr>\n";

  // Data rows
  for (unsigned int index : indexVector)
  {
    const WindDataItemUnit& item = (*windData[index])(areaType);
    append_html_wind_data_row(html, item, startIdx, endIdx, shareOf);
  }

  html << "</table>\n";
  return html.str();
}

std::string get_js_code(std::size_t js_id, bool addExternalScripts, unsigned int peakWindSpeed)
{
  std::stringstream js_code;

  if (addExternalScripts)
    js_code << '\n'
            << "<script src=\"https://code.jquery.com/jquery-1.12.0.min.js\"></script>\n"
            << "<script src=\"https://code.highcharts.com/highcharts.js\"></script>\n"
            << "<script src=\"https://code.highcharts.com/modules/data.js\"></script>\n"
            << "<script src=\"https://code.highcharts.com/modules/exporting.js\"></script>" << '\n';

  js_code << "<script>"
             "$(function () {"
             "$('#ws_container"
          << js_id << "').highcharts({yAxis: { title: {text: 'm/s' }, min:0, max:" << peakWindSpeed
          << ", tickInterval: 2.5},"
             "title: {"
             "text: 'Wind speed'"
             "},"
             "data: {"
             "csv: document.getElementById('csv"
          << js_id
          << "').innerHTML"
             "},"
             "plotOptions: {"
             "series: {"
             "marker: {"
             "enabled: false"
             "}"
             "}"
             "},"
             "series: [{"
             "lineWidth: 1"
             "}, {"
             "type: 'line',"
             "color: '#c4392d',"
             "negativeColor: '#5679c4',"
             "fillOpacity: 0.5"
             "}]"
             "});"
             "});"
             "$(function () {"
             "$('#wd_container"
          << js_id + 1
          << "').highcharts({yAxis: { title: {text: 'degrees' }, min:0, max:360, tickInterval: 45},"
             "title: {"
             "text: 'Wind direction'"
             "},"
             "data: {"
             "csv: document.getElementById('csv"
          << js_id + 1
          << "').innerHTML"
             "},"
             "plotOptions: {"
             "series: {"
             "marker: {"
             "enabled: false"
             "}"
             "}"
             "},"
             "series: [{"
             "lineWidth: 1"
             "}, {"
             "type: 'line',"
             "color: '#c4392d',"
             "negativeColor: '#5679c4',"
             "fillOpacity: 0.5"
             "}]"
             "});"
             "});"
             "</script>"
          << '\n';

  return js_code.str();
}

std::string get_js_data(wo_story_params& storyParams, const std::string& param, std::size_t js_id)
{
  std::stringstream js_data;

  js_data << "<div id=\""
          << std::string(param.empty() || param == "windspeed" ? "ws_container" : "wd_container")
          << js_id << R"(" style="min-width: 310px; height: 400px; margin: 0 auto"></div>\n\n)"
          << "<pre id=\"csv" << js_id << R"(" style="display:none">\n)"
          << get_csv_data(storyParams, param) << '\n'
          << "</pre>\n";

  return js_data.str();
}

void generate_csv_file(wo_story_params& storyParams)
{
  for (unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
  {
    std::stringstream csv_data;
    csv_data << '\n'
             << "time,min,max,eq-max,median,eq-median,mean,sdev,top-wind,eq-top-wind,direction,"
                "direction-sdev,eq-direction,gust"
             << '\n';

    WeatherArea::Type areaType(storyParams.theWeatherAreas[k].type());

    for (auto& i : storyParams.theWindDataVector)
    {
      const WindDataItemUnit& windDataItem = (*i)(areaType);

      csv_data << windDataItem.thePeriod.localStartTime() << ", " << fixed << setprecision(4)
               << windDataItem.theWindSpeedMin.value() << ", " << fixed << setprecision(4)
               << windDataItem.theWindSpeedMax.value() << ", " << fixed << setprecision(4)
               << windDataItem.theEqualizedMaxWind.value() << ", " << fixed << setprecision(4)
               << windDataItem.theWindSpeedMedian.value() << ", " << fixed << setprecision(4)
               << windDataItem.theEqualizedMedianWind.value() << ", " << fixed << setprecision(4)
               << windDataItem.theWindSpeedMean.value() << ", " << fixed << setprecision(4)
               << windDataItem.theWindSpeedMean.error() << ", " << fixed << setprecision(4)
               << windDataItem.theWindSpeedTop.value() << ", " << fixed << setprecision(4)
               << windDataItem.theEqualizedTopWind.value() << ", " << fixed << setprecision(4)
               << windDataItem.theCorrectedWindDirection.value() << ", " << fixed << setprecision(4)
               << windDataItem.theCorrectedWindDirection.error() << ", " << fixed << setprecision(4)
               << windDataItem.theEqualizedWindDirection.value() << ", " << fixed << setprecision(4)
               << windDataItem.theGustSpeed.value() << '\n';
    }

    std::string csv_filename =
        Settings::require_string("qdtext::outdir") + "html/" + storyParams.theArea.name() + ".csv";
    ofstream output_file(csv_filename.c_str(), ios::out);

    if (output_file.fail())
    {
      throw std::runtime_error("wind_overview failed to open '" + csv_filename + "' for writing");
    }
    output_file << csv_data.str();
  }
}

void log_raw_data(wo_story_params& storyParams)
{
  for (unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
  {
    WeatherArea::Type areaType(storyParams.theWeatherAreas[k].type());
    std::string areaIdentifier(get_area_type_string(areaType));

    storyParams.theLog << "*********** RAW DATA (" << areaIdentifier << ") ***********\n";

    for (unsigned int i = 0; i < storyParams.theWindDataVector.size(); i++)
    {
      const WindDataItemUnit& windDataItem = (*storyParams.theWindDataVector[i])(areaType);
      WindDirectionId windDirectionId = wind_direction_id(windDataItem.theCorrectedWindDirection,
                                                          windDataItem.theEqualizedTopWind,
                                                          storyParams.theVar,
                                                          storyParams.theWindDirectionMinSpeed);
      std::stringstream ss;
      printDataItem(ss, windDataItem, storyParams.theWindDirectionMinSpeed);
      storyParams.theLog << ss.str();

      storyParams.theLog << "; fraasi: " << wind_direction_string(windDirectionId) << '\n';
    }
  }
}

void log_windirection_distribution(wo_story_params& storyParams)
{
  // std::string areaIdentifier = get_area_type_string(storyParams.theArea.type());
  const wind_data_item_vector& theWindDataItemVector(storyParams.theWindDataVector);
  const vector<unsigned int>& theIndexVector(
      storyParams.originalWindDataIndexes(storyParams.theArea.type()));

  stringstream ss;
  ss << '\n'
     << std::setw(75) << std::setfill('*') << " WINDDIRECTION DISTRIBUTION "
     << storyParams.theArea.name() << " " << std::setw(50) << std::setfill('*') << " \n";

  ss << std::setw(31) << std::setfill(' ') << std::right << "POH" << std::setw(12)
     << std::setfill(' ') << std::right << "KOI" << std::setw(12) << std::setfill(' ') << std::right
     << "ITÄ" << std::setw(12) << std::setfill(' ') << std::right << "KAA" << std::setw(12)
     << std::setfill(' ') << std::right << "ETE" << std::setw(12) << std::setfill(' ') << std::right
     << "LOU" << std::setw(12) << std::setfill(' ') << std::right << "LÄN" << std::setw(12)
     << std::setfill(' ') << std::right << "LUO" << std::setw(12) << std::setfill(' ') << std::right
     << "hajonta\n";

  for (unsigned int index : theIndexVector)
  {
    const WindDataItemUnit& theWindDataItem =
        (*theWindDataItemVector[index])(storyParams.theArea.type());

    ss << theWindDataItem.thePeriod.localEndTime();

    for (unsigned int i = POHJOINEN; i <= POHJOINEN_LUODE; i++)
    {
      ss << std::setw(i == POHJOINEN ? 12 : 6) << std::setfill(' ') << std::fixed << std::right
         << setprecision(2)
         << theWindDataItem.getWindDirectionShare(static_cast<WindDirectionId>(i),
                                                  storyParams.theWindDirectionMinSpeed);
    }

    ss << std::setw(6) << std::setfill(' ') << std::fixed << std::right << setprecision(2)
       << theWindDataItem.theWindDirection.error() << '\n';
  }

  storyParams.theLog << ss.str() << '\n';
}

void log_wind_data_vector(wo_story_params& storyParams,
                          const vector<unsigned int>& indexVector,
                          WeatherArea::Type areaType)
{
  for (unsigned int index : indexVector)
  {
    const WindDataItemUnit& windDataItem = (*storyParams.theWindDataVector[index])(areaType);
    std::stringstream ss;
    printDataItem(ss, windDataItem, storyParams.theWindDirectionMinSpeed);
    storyParams.theLog << ss.str() << " \n";
  }
}

void log_equalized_wind_speed_data_vector(wo_story_params& storyParams)
{
  for (unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
  {
    WeatherArea::Type areaType(storyParams.theWeatherAreas[k].type());
    std::string areaIdentifier(get_area_type_string(areaType));

    storyParams.theLog << "*********** EQUALIZED MEDIAN WIND SPEED DATA (" << areaIdentifier
                       << ") ***********\n";

    const vector<unsigned int>& indexVector = storyParams.equalizedWSIndexesMedian(areaType);

    log_wind_data_vector(storyParams, indexVector, areaType);
  }

  for (unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
  {
    WeatherArea::Type areaType(storyParams.theWeatherAreas[k].type());
    std::string areaIdentifier(get_area_type_string(areaType));

    storyParams.theLog << "*********** EQUALIZED MAXIMUM WIND SPEED DATA (" << areaIdentifier
                       << ") ***********\n";

    vector<unsigned int>& indexVector = storyParams.equalizedWSIndexesMaxWind(areaType);

    log_wind_data_vector(storyParams, indexVector, areaType);
  }

  for (unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
  {
    WeatherArea::Type areaType(storyParams.theWeatherAreas[k].type());
    std::string areaIdentifier(get_area_type_string(areaType));

    storyParams.theLog << "*********** EQUALIZED TOP WIND SPEED DATA (" << areaIdentifier
                       << ") ***********\n";

    vector<unsigned int>& indexVector = storyParams.equalizedWSIndexesTopWind(areaType);

    log_wind_data_vector(storyParams, indexVector, areaType);
  }
}

void log_equalized_wind_direction_data_vector(wo_story_params& storyParams)
{
  for (unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
  {
    WeatherArea::Type areaType(storyParams.theWeatherAreas[k].type());
    std::string areaIdentifier(get_area_type_string(areaType));

    storyParams.theLog << "*********** EQUALIZED WIND DIRECTION DATA (" << areaIdentifier
                       << ") ***********\n";

    vector<unsigned int>& indexVector = storyParams.equalizedWDIndexes(areaType);

    log_wind_data_vector(storyParams, indexVector, areaType);
  }
}

void log_wind_speed_periods(wo_story_params& storyParams)
{
  storyParams.theLog << "*********** WIND SPEED PERIODS ***********\n";
  for (unsigned int i = 0; i < storyParams.theWindSpeedVector.size(); i++)
  {
    storyParams.theLog << *(storyParams.theWindSpeedVector[i]);
  }
}

void log_wind_direction_periods(wo_story_params& storyParams)
{
  storyParams.theLog << "*********** WIND DIRECTION PERIODS ***********\n";
  for (unsigned int i = 0; i < storyParams.theWindDirectionVector.size(); i++)
  {
    storyParams.theLog << *(storyParams.theWindDirectionVector[i]);
  }
}

void log_wind_event_periods(wo_story_params& storyParams)
{
  storyParams.theLog << "*********** WIND EVENT PERIODS ***********\n";

  wind_event_period_data_item_vector windEventPeriodVector;
  windEventPeriodVector.insert(windEventPeriodVector.end(),
                               storyParams.theWindSpeedEventPeriodVector.begin(),
                               storyParams.theWindSpeedEventPeriodVector.end());

  std::sort(windEventPeriodVector.begin(), windEventPeriodVector.end(), wind_event_period_sort);

  for (auto& i : windEventPeriodVector)
  {
    WindEventId windEventId = i->theWindEvent;

    storyParams.theLog << i->thePeriod.localStartTime() << "..." << i->thePeriod.localEndTime()
                       << ": " << get_wind_event_string(windEventId) << " ";

    if (windEventId == TUULI_HEIKKENEE || windEventId == TUULI_VOIMISTUU ||
        windEventId == TUULI_TYYNTYY || windEventId == MISSING_WIND_SPEED_EVENT)
    {
      float maxWindBeg = i->thePeriodBeginDataItem.theEqualizedTopWind.value();
      float maxWindEnd = i->thePeriodEndDataItem.theEqualizedTopWind.value();
      float medianWindBeg = i->thePeriodBeginDataItem.theEqualizedMedianWind.value();
      float medianWindEnd = i->thePeriodEndDataItem.theEqualizedMedianWind.value();

      storyParams.theLog << ": " << fixed << setprecision(2) << medianWindBeg << "..." << fixed
                         << setprecision(2) << maxWindBeg << " -> " << fixed << setprecision(2)
                         << medianWindEnd << "..." << fixed << setprecision(2) << maxWindEnd
                         << '\n';
    }
    else if (windEventId == TUULI_KAANTYY || windEventId == TUULI_MUUTTUU_VAIHTELEVAKSI ||
             windEventId == MISSING_WIND_DIRECTION_EVENT)
    {
      WeatherResult directionBeg(i->thePeriodBeginDataItem.theEqualizedWindDirection);
      WeatherResult directionEnd(i->thePeriodEndDataItem.theEqualizedWindDirection);
      WeatherResult speedBeg(i->thePeriodBeginDataItem.theEqualizedTopWind);
      WeatherResult speedEnd(i->thePeriodEndDataItem.theEqualizedTopWind);

      WindDirectionId directionIdBeg = wind_direction_id(
          directionBeg, speedBeg, storyParams.theVar, storyParams.theWindDirectionMinSpeed);
      WindDirectionId directionIdEnd = wind_direction_id(
          directionEnd, speedEnd, storyParams.theVar, storyParams.theWindDirectionMinSpeed);

      storyParams.theLog << ": " << wind_direction_string(directionIdBeg) << "->"
                         << wind_direction_string(directionIdEnd) << '\n';
    }
    else
    {
      storyParams.theLog << " \n";
    }
  }
}

void allocate_data_structures(wo_story_params& storyParams)
{
  TextGenPosixTime periodStartTime = storyParams.theDataPeriod.localStartTime();

  // also the last hour is included
  while (periodStartTime <= storyParams.theDataPeriod.localEndTime())
  {
    WeatherPeriod weatherPeriod(periodStartTime, periodStartTime);
    WeatherResult minWind(kFloatMissing, kFloatMissing);
    WeatherResult meanWind(kFloatMissing, kFloatMissing);
    WeatherResult medianWind(kFloatMissing, kFloatMissing);
    WeatherResult maxWind(kFloatMissing, kFloatMissing);
    WeatherResult maximumWind(kFloatMissing, kFloatMissing);
    WeatherResult windDirection(kFloatMissing, kFloatMissing);
    WeatherResult gustSpeed(kFloatMissing, kFloatMissing);

    auto* dataItemsByArea = new WindDataItemsByArea();

    for (auto& theWeatherArea : storyParams.theWeatherAreas)
    {
      WeatherArea::Type type = theWeatherArea.type();

      dataItemsByArea->addItem(weatherPeriod,
                               minWind,
                               maxWind,
                               meanWind,
                               medianWind,
                               maximumWind,
                               windDirection,
                               gustSpeed,
                               type);
    }

    storyParams.theWindDataVector.push_back(dataItemsByArea);

    periodStartTime.ChangeByHours(1);
  }

  for (unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
  {
    WeatherArea::Type type = storyParams.theWeatherAreas[k].type();

    storyParams.indexes.insert(make_pair(type, new index_vectors()));
  }
}

void deallocate_data_structures(wo_story_params& storyParams)
{
  for (auto& i : storyParams.theWindDataVector)
  {
    delete i;
  }
  storyParams.theWindDataVector.clear();

  for (auto& i : storyParams.theWindSpeedVector)
  {
    delete i;
  }
  storyParams.theWindSpeedVector.clear();

  for (auto& i : storyParams.theWindDirectionVector)
  {
    delete i;
  }
  storyParams.theWindDirectionVector.clear();

  for (auto& i : storyParams.theWindSpeedEventPeriodVector)
  {
    delete i;
  }
  storyParams.theWindSpeedEventPeriodVector.clear();

  for (unsigned int i = 0; i < storyParams.theWeatherAreas.size(); i++)
  {
    delete storyParams.indexes[storyParams.theWeatherAreas[i].type()];
  }
  storyParams.indexes.clear();
}

void populate_windspeed_distribution_time_series(
    const AnalysisSources& theSources,
    const WeatherArea& theArea,
    const WeatherPeriod& thePeriod,
    const string& theVar,
    vector<pair<float, WeatherResult> >& theWindSpeedDistribution,
    vector<pair<float, WeatherResult> >& theWindSpeedDistributionTop)
{
  GridForecaster forecaster;

  float ws_lower_limit(0.0);
  float ws_upper_limit(0.5);

  while (ws_lower_limit < HIRMUMYRSKY_LOWER_LIMIT)
  {
    RangeAcceptor acceptor;
    acceptor.lowerLimit(ws_lower_limit);
    if (ws_lower_limit < HIRMUMYRSKY_LOWER_LIMIT + 1)
      acceptor.upperLimit(ws_upper_limit - 0.0001);

    WeatherResult share = forecaster.analyze(theVar + "::fake::tyyni::share",
                                             theSources,
                                             WindSpeed,
                                             Mean,
                                             Percentage,
                                             theArea,
                                             thePeriod,
                                             DefaultAcceptor(),
                                             DefaultAcceptor(),
                                             acceptor);

    pair<float, WeatherResult> shareItem(ws_lower_limit, share);
    theWindSpeedDistribution.push_back(shareItem);

    share = forecaster.analyze(theVar + "::fake::tyyni::share",
                               theSources,
                               MaximumWind,
                               Mean,
                               Percentage,
                               theArea,
                               thePeriod,
                               DefaultAcceptor(),
                               DefaultAcceptor(),
                               acceptor);

    pair<float, WeatherResult> shareItemTop(ws_lower_limit, share);
    theWindSpeedDistributionTop.push_back(shareItemTop);

    ws_lower_limit += (ws_lower_limit == 0.0 ? 0.5 : 1.0);
    ws_upper_limit += 1.0;
  }
}

void populate_data_item_for_area(wo_story_params& storyParams,
                                 GridForecaster& forecaster,
                                 unsigned int i,
                                 const WeatherArea& weatherArea)
{
  WeatherArea::Type areaType(weatherArea.type());
  WindDataItemUnit& dataItem = (storyParams.theWindDataVector[i])->getDataItem(areaType);

  dataItem.theWindSpeedMin = forecaster.analyze(storyParams.theVar + "::fake::wind::speed::minimum",
                                                storyParams.theSources,
                                                WindSpeed,
                                                Minimum,
                                                Mean,
                                                weatherArea,
                                                dataItem.thePeriod);

  if (areaType == WeatherArea::Full)
    WeatherResultTools::checkMissingValue("wind_overview", WindSpeed, dataItem.theWindSpeedMin);

  dataItem.theWindSpeedMax = forecaster.analyze(storyParams.theVar + "::fake::wind::speed::maximum",
                                                storyParams.theSources,
                                                WindSpeed,
                                                Peak,
                                                Mean,
                                                weatherArea,
                                                dataItem.thePeriod);

  if (areaType == WeatherArea::Full)
    WeatherResultTools::checkMissingValue("wind_overview", WindSpeed, dataItem.theWindSpeedMax);

  dataItem.theEqualizedMaxWind = dataItem.theWindSpeedMax;

  dataItem.theWindSpeedMean = forecaster.analyze(storyParams.theVar + "::fake::wind::speed::mean",
                                                 storyParams.theSources,
                                                 WindSpeed,
                                                 Mean,
                                                 Mean,
                                                 weatherArea,
                                                 dataItem.thePeriod);

  if (areaType == WeatherArea::Full)
    WeatherResultTools::checkMissingValue("wind_overview", WindSpeed, dataItem.theWindSpeedMean);

  dataItem.theWindSpeedMedian = forecaster.analyze(storyParams.theVar + "::fake::wind::medianwind",
                                                   storyParams.theSources,
                                                   WindSpeed,
                                                   Median,
                                                   Mean,
                                                   weatherArea,
                                                   dataItem.thePeriod);

  if (areaType == WeatherArea::Full)
    WeatherResultTools::checkMissingValue("wind_overview", WindSpeed, dataItem.theWindSpeedMedian);

  dataItem.theEqualizedMedianWind = dataItem.theWindSpeedMedian;

  dataItem.theWindSpeedTop = forecaster.analyze(storyParams.theVar + "::fake::wind::maximumwind",
                                                storyParams.theSources,
                                                MaximumWind,
                                                Peak,
                                                Mean,
                                                weatherArea,
                                                dataItem.thePeriod);

  // 1.07 from Kaisa Solin, 16.1.2025 Teams meeting
  if (dataItem.theWindSpeedTop.value() == kFloatMissing)
  {
    dataItem.theWindSpeedTop = WeatherResult(dataItem.theWindSpeedMean.value() * 1.07,
                                             dataItem.theWindSpeedMean.error() * 1.07);
  }

  dataItem.theEqualizedTopWind = dataItem.theWindSpeedTop;

  dataItem.theWindDirection = forecaster.analyze(storyParams.theVar + "::fake::wind:direction",
                                                 storyParams.theSources,
                                                 WindDirection,
                                                 Mean,
                                                 Mean,
                                                 weatherArea,
                                                 dataItem.thePeriod);

  if (areaType == WeatherArea::Full)
    WeatherResultTools::checkMissingValue(
        "wind_overview", WindDirection, dataItem.theWindDirection);

  dataItem.theCorrectedWindDirection = dataItem.theWindDirection;
  dataItem.theEqualizedWindDirection = dataItem.theWindDirection;

  dataItem.theGustSpeed = forecaster.analyze(storyParams.theVar + "::fake::gust::speed",
                                             storyParams.theSources,
                                             GustSpeed,
                                             Maximum,
                                             Mean,
                                             weatherArea,
                                             dataItem.thePeriod);

  if (dataItem.theGustSpeed.value() == kFloatMissing)
    dataItem.theGustSpeed = dataItem.theWindSpeedMax;

  populate_windspeed_distribution_time_series(storyParams.theSources,
                                              weatherArea,
                                              dataItem.thePeriod,
                                              storyParams.theVar,
                                              dataItem.theWindSpeedDistribution,
                                              dataItem.theWindSpeedDistributionTop);

  populate_winddirection_distribution_time_series(storyParams.theSources,
                                                  weatherArea,
                                                  dataItem.thePeriod,
                                                  storyParams.theVar,
                                                  dataItem.theWindDirectionDistribution16,
                                                  WindStoryTools::CompassType::sixteen_directions);

  populate_winddirection_distribution_time_series(storyParams.theSources,
                                                  weatherArea,
                                                  dataItem.thePeriod,
                                                  storyParams.theVar,
                                                  dataItem.theWindDirectionDistribution8,
                                                  WindStoryTools::CompassType::eight_directions);

  WeatherResult correctedDirection =
      WindStoryTools::mode_wind_direction(storyParams.theSources,
                                          weatherArea,
                                          dataItem.thePeriod,
                                          dataItem.theWindSpeedMedian,
                                          dataItem.theWindSpeedTop,
                                          storyParams.theVar);

  dataItem.theCorrectedWindDirection = correctedDirection;
  dataItem.theEqualizedWindDirection = correctedDirection;

  storyParams.originalWindDataIndexes(areaType).push_back(i);
  storyParams.equalizedWSIndexesMedian(areaType).push_back(i);
  storyParams.equalizedWSIndexesMaxWind(areaType).push_back(i);
  storyParams.equalizedWSIndexesTopWind(areaType).push_back(i);
  storyParams.equalizedWSIndexesCalcWind(areaType).push_back(i);
  storyParams.equalizedWDIndexes(areaType).push_back(i);
}

void check_weak_top_wind(wo_story_params& storyParams)
{
  storyParams.theWeakTopWind = true;
  unsigned int total_counter = 0;
  unsigned int counter = 0;

  for (unsigned int i = 0; i < storyParams.theWindDataVector.size(); i++)
  {
    for (unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
    {
      const WeatherArea& weatherArea = storyParams.theWeatherAreas[k];
      WeatherArea::Type areaType(weatherArea.type());

      WindDataItemUnit& dataItem = (storyParams.theWindDataVector[i])->getDataItem(areaType);
      if (dataItem.theWindSpeedTop.value() >= storyParams.theWindSpeedWarningThreshold)
        counter++;
      total_counter++;
    }
  }
  // if more than 10% above 'storyParams.theWindSpeedWarningThreshold' m/s wind is not weak
  double topWindAboveWeakShare =
      ((static_cast<float>(counter) / static_cast<float>(total_counter)) * 100.0);
  if (topWindAboveWeakShare > 10.0)
    storyParams.theWeakTopWind = false;

  storyParams.theLog << "Top wind is " << (storyParams.theWeakTopWind ? "weak" : "NOT weak")
                     << " at period " << as_string(storyParams.theForecastPeriod) << ", " << fixed
                     << setprecision(2) << topWindAboveWeakShare << "% is above "
                     << storyParams.theWindSpeedWarningThreshold << " m/s\n";
}

void populate_calculated_wind_speeds(wo_story_params& storyParams)
{
  for (unsigned int i = 0; i < storyParams.theWindDataVector.size(); i++)
  {
    for (unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
    {
      const WeatherArea& weatherArea = storyParams.theWeatherAreas[k];
      WeatherArea::Type areaType(weatherArea.type());

      WindDataItemUnit& dataItem = (storyParams.theWindDataVector[i])->getDataItem(areaType);

      // calculated wind speed
      dataItem.theWindSpeedCalc = WeatherResult(
          calculate_weighted_wind_speed(
              storyParams, dataItem.theWindSpeedTop.value(), dataItem.theWindSpeedMedian.value()),
          0.0);
      dataItem.theEqualizedCalcWind = dataItem.theWindSpeedCalc;
    }
  }
}

bool populate_time_series(wo_story_params& storyParams)
{
  GridForecaster forecaster;

  for (unsigned int i = 0; i < storyParams.theWindDataVector.size(); i++)
    for (unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
      populate_data_item_for_area(storyParams, forecaster, i, storyParams.theWeatherAreas[k]);

  check_weak_top_wind(storyParams);
  populate_calculated_wind_speeds(storyParams);
  return true;
}

void find_out_wind_speed_periods(wo_story_params& storyParams)

{
  unsigned int equalizedDataIndex;
  WeatherArea::Type areaType(storyParams.theArea.type());

  vector<unsigned int>& equalizedWSIndexesMedian = storyParams.equalizedWSIndexesMedian(areaType);

  if (equalizedWSIndexesMedian.empty())
    return;
  if (equalizedWSIndexesMedian.size() == 1)
  {
    equalizedDataIndex = equalizedWSIndexesMedian[0];
    const WindDataItemUnit& dataItem =
        (*storyParams.theWindDataVector[equalizedDataIndex])(areaType);
    storyParams.theWindSpeedVector.push_back(new WindSpeedPeriodDataItem(
        dataItem.thePeriod, wind_speed_id(dataItem.theEqualizedMedianWind)));
    return;
  }

  unsigned int periodStartEqualizedDataIndex = equalizedWSIndexesMedian[0];
  const WindDataItemUnit& dataItemFirst =
      (*storyParams.theWindDataVector[periodStartEqualizedDataIndex])(areaType);

  WindSpeedId previous_wind_speed_id(wind_speed_id(dataItemFirst.theEqualizedMedianWind));

  for (unsigned int i = 1; i < equalizedWSIndexesMedian.size(); i++)
  {
    equalizedDataIndex = equalizedWSIndexesMedian[i];
    const WindDataItemUnit& dataItemCurrent =
        (*storyParams.theWindDataVector[equalizedDataIndex])(areaType);

    WindSpeedId current_wind_speed_id(wind_speed_id(dataItemCurrent.theEqualizedMedianWind));

    if (current_wind_speed_id != previous_wind_speed_id)
    {
      const WindDataItemUnit& dataItemPrevious =
          (*storyParams.theWindDataVector[periodStartEqualizedDataIndex])(areaType);

      TextGenPosixTime periodStartTime(dataItemPrevious.thePeriod.localStartTime());
      TextGenPosixTime periodEndTime(dataItemCurrent.thePeriod.localStartTime());
      periodEndTime.ChangeByHours(-1);

      WeatherPeriod windSpeedPeriod(periodStartTime, periodEndTime);

      storyParams.theWindSpeedVector.push_back(
          new WindSpeedPeriodDataItem(windSpeedPeriod, previous_wind_speed_id));
      periodStartEqualizedDataIndex = equalizedDataIndex;
      previous_wind_speed_id = current_wind_speed_id;
    }
  }
  const WindDataItemUnit& dataItemBeforeLast =
      (*storyParams.theWindDataVector[periodStartEqualizedDataIndex])(areaType);
  const WindDataItemUnit& dataItemLast =
      (*storyParams.theWindDataVector[storyParams.theWindDataVector.size() - 1])(areaType);

  WeatherPeriod windSpeedPeriod(dataItemBeforeLast.thePeriod.localStartTime(),
                                dataItemLast.thePeriod.localEndTime());

  storyParams.theWindSpeedVector.push_back(
      new WindSpeedPeriodDataItem(windSpeedPeriod, previous_wind_speed_id));
}

void find_out_raw_wind_direction_periods(wo_story_params& storyParams)
{
  WeatherArea::Type areaType(storyParams.theArea.type());

  const wind_data_item_vector& dataVector = storyParams.theWindDataVector;

  if (dataVector.empty())
    return;
  if (dataVector.size() == 1)
  {
    const WindDataItemUnit& dataItem = (*dataVector[0])(areaType);

    storyParams.theWindDirectionVector.push_back(
        new WindDirectionPeriodDataItem(dataItem.thePeriod,
                                        wind_direction_id(dataItem.theEqualizedWindDirection,
                                                          dataItem.theEqualizedTopWind,
                                                          storyParams.theVar,
                                                          storyParams.theWindDirectionMinSpeed)));
    return;
  }

  const WindDataItemUnit& dataItemFirst = (*dataVector[0])(areaType);
  WindDirectionId previous_wind_direction_id(
      wind_direction_id(dataItemFirst.theEqualizedWindDirection,
                        dataItemFirst.theEqualizedTopWind,
                        storyParams.theVar,
                        storyParams.theWindDirectionMinSpeed));

  TextGenPosixTime periodStartTime(dataItemFirst.thePeriod.localStartTime());
  TextGenPosixTime periodEndTime(dataItemFirst.thePeriod.localStartTime());

  for (unsigned int i = 1; i < dataVector.size(); i++)
  {
    const WindDataItemUnit& dataItemCurrent = (*dataVector[i])(areaType);

    WindDirectionId current_wind_direction_id(
        wind_direction_id(dataItemCurrent.theEqualizedWindDirection,
                          dataItemCurrent.theEqualizedTopWind,
                          storyParams.theVar,
                          storyParams.theWindDirectionMinSpeed));

    if (current_wind_direction_id != previous_wind_direction_id)
    {
      if (i < dataVector.size() - 1)
      {
        const WindDataItemUnit& dataItemPrevious = (*dataVector[i - 1])(areaType);
        const WindDataItemUnit& dataItemNext = (*dataVector[i + 1])(areaType);
        if (wind_direction_id(dataItemPrevious.theEqualizedWindDirection,
                              dataItemPrevious.theEqualizedTopWind,
                              storyParams.theVar,
                              storyParams.theWindDirectionMinSpeed) ==
            wind_direction_id(dataItemNext.theEqualizedWindDirection,
                              dataItemNext.theEqualizedTopWind,
                              storyParams.theVar,
                              storyParams.theWindDirectionMinSpeed))
        {
          continue;
        }
      }

      periodEndTime = dataItemCurrent.thePeriod.localStartTime();
      periodEndTime.ChangeByHours(-1);

      WeatherPeriod windDirectionPeriod(periodStartTime, periodEndTime);

      storyParams.theWindDirectionVector.push_back(
          new WindDirectionPeriodDataItem(windDirectionPeriod, previous_wind_direction_id));
      previous_wind_direction_id = current_wind_direction_id;

      periodStartTime = periodEndTime;
      periodStartTime.ChangeByHours(1);
    }
  }
  if (periodEndTime != dataItemFirst.thePeriod.localStartTime())
  {
    periodEndTime.ChangeByHours(1);
  }

  const WindDataItemUnit& dataItemLast =
      (*dataVector[storyParams.theWindDataVector.size() - 1])(areaType);

  WeatherPeriod windDirectionLastPeriod(periodEndTime, dataItemLast.thePeriod.localEndTime());

  storyParams.theWindDirectionVector.push_back(
      new WindDirectionPeriodDataItem(windDirectionLastPeriod, previous_wind_direction_id));
}

WindEventId get_wind_speed_event(float windSpeedAtStart,
                                 float windSpeedAtEnd,
                                 double windSpeedThreshold)
{
  // round the wind speed to nearest integer
  double difference = (windSpeedAtEnd - windSpeedAtStart);

  if (abs(difference) <= windSpeedThreshold)
    return MISSING_WIND_SPEED_EVENT;
  if (difference < 0.0 && windSpeedAtEnd >= 0.0 && windSpeedAtEnd < 0.5)
    return TUULI_TYYNTYY;
  if (difference < 0.0)
    return TUULI_HEIKKENEE;
  return TUULI_VOIMISTUU;
}

WindEventId get_wind_direction_event(const WeatherResult& windDirection1,
                                     const WeatherResult& windDirection2,
                                     const WeatherResult& maximumWind1,
                                     const WeatherResult& maximumWind2,
                                     const string& var,
                                     double windDirectionThreshold,
                                     double theWindDirectionMinSpeed)
{
  WindDirectionId directionId1 =
      wind_direction_id(windDirection1, maximumWind1, var, theWindDirectionMinSpeed);
  WindDirectionId directionId2 =
      wind_direction_id(windDirection2, maximumWind2, var, theWindDirectionMinSpeed);

  if (directionId1 != VAIHTELEVA && directionId2 == VAIHTELEVA)
    return TUULI_MUUTTUU_VAIHTELEVAKSI;
  if (directionId1 == directionId2)
    return MISSING_WIND_DIRECTION_EVENT;
  if (directionId1 == VAIHTELEVA)
  {
    return TUULI_KAANTYY;
  }

  bool windDirectionDifferEnough =
      wind_direction_differ_enough(windDirection1, windDirection2, windDirectionThreshold);

  return (windDirectionDifferEnough && directionId1 != directionId2 ? TUULI_KAANTYY
                                                                    : MISSING_WIND_DIRECTION_EVENT);
}

WindEventId get_wind_direction_event(const WindEventPeriodDataItem& windEventPeriodDataItem,
                                     const string& var,
                                     double windDirectionThreshold,
                                     double theWindDirectionMinSpeed)

{
  const WeatherResult& windDirection1(
      windEventPeriodDataItem.thePeriodBeginDataItem.theEqualizedWindDirection);
  const WeatherResult& windDirection2(
      windEventPeriodDataItem.thePeriodEndDataItem.theEqualizedWindDirection);
  const WeatherResult& maximumWind1(
      windEventPeriodDataItem.thePeriodBeginDataItem.theEqualizedTopWind);
  const WeatherResult& maximumWind2(
      windEventPeriodDataItem.thePeriodEndDataItem.theEqualizedTopWind);

  return get_wind_direction_event(windDirection1,
                                  windDirection2,
                                  maximumWind1,
                                  maximumWind2,
                                  var,
                                  windDirectionThreshold,
                                  theWindDirectionMinSpeed);
}

// find out wind events: strengthening wind, weakening wind,
// or missing wind speed event:
// fill up the storyParams.theWindSpeedEventPeriodVector
void find_out_wind_speed_event_periods(wo_story_params& storyParams)
{
  WeatherArea::Type areaType(storyParams.theArea.type());

  const vector<unsigned int>& theEqualizedIndexes =
      storyParams.equalizedWSIndexesCalcWind(areaType);

  if (theEqualizedIndexes.empty())
  {
    return;
  }
  if (theEqualizedIndexes.size() == 1)
  {
    unsigned int dataIndex = theEqualizedIndexes[0];
    const WindDataItemUnit& dataItem = (*storyParams.theWindDataVector[dataIndex])(areaType);

    storyParams.theWindSpeedEventPeriodVector.push_back(new WindEventPeriodDataItem(
        dataItem.thePeriod, MISSING_WIND_SPEED_EVENT, dataItem, dataItem));
    return;
  }

  for (unsigned int i = 1; i < theEqualizedIndexes.size(); i++)
  {
    unsigned int periodBeginDataIndex = theEqualizedIndexes[i - 1];
    unsigned int periodEndDataIndex = theEqualizedIndexes[i];

    // find the data item for the period start and end
    const WindDataItemUnit& dataItemPeriodBegin =
        (*storyParams.theWindDataVector[periodBeginDataIndex])(areaType);
    const WindDataItemUnit& dataItemPeriodEnd =
        (*storyParams.theWindDataVector[periodEndDataIndex])(areaType);

    // define the event period
    WeatherPeriod windEventPeriod(dataItemPeriodBegin.thePeriod.localStartTime(),
                                  dataItemPeriodEnd.thePeriod.localStartTime());

    // find out wind speed event; for that we need speed at the beginnig and the end

    float begSpeed = dataItemPeriodBegin.theEqualizedCalcWind.value();
    float endSpeed = dataItemPeriodEnd.theEqualizedCalcWind.value();

    // at first use small threshold value, in the end
    // when periods has been (possibly) merged, check again with actual threshold
    WindEventId currentPeriodWindEvent = get_wind_speed_event(begSpeed, endSpeed, 0.001);

    bool firstRound(i == 1);
    WindEventPeriodDataItem* previousEventPeriod =
        (firstRound
             ? nullptr
             : storyParams
                   .theWindSpeedEventPeriodVector[storyParams.theWindSpeedEventPeriodVector.size() -
                                                  1]);

    if (firstRound ||
        (previousEventPeriod && previousEventPeriod->theWindEvent !=
                                    currentPeriodWindEvent))  // first round or different event
    {
      // add the original period
      storyParams.theWindSpeedEventPeriodVector.push_back(new WindEventPeriodDataItem(
          windEventPeriod, currentPeriodWindEvent, dataItemPeriodBegin, dataItemPeriodEnd));

      continue;
    }

    WeatherPeriod mergedPeriod(previousEventPeriod->thePeriod.localStartTime(),
                               windEventPeriod.localEndTime());

    auto* newEventPeriod = new WindEventPeriodDataItem(mergedPeriod,
                                                       currentPeriodWindEvent,
                                                       previousEventPeriod->thePeriodBeginDataItem,
                                                       dataItemPeriodEnd);

    delete previousEventPeriod;

    storyParams.theWindSpeedEventPeriodVector.erase(
        storyParams.theWindSpeedEventPeriodVector.begin() +
        storyParams.theWindSpeedEventPeriodVector.size() - 1);

    storyParams.theWindSpeedEventPeriodVector.push_back(newEventPeriod);
  }

  // iterate through and check against actual threshold value that wind speed differ enough
  // if it doesnt, set event as missing
  for (WindEventPeriodDataItem* windEventPeriodDataItem : storyParams.theWindSpeedEventPeriodVector)
  {
    if (!wind_speed_differ_enough(storyParams, windEventPeriodDataItem->thePeriod))
    {
      windEventPeriodDataItem->theWindEvent = MISSING_WIND_SPEED_EVENT;
    }
  }
}

void get_calculated_max_min(const wo_story_params& storyParams,
                            const WindEventPeriodDataItem& dataItem,
                            float& max,
                            float& min)
{
  float begSpeed = calculate_weighted_wind_speed(storyParams, dataItem.thePeriodBeginDataItem);
  float endSpeed = calculate_weighted_wind_speed(storyParams, dataItem.thePeriodEndDataItem);

  max = std::max(begSpeed, max);
  max = std::max(endSpeed, max);
  min = std::min(begSpeed, min);
  min = std::min(endSpeed, min);
}

// iterate merged event periods and remove short (<= 6h) missing period if it is between
// strenghtening/weakening period
wind_event_period_data_item_vector remove_short_missing_periods(
    wo_story_params& storyParams, const wind_event_period_data_item_vector& eventPeriodVector)
{
  if (eventPeriodVector.size() <= 2)
    return eventPeriodVector;

  wind_event_period_data_item_vector cleanedEventPeriods;

  for (unsigned int i = 0; i < eventPeriodVector.size(); i++)
  {
    WindEventPeriodDataItem* currentDataItem = eventPeriodVector[i];

    if (i >= eventPeriodVector.size() - 2)
    {
      cleanedEventPeriods.push_back(currentDataItem);
      continue;
    }

    WindEventPeriodDataItem* nextDataItem = eventPeriodVector[i + 1];
    WindEventPeriodDataItem* afterNextDataItem = eventPeriodVector[i + 2];

    if (currentDataItem == nullptr || nextDataItem == nullptr || afterNextDataItem == nullptr)
    {
      cleanedEventPeriods.push_back(currentDataItem);
      continue;
    }

    if (nextDataItem->theWindEvent == MISSING_WIND_SPEED_EVENT &&
        currentDataItem->theWindEvent == afterNextDataItem->theWindEvent)
    {
      if (get_period_length(nextDataItem->thePeriod) <= MISSING_EVENT_THRESHOLD_HOURS)
      {
        // merge the three event periods
        WeatherPeriod newPeriod(currentDataItem->thePeriod.localStartTime(),
                                afterNextDataItem->thePeriod.localEndTime());

        float begSpeed =
            calculate_weighted_wind_speed(storyParams, currentDataItem->thePeriodBeginDataItem);
        float endSpeed =
            calculate_weighted_wind_speed(storyParams, afterNextDataItem->thePeriodEndDataItem);
        WindEventId newWindEvent =
            get_wind_speed_event(begSpeed, endSpeed, storyParams.theWindSpeedThreshold);

        auto* newDataItem = new WindEventPeriodDataItem(newPeriod,
                                                        newWindEvent,
                                                        currentDataItem->thePeriodBeginDataItem,
                                                        afterNextDataItem->thePeriodEndDataItem);
        cleanedEventPeriods.push_back(newDataItem);
        i += 2;
      }
      else
      {
        afterNextDataItem->theSuccessiveEventFlag = true;
        cleanedEventPeriods.push_back(currentDataItem);
      }
    }
    else
    {
      cleanedEventPeriods.push_back(currentDataItem);
    }
  }

  return cleanedEventPeriods;
}

using SpeedDifferencePeriod = std::pair<float, WeatherPeriod>;
bool comp_sdp(const SpeedDifferencePeriod& sdp1, const SpeedDifferencePeriod& sdp2)
{
  return sdp1.first > sdp2.first;
}

void remove_overlapping_speed_difference_periods(std::vector<SpeedDifferencePeriod>& vec)
{
  for (unsigned int i = 0; i < vec.size() - 1; i++)
  {
    for (unsigned int j = i + 1; j < vec.size(); j++)
    {
      if (vec[i].second.localEndTime().GetYear() == 1970 ||
          vec[j].second.localEndTime().GetYear() == 1970)
        continue;

      WeatherPeriod intersection = intersecting_period(vec[i].second, vec[j].second);

      if (intersection.localStartTime().GetYear() != 1970)
        vec[j].second = WeatherPeriod(TextGenPosixTime(1970, 1, 1),
                                      TextGenPosixTime(1970, 1, 1));  // set period length to zero
    }
  }
}

WindEventPeriodDataItem* make_wind_event_period_item(wo_story_params& storyParams,
                                                     const TextGenPosixTime& startTime,
                                                     const TextGenPosixTime& endTime)
{
  const WindDataItemUnit& begDataItem = get_data_item(storyParams, startTime);
  const WindDataItemUnit& endDataItem = get_data_item(storyParams, endTime);
  float begSpeed = calculate_weighted_wind_speed(storyParams, begDataItem);
  float endSpeed = calculate_weighted_wind_speed(storyParams, endDataItem);
  return new WindEventPeriodDataItem(
      WeatherPeriod(startTime, endTime),
      get_wind_speed_event(begSpeed, endSpeed, storyParams.theWindSpeedThreshold),
      begDataItem,
      endDataItem);
}

void append_missing_gap_periods(
    wo_story_params& storyParams,
    const WindEventPeriodDataItem& dataItem,
    const std::vector<SpeedDifferencePeriod>& cleanedSpeedDifferenceVector,
    wind_event_period_data_item_vector& ret)
{
  TextGenPosixTime missingStartTime = TextGenPosixTime(1970, 1, 1);
  TextGenPosixTime missingEndTime = TextGenPosixTime(1970, 1, 1);
  TextGenPosixTime timeIter = dataItem.thePeriod.localStartTime();

  while (timeIter <= dataItem.thePeriod.localEndTime())
  {
    bool timeIterInside = false;
    for (const auto& item : cleanedSpeedDifferenceVector)
    {
      if (is_inside(timeIter, item.second))
      {
        timeIterInside = true;
        break;
      }
    }

    if (!timeIterInside)
    {
      if (missingStartTime.GetYear() == 1970)
        missingStartTime = timeIter;
      missingEndTime = timeIter;
      if (timeIter == dataItem.thePeriod.localEndTime())
        ret.push_back(make_wind_event_period_item(storyParams, missingStartTime, missingEndTime));
    }
    else if (missingStartTime.GetYear() != 1970)
    {
      ret.push_back(make_wind_event_period_item(storyParams, missingStartTime, missingEndTime));
      missingStartTime = TextGenPosixTime(1970, 1, 1);
      missingEndTime = TextGenPosixTime(1970, 1, 1);
    }
    timeIter.ChangeByHours(1);
  }
}

// examine MISSING_WIND_SPEED_EVENT periods, if there are strenghtening/weakening periods inside
// separate them
wind_event_period_data_item_vector examine_merged_missing_event_period(
    wo_story_params& storyParams, const WindEventPeriodDataItem& dataItem)
{
  wind_event_period_data_item_vector ret;

  std::vector<SpeedDifferencePeriod> speedDifferenceVector;
  unsigned int periodLength = get_period_length(dataItem.thePeriod);
  for (unsigned int j = 0; j < periodLength - 1; j++)
  {
    TextGenPosixTime startTime = dataItem.thePeriod.localStartTime();
    startTime.ChangeByHours(j);
    for (unsigned int k = j + 1; k < periodLength; k++)
    {
      TextGenPosixTime endTime = startTime;
      endTime.ChangeByHours(k - j);
      const WindDataItemUnit& begDataItem = get_data_item(storyParams, startTime);
      const WindDataItemUnit& endDataItem = get_data_item(storyParams, endTime);
      float begSpeed = calculate_weighted_wind_speed(storyParams, begDataItem);
      float endSpeed = calculate_weighted_wind_speed(storyParams, endDataItem);

      speedDifferenceVector.emplace_back(fabs(begSpeed - endSpeed),
                                         WeatherPeriod(startTime, endTime));
    }
  }

  // biggest difference in the beginnig
  std::sort(speedDifferenceVector.begin(), speedDifferenceVector.end(), comp_sdp);

  // remove overlapping periods
  remove_overlapping_speed_difference_periods(speedDifferenceVector);

  std::vector<SpeedDifferencePeriod> cleanedSpeedDifferenceVector;
  for (auto& i : speedDifferenceVector)
  {
    const SpeedDifferencePeriod& sdp = i;
    if (get_period_length(sdp.second) == 0 || sdp.first < storyParams.theWindSpeedThreshold)
      continue;
    cleanedSpeedDifferenceVector.push_back(i);
  }
  speedDifferenceVector.clear();

  for (const auto& item : cleanedSpeedDifferenceVector)
    ret.push_back(make_wind_event_period_item(
        storyParams, item.second.localStartTime(), item.second.localEndTime()));

  // add missing event period outside speed events
  append_missing_gap_periods(storyParams, dataItem, cleanedSpeedDifferenceVector, ret);

  return ret;
}

void merge_missing_wind_speed_event_periods2(wo_story_params& storyParams)
{
  wind_event_period_data_item_vector mergedEventPeriods;

  // 1) merge all successive missing periods
  for (unsigned int i = 0; i < storyParams.theWindSpeedEventPeriodVector.size(); i++)
  {
    WindEventPeriodDataItem* currentDataItem = storyParams.theWindSpeedEventPeriodVector[i];

    if (currentDataItem->theWindEvent != MISSING_WIND_SPEED_EVENT ||
        i == storyParams.theWindSpeedEventPeriodVector.size() - 1)
    {
      mergedEventPeriods.push_back(currentDataItem);
      continue;
    }

    unsigned int lastMissingIndex = 0;
    for (unsigned int k = i + 1; k < storyParams.theWindSpeedEventPeriodVector.size(); k++)
    {
      if (storyParams.theWindSpeedEventPeriodVector[k]->theWindEvent == MISSING_WIND_SPEED_EVENT)
      {
        lastMissingIndex = k;
        continue;
      }
      break;
    }

    if (lastMissingIndex == 0)
    {
      mergedEventPeriods.push_back(currentDataItem);
      continue;
    }

    WindEventPeriodDataItem* nextDataItem =
        storyParams.theWindSpeedEventPeriodVector[lastMissingIndex];

    // merge periods
    WeatherPeriod newPeriod(currentDataItem->thePeriod.localStartTime(),
                            nextDataItem->thePeriod.localEndTime());
    auto* newDataItem = new WindEventPeriodDataItem(newPeriod,
                                                    MISSING_WIND_SPEED_EVENT,
                                                    currentDataItem->thePeriodBeginDataItem,
                                                    nextDataItem->thePeriodEndDataItem);
    mergedEventPeriods.push_back(newDataItem);

    i = lastMissingIndex;
  }

  // iterate merged event periods and remove short (<= 6h) missing period if it is between
  // strenghtening/weakening period
  mergedEventPeriods = remove_short_missing_periods(storyParams, mergedEventPeriods);

  wind_event_period_data_item_vector cleanedEventPeriods;

  // 2) examine the merged missing period if there are weakening/strenghtening periods inside
  for (auto& mergedEventPeriod : mergedEventPeriods)
  {
    WindEventPeriodDataItem* dataItem = mergedEventPeriod;

    if (dataItem->theWindEvent != MISSING_WIND_SPEED_EVENT ||
        get_period_length(dataItem->thePeriod) < 4)
    {
      cleanedEventPeriods.push_back(mergedEventPeriod);
      continue;
    }

    wind_event_period_data_item_vector retVector =
        examine_merged_missing_event_period(storyParams, *mergedEventPeriod);

    if (!retVector.empty())
      cleanedEventPeriods.insert(cleanedEventPeriods.end(), retVector.begin(), retVector.end());
    else
      cleanedEventPeriods.push_back(mergedEventPeriod);
  }

  std::sort(cleanedEventPeriods.begin(), cleanedEventPeriods.end(), wind_event_period_sort);

  // 3) In the end merge successive streghtening/weakening periods if there are any
  size_t vectorOriginalSize = cleanedEventPeriods.size();
  for (size_t i = vectorOriginalSize - 1; i > 0; i--)
  {
    WindEventPeriodDataItem* prevItem = cleanedEventPeriods[i - 1];
    WindEventPeriodDataItem* currentItem = cleanedEventPeriods[i];
    if (currentItem->theWindEvent == prevItem->theWindEvent)
    {
      auto* newWindEventPeriodDataItem =
          new WindEventPeriodDataItem(WeatherPeriod(prevItem->thePeriod.localStartTime(),
                                                    currentItem->thePeriod.localEndTime()),
                                      prevItem->theWindEvent,
                                      prevItem->thePeriodBeginDataItem,
                                      currentItem->thePeriodEndDataItem);
      delete cleanedEventPeriods[i];
      cleanedEventPeriods[i] = nullptr;
      delete cleanedEventPeriods[i - 1];
      cleanedEventPeriods[i - 1] = newWindEventPeriodDataItem;
    }
  }

  // iterate merged event periods and remove short (<= 6h) missing period if it is between
  // strenghtening/weakening period
  cleanedEventPeriods = remove_short_missing_periods(storyParams, cleanedEventPeriods);

  storyParams.theWindSpeedEventPeriodVector.clear();

  for (WindEventPeriodDataItem* eventPeriod : cleanedEventPeriods)
  {
    if (eventPeriod)
      storyParams.theWindSpeedEventPeriodVector.push_back(eventPeriod);
  }
}

namespace
{
// Append merged events for a run of consecutive MISSING items.
// even if wind speed strengthens/weakens on merged period, there can be MISSING period in the beg.
void append_missing_merge_events(wo_story_params& storyParams,
                                 wind_event_period_data_item_vector& result,
                                 WindEventPeriodDataItem* firstItem,
                                 WindEventPeriodDataItem* lastItem,
                                 WindEventPeriodDataItem* maxSpeedDataItem,
                                 WindEventPeriodDataItem* nextDataItemAfterMax,
                                 WindEventPeriodDataItem* minSpeedDataItem,
                                 WindEventPeriodDataItem* nextDataItemAfterMin)
{
  WeatherPeriod newPeriod(firstItem->thePeriod.localStartTime(),
                          lastItem->thePeriod.localEndTime());
  float begSpeed = calculate_weighted_wind_speed(storyParams, firstItem->thePeriodBeginDataItem);
  float endSpeed = calculate_weighted_wind_speed(storyParams, lastItem->thePeriodEndDataItem);
  WindEventId newWindEvent =
      get_wind_speed_event(begSpeed, endSpeed, storyParams.theWindSpeedThreshold);

  bool modifyPeriod = (newWindEvent == TUULI_HEIKKENEE && maxSpeedDataItem != lastItem) ||
                      (newWindEvent == TUULI_VOIMISTUU && minSpeedDataItem != lastItem);

  if (modifyPeriod)
  {
    bool tuuliHeikkenee = (newWindEvent == TUULI_HEIKKENEE);
    WindEventPeriodDataItem* splitItem = tuuliHeikkenee ? minSpeedDataItem : maxSpeedDataItem;
    newPeriod =
        WeatherPeriod(firstItem->thePeriod.localStartTime(), splitItem->thePeriod.localEndTime());
    endSpeed = calculate_weighted_wind_speed(storyParams, splitItem->thePeriodEndDataItem);
    newWindEvent = get_wind_speed_event(begSpeed, endSpeed, storyParams.theWindSpeedThreshold);
    result.push_back(new WindEventPeriodDataItem(newPeriod,
                                                 newWindEvent,
                                                 firstItem->thePeriodBeginDataItem,
                                                 splitItem->thePeriodEndDataItem));

    if (nextDataItemAfterMin &&
        nextDataItemAfterMin->thePeriod.localStartTime() < newPeriod.localEndTime())
      nextDataItemAfterMin = nullptr;
    if (nextDataItemAfterMax &&
        nextDataItemAfterMax->thePeriod.localStartTime() < newPeriod.localEndTime())
      nextDataItemAfterMax = nullptr;

    WindEventPeriodDataItem* tailItem =
        tuuliHeikkenee ? nextDataItemAfterMin : nextDataItemAfterMax;
    if (tailItem)
    {
      WeatherPeriod tailPeriod(tailItem->thePeriod.localStartTime(),
                               lastItem->thePeriod.localEndTime());
      float tb = calculate_weighted_wind_speed(storyParams, tailItem->thePeriodBeginDataItem);
      float te = calculate_weighted_wind_speed(storyParams, lastItem->thePeriodEndDataItem);
      WindEventId tailEvent = get_wind_speed_event(tb, te, storyParams.theWindSpeedThreshold);
      result.push_back(new WindEventPeriodDataItem(
          tailPeriod, tailEvent, tailItem->thePeriodBeginDataItem, lastItem->thePeriodEndDataItem));
    }
  }
  else
  {
    result.push_back(new WindEventPeriodDataItem(newPeriod,
                                                 newWindEvent,
                                                 firstItem->thePeriodBeginDataItem,
                                                 lastItem->thePeriodEndDataItem));
  }
}

struct MissingRunInfo
{
  WindEventPeriodDataItem* lastMissing = nullptr;
  WindEventPeriodDataItem* maxItem = nullptr;
  WindEventPeriodDataItem* nextAfterMax = nullptr;
  WindEventPeriodDataItem* minItem = nullptr;
  WindEventPeriodDataItem* nextAfterMin = nullptr;
  float prevMax = 0;
  float prevMin = 0;
};

MissingRunInfo scan_missing_run(const wind_event_period_data_item_vector& epv,
                                wo_story_params& storyParams,
                                unsigned int& i,
                                float initMax,
                                float initMin)
{
  MissingRunInfo info;
  info.prevMax = initMax;
  info.prevMin = initMin;
  info.maxItem = epv[i];
  info.minItem = epv[i];
  info.nextAfterMax = (i + 1 < epv.size() ? epv[i + 1] : nullptr);
  info.nextAfterMin = info.nextAfterMax;
  size_t n = epv.size();
  for (unsigned int k = i + 1; k < n; k++)
  {
    if (epv[k]->theWindEvent != MISSING_WIND_SPEED_EVENT)
      break;
    info.lastMissing = epv[k];
    i = k;
    float mx = info.prevMax, mn = info.prevMin;
    get_calculated_max_min(storyParams, *info.lastMissing, mx, mn);
    if (mx > info.prevMax)
    {
      info.prevMax = mx;
      info.maxItem = info.lastMissing;
      info.nextAfterMax = (k + 1 < n ? epv[k + 1] : nullptr);
    }
    if (mn < info.prevMin)
    {
      info.prevMin = mn;
      info.minItem = info.lastMissing;
      info.nextAfterMin = (k + 1 < n ? epv[k + 1] : nullptr);
    }
  }
  return info;
}

wind_event_period_data_item_vector merge_missing_first_pass(wo_story_params& storyParams)
{
  wind_event_period_data_item_vector result;
  const auto& epv = storyParams.theWindSpeedEventPeriodVector;
  size_t n = epv.size();

  for (unsigned int i = 0; i < n; i++)
  {
    auto* current = epv[i];
    if (current->theWindEvent != MISSING_WIND_SPEED_EVENT)
    {
      result.push_back(current);
      continue;
    }

    float prevMax = -kFloatMissing;
    float prevMin = kFloatMissing;
    get_calculated_max_min(storyParams, *current, prevMax, prevMin);

    auto info = scan_missing_run(epv, storyParams, i, prevMax, prevMin);

    if (!info.lastMissing)
    {
      result.push_back(current);
      continue;
    }

    append_missing_merge_events(storyParams,
                                result,
                                current,
                                info.lastMissing,
                                info.maxItem,
                                info.nextAfterMax,
                                info.minItem,
                                info.nextAfterMin);
  }
  return result;
}

wind_event_period_data_item_vector split_missing_periods_pass(
    wo_story_params& storyParams, const wind_event_period_data_item_vector& cleaned)
{
  wind_event_period_data_item_vector result;
  WeatherArea::Type areaType(storyParams.theArea.type());

  for (WindEventPeriodDataItem* p : cleaned)
  {
    if (p->theWindEvent != MISSING_WIND_SPEED_EVENT)
    {
      result.push_back(p);
      continue;
    }

    bool split = false;
    float endSpeed = calculate_weighted_wind_speed(storyParams, p->thePeriodEndDataItem);

    for (unsigned int i = 0; i < storyParams.theWindDataVector.size(); i++)
    {
      const WindDataItemUnit& dataItem = (*storyParams.theWindDataVector[i])(areaType);
      if (dataItem.thePeriod.localStartTime() <= p->thePeriod.localStartTime() ||
          dataItem.thePeriod.localStartTime() >= p->thePeriod.localEndTime())
        continue;
      float begSpeed = calculate_weighted_wind_speed(storyParams, dataItem);
      WindEventId newEvent =
          get_wind_speed_event(begSpeed, endSpeed, storyParams.theWindSpeedThreshold);
      if (newEvent == MISSING_WIND_SPEED_EVENT)
        continue;
      // split missing period into two
      WeatherPeriod p1(p->thePeriod.localStartTime(), dataItem.thePeriod.localStartTime());
      WeatherPeriod p2(dataItem.thePeriod.localStartTime(), p->thePeriod.localEndTime());
      result.push_back(
          new WindEventPeriodDataItem(p1, p->theWindEvent, p->thePeriodBeginDataItem, dataItem));
      result.push_back(
          new WindEventPeriodDataItem(p2, newEvent, dataItem, p->thePeriodEndDataItem));
      split = true;
      storyParams.theLog << "Missing wind speed event period " << as_string(p->thePeriod)
                         << " split into two:\n"
                         << as_string(p1) << " -> " << get_wind_event_string(p->theWindEvent)
                         << " and " << as_string(p2) << " -> " << get_wind_event_string(newEvent)
                         << '\n';
      break;
    }

    if (!split)
      result.push_back(p);
  }
  return result;
}

void merge_consecutive_same_events(wind_event_period_data_item_vector& v)
{
  for (size_t i = v.size() - 1; i > 0; i--)
  {
    auto* prev = v[i - 1];
    auto* curr = v[i];
    if (curr->theWindEvent != prev->theWindEvent)
      continue;
    v[i - 1] = new WindEventPeriodDataItem(
        WeatherPeriod(prev->thePeriod.localStartTime(), curr->thePeriod.localEndTime()),
        prev->theWindEvent,
        prev->thePeriodBeginDataItem,
        curr->thePeriodEndDataItem);
    delete v[i];
    v[i] = nullptr;
    delete prev;
  }
}
}  // namespace

void merge_missing_wind_speed_event_periods(wo_story_params& storyParams)
{
  auto merged = merge_missing_first_pass(storyParams);
  auto cleaned = remove_short_missing_periods(storyParams, merged);
  merged.clear();

  auto split = split_missing_periods_pass(storyParams, cleaned);
  cleaned = remove_short_missing_periods(storyParams, split);

  // in the end merge successive strengthening/weakening periods if there are any
  merge_consecutive_same_events(cleaned);

  storyParams.theWindSpeedEventPeriodVector.clear();
  for (WindEventPeriodDataItem* ep : cleaned)
    if (ep)
      storyParams.theWindSpeedEventPeriodVector.push_back(ep);
}

void check_first_period(wo_story_params& storyParams)
{
  if (storyParams.theWindSpeedEventPeriodVector.empty())
  {
    storyParams.theLog << "Error: size of storyParams.theWindSpeedEventPeriodVector is zero!!"
                       << '\n';
    return;
  }

  WindEventPeriodDataItem* firstSpeedDataItem = storyParams.theWindSpeedEventPeriodVector.at(0);
  WindEventId firstWindEvent = firstSpeedDataItem->theWindEvent;
  WeatherPeriod firstDirectionPeriod = storyParams.theWindDirectionPeriods.at(0);
  WindDirectionId firstDirection = get_wind_direction(storyParams, firstDirectionPeriod).id;

  // if wind is strenghtening at start and it is varying, add missing wind speed event to the
  // beginning
  if (get_period_length(firstDirectionPeriod) > 2 && firstWindEvent == TUULI_VOIMISTUU &&
      firstDirection == VAIHTELEVA)
  {
    WeatherArea::Type areaType(storyParams.theArea.type());
    const WindDataItemUnit* firstDataItem = nullptr;
    const WindDataItemUnit* lastDataItem = nullptr;
    const WindDataItemUnit* afterLastDataItem = nullptr;
    for (auto& i : storyParams.theWindDataVector)
    {
      const WindDataItemUnit& dataItem = (*i)(areaType);

      if (is_inside(dataItem.thePeriod.localStartTime(), firstDirectionPeriod))
      {
        if (!firstDataItem)
          firstDataItem = &dataItem;
        lastDataItem = &dataItem;
      }
      else if (!afterLastDataItem && lastDataItem)
      {
        afterLastDataItem = &dataItem;
      }
    }

    // split the period into two
    if (firstDataItem && lastDataItem)
    {
      WeatherPeriod newPeriod1(firstDataItem->thePeriod.localStartTime(),
                               lastDataItem->thePeriod.localStartTime());

      auto* newDataItem1 = new WindEventPeriodDataItem(
          newPeriod1, MISSING_WIND_SPEED_EVENT, *firstDataItem, *lastDataItem);
      WindEventPeriodDataItem* newDataItem2 = nullptr;
      if (afterLastDataItem)
      {
        WeatherPeriod newPeriod2(afterLastDataItem->thePeriod.localStartTime(),
                                 firstSpeedDataItem->thePeriod.localEndTime());
        newDataItem2 = new WindEventPeriodDataItem(newPeriod2,
                                                   firstSpeedDataItem->theWindEvent,
                                                   *afterLastDataItem,
                                                   firstSpeedDataItem->thePeriodEndDataItem);
      }

      delete firstSpeedDataItem;
      storyParams.theWindSpeedEventPeriodVector[0] = newDataItem1;
      if (newDataItem2)
        storyParams.theWindSpeedEventPeriodVector.insert(
            storyParams.theWindSpeedEventPeriodVector.begin() + 1, newDataItem2);
    }
  }
}

void find_out_wind_event_periods(wo_story_params& storyParams)
{
  find_out_wind_speed_event_periods(storyParams);
  find_out_wind_direction_periods(storyParams);
  merge_missing_wind_speed_event_periods2(storyParams);
  check_first_period(storyParams);

  if (storyParams.theWindSpeedEventPeriodVector.size() > 1)
    std::sort(storyParams.theWindSpeedEventPeriodVector.begin(),
              storyParams.theWindSpeedEventPeriodVector.end(),
              wind_event_period_sort);
}

double distance_from_line(const NFmiPoint& point,
                          const NFmiPoint& lineBeg,
                          const NFmiPoint& lineEnd)
{
  /*
  double side1Len =
      sqrt(pow(fabs(point.X() - lineEnd.X()), 2) + pow(fabs(point.Y() - lineEnd.Y()), 2));
  double side2Len =
      sqrt(pow(fabs(point.X() - lineBeg.X()), 2) + pow(fabs(point.Y() - lineBeg.Y()), 2));
  double baseLen =
      sqrt(pow(fabs(lineEnd.X() - lineBeg.X()), 2) + pow(fabs(lineEnd.Y() - lineBeg.Y()), 2));
  double s = (side1Len + side2Len + baseLen) / 2.0;
  double A = sqrt(s * (s - side1Len) * (s - side2Len) * (s - baseLen));
  //	double distance = (0.5*baseLen == 0 ? 0 : (A/(0.5*baseLen)));
  double distance = (A / (0.5 * baseLen));

  return distance;
  */

  double slope = (lineEnd.Y() - lineBeg.Y()) / (lineEnd.X() - lineBeg.X());

  double yvalue = lineBeg.Y() + (slope * (point.X() - lineBeg.X()));

  double deviation = abs(point.Y() - yvalue);

  return deviation;
}

enum class StatValueType
{
  MIN,
  MAX,
  TOP,
  MEDIAN,
  CALC
};

namespace
{
float get_stat_wind_value(const WindDataItemUnit& item, StatValueType type)
{
  switch (type)
  {
    case StatValueType::MAX:
      return item.theWindSpeedMax.value();
    case StatValueType::MIN:
      return item.theWindSpeedMin.value();
    case StatValueType::TOP:
      return item.theWindSpeedTop.value();
    case StatValueType::MEDIAN:
      return item.theWindSpeedMedian.value();
    case StatValueType::CALC:
      return item.theWindSpeedCalc.value();
  }
  return 0.0f;
}
}  // namespace

bool add_local_min_max_values(vector<unsigned int>& eqIndexVector,
                              wo_story_params& storyParams,
                              StatValueType statType)
{
  // include the local maximum and minimum values that were possibly removed
  // in the previous step
  WeatherArea::Type areaType(storyParams.theArea.type());
  vector<unsigned int> indexesToAdd;
  for (unsigned int i = 1; i < eqIndexVector.size(); i++)
  {
    unsigned int currentIndex = eqIndexVector[i];
    unsigned int previousIndex = eqIndexVector[i - 1];
    const WindDataItemUnit& previousItem =
        (*storyParams.theWindDataVector[previousIndex])(areaType);
    const WindDataItemUnit& currentItem = (*storyParams.theWindDataVector[currentIndex])(areaType);

    if (currentIndex == previousIndex + 1)
      continue;

    float prevVal = get_stat_wind_value(previousItem, statType);
    float currVal = get_stat_wind_value(currentItem, statType);
    float localMax = std::max(prevVal, currVal);
    float localMin = std::min(prevVal, currVal);

    unsigned localMaxIndex = UINT_MAX;
    unsigned localMinIndex = UINT_MAX;

    for (unsigned int k = previousIndex + 1; k < currentIndex; k++)
    {
      const WindDataItemUnit& itemK = ((*storyParams.theWindDataVector[k])(areaType));
      float currentValue = get_stat_wind_value(itemK, statType);

      if (currentValue > localMax)
      {
        localMax = currentValue;
        localMaxIndex = k;
      }
      else if (currentValue < localMin)
      {
        localMin = currentValue;
        localMinIndex = k;
      }
    }

    if (localMaxIndex != UINT_MAX)
    {
      indexesToAdd.push_back(localMaxIndex);
    }

    if (localMinIndex != UINT_MAX)
    {
      indexesToAdd.push_back(localMinIndex);
    }
  }
  eqIndexVector.insert(eqIndexVector.begin(), indexesToAdd.begin(), indexesToAdd.end());
  sort(eqIndexVector.begin(), eqIndexVector.end());

  return !indexesToAdd.empty();
}

void calculate_equalized_wind_speed_indexes_for_median_wind(wo_story_params& storyParams)
{
  for (unsigned int j = 0; j < storyParams.theWeatherAreas.size(); j++)
  {
    const WeatherArea& weatherArea = storyParams.theWeatherAreas[j];

    unsigned int index1;
    unsigned int index2;
    unsigned int index3;
    WeatherArea::Type areaType(weatherArea.type());  // storyParams.theArea.type());

    vector<unsigned int>& eqIndexVector = storyParams.equalizedWSIndexesMedian(areaType);

    while (true)
    {
      double minError = UINT_MAX;
      unsigned int minErrorIndex = UINT_MAX;

      for (unsigned int i = 0; i < eqIndexVector.size() - 2; i++)
      {
        index1 = eqIndexVector[i];
        index2 = eqIndexVector[i + 1];
        index3 = eqIndexVector[i + 2];

        const WindDataItemUnit& dataItemIndex1 = (*storyParams.theWindDataVector[index1])(areaType);
        const WindDataItemUnit& dataItemIndex2 = (*storyParams.theWindDataVector[index2])(areaType);
        const WindDataItemUnit& dataItemIndex3 = (*storyParams.theWindDataVector[index3])(areaType);

        double lineBegX = index1;
        double lineBegY = dataItemIndex1.theWindSpeedMedian.value();
        double lineEndX = index3;
        double lineEndY = dataItemIndex3.theWindSpeedMedian.value();
        double coordX = index2;
        double coordY = dataItemIndex2.theWindSpeedMedian.value();
        NFmiPoint point(coordX, coordY);
        NFmiPoint lineBegPoint(lineBegX, lineBegY);
        NFmiPoint lineEndPoint(lineEndX, lineEndY);
        double deviation_from_line = distance_from_line(point, lineBegPoint, lineEndPoint);

        if (deviation_from_line < minError)
        {
          minError = deviation_from_line;
          minErrorIndex = i + 1;
        }
      }

      if (minError > storyParams.theWindSpeedMaxError)
      {
        break;
      }
      // remove the point with minimum error
      eqIndexVector.erase(eqIndexVector.begin() + minErrorIndex);
    }  // while

    bool valuesToAdd = true;
    do
    {
      valuesToAdd = add_local_min_max_values(eqIndexVector, storyParams, StatValueType::MEDIAN);
    } while (valuesToAdd);

    // re-calculate equalized values for the removed points
    for (unsigned int i = 1; i < eqIndexVector.size(); i++)
    {
      unsigned int currentIndex = eqIndexVector[i];
      unsigned int previousIndex = eqIndexVector[i - 1];

      const WindDataItemUnit& previousItem =
          (*storyParams.theWindDataVector[previousIndex])(areaType);
      const WindDataItemUnit& currentItem =
          (*storyParams.theWindDataVector[currentIndex])(areaType);

      if (currentIndex == previousIndex + 1)
        continue;

      float oppositeLen =
          currentItem.theWindSpeedMedian.value() - previousItem.theWindSpeedMedian.value();
      float adjacentLen = currentIndex - previousIndex;
      float slope = oppositeLen / adjacentLen;

      for (unsigned int k = previousIndex + 1; k < currentIndex; k++)
      {
        float yValue = (slope * (k - previousIndex)) + previousItem.theWindSpeedMedian.value();
        WindDataItemUnit& item = (storyParams.theWindDataVector[k])->getDataItem(areaType);
        item.theEqualizedMedianWind = WeatherResult(yValue, 0.0);
      }
    }
  }
}

namespace
{
// Find index (into eqIndexVector) of the middle point with minimum line-deviation.
// Sets minError to that minimum deviation and returns the middle-point position,
// or UINT_MAX when the vector has fewer than 3 elements.
unsigned int find_min_error_speed_index(const vector<unsigned int>& eqIndexVector,
                                        const wind_data_item_vector& windData,
                                        WeatherArea::Type areaType,
                                        bool topWind,
                                        double& minError)
{
  minError = UINT_MAX;
  unsigned int minErrorIndex = UINT_MAX;

  for (unsigned int i = 0; i + 2 < eqIndexVector.size(); i++)
  {
    const auto& d1 = (*windData[eqIndexVector[i]])(areaType);
    const auto& d2 = (*windData[eqIndexVector[i + 1]])(areaType);
    const auto& d3 = (*windData[eqIndexVector[i + 2]])(areaType);
    auto spd = [topWind](const WindDataItemUnit& d)
    { return topWind ? d.theWindSpeedTop.value() : d.theWindSpeedMax.value(); };

    double dev = distance_from_line(NFmiPoint(eqIndexVector[i + 1], spd(d2)),
                                    NFmiPoint(eqIndexVector[i], spd(d1)),
                                    NFmiPoint(eqIndexVector[i + 2], spd(d3)));
    if (dev < minError)
    {
      minError = dev;
      minErrorIndex = i + 1;
    }
  }
  return minErrorIndex;
}

// Linearly interpolate equalized wind speed for all points between prevIdx and currIdx.
void interpolate_max_wind_segment(wo_story_params& storyParams,
                                  WeatherArea::Type areaType,
                                  unsigned int prevIdx,
                                  unsigned int currIdx,
                                  bool topWind)
{
  if (currIdx <= prevIdx + 1)
    return;
  const auto& prevItem = (*storyParams.theWindDataVector[prevIdx])(areaType);
  const auto& currItem = (*storyParams.theWindDataVector[currIdx])(areaType);
  float prevVal = topWind ? prevItem.theWindSpeedTop.value() : prevItem.theWindSpeedMax.value();
  float currVal = topWind ? currItem.theWindSpeedTop.value() : currItem.theWindSpeedMax.value();
  float slope = (currVal - prevVal) / float(currIdx - prevIdx);
  for (unsigned int k = prevIdx + 1; k < currIdx; k++)
  {
    float yValue = slope * float(k - prevIdx) + prevVal;
    WindDataItemUnit& item = storyParams.theWindDataVector[k]->getDataItem(areaType);
    if (topWind)
      item.theEqualizedTopWind = WeatherResult(yValue, 0.0);
    else
      item.theEqualizedMaxWind = WeatherResult(yValue, 0.0);
  }
}
}  // namespace

void calculate_equalized_wind_speed_indexes_for_maximum_wind(wo_story_params& storyParams,
                                                             bool topWind = true)
{
  for (unsigned int j = 0; j < storyParams.theWeatherAreas.size(); j++)
  {
    WeatherArea::Type areaType(storyParams.theWeatherAreas[j].type());
    auto& eqIndexVector = topWind ? storyParams.equalizedWSIndexesTopWind(areaType)
                                  : storyParams.equalizedWSIndexesMaxWind(areaType);

    // Phase 1: iteratively remove the point with the smallest line deviation
    while (true)
    {
      double minError = UINT_MAX;
      unsigned int minErrorIndex = find_min_error_speed_index(
          eqIndexVector, storyParams.theWindDataVector, areaType, topWind, minError);
      if (minError > storyParams.theWindSpeedMaxError)
        break;
      eqIndexVector.erase(eqIndexVector.begin() + minErrorIndex);
    }

    // Phase 2: add back any skipped local extrema
    StatValueType statType = topWind ? StatValueType::TOP : StatValueType::MAX;
    while (add_local_min_max_values(eqIndexVector, storyParams, statType))
    {
    }

    // Phase 3: re-interpolate equalized values for removed intermediate points
    for (unsigned int i = 1; i < eqIndexVector.size(); i++)
      interpolate_max_wind_segment(
          storyParams, areaType, eqIndexVector[i - 1], eqIndexVector[i], topWind);
  }
}

void calculate_equalized_wind_speed_indexes_for_calc_wind(wo_story_params& storyParams)
{
  // get claculated wind speed
  for (unsigned int k = 0; k < storyParams.theWeatherAreas.size(); k++)
  {
    const WeatherArea& weatherArea = storyParams.theWeatherAreas[k];
    WeatherArea::Type areaType(weatherArea.type());

    //	add equalized indexes of top and median wind first to set to get unique indexes
    std::set<unsigned int> index_set;

    for (unsigned int ind : storyParams.equalizedWSIndexesTopWind(areaType))
      index_set.insert(ind);

    for (unsigned int ind : storyParams.equalizedWSIndexesMedian(areaType))
      index_set.insert(ind);

    // clear equalized calculated wind indexes and then insert indexes from set
    vector<unsigned int>* eqCalcWindIndexVector =
        &(storyParams.equalizedWSIndexesCalcWind(areaType));
    eqCalcWindIndexVector->clear();

    for (unsigned int ind : index_set)
      eqCalcWindIndexVector->push_back(ind);

    // recalculate equalized calculated wind speed
    for (unsigned int i = 0; i < storyParams.theWindDataVector.size(); i++)
    {
      WindDataItemUnit& dataItem = (storyParams.theWindDataVector[i])->getDataItem(areaType);
      dataItem.theEqualizedCalcWind =
          WeatherResult(calculate_weighted_wind_speed(storyParams,
                                                      dataItem.theEqualizedTopWind.value(),
                                                      dataItem.theEqualizedMedianWind.value()),
                        0.0);
    }
  }
}

// Returns UINT_MAX if no index with small enough deviation is found.
unsigned int find_min_deviation_index(wo_story_params& storyParams,
                                      const vector<unsigned int>& eqIndexVector,
                                      WeatherArea::Type areaType,
                                      double& minError)
{
  unsigned int minErrorIndex = UINT_MAX;
  minError = UINT_MAX;

  for (unsigned int i = 0; i < eqIndexVector.size() - 2; i++)
  {
    unsigned int index1 = eqIndexVector[i];
    unsigned int index2 = eqIndexVector[i + 1];
    unsigned int index3 = eqIndexVector[i + 2];

    const WindDataItemUnit& dataItemIndex1 = (*storyParams.theWindDataVector[index1])(areaType);
    const WindDataItemUnit& dataItemIndex2 = (*storyParams.theWindDataVector[index2])(areaType);
    const WindDataItemUnit& dataItemIndex3 = (*storyParams.theWindDataVector[index3])(areaType);

    WindDirectionId dirId1 = wind_direction_id(dataItemIndex1.theEqualizedWindDirection,
                                               dataItemIndex1.theEqualizedTopWind,
                                               storyParams.theVar,
                                               storyParams.theWindDirectionMinSpeed);
    WindDirectionId dirId2 = wind_direction_id(dataItemIndex2.theEqualizedWindDirection,
                                               dataItemIndex3.theEqualizedTopWind,
                                               storyParams.theVar,
                                               storyParams.theWindDirectionMinSpeed);
    WindDirectionId dirId3 = wind_direction_id(dataItemIndex3.theEqualizedWindDirection,
                                               dataItemIndex3.theEqualizedTopWind,
                                               storyParams.theVar,
                                               storyParams.theWindDirectionMinSpeed);

    // dont remove variable wind
    if (dirId1 == VAIHTELEVA || dirId2 == VAIHTELEVA || dirId3 == VAIHTELEVA)
      continue;

    double lineBegY = dataItemIndex1.theEqualizedWindDirection.value();
    double lineEndY = dataItemIndex3.theEqualizedWindDirection.value();
    double coordY = dataItemIndex2.theEqualizedWindDirection.value();

    // if wind changes more than 180 degrees in three steps, dont try to smoothen
    if (abs(coordY - lineBegY) > 180.0 || abs(coordY - lineEndY) > 180.0 ||
        abs(lineBegY - lineEndY) > 180.0)
      continue;

    NFmiPoint point(index2, coordY);
    NFmiPoint lineBegPoint(index1, lineBegY);
    NFmiPoint lineEndPoint(index3, lineEndY);
    double deviation_from_line = distance_from_line(point, lineBegPoint, lineEndPoint);

    if (deviation_from_line < minError)
    {
      minError = deviation_from_line;
      minErrorIndex = i + 1;
    }
  }

  return minErrorIndex;
}

void recalculate_interpolated_directions(wo_story_params& storyParams,
                                         const vector<unsigned int>& eqIndexVector,
                                         WeatherArea::Type areaType)
{
  for (unsigned int i = 1; i < eqIndexVector.size(); i++)
  {
    unsigned int currentIndex = eqIndexVector[i];
    unsigned int previousIndex = eqIndexVector[i - 1];
    const WindDataItemUnit& previousItem =
        (*storyParams.theWindDataVector[previousIndex])(areaType);
    const WindDataItemUnit& currentItem = (*storyParams.theWindDataVector[currentIndex])(areaType);

    if (currentIndex == previousIndex + 1)
      continue;

    float oppositeLen = currentItem.theEqualizedWindDirection.value() -
                        previousItem.theEqualizedWindDirection.value();
    float adjacentLen = currentIndex - previousIndex;
    float slope = oppositeLen / adjacentLen;

    for (unsigned int k = previousIndex + 1; k < currentIndex; k++)
    {
      float yValue = (slope * (k - previousIndex)) + previousItem.theEqualizedWindDirection.value();

      WindDataItemUnit& item = (storyParams.theWindDataVector[k])->getDataItem(areaType);
      item.theEqualizedWindDirection = WeatherResult(yValue, item.theWindDirection.error());
    }
  }
}

void fix_isolated_variable_wind(wo_story_params& storyParams,
                                unsigned int startIndex,
                                unsigned int endIndex,
                                WeatherArea::Type areaType)
{
  for (unsigned int i = startIndex + 1; i < endIndex - 1; i++)
  {
    WindDataItemUnit& item1 = (storyParams.theWindDataVector[i - 1])->getDataItem(areaType);
    WindDataItemUnit& item2 = (storyParams.theWindDataVector[i])->getDataItem(areaType);
    WindDataItemUnit& item3 = (storyParams.theWindDataVector[i + 1])->getDataItem(areaType);

    WindDirectionId directionId1(wind_direction_id(item1.theEqualizedWindDirection,
                                                   item1.theEqualizedTopWind,
                                                   storyParams.theVar,
                                                   storyParams.theWindDirectionMinSpeed));
    WindDirectionId directionId2(wind_direction_id(item2.theEqualizedWindDirection,
                                                   item2.theEqualizedTopWind,
                                                   storyParams.theVar,
                                                   storyParams.theWindDirectionMinSpeed));
    WindDirectionId directionId3(wind_direction_id(item3.theEqualizedWindDirection,
                                                   item3.theEqualizedTopWind,
                                                   storyParams.theVar,
                                                   storyParams.theWindDirectionMinSpeed));
    if (directionId1 != VAIHTELEVA && directionId2 == VAIHTELEVA && directionId3 != VAIHTELEVA)
    {
      item2.theEqualizedWindDirection =
          WeatherResult(item2.theEqualizedWindDirection.value(), 40.0);
      item2.theCorrectedWindDirection =
          WeatherResult(item2.theEqualizedWindDirection.value(), 40.0);
    }
  }
}

void calculate_equalized_wind_direction_indexes(wo_story_params& storyParams)
{
  for (unsigned int j = 0; j < storyParams.theWeatherAreas.size(); j++)
  {
    const WeatherArea& weatherArea = storyParams.theWeatherAreas[j];
    WeatherArea::Type areaType(weatherArea.type());

    vector<unsigned int>& eqIndexVector = storyParams.equalizedWDIndexes(areaType);

    while (true)
    {
      double minError = UINT_MAX;
      unsigned int minErrorIndex =
          find_min_deviation_index(storyParams, eqIndexVector, areaType, minError);

      if (minError > storyParams.theWindDirectionMaxError)
        break;

      eqIndexVector.erase(eqIndexVector.begin() + minErrorIndex);
    }

    // re-calculate equalized values for the removed points
    recalculate_interpolated_directions(storyParams, eqIndexVector, areaType);

    unsigned int startIndex(eqIndexVector[0]);
    unsigned int endIndex(eqIndexVector[eqIndexVector.size() - 1]);

    fix_isolated_variable_wind(storyParams, startIndex, endIndex, areaType);
  }
}

void calculate_equalized_data(wo_story_params& storyParams)
{
  WeatherArea::Type areaType(storyParams.theArea.type());
  // first calculate the indexes
  if (storyParams.equalizedWSIndexesMedian(areaType).size() > 3)
    calculate_equalized_wind_speed_indexes_for_median_wind(storyParams);

  if (storyParams.equalizedWSIndexesTopWind(areaType).size() > 3)
    calculate_equalized_wind_speed_indexes_for_maximum_wind(storyParams, true);

  if (storyParams.equalizedWSIndexesMaxWind(areaType).size() > 3)
    calculate_equalized_wind_speed_indexes_for_maximum_wind(storyParams, false);

  if (storyParams.equalizedWDIndexes(areaType).size() > 3)
    calculate_equalized_wind_direction_indexes(storyParams);

  if (storyParams.equalizedWSIndexesCalcWind(areaType).size() > 3)
    calculate_equalized_wind_speed_indexes_for_calc_wind(storyParams);
}

// ----------------------------------------------------------------------------------------------
// Local convective anomaly handling.
//
// Modern NWP models simulate short-lived, spatially confined convective cells that can produce
// transient storm-level winds in a small portion of the forecast area. These are NOT synoptic
// storms and should not be reported as such.
//
// Pipeline (invoked after populate_time_series, before calculate_equalized_data):
//
//   1. detect_convective_anomalies() scans raw per-timestep data in the primary area. A timestep
//      is flagged when some fraction of the area exceeds theConvectiveStormCutoff but that
//      fraction is smaller than theConvectiveStormMinAreaFraction %. Contiguous flagged
//      timesteps form an anomaly period; periods shorter than theConvectiveStormMinDuration
//      hours are kept as anomalies.
//
//   2. remove_cell_from_timestep_stats() reruns the GridForecaster analyses for each anomalous
//      timestep with a RangeAcceptor upper limit = cutoff. This excludes the cell's grid points
//      from area statistics (Top / Max / Mean / Median). The tail of the per-timestep histogram
//      (buckets >= cutoff) is zeroed so downstream consumers of theWindSpeedDistribution(Top)
//      see a clean distribution.
//
//   3. determine_anomaly_quadrant() reruns Peak MaximumWind over each of Northern / Southern /
//      Eastern / Western for the anomaly period. If one quadrant's peak is clearly above the
//      others (configurable margin) the anomaly is tagged with that quadrant; otherwise the
//      cell is deemed to have moved / been ambiguous and the tag stays at WeatherArea::Full.
//
//   4. calculate_equalized_data() and find_out_wind_event_periods() then run on the cleaned
//      data as usual — no special casing in the forecast code.
//
//   5. append_convective_anomaly_sentences() appends one follow-up sentence per anomaly,
//      selecting the phrasing by theConvectiveStormStyle.
// ----------------------------------------------------------------------------------------------

namespace
{
// Quadrant margin (m/s) — dominantQuadrant is only set when the highest-peak quadrant exceeds
// the second-highest by at least this much. Otherwise the cell is treated as moving/ambiguous.
constexpr float CONVECTIVE_ANOMALY_QUADRANT_MARGIN = 1.0F;

std::string quadrant_phrase_fi(WeatherArea::Type type)
{
  switch (type)
  {
    case WeatherArea::Northern:
      return "pohjoisosissa";
    case WeatherArea::Southern:
      return "eteläosissa";
    case WeatherArea::Eastern:
      return "itäosissa";
    case WeatherArea::Western:
      return "länsiosissa";
    default:
      return "";
  }
}
}  // namespace

// Groups contiguous flagged timesteps in the primary area into anomaly periods. Timesteps are
// flagged when 0 < getTopWindSpeedShare(cutoff, +inf) < theConvectiveStormMinAreaFraction.
// Anomaly periods shorter than theConvectiveStormMinDuration hours are returned. The peak top
// wind over the period (pre-removal) is recorded for reporting.
std::vector<ConvectiveStormAnomaly> detect_convective_anomalies(const wo_story_params& storyParams)
{
  std::vector<ConvectiveStormAnomaly> anomalies;

  if (storyParams.theConvectiveStormMinDuration <= 0.0 &&
      storyParams.theConvectiveStormMinAreaFraction <= 0.0)
    return anomalies;

  // Detection runs only on the primary (full) area; split sub-areas are recomputed in lockstep.
  const WeatherArea::Type areaType = storyParams.theWeatherAreas[0].type();
  const unsigned int n = storyParams.theWindDataVector.size();
  const float cutoff = static_cast<float>(storyParams.theConvectiveStormCutoff);
  const double minAreaFraction = storyParams.theConvectiveStormMinAreaFraction;

  auto timestepIsAnomalous = [&](unsigned int i)
  {
    const WindDataItemUnit& item = storyParams.theWindDataVector[i]->getDataItem(areaType);
    const float topShare = item.getTopWindSpeedShare(cutoff, std::numeric_limits<float>::max());
    // An anomaly timestep has SOME cell exceeding cutoff but covering less than minAreaFraction %.
    // minAreaFraction == 0 disables the spatial check (any cell above cutoff becomes anomalous).
    return topShare > 0.0F &&
           (minAreaFraction <= 0.0 || topShare < static_cast<float>(minAreaFraction));
  };

  unsigned int i = 0;
  while (i < n)
  {
    if (!timestepIsAnomalous(i))
    {
      ++i;
      continue;
    }

    const unsigned int runStart = i;
    float peakTopWind = 0.0F;
    while (i < n && timestepIsAnomalous(i))
    {
      peakTopWind =
          std::max(peakTopWind,
                   storyParams.theWindDataVector[i]->getDataItem(areaType).theWindSpeedTop.value());
      ++i;
    }
    const unsigned int runEnd = i;  // exclusive

    const unsigned int durationHours = runEnd - runStart;
    const bool tooShort =
        (storyParams.theConvectiveStormMinDuration <= 0.0 ||
         static_cast<double>(durationHours) < storyParams.theConvectiveStormMinDuration);

    if (!tooShort)
      continue;

    const WeatherPeriod period(
        storyParams.theWindDataVector[runStart]->getDataItem(areaType).thePeriod.localStartTime(),
        storyParams.theWindDataVector[runEnd - 1]->getDataItem(areaType).thePeriod.localEndTime());

    storyParams.theLog << "Convective anomaly: " << period.localStartTime() << " - "
                       << period.localEndTime() << " duration=" << durationHours
                       << "h peak_top=" << fixed << setprecision(1) << peakTopWind << " m/s"
                       << " (cutoff=" << cutoff << " m/s, min area fraction=" << minAreaFraction
                       << "%, max duration=" << storyParams.theConvectiveStormMinDuration
                       << "h). Removing cell.\n";

    anomalies.emplace_back(period, peakTopWind);
  }

  return anomalies;
}

// Zero out the tail of a distribution (buckets whose lower_limit >= cutoff) so downstream
// code sees the anomalous cell as absent from the area.
void zero_distribution_tail(value_distribution_data_vector& dist, float cutoff)
{
  for (auto& bucket : dist)
    if (bucket.first >= cutoff)
      bucket.second = WeatherResult(0.0F, 0.0F);
}

// Reruns GridForecaster analyses for one timestep, one area, with a RangeAcceptor upper limit
// of cutoff. Replaces theWindSpeed{Top,Max,Mean,Median} and truncates the distribution tails.
// Leaves theWindSpeedMin, theGustSpeed and theWindDirection unchanged (the cell's direction
// is assumed to match the regional wind and gust filtering is out of scope for this step).
void remove_cell_from_timestep_stats(wo_story_params& storyParams,
                                     GridForecaster& forecaster,
                                     unsigned int i,
                                     const WeatherArea& weatherArea,
                                     float cutoff)
{
  const WeatherArea::Type areaType = weatherArea.type();
  WindDataItemUnit& dataItem = storyParams.theWindDataVector[i]->getDataItem(areaType);

  RangeAcceptor belowCutoff;
  belowCutoff.upperLimit(cutoff - 0.0001F);

  dataItem.theWindSpeedMax =
      forecaster.analyze(storyParams.theVar + "::fake::wind::speed::maximum::no_cell",
                         storyParams.theSources,
                         WindSpeed,
                         Peak,
                         Mean,
                         weatherArea,
                         dataItem.thePeriod,
                         belowCutoff);

  dataItem.theWindSpeedMean =
      forecaster.analyze(storyParams.theVar + "::fake::wind::speed::mean::no_cell",
                         storyParams.theSources,
                         WindSpeed,
                         Mean,
                         Mean,
                         weatherArea,
                         dataItem.thePeriod,
                         belowCutoff);

  dataItem.theWindSpeedMedian =
      forecaster.analyze(storyParams.theVar + "::fake::wind::medianwind::no_cell",
                         storyParams.theSources,
                         WindSpeed,
                         Median,
                         Mean,
                         weatherArea,
                         dataItem.thePeriod,
                         belowCutoff);

  dataItem.theWindSpeedTop =
      forecaster.analyze(storyParams.theVar + "::fake::wind::maximumwind::no_cell",
                         storyParams.theSources,
                         MaximumWind,
                         Peak,
                         Mean,
                         weatherArea,
                         dataItem.thePeriod,
                         belowCutoff);

  // Fallback: if the filtered analysis yields kFloatMissing (e.g. entire area was filtered
  // out, which would require the cell to cover 100% — contradictory to the anomaly flag),
  // keep the mean-scaled estimate used by populate_data_item_for_area.
  if (dataItem.theWindSpeedTop.value() == kFloatMissing)
  {
    dataItem.theWindSpeedTop = WeatherResult(dataItem.theWindSpeedMean.value() * 1.07F,
                                             dataItem.theWindSpeedMean.error() * 1.07F);
  }

  // Reset equalization seeds (equalization will be called after us)
  dataItem.theEqualizedMaxWind = dataItem.theWindSpeedMax;
  dataItem.theEqualizedMedianWind = dataItem.theWindSpeedMedian;
  dataItem.theEqualizedTopWind = dataItem.theWindSpeedTop;

  // Recalculate the composite calc wind used by event detection (same weighting as
  // populate_calculated_wind_speeds).
  dataItem.theWindSpeedCalc = WeatherResult(
      calculate_weighted_wind_speed(
          storyParams, dataItem.theWindSpeedTop.value(), dataItem.theWindSpeedMedian.value()),
      0.0F);
  dataItem.theEqualizedCalcWind = dataItem.theWindSpeedCalc;

  zero_distribution_tail(dataItem.theWindSpeedDistribution, cutoff);
  zero_distribution_tail(dataItem.theWindSpeedDistributionTop, cutoff);
}

// Iterates every anomaly period and every (timestep, area) pair inside it, removing the cell.
void remove_cells_from_area_stats(wo_story_params& storyParams,
                                  const std::vector<ConvectiveStormAnomaly>& anomalies)
{
  if (anomalies.empty())
    return;

  GridForecaster forecaster;
  const float cutoff = static_cast<float>(storyParams.theConvectiveStormCutoff);

  for (const auto& anomaly : anomalies)
  {
    for (unsigned int i = 0; i < storyParams.theWindDataVector.size(); i++)
    {
      const WindDataItemUnit& primary =
          storyParams.theWindDataVector[i]->getDataItem(storyParams.theWeatherAreas[0].type());
      if (!is_inside(primary.thePeriod, anomaly.period))
        continue;

      for (const auto& area : storyParams.theWeatherAreas)
        remove_cell_from_timestep_stats(storyParams, forecaster, i, area, cutoff);
    }
  }
}

// Picks the dominant quadrant for one anomaly period, or returns WeatherArea::Full when the
// cell moved / was ambiguous. Uses the primary area's existing name (coordinates) with the
// quadrant type applied — mask resolution is handled by the NorthernMaskSource etc. already
// registered in AnalysisSources.
void determine_anomaly_quadrants(wo_story_params& storyParams,
                                 std::vector<ConvectiveStormAnomaly>& anomalies)
{
  if (anomalies.empty())
    return;

  // Only meaningful for real areas with a geographic extent
  if (storyParams.theArea.isPoint())
    return;

  GridForecaster forecaster;
  const std::array<WeatherArea::Type, 4> quadrants = {
      WeatherArea::Northern, WeatherArea::Southern, WeatherArea::Eastern, WeatherArea::Western};

  for (auto& anomaly : anomalies)
  {
    std::array<float, 4> quadrantPeak{};
    bool anyValid = false;
    for (std::size_t q = 0; q < quadrants.size(); q++)
    {
      WeatherArea qArea(storyParams.theArea);
      qArea.type(quadrants[q]);

      WeatherResult peak =
          forecaster.analyze(storyParams.theVar + "::fake::wind::maximumwind::quadrant",
                             storyParams.theSources,
                             MaximumWind,
                             Peak,
                             Mean,
                             qArea,
                             anomaly.period);
      quadrantPeak[q] = (peak.value() == kFloatMissing) ? 0.0F : peak.value();
      if (peak.value() != kFloatMissing)
        anyValid = true;
    }

    if (!anyValid)
      continue;

    std::size_t bestIdx = 0;
    for (std::size_t q = 1; q < quadrants.size(); q++)
      if (quadrantPeak[q] > quadrantPeak[bestIdx])
        bestIdx = q;

    float secondBest = 0.0F;
    for (std::size_t q = 0; q < quadrants.size(); q++)
      if (q != bestIdx && quadrantPeak[q] > secondBest)
        secondBest = quadrantPeak[q];

    if (quadrantPeak[bestIdx] - secondBest >= CONVECTIVE_ANOMALY_QUADRANT_MARGIN)
      anomaly.dominantQuadrant = quadrants[bestIdx];

    storyParams.theLog << "Convective anomaly quadrant peaks: N=" << fixed << setprecision(1)
                       << quadrantPeak[0] << " S=" << quadrantPeak[1] << " E=" << quadrantPeak[2]
                       << " W=" << quadrantPeak[3]
                       << " -> dominant=" << get_area_type_string(anomaly.dominantQuadrant) << "\n";
  }
}

// Emits one follow-up sentence per anomaly period. Phrasing is controlled by
// theConvectiveStormStyle: "sentence" produces an undirected form, "quadrant" injects the
// dominant quadrant if one was found (falling back to the undirected form otherwise).
// The Finnish surface text is intentionally kept hardcoded here while meteorologists review
// real-world output; once the phrasing stabilises this will be migrated to dictionary keys.
void append_convective_anomaly_sentences(const wo_story_params& storyParams,
                                         const std::vector<ConvectiveStormAnomaly>& anomalies,
                                         Paragraph& paragraph)
{
  for (const auto& anomaly : anomalies)
  {
    Sentence sentence;
    const int peakMs = static_cast<int>(std::round(anomaly.peakWindSpeed));
    const bool useQuadrant = (storyParams.theConvectiveStormStyle == "quadrant" &&
                              anomaly.dominantQuadrant != WeatherArea::Full);

    if (useQuadrant)
      sentence << quadrant_phrase_fi(anomaly.dominantQuadrant);

    sentence << "paikoin"
             << "hyvin voimakkaita puuskia" << Delimiter(",") << "kovimmillaan" << peakMs
             << *UnitFactory::create(MetersPerSecond);
    paragraph << sentence;
  }
}

void read_configuration_params(wo_story_params& storyParams)
{
  double windSpeedMaxError =
      Settings::optional_double(storyParams.theVar + "::max_error_wind_speed", 2.0);
  double windDirectionMaxError =
      Settings::optional_double(storyParams.theVar + "::max_error_wind_direction", 10.0);
  double windSpeedThreshold =
      Settings::optional_double(storyParams.theVar + "::wind_speed_threshold", 3.0);
  double windSpeedWarningThreshold =
      Settings::optional_double(storyParams.theVar + "::wind_speed_warning_threshold", 11.0);
  double windDirectionThreshold =
      Settings::optional_double(storyParams.theVar + "::wind_direction_threshold", 25.0);
  double windCalcTopShare =
      Settings::optional_double(storyParams.theVar + "::wind_calc_top_share", 80.0);
  double windCalcTopShareWeak =
      Settings::optional_double(storyParams.theVar + "::wind_calc_top_share_weak", 80.0);
  double windSpeedTopCoverage =
      Settings::optional_double(storyParams.theVar + "::wind_speed_top_coverage", 98.0);
  double gustyWindTopWindDifference =
      Settings::optional_double(storyParams.theVar + "::gusty_wind_max_wind_difference", 5.0);
  string rangeSeparator = Settings::optional_string(storyParams.theVar + "::rangeseparator", "-");
  unsigned int minIntervalSize =
      Settings::optional_int(storyParams.theVar + "::wind_speed_interval_min_size", 2);
  unsigned int maxIntervalSize =
      Settings::optional_int(storyParams.theVar + "::wind_speed_interval_max_size", 5);
  double windDirectionMinSpeed =
      Settings::optional_double(storyParams.theVar + "::wind_direction_min_speed", 6.5);

  bool weekdaysUsed = Settings::optional_bool(storyParams.theVar + "::weekdays", true);

  double convectiveStormMinDuration =
      Settings::optional_double(storyParams.theVar + "::convective_storm_min_duration", 3.0);
  double convectiveStormMinAreaFraction =
      Settings::optional_double(storyParams.theVar + "::convective_storm_min_area_fraction", 10.0);
  double convectiveStormCutoff = Settings::optional_double(
      storyParams.theVar + "::convective_storm_cutoff", MYRSKY_LOWER_LIMIT);
  bool convectiveStormReporting =
      Settings::optional_bool(storyParams.theVar + "::convective_storm_reporting", false);
  std::string convectiveStormStyle =
      Settings::optional_string(storyParams.theVar + "::convective_storm_style", "sentence");

  storyParams.theWindSpeedMaxError = windSpeedMaxError;
  storyParams.theWindDirectionMaxError = windDirectionMaxError;
  storyParams.theWindSpeedThreshold = windSpeedThreshold;
  storyParams.theWindSpeedWarningThreshold = windSpeedWarningThreshold;
  storyParams.theWindDirectionThreshold = windDirectionThreshold;
  storyParams.theWindCalcTopShare = windCalcTopShare;
  storyParams.theWindCalcTopShareWeak = windCalcTopShareWeak;
  storyParams.theWindSpeedTopCoverage = windSpeedTopCoverage;
  storyParams.theWindDirectionMinSpeed = windDirectionMinSpeed;
  storyParams.theGustyWindTopWindDifference = gustyWindTopWindDifference;
  storyParams.theRangeSeparator = rangeSeparator;
  storyParams.theMinIntervalSize = minIntervalSize;
  storyParams.theMaxIntervalSize = maxIntervalSize;
  storyParams.theContextualMaxIntervalSize = maxIntervalSize;
  storyParams.theWeekdaysUsed = weekdaysUsed;
  storyParams.theConvectiveStormMinDuration = convectiveStormMinDuration;
  storyParams.theConvectiveStormMinAreaFraction = convectiveStormMinAreaFraction;
  storyParams.theConvectiveStormCutoff = convectiveStormCutoff;
  storyParams.theConvectiveStormReporting = convectiveStormReporting;
  storyParams.theConvectiveStormStyle = convectiveStormStyle;

  storyParams.theWeatherAreas.push_back(storyParams.theArea);

  std::string split_section_name("textgen::split_the_area::" +
                                 get_area_name_string(storyParams.theArea));
  std::string split_method_name(split_section_name + "::method");

  if (Settings::isset(split_method_name))
  {
    WeatherArea northernArea(storyParams.theArea);
    WeatherArea southernArea(storyParams.theArea);
    WeatherArea easternArea(storyParams.theArea);
    WeatherArea westernArea(storyParams.theArea);
    northernArea.type(WeatherArea::Northern);
    southernArea.type(WeatherArea::Southern);
    easternArea.type(WeatherArea::Eastern);
    westernArea.type(WeatherArea::Western);

    std::string split_method = Settings::require_string(split_method_name);
    if (split_method == "vertical")
    {
      storyParams.theWeatherAreas.push_back(easternArea);
      storyParams.theWeatherAreas.push_back(westernArea);
      storyParams.theSplitMethod = VERTICAL;
    }
    else if (split_method == "horizontal")
    {
      storyParams.theWeatherAreas.push_back(southernArea);
      storyParams.theWeatherAreas.push_back(northernArea);
      storyParams.theSplitMethod = HORIZONTAL;
    }
  }
}

float get_wind_direction_share(
    const vector<pair<float, WeatherResult> >& theWindDirectionDistribution,
    WindDirectionId windDirectionId,
    double theWindDirectionMinSpeed)
{
  float retval(0.0);

  for (const auto& i : theWindDirectionDistribution)
  {
    WeatherResult directionVar(i.first + 1.0, 0.0);
    WeatherResult speedVar(WeatherResult(5.0, 0.0));
    WindDirectionId directionId =
        wind_direction_id(directionVar, speedVar, "", theWindDirectionMinSpeed);
    if (directionId == windDirectionId)
      retval += i.second.value();
  }

  return retval;
}

float get_wind_direction_share(const wo_story_params& storyParams,
                               const WeatherPeriod& period,
                               const WindDirectionId& windDirectionId,
                               WindStoryTools::CompassType compass_type /*= sixteen_directions*/)
{
  float sumShare(0.0);
  unsigned int counter(0);

  for (unsigned int i = 0; i < storyParams.theWindDataVector.size(); i++)
  {
    WindDataItemUnit& item =
        storyParams.theWindDataVector[i]->getDataItem(storyParams.theArea.type());
    if (is_inside(item.thePeriod.localStartTime(), period))
    {
      sumShare += get_wind_direction_share(
          (compass_type == sixteen_directions ? item.theWindDirectionDistribution16
                                              : item.theWindDirectionDistribution8),
          windDirectionId,
          storyParams.theWindDirectionMinSpeed);
      counter++;
    }
  }

  return (counter == 0 ? 0.0 : sumShare / counter);
}

float WindDataItemUnit::getWindSpeedShare(float theLowerLimit, float theUpperLimit) const
{
  float retval = 0.0;

  for (const auto& i : theWindSpeedDistribution)
  {
    if (i.first >= theLowerLimit && i.first < theUpperLimit)
      retval += i.second.value();
  }

  return retval;
}

float WindDataItemUnit::getTopWindSpeedShare(float theLowerLimit, float theUpperLimit) const
{
  float retval = 0.0;

  for (const auto& i : theWindSpeedDistributionTop)
  {
    if (i.first >= theLowerLimit && i.first < theUpperLimit)
      retval += i.second.value();
  }

  return retval;
}

float WindDataItemUnit::getWindDirectionShare(
    WindDirectionId windDirectionId,
    double theWindDirectionMinSpeed,
    WindStoryTools::CompassType compass_type /* = sixteen_directions*/) const
{
  return get_wind_direction_share(
      (compass_type == sixteen_directions ? theWindDirectionDistribution16
                                          : theWindDirectionDistribution8),
      windDirectionId,
      theWindDirectionMinSpeed);
}

Paragraph WindStory::overview() const
{
  MessageLogger logger("WeatherStory::wind_overview");

  std::string areaName;
  if (itsArea.isNamed())
  {
    areaName = itsArea.name();
    logger << "** " << areaName << " **\n";
  }

  Paragraph paragraph;

  wo_story_params storyParams(itsVar, itsArea, itsPeriod, itsForecastTime, itsSources, logger);

  // read the configuration parameters
  read_configuration_params(storyParams);

  // allocate data structures for hourly data
  allocate_data_structures(storyParams);

  // populate the data structures with the relevant data
  if (populate_time_series(storyParams))
  {
    // Detect local convective cells on the RAW per-timestep data, remove their grid cells from
    // the area statistics (via RangeAcceptor), then let equalization and event detection run
    // on cleaned data. The returned anomalies are reported as follow-up sentences below.
    auto convectiveAnomalies = detect_convective_anomalies(storyParams);
    remove_cells_from_area_stats(storyParams, convectiveAnomalies);
    determine_anomaly_quadrants(storyParams, convectiveAnomalies);

    // equalize the data (now sees cleaned statistics for anomalous timesteps)
    calculate_equalized_data(storyParams);

    // find out wind event periods:
    // event periods are used to produce the story
    find_out_wind_event_periods(storyParams);

#ifndef NDEBUG
    log_raw_data(storyParams);

    // find out the wind speed periods (for logging purposes)
    find_out_wind_speed_periods(storyParams);
    // find out the wind direction periods of 16-direction compass (for logging purposes)
    find_out_raw_wind_direction_periods(storyParams);

    // log functions
    // save_raw_data(storyParams);
    log_windirection_distribution(storyParams);
    log_raw_data(storyParams);
    log_equalized_wind_speed_data_vector(storyParams);
    log_equalized_wind_direction_data_vector(storyParams);
    log_wind_speed_periods(storyParams);
    log_wind_direction_periods(storyParams);
    log_wind_event_periods(storyParams);
#else
    log_wind_speed_periods(storyParams);
    log_wind_direction_periods(storyParams);
    log_raw_data(storyParams);
#endif
    WindForecast windForecast(storyParams);

    paragraph << windForecast.getWindStory(itsPeriod);

    if (storyParams.theConvectiveStormReporting && !convectiveAnomalies.empty())
      append_convective_anomaly_sentences(storyParams, convectiveAnomalies, paragraph);

    std::size_t js_id = generateUniqueID();

    if (Settings::optional_bool("qdtext::append_graph", false))
    {
      std::string html_string(Settings::optional_string("html__append", ""));
      html_string += get_js_code(js_id,
                                 html_string.empty(),
                                 ceil(get_top_wind(storyParams.theForecastPeriod, storyParams)));

      html_string += "</br><br>\n";
      html_string += "<hr size=\"3\" color=\"black\">\n";
      html_string += "<h5>";
      Fmi::Date startDate(Fmi::Date::from_iso_string(itsPeriod.localStartTime().ToStr(kYYYYMMDD)));
      html_string += Fmi::date_time::to_simple_string(startDate);
      html_string += (itsPeriod.localStartTime().GetHour() < 10 ? " 0" : " ");
      html_string += Fmi::to_string(itsPeriod.localStartTime().GetHour());
      html_string += (itsPeriod.localStartTime().GetMin() < 10 ? ":0" : ":");
      html_string += Fmi::to_string(itsPeriod.localStartTime().GetMin());
      html_string += " - ";
      Fmi::Date endDate(Fmi::Date::from_iso_string(itsPeriod.localEndTime().ToStr(kYYYYMMDD)));
      html_string += Fmi::date_time::to_simple_string(endDate);
      html_string += (itsPeriod.localEndTime().GetHour() < 10 ? " 0" : " ");
      html_string += Fmi::to_string(itsPeriod.localEndTime().GetHour());
      html_string += (itsPeriod.localEndTime().GetMin() < 10 ? ":0" : ":");
      html_string += Fmi::to_string(itsPeriod.localEndTime().GetMin());
      html_string += "</h5>";

      Settings::set("html__append", html_string);
    }

    if (Settings::optional_bool("qdtext::append_graph", false))
    {
      std::string html_string(Settings::optional_string("html__append", ""));
      html_string += get_js_data(storyParams, "windspeed", js_id);
      js_id++;
      html_string += get_js_data(storyParams, "winddirection", js_id);
      js_id++;
      Settings::set("html__append", html_string);
    }

    if (Settings::optional_bool("qdtext::append_rawdata", false))
    {
      std::string html_string(Settings::optional_string("html__append", ""));
      html_string += get_html_rawdata(storyParams);
      Settings::set("html__append", html_string);
    }
    if (Settings::optional_bool("qdtext::append_windspeed_distribution", false))
    {
      std::string html_string(Settings::optional_string("html__append", ""));
      html_string += get_html_windspeed_distribution(storyParams, "mean");
      html_string += get_html_windspeed_distribution(storyParams, "top");
      Settings::set("html__append", html_string);
    }
    if (Settings::optional_bool("qdtext::append_winddirection_distribution", false))
    {
      std::string html_string(Settings::optional_string("html__append", ""));
      html_string += get_html_winddirection_distribution(storyParams, sixteen_directions);
      html_string += get_html_winddirection_distribution(storyParams, eight_directions);
      Settings::set("html__append", html_string);
    }
  }

  deallocate_data_structures(storyParams);

  // logger << paragraph;

  return paragraph;
}
}  // namespace TextGen

// ======================================================================
