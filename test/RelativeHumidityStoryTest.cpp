#include "regression/tframe.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "PlainTextFormatter.h"
#include "RelativeHumidityStory.h"
#include "Story.h"

#include "newbase/NFmiSettings.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;


namespace RelativeHumidityStoryTest
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
   * \brief Test RelativeHumidityStory::lowest()
   */
  // ----------------------------------------------------------------------

  void relativehumidity_lowest()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("dummy");

	const string fun = "relativehumidity_lowest";

	string result;

	NFmiSettings::instance().set("lowest::day::maxstarthour","6");
	NFmiSettings::instance().set("lowest::day::minendhour","18");
	NFmiSettings::instance().set("lowest::precision","10");
	NFmiSettings::instance().set("lowest::comparison::significantly_greater","50");
	NFmiSettings::instance().set("lowest::comparison::greater","30");
	NFmiSettings::instance().set("lowest::comparison::somewhat_greater","10");
	NFmiSettings::instance().set("lowest::comparison::somewhat_smaller","10");
	NFmiSettings::instance().set("lowest::comparison::smaller","30");
	NFmiSettings::instance().set("lowest::comparison::significantly_smaller","50");

	// 1-day forecasts
	{
	  NFmiTime time1(2003,6,3,6,0);
	  NFmiTime time2(2003,6,4,6,0);
	  WeatherPeriod period(time1,time2);
	  RelativeHumidityStory story(time1,sources,area,period,"lowest");
	  
	  NFmiSettings::instance().set("lowest::fake::day1::minimum","10,1");
	  REQUIRE(story,"fi",fun,"Alin suhteellinen kosteus on 10%.");
	  REQUIRE(story,"sv",fun,"Den lägsta relativa fuktigheten är 10%.");
	  REQUIRE(story,"en",fun,"The smallest relative humidity is 10%.");
	  
	}

	// Another 1-day forecast, because 17 < 18 (minendhour)

	{
	  NFmiTime time1(2003,6,3,6,0);
	  NFmiTime time2(2003,6,4,17,0);
	  WeatherPeriod period(time1,time2);
	  RelativeHumidityStory story(time1,sources,area,period,"lowest");
	  
	  NFmiSettings::instance().set("lowest::fake::day1::minimum","20,1");
	  REQUIRE(story,"fi",fun,"Alin suhteellinen kosteus on 20%.");
	  REQUIRE(story,"sv",fun,"Den lägsta relativa fuktigheten är 20%.");
	  REQUIRE(story,"en",fun,"The smallest relative humidity is 20%.");
	  
	}

	// 2-day forecasts

	{
	  NFmiTime time1(2003,6,3,6,0);
	  NFmiTime time2(2003,6,4,18,0);
	  WeatherPeriod period(time1,time2);
	  RelativeHumidityStory story(time1,sources,area,period,"lowest");
	  
	  NFmiSettings::instance().set("lowest::fake::day1::minimum","50,1");
	  NFmiSettings::instance().set("lowest::fake::day2::minimum","50,1");
	  REQUIRE(story,"fi",fun,"Alin suhteellinen kosteus on 50%, huomenna sama.");
	  REQUIRE(story,"sv",fun,"Den lägsta relativa fuktigheten är 50%, i morgon densamma.");
	  REQUIRE(story,"en",fun,"The smallest relative humidity is 50%, tomorrow the same.");

	  NFmiSettings::instance().set("lowest::fake::day1::minimum","50,1");
	  NFmiSettings::instance().set("lowest::fake::day2::minimum","0,1");
	  REQUIRE(story,"fi",fun,"Alin suhteellinen kosteus on 50%, huomenna huomattavasti pienempi.");
	  REQUIRE(story,"sv",fun,"Den lägsta relativa fuktigheten är 50%, i morgon betydligt mindre.");
	  REQUIRE(story,"en",fun,"The smallest relative humidity is 50%, tomorrow significantly smaller.");

	  NFmiSettings::instance().set("lowest::fake::day1::minimum","50,1");
	  NFmiSettings::instance().set("lowest::fake::day2::minimum","20,1");
	  REQUIRE(story,"fi",fun,"Alin suhteellinen kosteus on 50%, huomenna pienempi.");
	  REQUIRE(story,"sv",fun,"Den lägsta relativa fuktigheten är 50%, i morgon mindre.");
	  REQUIRE(story,"en",fun,"The smallest relative humidity is 50%, tomorrow smaller.");

	  NFmiSettings::instance().set("lowest::fake::day1::minimum","50,1");
	  NFmiSettings::instance().set("lowest::fake::day2::minimum","40,1");
	  REQUIRE(story,"fi",fun,"Alin suhteellinen kosteus on 50%, huomenna hieman pienempi.");
	  REQUIRE(story,"sv",fun,"Den lägsta relativa fuktigheten är 50%, i morgon något mindre.");
	  REQUIRE(story,"en",fun,"The smallest relative humidity is 50%, tomorrow somewhat smaller.");

	  NFmiSettings::instance().set("lowest::fake::day1::minimum","50,1");
	  NFmiSettings::instance().set("lowest::fake::day2::minimum","60,1");
	  REQUIRE(story,"fi",fun,"Alin suhteellinen kosteus on 50%, huomenna hieman suurempi.");
	  REQUIRE(story,"sv",fun,"Den lägsta relativa fuktigheten är 50%, i morgon något större.");
	  REQUIRE(story,"en",fun,"The smallest relative humidity is 50%, tomorrow somewhat greater.");

	  NFmiSettings::instance().set("lowest::fake::day1::minimum","50,1");
	  NFmiSettings::instance().set("lowest::fake::day2::minimum","80,1");
	  REQUIRE(story,"fi",fun,"Alin suhteellinen kosteus on 50%, huomenna suurempi.");
	  REQUIRE(story,"sv",fun,"Den lägsta relativa fuktigheten är 50%, i morgon större.");
	  REQUIRE(story,"en",fun,"The smallest relative humidity is 50%, tomorrow greater.");

	  NFmiSettings::instance().set("lowest::fake::day1::minimum","50,1");
	  NFmiSettings::instance().set("lowest::fake::day2::minimum","100,1");
	  REQUIRE(story,"fi",fun,"Alin suhteellinen kosteus on 50%, huomenna huomattavasti suurempi.");
	  REQUIRE(story,"sv",fun,"Den lägsta relativa fuktigheten är 50%, i morgon betydligt större.");
	  REQUIRE(story,"en",fun,"The smallest relative humidity is 50%, tomorrow significantly greater.");
	  
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
	  TEST(relativehumidity_lowest);
	}

  }; // class tests

} // namespace RelativeHumidityStoryTest


int main(void)
{
  using namespace RelativeHumidityStoryTest;

  cout << endl
	   << "RelativeHumidityStory tests" << endl
	   << "===========================" << endl;

  dict = TextGen::DictionaryFactory::create("multimysql");

  dict->init("fi");
  dict->init("sv");
  dict->init("en");

  tests t;
  return t.run();
}
