#include "regression/tframe.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "PlainTextFormatter.h"
#include "WindStory.h"
#include "Story.h"

#include "newbase/NFmiSettings.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;


namespace WindStoryTest
{
  shared_ptr<TextGen::Dictionary> dict;
  TextGen::PlainTextFormatter formatter;
  

  string require(const TextGen::Story & theStory,
				 const string & theLanguage,
				 const string & theName,
				 const string & theExpected)
  {
	dict->init(theLanguage);
	formatter.dictionary(dict);

	TextGen::Paragraph para = theStory.makeStory(theName);
	const string result = para.realize(formatter);
	
	if(result != theExpected)
	  return (result + " < > " + theExpected);
	else
	  return "";
  }

#define REQUIRE(story,lang,name,expected) \
result = require(story,lang,name,expected); \
if(!result.empty()) TEST_FAILED(result.c_str());


  // ----------------------------------------------------------------------
  /*!
   * \brief WindStory::daily_ranges
   */
  // ----------------------------------------------------------------------

  void wind_daily_ranges()
  {
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	const WeatherArea area("dummy");

	const string fun = "wind_daily_ranges";

	const NFmiTime time1(2003,6,1,0,0);
	const NFmiTime time2(2003,6,2,0,0);
	const NFmiTime time3(2003,6,3,0,0);
	const NFmiTime time4(2003,6,4,0,0);

	string result;

	NFmiSettings::instance().set("a::day::starthour","6");
	NFmiSettings::instance().set("a::day::endhour","18");

	// Test 1-day forecasts
	{
	  const WeatherPeriod period(time1,time2);
	  WindStory story(time1,sources,area,period,"a");

	  NFmiSettings::instance().set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::instance().set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::instance().set("a::fake::day1::speed::maximum","2,0");

	  NFmiSettings::instance().set("a::today::phrases","none,today,tomorrow,weekday");

	  NFmiSettings::instance().set("a::fake::day1::direction::mean","0,0");
	  REQUIRE(story,"fi",fun,"Pohjoistuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"Nordlig vind 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Northerly wind 0...2 m/s.");

	  NFmiSettings::instance().set("a::fake::day1::direction::mean","0,30");
	  REQUIRE(story,"fi",fun,"Pohjoisen puoleista tuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"Vind omkring nord 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Approximately northerly wind 0...2 m/s.");

	  NFmiSettings::instance().set("a::fake::day1::direction::mean","0,50");
	  REQUIRE(story,"fi",fun,"Suunnaltaan vaihtelevaa tuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"Varierande vind 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Variable wind 0...2 m/s.");

	  NFmiSettings::instance().set("a::today::phrases","today,tomorrow,weekday");

	  NFmiSettings::instance().set("a::fake::day1::direction::mean","0,0");
	  REQUIRE(story,"fi",fun,"T�n��n pohjoistuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"I dag nordlig vind 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Today northerly wind 0...2 m/s.");

	}

	// Test 1-day forecast for tomorrow

	{
	  const WeatherPeriod period(time2,time3);
	  WindStory story(time1,sources,area,period,"a");

	  NFmiSettings::instance().set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::instance().set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::instance().set("a::fake::day1::speed::maximum","2,0");
	  NFmiSettings::instance().set("a::fake::day1::direction::mean","0,0");

	  NFmiSettings::instance().set("a::today::phrases","none,today,tomorrow,weekday");
	  REQUIRE(story,"fi",fun,"Huomenna pohjoistuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"I morgon nordlig vind 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Tomorrow northerly wind 0...2 m/s.");

	  NFmiSettings::instance().set("a::today::phrases","today,tomorrow,weekday");
	  REQUIRE(story,"fi",fun,"Huomenna pohjoistuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"I morgon nordlig vind 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Tomorrow northerly wind 0...2 m/s.");

	}

	// Test 1-day forecast for a later day

	{
	  const WeatherPeriod period(time3,time4);
	  WindStory story(time1,sources,area,period,"a");

	  NFmiSettings::instance().set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::instance().set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::instance().set("a::fake::day1::speed::maximum","2,0");
	  NFmiSettings::instance().set("a::fake::day1::direction::mean","0,0");

	  NFmiSettings::instance().set("a::today::phrases","none,today,tomorrow,weekday");
	  REQUIRE(story,"fi",fun,"Tiistaina pohjoistuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"P� tisdagen nordlig vind 0...2 m/s.");
	  REQUIRE(story,"en",fun,"On Tuesday northerly wind 0...2 m/s.");

	}



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
	virtual const char * const error_message_prefix() const
	{
	  return "\n\t";
	}

	//! Main test suite
	void test(void)
	{
	  TEST(wind_daily_ranges);
	}

  }; // class tests

} // namespace WindStoryTest


int main(void)
{
  using namespace WindStoryTest;

  cout << endl
	   << "WindStory tests" << endl
	   << "===============" << endl;

  dict = TextGen::DictionaryFactory::create("multimysql");

  dict->init("fi");
  dict->init("sv");
  dict->init("en");

  tests t;
  return t.run();
}
