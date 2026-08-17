// ======================================================================
/*!
 * \brief Tests for WindEventPeriodDataItem::theSuccessiveEventFlag
 *
 * The flag drives the 'edelleen' ('continues to strengthen/weaken') phrase in
 * WindForecast::populateFirstReportingPointSentence. It may only be set when a
 * strengthening/weakening period continues an EARLIER period of the SAME kind
 * across a long (> 6 h) flat period. Getting it wrong produces forecasts like
 * "Heikkenevää pohjoisenpuoleista tuulta ... Aamuyöstä alkaen edelleen
 * voimistuvaa tuulta", i.e. wind that "continues to strengthen" right after it
 * was reported weakening.
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

#include <algorithm>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;
using namespace TextGen;

// Declared here rather than in a header: the function has external linkage in
// wind_overview.cpp but is not part of the library's public interface.
namespace TextGen
{
wind_event_period_data_item_vector remove_short_missing_periods(
    wo_story_params& storyParams, const wind_event_period_data_item_vector& eventPeriodVector);
}

namespace WindSuccessiveEventTest
{
const string var = "successive";

WeatherPeriod hours(int beginHour, int endHour)
{
  return {TextGenPosixTime(2026, 7, 23, beginHour, 0, 0),
          TextGenPosixTime(2026, 7, 23, endHour, 0, 0)};
}

// A data unit is only dereferenced by remove_short_missing_periods when it
// merges periods, which the >6h-gap cases below never do, so plain 5 m/s
// values are enough.
WindDataItemUnit& data_unit()
{
  static WeatherResult value(5.0, 0.0);
  static WindDataItemUnit unit(hours(0, 1), value, value, value, value, value, value, value);
  return unit;
}

// Owns the items so each test case cleans up after itself.
struct EventPeriods
{
  wind_event_period_data_item_vector items;

  ~EventPeriods()
  {
    for (auto* item : items)
      delete item;
  }

  WindEventPeriodDataItem* add(WindEventId event, int beginHour, int endHour)
  {
    auto* item =
        new WindEventPeriodDataItem(hours(beginHour, endHour), event, data_unit(), data_unit());
    items.push_back(item);
    return item;
  }

  // Periods merged by remove_short_missing_periods come back as new items the
  // caller owns; take them over so the test leaks nothing.
  void adopt(const wind_event_period_data_item_vector& result)
  {
    for (auto* item : result)
      if (item && std::find(items.begin(), items.end(), item) == items.end())
        items.push_back(item);
  }
};

struct Params
{
  Params()
      : area("25,60"),
        forecastPeriod(hours(0, 24)),
        forecastTime(2026, 7, 23, 0, 0, 0),
        log("WindSuccessiveEventTest"),
        params(var, area, forecastPeriod, forecastTime, sources, log)
  {
  }

  AnalysisSources sources;
  WeatherArea area;
  WeatherPeriod forecastPeriod;
  TextGenPosixTime forecastTime;
  MessageLogger log;
  wo_story_params params;
};

// ----------------------------------------------------------------------
/*!
 * \brief The flag must be false in a freshly constructed item
 *
 * The items are allocated with new and the flag used to be left out of the
 * constructor, so it held whatever byte the recycled heap block contained. In
 * a long-running textgen process that byte is almost always non-zero, which
 * made 'edelleen' appear in nearly every wind story.
 */
// ----------------------------------------------------------------------

void default_construction()
{
  const size_t itemSize = sizeof(WindEventPeriodDataItem);

  // Dirty the heap so a fresh allocation is likely to reuse poisoned bytes.
  vector<unsigned char*> junk;
  for (int i = 0; i < 64; i++)
  {
    auto* block = new unsigned char[itemSize];
    for (size_t j = 0; j < itemSize; j++)
      block[j] = 0xAB;
    junk.push_back(block);
  }
  for (auto* block : junk)
    delete[] block;

  for (int i = 0; i < 64; i++)
  {
    unique_ptr<WindEventPeriodDataItem> item(
        new WindEventPeriodDataItem(hours(0, 6), TUULI_VOIMISTUU, data_unit(), data_unit()));

    // Not "if (flag)": the compiler assumes a bool is 0 or 1, so a garbage
    // byte has to be inspected through its raw representation.
    unsigned char raw = 0;
    memcpy(&raw, &item->theSuccessiveEventFlag, 1);
    if (raw != 0)
      TEST_FAILED("theSuccessiveEventFlag is not initialised: raw byte value is " +
                  std::to_string(static_cast<int>(raw)));
  }

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief A weakening period must not make the next strengthening one 'edelleen'
 *
 * This is the reported bug: strengthening wind was announced as continuing to
 * strengthen even though the previous sentence reported it weakening.
 */
// ----------------------------------------------------------------------

void opposite_events_are_not_successive()
{
  Params p;
  EventPeriods periods;
  periods.add(TUULI_HEIKKENEE, 0, 4);
  periods.add(MISSING_WIND_SPEED_EVENT, 4, 14);  // 10 h > MISSING_EVENT_THRESHOLD_HOURS
  auto* strengthening = periods.add(TUULI_VOIMISTUU, 14, 24);

  periods.adopt(remove_short_missing_periods(p.params, periods.items));

  if (strengthening->theSuccessiveEventFlag)
    TEST_FAILED("strengthening after a weakening period was marked as successive");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Two strengthening periods across a long flat period are successive
 *
 * The feature itself: this is the one case that legitimately produces
 * 'voimistuu edelleen'.
 */
// ----------------------------------------------------------------------

void same_events_across_long_gap_are_successive()
{
  Params p;
  EventPeriods periods;
  periods.add(TUULI_VOIMISTUU, 0, 4);
  periods.add(MISSING_WIND_SPEED_EVENT, 4, 14);
  auto* second = periods.add(TUULI_VOIMISTUU, 14, 24);

  periods.adopt(remove_short_missing_periods(p.params, periods.items));

  if (!second->theSuccessiveEventFlag)
    TEST_FAILED("second strengthening period across a long flat period is not successive");

  // Same for weakening
  Params p2;
  EventPeriods weakening;
  weakening.add(TUULI_HEIKKENEE, 0, 4);
  weakening.add(MISSING_WIND_SPEED_EVENT, 4, 14);
  auto* secondWeakening = weakening.add(TUULI_HEIKKENEE, 14, 24);

  weakening.adopt(remove_short_missing_periods(p2.params, weakening.items));

  if (!secondWeakening->theSuccessiveEventFlag)
    TEST_FAILED("second weakening period across a long flat period is not successive");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief A flat period must not be marked as continuing an earlier one
 *
 * The condition used to compare the two events for equality only, so a run of
 * MISSING periods satisfied it and tagged a flat period as a continuation.
 */
// ----------------------------------------------------------------------

void missing_events_are_not_successive()
{
  Params p;
  EventPeriods periods;
  periods.add(MISSING_WIND_SPEED_EVENT, 0, 4);
  periods.add(MISSING_WIND_SPEED_EVENT, 4, 14);
  auto* third = periods.add(MISSING_WIND_SPEED_EVENT, 14, 24);

  periods.adopt(remove_short_missing_periods(p.params, periods.items));

  if (third->theSuccessiveEventFlag)
    TEST_FAILED("a flat period was marked as continuing an earlier flat period");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief A stale flag is cleared when the event sequence no longer supports it
 *
 * remove_short_missing_periods runs twice per story, and the passes in between
 * split long flat periods into real events and merge neighbouring ones. A flag
 * concluded by an earlier pass must not survive into a sequence where the
 * preceding event is now of the opposite kind.
 */
// ----------------------------------------------------------------------

void stale_flag_is_recomputed()
{
  Params p;
  EventPeriods periods;
  periods.add(TUULI_HEIKKENEE, 0, 4);
  periods.add(MISSING_WIND_SPEED_EVENT, 4, 14);
  auto* strengthening = periods.add(TUULI_VOIMISTUU, 14, 24);

  // What an earlier pass concluded, before a weakening period appeared in the
  // gap ahead of it.
  strengthening->theSuccessiveEventFlag = true;

  periods.adopt(remove_short_missing_periods(p.params, periods.items));

  if (strengthening->theSuccessiveEventFlag)
    TEST_FAILED("a stale successive-event flag survived a later pass");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief A short flat period between two equal events does not set the flag
 *
 * Short gaps are merged away instead: the two periods end up in one sentence,
 * which needs no 'edelleen'.
 */
// ----------------------------------------------------------------------

void short_gap_is_not_successive()
{
  Params p;
  EventPeriods periods;
  periods.add(TUULI_VOIMISTUU, 0, 4);
  periods.add(MISSING_WIND_SPEED_EVENT, 4, 8);  // 4 h <= MISSING_EVENT_THRESHOLD_HOURS
  auto* second = periods.add(TUULI_VOIMISTUU, 8, 24);

  periods.adopt(remove_short_missing_periods(p.params, periods.items));

  if (second->theSuccessiveEventFlag)
    TEST_FAILED("periods separated by a short flat period were marked as successive");

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
    TEST(default_construction);
    TEST(opposite_events_are_not_successive);
    TEST(same_events_across_long_gap_are_successive);
    TEST(missing_events_are_not_successive);
    TEST(stale_flag_is_recomputed);
    TEST(short_gap_is_not_successive);
  }

};  // class tests

}  // namespace WindSuccessiveEventTest

int main()
{
  boost::locale::generator generator;
  std::locale::global(generator(""));

  NFmiSettings::Init();
  Settings::set(NFmiSettings::ToString());

  cout << endl
       << "WindEventPeriodDataItem successive event tests" << endl
       << "=============================================" << endl;
  WindSuccessiveEventTest::tests t;
  return t.run();
}
