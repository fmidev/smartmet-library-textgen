#include <regression/tframe.h>
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "PlainTextFormatter.h"
#include "CloudinessStory.h"
#include "Story.h"

#include <newbase/NFmiSettings.h>

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;


namespace CloudinessStoryTest
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
   * \brief Test CloudinessStory::overview()
   */
  // ----------------------------------------------------------------------

  void overview()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("25,60");

	const string fun = "cloudiness_overview";

	string result;

	NFmiSettings::Set("cloudiness_overview::day::starthour","0");
	NFmiSettings::Set("cloudiness_overview::day::endhour","0");

	// 1-day forecasts for today
	{
	  NFmiTime time1(2003,6,3,0,0);
	  NFmiTime time2(2003,6,4,0,0);
	  WeatherPeriod period(time1,time2);
	  CloudinessStory story(time1,sources,area,period,"cloudiness_overview");
	  
	  NFmiSettings::Set("cloudiness_overview::fake::day1::cloudy","90,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::clear","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::trend","0,0");
	  REQUIRE(story,"fi",fun,"Pilvist‰.");
	  REQUIRE(story,"sv",fun,"Mulet.");
	  REQUIRE(story,"en",fun,"Cloudy.");

	  NFmiSettings::Set("cloudiness_overview::fake::day1::cloudy","70,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::clear","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::trend","0,0");
	  REQUIRE(story,"fi",fun,"Pilvist‰ tai puolipilvist‰.");
	  REQUIRE(story,"sv",fun,"Mulet eller halvmulet.");
	  REQUIRE(story,"en",fun,"Cloudy or partly cloudy.");

	  NFmiSettings::Set("cloudiness_overview::fake::day1::cloudy","70,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::clear","30,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::trend","0,0");
	  REQUIRE(story,"fi",fun,"Vaihtelevaa pilvisyytt‰.");
	  REQUIRE(story,"sv",fun,"V‰xlande molnighet.");
	  REQUIRE(story,"en",fun,"Variable cloudiness.");

	  NFmiSettings::Set("cloudiness_overview::fake::day1::cloudy","10,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::clear","10,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::trend","0,0");
	  REQUIRE(story,"fi",fun,"Enimm‰kseen puolipilvist‰.");
	  REQUIRE(story,"sv",fun,"Mestadels halvmulet.");
	  REQUIRE(story,"en",fun,"Mostly partly cloudy.");

	  NFmiSettings::Set("cloudiness_overview::fake::day1::cloudy","10,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::clear","80,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::trend","0,0");
	  REQUIRE(story,"fi",fun,"Enimm‰kseen selke‰‰.");
	  REQUIRE(story,"sv",fun,"Mestadels klart.");
	  REQUIRE(story,"en",fun,"Mostly sunny.");

	  NFmiSettings::Set("cloudiness_overview::fake::day1::cloudy","30,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::clear","30,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::trend","90,0");
	  REQUIRE(story,"fi",fun,"Pilvistyv‰‰.");
	  REQUIRE(story,"sv",fun,"÷kad molnighet.");
	  REQUIRE(story,"en",fun,"Increasing cloudiness.");
	  
	}

	// 2-day forecasts starting today
	{
	  NFmiTime time1(2003,6,3,0,0);
	  NFmiTime time2(2003,6,4,0,0);
	  NFmiTime time3(2003,6,5,0,0);
	  WeatherPeriod period(time1,time3);
	  CloudinessStory story(time1,sources,area,period,"cloudiness_overview");
	  
	  NFmiSettings::Set("cloudiness_overview::fake::day1::cloudy","90,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::clear","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::trend","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::cloudy","90,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::clear","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::trend","0,0");
	  REQUIRE(story,"fi",fun,"Pilvist‰.");
	  REQUIRE(story,"sv",fun,"Mulet.");
	  REQUIRE(story,"en",fun,"Cloudy.");

	  NFmiSettings::Set("cloudiness_overview::fake::day1::cloudy","90,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::clear","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::trend","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::cloudy","10,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::clear","10,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::trend","0,0");
	  REQUIRE(story,"fi",fun,"Pilvist‰ tai puolipilvist‰.");
	  REQUIRE(story,"sv",fun,"Mulet eller halvmulet.");
	  REQUIRE(story,"en",fun,"Cloudy or partly cloudy.");

	  NFmiSettings::Set("cloudiness_overview::fake::day1::cloudy","90,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::clear","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::trend","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::cloudy","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::clear","80,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::trend","0,0");
	  REQUIRE(story,"fi",fun,"Pilvist‰, huomenna enimm‰kseen selke‰‰.");
	  REQUIRE(story,"sv",fun,"Mulet, i morgon mestadels klart.");
	  REQUIRE(story,"en",fun,"Cloudy, tomorrow mostly sunny.");

	  NFmiSettings::Set("cloudiness_overview::fake::day1::cloudy","30,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::clear","30,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::trend","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::cloudy","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::clear","90,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::trend","0,0");
	  REQUIRE(story,"fi",fun,"Vaihtelevaa pilvisyytt‰, huomenna selke‰‰.");
	  REQUIRE(story,"sv",fun,"V‰xlande molnighet, i morgon klart.");
	  REQUIRE(story,"en",fun,"Variable cloudiness, tomorrow sunny.");

	  NFmiSettings::Set("cloudiness_overview::fake::day1::cloudy","30,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::clear","30,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::trend","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::cloudy","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::clear","80,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::trend","0,0");
	  REQUIRE(story,"fi",fun,"Vaihtelevaa pilvisyytt‰, huomenna enimm‰kseen selke‰‰.");
	  REQUIRE(story,"sv",fun,"V‰xlande molnighet, i morgon mestadels klart.");
	  REQUIRE(story,"en",fun,"Variable cloudiness, tomorrow mostly sunny.");

	}

	// 3-day forecasts starting today
	{
	  NFmiTime time1(2003,6,3,0,0);
	  NFmiTime time2(2003,6,4,0,0);
	  NFmiTime time3(2003,6,5,0,0);
	  NFmiTime time4(2003,6,6,0,0);
	  WeatherPeriod period(time1,time4);
	  CloudinessStory story(time1,sources,area,period,"cloudiness_overview");
	  
	  NFmiSettings::Set("cloudiness_overview::fake::day1::cloudy","90,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::clear","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::trend","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::cloudy","90,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::clear","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::trend","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day3::cloudy","90,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day3::clear","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day3::trend","0,0");
	  REQUIRE(story,"fi",fun,"Pilvist‰.");
	  REQUIRE(story,"sv",fun,"Mulet.");
	  REQUIRE(story,"en",fun,"Cloudy.");

	  NFmiSettings::Set("cloudiness_overview::fake::day3::cloudy","10,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day3::clear","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day3::trend","0,0");
	  REQUIRE(story,"fi",fun,"Pilvist‰ tai puolipilvist‰.");
	  REQUIRE(story,"sv",fun,"Mulet eller halvmulet.");
	  REQUIRE(story,"en",fun,"Cloudy or partly cloudy.");

	  NFmiSettings::Set("cloudiness_overview::fake::day3::cloudy","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day3::clear","90,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day3::trend","0,0");
	  REQUIRE(story,"fi",fun,"Pilvist‰, torstaina selke‰‰.");
	  REQUIRE(story,"sv",fun,"Mulet, pÂ torsdagen klart.");
	  REQUIRE(story,"en",fun,"Cloudy, on Thursday sunny.");

	  NFmiSettings::Set("cloudiness_overview::fake::day1::cloudy","90,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::clear","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::trend","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::cloudy","20,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::clear","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::trend","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day3::cloudy","10,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day3::clear","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day3::trend","0,0");
	  REQUIRE(story,"fi",fun,"Pilvist‰ tai puolipilvist‰.");
	  REQUIRE(story,"sv",fun,"Mulet eller halvmulet.");
	  REQUIRE(story,"en",fun,"Cloudy or partly cloudy.");

	  NFmiSettings::Set("cloudiness_overview::fake::day1::cloudy","90,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::clear","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day1::trend","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::cloudy","10,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::clear","80,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day2::trend","0,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day3::cloudy","10,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day3::clear","90,0");
	  NFmiSettings::Set("cloudiness_overview::fake::day3::trend","0,0");
	  REQUIRE(story,"fi",fun,"Pilvist‰, huomenna ja seuraavana p‰iv‰n‰ enimm‰kseen selke‰‰.");
	  REQUIRE(story,"sv",fun,"Mulet, i morgon och fˆljande dag mestadels klart.");
	  REQUIRE(story,"en",fun,"Cloudy, tomorrow and the following day mostly sunny.");


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
	  TEST(overview);
	}

  }; // class tests

} // namespace CloudinessStoryTest


int main(void)
{
  using namespace CloudinessStoryTest;

  cout << endl
	   << "CloudinessStory tests" << endl
	   << "=====================" << endl;

  dict.reset(TextGen::DictionaryFactory::create("multimysql"));

  dict->init("fi");
  dict->init("sv");
  dict->init("en");

  tests t;
  return t.run();
}
