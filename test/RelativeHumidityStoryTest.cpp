#include <regression/tframe.h>
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "PlainTextFormatter.h"
#include "RelativeHumidityStory.h"
#include "Story.h"

#include <newbase/NFmiSettings.h>

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
	WeatherArea area("25,60");

	const string fun = "relativehumidity_lowest";

	string result;

	NFmiSettings::Set("lowest::day::maxstarthour","6");
	NFmiSettings::Set("lowest::day::minendhour","18");
	NFmiSettings::Set("lowest::precision","10");
	NFmiSettings::Set("lowest::comparison::significantly_greater","50");
	NFmiSettings::Set("lowest::comparison::greater","30");
	NFmiSettings::Set("lowest::comparison::somewhat_greater","10");
	NFmiSettings::Set("lowest::comparison::somewhat_smaller","10");
	NFmiSettings::Set("lowest::comparison::smaller","30");
	NFmiSettings::Set("lowest::comparison::significantly_smaller","50");

	// 1-day forecasts
	{
	  NFmiTime time1(2003,6,3,6,0);
	  NFmiTime time2(2003,6,4,6,0);
	  WeatherPeriod period(time1,time2);
	  RelativeHumidityStory story(time1,sources,area,period,"lowest");
	  
	  NFmiSettings::Set("lowest::fake::day1::minimum","10,0");
	  REQUIRE(story,"fi",fun,"Alin suhteellinen kosteus on 10%.");
	  REQUIRE(story,"sv",fun,"Den lägsta relativa fuktigheten är 10%.");
	  REQUIRE(story,"en",fun,"The lowest relative humidity is 10%.");
	  
	}

	// Another 1-day forecast, because 17 < 18 (minendhour)

	{
	  NFmiTime time1(2003,6,3,6,0);
	  NFmiTime time2(2003,6,4,17,0);
	  WeatherPeriod period(time1,time2);
	  RelativeHumidityStory story(time1,sources,area,period,"lowest");
	  
	  NFmiSettings::Set("lowest::fake::day1::minimum","20,0");
	  REQUIRE(story,"fi",fun,"Alin suhteellinen kosteus on 20%.");
	  REQUIRE(story,"sv",fun,"Den lägsta relativa fuktigheten är 20%.");
	  REQUIRE(story,"en",fun,"The lowest relative humidity is 20%.");
	  
	}

	// 2-day forecasts

	{
	  NFmiTime time1(2003,6,3,6,0);
	  NFmiTime time2(2003,6,4,18,0);
	  WeatherPeriod period(time1,time2);
	  RelativeHumidityStory story(time1,sources,area,period,"lowest");
	  
	  NFmiSettings::Set("lowest::fake::day1::minimum","50,0");
	  NFmiSettings::Set("lowest::fake::day2::minimum","50,0");
	  REQUIRE(story,"fi",fun,"Alin suhteellinen kosteus on 50%, huomenna sama.");
	  REQUIRE(story,"sv",fun,"Den lägsta relativa fuktigheten är 50%, i morgon densamma.");
	  REQUIRE(story,"en",fun,"The lowest relative humidity is 50%, tomorrow the same.");

	  NFmiSettings::Set("lowest::fake::day1::minimum","50,0");
	  NFmiSettings::Set("lowest::fake::day2::minimum","0,0");
	  REQUIRE(story,"fi",fun,"Alin suhteellinen kosteus on 50%, huomenna huomattavasti pienempi.");
	  REQUIRE(story,"sv",fun,"Den lägsta relativa fuktigheten är 50%, i morgon betydligt mindre.");
	  REQUIRE(story,"en",fun,"The lowest relative humidity is 50%, tomorrow significantly smaller.");

	  NFmiSettings::Set("lowest::fake::day1::minimum","50,0");
	  NFmiSettings::Set("lowest::fake::day2::minimum","20,0");
	  REQUIRE(story,"fi",fun,"Alin suhteellinen kosteus on 50%, huomenna pienempi.");
	  REQUIRE(story,"sv",fun,"Den lägsta relativa fuktigheten är 50%, i morgon mindre.");
	  REQUIRE(story,"en",fun,"The lowest relative humidity is 50%, tomorrow smaller.");

	  NFmiSettings::Set("lowest::fake::day1::minimum","50,0");
	  NFmiSettings::Set("lowest::fake::day2::minimum","40,0");
	  REQUIRE(story,"fi",fun,"Alin suhteellinen kosteus on 50%, huomenna hieman pienempi.");
	  REQUIRE(story,"sv",fun,"Den lägsta relativa fuktigheten är 50%, i morgon något mindre.");
	  REQUIRE(story,"en",fun,"The lowest relative humidity is 50%, tomorrow somewhat smaller.");

	  NFmiSettings::Set("lowest::fake::day1::minimum","50,0");
	  NFmiSettings::Set("lowest::fake::day2::minimum","60,0");
	  REQUIRE(story,"fi",fun,"Alin suhteellinen kosteus on 50%, huomenna hieman suurempi.");
	  REQUIRE(story,"sv",fun,"Den lägsta relativa fuktigheten är 50%, i morgon något större.");
	  REQUIRE(story,"en",fun,"The lowest relative humidity is 50%, tomorrow somewhat greater.");

	  NFmiSettings::Set("lowest::fake::day1::minimum","50,0");
	  NFmiSettings::Set("lowest::fake::day2::minimum","80,0");
	  REQUIRE(story,"fi",fun,"Alin suhteellinen kosteus on 50%, huomenna suurempi.");
	  REQUIRE(story,"sv",fun,"Den lägsta relativa fuktigheten är 50%, i morgon större.");
	  REQUIRE(story,"en",fun,"The lowest relative humidity is 50%, tomorrow greater.");

	  NFmiSettings::Set("lowest::fake::day1::minimum","50,0");
	  NFmiSettings::Set("lowest::fake::day2::minimum","100,0");
	  REQUIRE(story,"fi",fun,"Alin suhteellinen kosteus on 50%, huomenna huomattavasti suurempi.");
	  REQUIRE(story,"sv",fun,"Den lägsta relativa fuktigheten är 50%, i morgon betydligt större.");
	  REQUIRE(story,"en",fun,"The lowest relative humidity is 50%, tomorrow significantly greater.");
	  
	}

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test RelativeHumidityStory::range()
   */
  // ----------------------------------------------------------------------

  void relativehumidity_range()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("25,60");
	const string fun = "relativehumidity_range";

	NFmiSettings::Set("range::precision","10");

	NFmiTime time1(2003,6,1);
	NFmiTime time2(2003,6,2);
	WeatherPeriod period(time1,time2);
	RelativeHumidityStory story(time1,sources,area,period,"range");
	
	NFmiSettings::Set("range::fake::minimum","5,0");
	NFmiSettings::Set("range::fake::mean","5,0");
	NFmiSettings::Set("range::fake::maximum","5,0");
	require(story,"fi",fun,"Suhteellinen kosteus on noin 10%.");
	require(story,"sv",fun,"Relativ fuktighet är cirka 10%.");
	require(story,"en",fun,"Relative humidity is about 10%.");
	
	NFmiSettings::Set("range::fake::minimum","50,0");
	NFmiSettings::Set("range::fake::mean","60,0");
	NFmiSettings::Set("range::fake::maximum","60,0");
	require(story,"fi",fun,"Suhteellinen kosteus 50-60%.");
	require(story,"sv",fun,"Relativ fuktighet är 50-60%.");
	require(story,"en",fun,"Relative humidity is 50-60%.");

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
	  TEST(relativehumidity_range);
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
