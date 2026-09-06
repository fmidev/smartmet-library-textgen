// Regression tests for WindStory::sea_overview ("wind_sea_overview")
//
// The story is driven with synthetic hourly area statistics through the
// ::fake:: settings mechanism, so no gridded data is needed. Each test
// describes one wind situation and checks the produced text in Finnish,
// Swedish and English.

#include <regression/tframe.h>

#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "PlainTextFormatter.h"
#include "Story.h"
#include "WindStory.h"
#include <calculator/AnalysisSources.h>
#include <calculator/Settings.h>
#include <calculator/TextGenPosixTime.h>
#include <calculator/WeatherArea.h>
#include <calculator/WeatherPeriod.h>
#include <newbase/NFmiSettings.h>

#include <boost/locale.hpp>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

namespace WindSeaOverviewTest
{
std::shared_ptr<TextGen::Dictionary> dict;
TextGen::PlainTextFormatter formatter;

const string VAR = "sea";
const TextGenPosixTime START(2026, 9, 6, 12, 0);
const TextGenPosixTime END(2026, 9, 7, 12, 0);

string require(const TextGen::Story& theStory, const string& theLanguage, const string& theExpected)
{
  dict->init(theLanguage);
  formatter.dictionary(dict);
  TextGen::Paragraph para = theStory.makeStory("wind_sea_overview");
  const string result = para.realize(formatter);
  if (result != theExpected)
    return (result + " < > " + theExpected);
  return "";
}

#define REQUIRE(story, lang, expected)     \
  result = require(story, lang, expected); \
  if (!result.empty())                     \
    TEST_FAILED(result.c_str());

string stamp(const TextGenPosixTime& t)
{
  ostringstream os;
  os << setfill('0') << setw(4) << t.GetYear() << setw(2) << t.GetMonth() << setw(2) << t.GetDay()
     << setw(2) << t.GetHour() << setw(2) << t.GetMin();
  return os.str();
}

string value(double v, double err = 0.0)
{
  ostringstream os;
  os << fixed << setprecision(2) << v << "," << err;
  return os.str();
}

// One synthetic hour: area mean, spatial percentiles, direction (deg, spread), gust
void set_hour(int hourOffset,
              double mean,
              double lower,
              double upper,
              double direction,
              double spread,
              double gust = -1.0)
{
  TextGenPosixTime t(START);
  t.ChangeByHours(hourOffset);
  const string base = VAR + "::fake::" + stamp(t);
  Settings::set(base + "::speed::mean", value(mean));
  Settings::set(base + "::speed::lower", value(lower));
  Settings::set(base + "::speed::upper", value(upper));
  Settings::set(base + "::direction::mean", value(direction, spread));
  if (gust >= 0.0)
    Settings::set(base + "::gust::maximum", value(gust));
}

// Reset the story settings to the documented defaults
void reset_settings()
{
  Settings::set(VAR + "::speed_change_threshold", "3.0");
  Settings::set(VAR + "::weak_wind_limit", "4.0");
  Settings::set(VAR + "::smoothing_hours", "3");
  Settings::set(VAR + "::max_changes", "3");
  Settings::set(VAR + "::gradual_hours", "8");
  Settings::set(VAR + "::fast_hours", "3");
  Settings::set(VAR + "::fast_change", "5.0");
  Settings::set(VAR + "::range_min_width", "2");
  Settings::set(VAR + "::range_max_width", "5");
  Settings::set(VAR + "::range_hours", "3");
  Settings::set(VAR + "::direction_change_threshold", "45");
  Settings::set(VAR + "::direction_variable_max_speed", "6.0");
  Settings::set(VAR + "::direction_split_min_hours", "6");
  Settings::set(VAR + "::gust_reporting", "false");
  Settings::set(VAR + "::gust_limit", "15");
  Settings::set(VAR + "::turn_phrases", "plain");
}

// WindStory keeps references to these, so they must outlive the story objects
TextGen::AnalysisSources sources;
TextGen::WeatherArea area("25,60");
TextGen::WeatherPeriod period(START, END);

TextGen::WindStory make_story()
{
  return TextGen::WindStory(START, sources, area, period, VAR);
}

// ----------------------------------------------------------------------
// Steady north-westerly wind: one sentence
// ----------------------------------------------------------------------

void steady_wind()
{
  reset_settings();
  for (int h = 0; h <= 24; h++)
    set_hour(h, 7.0, 6.0, 8.0, 315.0, 5.0);

  TextGen::WindStory story = make_story();
  string result;
  REQUIRE(story, "fi", "Luoteistuulta 6-8 m/s.");
  REQUIRE(story, "sv", "Nordvästlig vind 6-8 m/s.");
  REQUIRE(story, "en", "North-westerly wind 6-8 m/s.");
  TEST_PASSED();
}

// ----------------------------------------------------------------------
// Small wiggles below the threshold are not changes
// ----------------------------------------------------------------------

void noisy_steady_wind()
{
  reset_settings();
  for (int h = 0; h <= 24; h++)
  {
    const double noise = ((h % 3) - 1) * 1.0;  // -1, 0, +1
    set_hour(h, 8.0 + noise, 7.0 + noise, 9.0 + noise, 200.0, 8.0);
  }

  TextGen::WindStory story = make_story();
  string result;
  REQUIRE(story, "fi", "Etelätuulta 7-9 m/s.");
  TEST_PASSED();
}

// ----------------------------------------------------------------------
// Wind strengthens from the start of the period, then stays
// ----------------------------------------------------------------------

void strengthening_from_start()
{
  reset_settings();
  for (int h = 0; h <= 24; h++)
  {
    const double mean = min(12.0, 4.0 + h);  // 4 -> 12 by 20:00
    set_hour(h, mean, mean - 1.0, mean + 1.0, 180.0, 5.0);
  }

  TextGen::WindStory story = make_story();
  string result;
  REQUIRE(story, "fi", "Vähitellen voimistuvaa etelätuulta, aluksi 4-6 m/s, illalla 11-13 m/s.");
  REQUIRE(story, "sv", "Sydlig vind som tilltar småningom, först 4-6 m/s, på kvällen 11-13 m/s.");
  REQUIRE(story,
          "en",
          "Gradually strengthening southerly wind, at first 4-6 m/s, in the evening 11-13 m/s.");
  TEST_PASSED();
}

// ----------------------------------------------------------------------
// Steady westerly, then weakening overnight while turning to north
// ----------------------------------------------------------------------

void weakening_with_turn()
{
  reset_settings();
  for (int h = 0; h <= 24; h++)
  {
    double mean = 10.0;
    if (h > 6 && h <= 18)
      mean = 10.0 - (h - 6) * (7.0 / 12.0);  // 10 -> 3 between 18:00 and 06:00
    else if (h > 18)
      mean = 3.0;
    double dir = 270.0;
    if (h >= 12 && h <= 16)
      dir = 270.0 + (h - 12) * 22.5;  // turns to north 00:00 - 04:00
    else if (h > 16)
      dir = 360.0;
    set_hour(h, mean, mean - 1.0, mean + 1.0, dir, 5.0);
  }

  TextGen::WindStory story = make_story();
  string result;
  REQUIRE(story,
          "fi",
          "Länsituulta 9-11 m/s. Illasta alkaen vähitellen heikkenevää tuulta, aamulla "
          "pohjoistuulta 2-4 m/s.");
  REQUIRE(story,
          "en",
          "Westerly wind 9-11 m/s. Gradually weakening wind from the evening, northerly wind in "
          "the morning 2-4 m/s.");

  // With veering/backing phrases the westerly wind turning to north is veering (clockwise)
  Settings::set(VAR + "::turn_phrases", "veering_backing");
  REQUIRE(story,
          "fi",
          "Länsituulta 9-11 m/s. Illasta alkaen vähitellen heikkenevää tuulta, aamulla "
          "myötäpäivään kääntyvää pohjoistuulta 2-4 m/s.");
  REQUIRE(story,
          "en",
          "Westerly wind 9-11 m/s. Gradually weakening wind from the evening, veering northerly "
          "wind in the morning 2-4 m/s.");
  REQUIRE(story,
          "en-marine",
          "West 9-11 m/s. Gradually decreasing from the evening, veering north in the morning "
          "2-4 m/s.");
  TEST_PASSED();
}

// ----------------------------------------------------------------------
// Direction turns without a speed change
// ----------------------------------------------------------------------

void turn_only()
{
  reset_settings();
  for (int h = 0; h <= 24; h++)
  {
    double dir = 180.0;
    if (h >= 10 && h <= 14)
      dir = 180.0 + (h - 10) * 22.5;  // south -> west between 22:00 and 02:00
    else if (h > 14)
      dir = 270.0;
    set_hour(h, 8.0, 7.0, 9.0, dir, 5.0);
  }

  TextGen::WindStory story = make_story();
  string result;
  REQUIRE(story, "fi", "Etelätuulta 7-9 m/s. Keskiyöllä tuuli kääntyy länteen.");
  REQUIRE(story, "sv", "Sydlig vind 7-9 m/s. Vid midnatt vrider vinden mot väst.");
  REQUIRE(story, "en", "Southerly wind 7-9 m/s. The wind turns to the west at midnight.");
  TEST_PASSED();
}

// ----------------------------------------------------------------------
// Veering (clockwise) and backing (counterclockwise) turns
// ----------------------------------------------------------------------

void veering_and_backing()
{
  reset_settings();
  Settings::set(VAR + "::turn_phrases", "veering_backing");

  // south -> west is clockwise: veering
  for (int h = 0; h <= 24; h++)
  {
    double dir = 180.0;
    if (h >= 10 && h <= 14)
      dir = 180.0 + (h - 10) * 22.5;
    else if (h > 14)
      dir = 270.0;
    set_hour(h, 8.0, 7.0, 9.0, dir, 5.0);
  }
  TextGen::WindStory story = make_story();
  string result;
  REQUIRE(story, "fi", "Etelätuulta 7-9 m/s. Keskiyöllä tuuli kääntyy myötäpäivään länteen.");
  REQUIRE(story, "sv", "Sydlig vind 7-9 m/s. Vid midnatt vrider vinden medurs mot väst.");
  REQUIRE(story, "en", "Southerly wind 7-9 m/s. The wind veers to the west at midnight.");
  REQUIRE(story, "en-marine", "South 7-9 m/s. Veering to the west at midnight.");

  // south -> east is counterclockwise: backing
  for (int h = 0; h <= 24; h++)
  {
    double dir = 180.0;
    if (h >= 10 && h <= 14)
      dir = 180.0 - (h - 10) * 22.5;
    else if (h > 14)
      dir = 90.0;
    set_hour(h, 8.0, 7.0, 9.0, dir, 5.0);
  }
  REQUIRE(story, "fi", "Etelätuulta 7-9 m/s. Keskiyöllä tuuli kääntyy vastapäivään itään.");
  REQUIRE(story, "sv", "Sydlig vind 7-9 m/s. Vid midnatt vrider vinden moturs mot ost.");
  REQUIRE(story, "en", "Southerly wind 7-9 m/s. The wind backs to the east at midnight.");
  REQUIRE(story, "en-marine", "South 7-9 m/s. Backing to the east at midnight.");
  TEST_PASSED();
}

// ----------------------------------------------------------------------
// Weak wind with variable direction: no changes are reported
// ----------------------------------------------------------------------

void weak_variable_wind()
{
  reset_settings();
  for (int h = 0; h <= 24; h++)
  {
    const double mean = 2.0 + ((h % 5) * 0.4);  // 2.0 ... 3.6
    set_hour(h, mean, 1.0, 3.0, (h * 47) % 360, 70.0);
  }

  TextGen::WindStory story = make_story();
  string result;
  REQUIRE(story, "fi", "Suunnaltaan vaihtelevaa tuulta 1-3 m/s.");
  REQUIRE(story, "en", "Variable wind 1-3 m/s.");
  TEST_PASSED();
}

// ----------------------------------------------------------------------
// Optional gust sentence
// ----------------------------------------------------------------------

void gust_sentence()
{
  reset_settings();
  Settings::set(VAR + "::gust_reporting", "true");
  for (int h = 0; h <= 24; h++)
    set_hour(h, 7.0, 6.0, 8.0, 315.0, 5.0, (h == 3 ? 18.0 : 9.0));

  TextGen::WindStory story = make_story();
  string result;
  REQUIRE(story,
          "fi",
          "Luoteistuulta 6-8 m/s. Iltapäivällä paikoin voimakkaita puuskia, kovimmillaan 18 m/s.");
  REQUIRE(story,
          "en",
          "North-westerly wind 6-8 m/s. In the afternoon, in some places strong gusts, up to 18 "
          "m/s.");

  // below the limit nothing is said
  set_hour(3, 7.0, 6.0, 8.0, 315.0, 5.0, 14.0);
  REQUIRE(story, "fi", "Luoteistuulta 6-8 m/s.");
  TEST_PASSED();
}

// ----------------------------------------------------------------------
// The number of reported changes is capped
// ----------------------------------------------------------------------

void capped_changes()
{
  reset_settings();
  Settings::set(VAR + "::max_changes", "1");
  for (int h = 0; h <= 24; h++)
  {
    // 5 -> 10 -> 5 -> 10 -> 5, a change every six hours
    const double phase = (h % 12) < 6 ? (h % 12) : (12 - (h % 12));
    const double mean = 5.0 + phase * (5.0 / 6.0);
    set_hour(h, mean, mean - 1.0, mean + 1.0, 225.0, 5.0);
  }

  TextGen::WindStory story = make_story();
  string result;
  // only the largest change is kept; the rest is described as steady
  dict->init("fi");
  formatter.dictionary(dict);
  TextGen::Paragraph para = story.makeStory("wind_sea_overview");
  const string text = para.realize(formatter);
  const size_t sentences = std::count(text.begin(), text.end(), '.');
  if (sentences > 2)
    TEST_FAILED(("too many sentences with max_changes=1: " + text).c_str());
  if (text.find("m/s") == string::npos)
    TEST_FAILED(("no wind speed reported: " + text).c_str());
  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief The actual test driver
 */
// ----------------------------------------------------------------------

class tests : public tframe::tests
{
  //! Overridden message separator
  virtual const char* error_message_prefix() const { return "\n\t"; }
  //! Main test suite
  void test(void)
  {
    TEST(steady_wind);
    TEST(noisy_steady_wind);
    TEST(strengthening_from_start);
    TEST(weakening_with_turn);
    TEST(turn_only);
    TEST(veering_and_backing);
    TEST(weak_variable_wind);
    TEST(gust_sentence);
    TEST(capped_changes);
  }

};  // class tests

}  // namespace WindSeaOverviewTest

int main(void)
{
  boost::locale::generator generator;
  std::locale::global(generator(""));

  NFmiSettings::Init();
  Settings::set(NFmiSettings::ToString());

  using namespace WindSeaOverviewTest;

  cout << endl << "WindStory::sea_overview tests" << endl << "=============================" << endl;

  dict.reset(TextGen::DictionaryFactory::create("po"));
  dict->init("fi");
  dict->init("sv");
  dict->init("en");
  dict->init("en-marine");

  tests t;
  return t.run();
}
