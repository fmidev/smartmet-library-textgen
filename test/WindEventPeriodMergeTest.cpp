// ======================================================================
/*!
 * \brief Tests for merge_missing_wind_speed_event_periods2
 *
 * The pass rewrites storyParams.theWindSpeedEventPeriodVector: runs of flat
 * (MISSING) periods are merged, long merged ones are re-examined for events
 * hiding inside them, and neighbouring equal events are merged again. Every
 * step replaces items with new ones, and only what is left in the vector at
 * the end is freed by deallocate_data_structures, so items dropped along the
 * way have to be deleted by the pass itself.
 *
 * The test therefore hands its items over to the pass and frees only the
 * result. Run it under valgrind to check both halves of that contract: leaks
 * mean an item was dropped without being deleted, invalid frees mean one was
 * deleted while still in the vector.
 */
// ======================================================================

#include "MessageLogger.h"
#include "WindForecastStructs.h"

#include <calculator/AnalysisSources.h>
#include <calculator/Settings.h>
#include <calculator/WeatherArea.h>
#include <calculator/WeatherPeriod.h>
#include <regression/tframe.h>

#include <newbase/NFmiSettings.h>

#include <boost/locale.hpp>

#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace TextGen;

// Neither function is part of the library's public interface, but both have
// external linkage in wind_overview.cpp.
namespace TextGen
{
void merge_missing_wind_speed_event_periods2(wo_story_params& storyParams);
int get_period_length(const WeatherPeriod& thePeriod);
}  // namespace TextGen

namespace WindEventPeriodMergeTest
{
const string var = "merge";

TextGenPosixTime at(short hour)
{
  return {2026, 7, 23, hour, 0, 0};
}

WeatherPeriod hours(short beginHour, short endHour)
{
  return {at(beginHour), at(endHour)};
}

// Wind speeds an hour at a time. The pass only ever reads top and median
// wind, from which calculate_weighted_wind_speed derives the speed it
// compares against theWindSpeedThreshold.
struct Fixture
{
  Fixture(const vector<float>& hourlySpeeds)
      : area("25,60"),
        forecastPeriod(hours(0, static_cast<int>(hourlySpeeds.size()) - 1)),
        forecastTime(at(0)),
        log("WindEventPeriodMergeTest"),
        params(var, area, forecastPeriod, forecastTime, sources, log)
  {
    for (size_t hour = 0; hour < hourlySpeeds.size(); hour++)
    {
      const WeatherResult speed(hourlySpeeds[hour], 0.0);
      auto* item = new WindDataItemsByArea;
      item->addItem(hours(static_cast<int>(hour), static_cast<int>(hour) + 1),
                    speed,
                    speed,
                    speed,
                    speed,
                    speed,
                    WeatherResult(180.0, 0.0),
                    speed,
                    area.type());
      params.theWindDataVector.push_back(item);
    }
  }

  ~Fixture()
  {
    // Mirrors deallocate_data_structures: whatever the pass left in the
    // vector is ours to free, and nothing else.
    for (auto* item : params.theWindSpeedEventPeriodVector)
      delete item;
    for (auto* item : params.theWindDataVector)
      delete item;
  }

  // Hands an event period over to the pass.
  void addEventPeriod(WindEventId event, int beginHour, int endHour)
  {
    params.theWindSpeedEventPeriodVector.push_back(
        new WindEventPeriodDataItem(hours(beginHour, endHour),
                                    event,
                                    (*params.theWindDataVector[beginHour])(area.type()),
                                    (*params.theWindDataVector[endHour])(area.type())));
  }

  AnalysisSources sources;
  WeatherArea area;
  WeatherPeriod forecastPeriod;
  TextGenPosixTime forecastTime;
  MessageLogger log;
  wo_story_params params;
};

string describe(const wind_event_period_data_item_vector& periods)
{
  string result;
  for (const auto* period : periods)
  {
    if (!result.empty())
      result += ", ";
    if (!period)
    {
      result += "null";
      continue;
    }
    result += get_wind_event_string(period->theWindEvent) + " " +
              to_string(period->thePeriod.localStartTime().GetHour()) + "-" +
              to_string(period->thePeriod.localEndTime().GetHour());
  }
  return result;
}

// ----------------------------------------------------------------------
/*!
 * \brief Weakening, a long flat period, then strengthening
 *
 * The sequence behind the reported forecast: the strengthening period must
 * not come out marked as continuing an earlier one, and every item in the
 * result must be usable.
 */
// ----------------------------------------------------------------------

void weakening_then_strengthening()
{
  // 12 m/s down to 5, flat for ten hours, then up to 14
  vector<float> speeds{12, 11, 9, 7, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
                       5,  6,  7, 8, 9, 10, 11, 12, 13, 14, 14};
  Fixture f(speeds);
  f.addEventPeriod(TUULI_HEIKKENEE, 0, 4);
  f.addEventPeriod(MISSING_WIND_SPEED_EVENT, 4, 14);
  f.addEventPeriod(TUULI_VOIMISTUU, 14, 24);

  merge_missing_wind_speed_event_periods2(f.params);

  const auto& result = f.params.theWindSpeedEventPeriodVector;

  if (result.empty())
    TEST_FAILED("the pass produced no event periods");

  for (const auto* period : result)
    if (!period)
      TEST_FAILED("the pass left a null event period in the vector: " + describe(result));

  bool strengtheningIsSuccessive = false;
  for (const auto* period : result)
    if (period->theWindEvent == TUULI_VOIMISTUU && period->theSuccessiveEventFlag)
      strengtheningIsSuccessive = true;

  if (strengtheningIsSuccessive)
    TEST_FAILED("strengthening after a weakening period was marked as successive: " +
                describe(result));

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Strengthening, a long flat period, then strengthening again
 *
 * The one sequence that legitimately produces 'voimistuu edelleen'. The two
 * periods must not be merged into one either, since the flat period between
 * them is longer than MISSING_EVENT_THRESHOLD_HOURS.
 */
// ----------------------------------------------------------------------

void strengthening_twice_is_successive()
{
  // 3 m/s up to 9, flat for ten hours, then up to 18
  vector<float> speeds{3, 4, 6, 8,  9,  9,  9,  9,  9,  9,  9, 9, 9,
                       9, 9, 10, 12, 13, 14, 15, 16, 17, 18, 18, 18};
  Fixture f(speeds);
  f.addEventPeriod(TUULI_VOIMISTUU, 0, 4);
  f.addEventPeriod(MISSING_WIND_SPEED_EVENT, 4, 14);
  f.addEventPeriod(TUULI_VOIMISTUU, 14, 24);

  merge_missing_wind_speed_event_periods2(f.params);

  const auto& result = f.params.theWindSpeedEventPeriodVector;

  int strengtheningCount = 0;
  bool anySuccessive = false;
  for (const auto* period : result)
  {
    if (!period)
      TEST_FAILED("the pass left a null event period in the vector: " + describe(result));
    if (period->theWindEvent == TUULI_VOIMISTUU)
    {
      strengtheningCount++;
      if (period->theSuccessiveEventFlag)
        anySuccessive = true;
    }
  }

  if (strengtheningCount < 2)
    TEST_FAILED("the two strengthening periods were merged across a long flat period: " +
                describe(result));

  if (!anySuccessive)
    TEST_FAILED("the second strengthening period is not marked as successive: " +
                describe(result));

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Consecutive flat periods are merged into one
 *
 * Step 1 of the pass. The merged period must cover the whole run, and the
 * items it replaced must be gone.
 */
// ----------------------------------------------------------------------

void consecutive_flat_periods_are_merged()
{
  vector<float> speeds(25, 6.0f);
  Fixture f(speeds);
  f.addEventPeriod(MISSING_WIND_SPEED_EVENT, 0, 8);
  f.addEventPeriod(MISSING_WIND_SPEED_EVENT, 8, 16);
  f.addEventPeriod(MISSING_WIND_SPEED_EVENT, 16, 24);

  merge_missing_wind_speed_event_periods2(f.params);

  const auto& result = f.params.theWindSpeedEventPeriodVector;

  if (result.size() != 1)
    TEST_FAILED("expected the flat periods to merge into one, got: " + describe(result));

  if (get_period_length(result.front()->thePeriod) != 24)
    TEST_FAILED("the merged flat period does not cover the whole run: " + describe(result));

  TEST_PASSED();
}

//! The actual test driver
class tests : public tframe::tests
{
  //! Overridden message separator
  const char* error_message_prefix() const override { return "\n\t"; }
  //! Main test suite
  void test() override
  {
    TEST(weakening_then_strengthening);
    TEST(strengthening_twice_is_successive);
    TEST(consecutive_flat_periods_are_merged);
  }

};  // class tests

}  // namespace WindEventPeriodMergeTest

int main()
{
  boost::locale::generator generator;
  std::locale::global(generator(""));

  NFmiSettings::Init();
  Settings::set(NFmiSettings::ToString());

  cout << endl
       << "Wind event period merge tests" << endl
       << "=============================" << endl;
  WindEventPeriodMergeTest::tests t;
  return t.run();
}
