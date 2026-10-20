// Regression tests for WindStory::overview ("wind_overview")
//
// The ::fake:: settings of wind_overview hold one value for every hour, so a
// direction change cannot be faked. These tests run the story on the wind of
// data/skandinavia_pinta.sqd instead. The data has no gust or maximum wind
// parameters, so the wind speed stands in for both.

#include <regression/tframe.h>

#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "PlainTextFormatter.h"
#include "Story.h"
#include "WindStory.h"
#include <calculator/AnalysisSources.h>
#include <calculator/RegularMaskSource.h>
#include <calculator/Settings.h>
#include <calculator/TextGenPosixTime.h>
#include <calculator/UserWeatherSource.h>
#include <calculator/WeatherArea.h>
#include <calculator/WeatherHistory.h>
#include <calculator/WeatherPeriod.h>
#include <newbase/NFmiQueryData.h>
#include <newbase/NFmiSettings.h>

#include <boost/locale.hpp>

#include <iostream>
#include <memory>
#include <string>

using namespace std;

namespace WindOverviewTest
{
std::shared_ptr<TextGen::Dictionary> dict;
TextGen::PlainTextFormatter formatter;

const string VAR = "land";

string require(const TextGen::Story& theStory, const string& theLanguage, const string& theExpected)
{
  Settings::set("textgen::podictionaries", "../po");
  dict->init(theLanguage);
  formatter.dictionary(dict);
  TextGen::Paragraph para = theStory.makeStory("wind_overview");
  const string result = para.realize(formatter);
  if (result != theExpected)
    return (result + " < > " + theExpected);
  return "";
}

#define REQUIRE(story, lang, expected)     \
  result = require(story, lang, expected); \
  if (!result.empty())                     \
    TEST_FAILED(result.c_str());

// WindStory keeps references to these, so they must outlive the story objects
TextGen::AnalysisSources sources;
TextGen::WeatherArea uusimaa("maps/uusimaa.svg:10", "uusimaa");

TextGenPosixTime hours_later(TextGenPosixTime theTime, int theHours)
{
  theTime.ChangeByHours(theHours);
  return theTime;
}

// A 30 hour forecast for Uusimaa starting at the given time, with the time phrase
// history of the area cleared so that each story starts afresh
struct Forecast
{
  TextGenPosixTime start;
  TextGen::WeatherPeriod period;
  TextGen::WindStory story;

  Forecast(int day, int hour)
      : start(2003, 8, day, hour, 0),
        period(start, hours_later(start, 30)),
        story(start, sources, uusimaa, period, VAR)
  {
    uusimaa.history() = TextGen::WeatherHistory();
  }
};

// ----------------------------------------------------------------------
// The default plain phrases state the new direction alone
// ----------------------------------------------------------------------

void plain_turns()
{
  Settings::set(VAR + "::turn_phrases", "plain");
  Forecast forecast(14, 6);
  string result;
  REQUIRE(forecast.story,
          "en",
          "South-easterly wind 4-9 m/s, up to 12 m/s. From the forenoon southerly wind, from "
          "early Friday morning south-easterly wind.");
  TEST_PASSED();
}

// ----------------------------------------------------------------------
// South-east to south is clockwise, south back to south-east counterclockwise
// ----------------------------------------------------------------------

void veering_and_backing()
{
  Settings::set(VAR + "::turn_phrases", "veering_backing");
  Forecast forecast(14, 6);
  string result;
  REQUIRE(forecast.story,
          "fi",
          "Kaakkoistuulta 4-9 m/s, ylimmillään 12 m/s. Aamupäivästä alkaen etelään kääntyvää "
          "tuulta, perjantaiaamuyöstä alkaen kaakkoon kääntyvää tuulta.");
  REQUIRE(forecast.story,
          "sv",
          "Sydostlig vind 4-9 m/s, upp till 12 m/s. Från och med förmiddagen medurs vridande "
          "sydlig vind, från och med fredag efternatt moturs vridande sydostlig vind.");
  REQUIRE(forecast.story,
          "en",
          "South-easterly wind 4-9 m/s, up to 12 m/s. From the forenoon veering southerly wind, "
          "from early Friday morning backing south-easterly wind.");
  REQUIRE(forecast.story,
          "en-marine",
          "Southeast 4-9 m/s, up to 12 m/s. From forenoon veering south, from early Friday "
          "morning backing southeast.");
  TEST_PASSED();
}

// ----------------------------------------------------------------------
// A turn from a variable wind is not qualified, the following one is
// ----------------------------------------------------------------------

void turn_from_variable_wind()
{
  Settings::set(VAR + "::turn_phrases", "veering_backing");
  Forecast forecast(19, 18);
  string result;
  REQUIRE(forecast.story,
          "en",
          "Variable wind 1-5 m/s. On Wednesday afternoon south-westerly wind, from the evening "
          "backing south to south-westerly wind.");
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
    TEST(plain_turns);
    TEST(veering_and_backing);
    TEST(turn_from_variable_wind);
  }

};  // class tests

}  // namespace WindOverviewTest

int main(void)
{
  boost::locale::generator generator;
  std::locale::global(generator(""));

  NFmiSettings::Init();
  Settings::set(NFmiSettings::ToString());

  using namespace WindOverviewTest;

  cout << endl << "WindStory::overview tests" << endl << "=========================" << endl;

  std::shared_ptr<TextGen::UserWeatherSource> weathersource(new TextGen::UserWeatherSource());
  weathersource->insert("data", std::make_shared<NFmiQueryData>("data/skandinavia_pinta.sqd"));
  sources.setWeatherSource(weathersource);
  sources.setMaskSource(std::make_shared<TextGen::RegularMaskSource>());

  Settings::set("textgen::default_forecast", "data");
  Settings::set("textgen::gustspeed", "WindSpeedMS");
  Settings::set("textgen::maximumwind", "WindSpeedMS");

  dict.reset(TextGen::DictionaryFactory::create("po"));
  dict->init("fi");
  dict->init("sv");
  dict->init("en");
  dict->init("en-marine");

  tests t;
  return t.run();
}
