#include "regression/tframe.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "PlainTextFormatter.h"
#include "WindStory.h"
#include "Story.h"

#include "NFmiSettings.h"

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

	NFmiSettings::Set("a::day::starthour","6");
	NFmiSettings::Set("a::day::endhour","18");

	// Test 1-day forecasts
	{
	  const WeatherPeriod period(time1,time2);
	  WindStory story(time1,sources,area,period,"a");

	  NFmiSettings::Set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day1::speed::maximum","2,0");

	  NFmiSettings::Set("a::today::phrases","none,today,tomorrow,weekday");

	  NFmiSettings::Set("a::fake::day1::direction::mean","0,0");
	  REQUIRE(story,"fi",fun,"Pohjoistuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"Nordlig vind 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Northerly wind 0...2 m/s.");

	  NFmiSettings::Set("a::fake::day1::direction::mean","0,30");
	  REQUIRE(story,"fi",fun,"Pohjoisen puoleista tuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"Vind omkring nord 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Approximately northerly wind 0...2 m/s.");

	  NFmiSettings::Set("a::fake::day1::direction::mean","0,50");
	  REQUIRE(story,"fi",fun,"Suunnaltaan vaihtelevaa tuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"Varierande vind 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Variable wind 0...2 m/s.");

	  NFmiSettings::Set("a::today::phrases","today,tomorrow,weekday");

	  NFmiSettings::Set("a::fake::day1::direction::mean","0,0");
	  REQUIRE(story,"fi",fun,"Tänään pohjoistuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"I dag nordlig vind 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Today northerly wind 0...2 m/s.");

	}

	// Test 1-day forecast for tomorrow

	{
	  const WeatherPeriod period(time2,time3);
	  WindStory story(time1,sources,area,period,"a");

	  NFmiSettings::Set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day1::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day1::direction::mean","0,0");

	  NFmiSettings::Set("a::today::phrases","none,today,tomorrow,weekday");
	  REQUIRE(story,"fi",fun,"Huomenna pohjoistuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"I morgon nordlig vind 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Tomorrow northerly wind 0...2 m/s.");

	  NFmiSettings::Set("a::today::phrases","today,tomorrow,weekday");
	  REQUIRE(story,"fi",fun,"Huomenna pohjoistuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"I morgon nordlig vind 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Tomorrow northerly wind 0...2 m/s.");

	}

	// Test 1-day forecast for a later day

	{
	  const WeatherPeriod period(time3,time4);
	  WindStory story(time1,sources,area,period,"a");

	  NFmiSettings::Set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day1::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day1::direction::mean","0,0");

	  NFmiSettings::Set("a::today::phrases","none,today,tomorrow,weekday");
	  REQUIRE(story,"fi",fun,"Tiistaina pohjoistuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"På tisdagen nordlig vind 0...2 m/s.");
	  REQUIRE(story,"en",fun,"On Tuesday northerly wind 0...2 m/s.");

	}

	// Test 2-day forecast starting today

	{
	  const WeatherPeriod period(time1,time3);
	  WindStory story(time1,sources,area,period,"a");

	  NFmiSettings::Set("a::today::phrases","none,today,tomorrow,weekday");
	  NFmiSettings::Set("a::next_day::phrases","tomorrow,following_day,weekday");
	  NFmiSettings::Set("a::days::phrases","none,today,tomorrow,followingday,weekday");

	  NFmiSettings::Set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day1::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day2::speed::mean","3,0");
	  NFmiSettings::Set("a::fake::day2::speed::minimum","4,0");
	  NFmiSettings::Set("a::fake::day2::speed::maximum","5,0");

	  NFmiSettings::Set("a::fake::day1::direction::mean","0,0");
	  NFmiSettings::Set("a::fake::day2::direction::mean","90,0");
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,50");
	  REQUIRE(story,"fi",fun,"Pohjoistuulta 0...2 m/s, huomenna itätuulta 4...5 m/s.");
	  REQUIRE(story,"sv",fun,"Nordlig vind 0...2 m/s, i morgon ostlig vind 4...5 m/s.");
	  REQUIRE(story,"en",fun,"Northerly wind 0...2 m/s, tomorrow easterly wind 4...5 m/s.");

	  NFmiSettings::Set("a::fake::day1::direction::mean","0,0");
	  NFmiSettings::Set("a::fake::day2::direction::mean","90,0");
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,30");
	  REQUIRE(story,"fi",fun,"Koillisen puoleista tuulta 0...2 m/s, huomenna 4...5 m/s.");
	  REQUIRE(story,"sv",fun,"Vind omkring nordost 0...2 m/s, i morgon 4...5 m/s.");
	  REQUIRE(story,"en",fun,"Approximately northeasterly wind 0...2 m/s, tomorrow 4...5 m/s.");

	  NFmiSettings::Set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day1::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day2::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day2::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day2::speed::maximum","2,0");

	  NFmiSettings::Set("a::fake::day1::direction::mean","0,0");
	  NFmiSettings::Set("a::fake::day2::direction::mean","90,0");
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,50");
	  REQUIRE(story,"fi",fun,"Pohjoistuulta 0...2 m/s, huomenna itätuulta.");
	  REQUIRE(story,"sv",fun,"Nordlig vind 0...2 m/s, i morgon ostlig vind.");
	  REQUIRE(story,"en",fun,"Northerly wind 0...2 m/s, tomorrow easterly wind.");

	  NFmiSettings::Set("a::days::phrases","none,today,tomorrow,followingday,weekday");
	  NFmiSettings::Set("a::fake::day1::direction::mean","0,0");
	  NFmiSettings::Set("a::fake::day2::direction::mean","90,0");
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,30");
	  REQUIRE(story,"fi",fun,"Koillisen puoleista tuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"Vind omkring nordost 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Approximately northeasterly wind 0...2 m/s.");

	  NFmiSettings::Set("a::days::phrases","today,tomorrow,followingday,weekday,none");
	  REQUIRE(story,"fi",fun,"Tänään ja huomenna koillisen puoleista tuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"I dag och i morgon vind omkring nordost 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Today and tomorrow approximately northeasterly wind 0...2 m/s.");

	  NFmiSettings::Set("a::days::phrases","today,weekday,tomorrow,followingday,none");
	  REQUIRE(story,"fi",fun,"Tänään ja sunnuntaina koillisen puoleista tuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"I dag och på söndagen vind omkring nordost 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Today and on Sunday approximately northeasterly wind 0...2 m/s.");

	}

	// Test 2-day forecast starting tomorrow

	{
	  const WeatherPeriod period(time2,time4);
	  WindStory story(time1,sources,area,period,"a");

	  NFmiSettings::Set("a::today::phrases","none,today,tomorrow,weekday");
	  NFmiSettings::Set("a::next_day::phrases","tomorrow,following_day,weekday");
	  NFmiSettings::Set("a::days::phrases","none,today,tomorrow,followingday,weekday");

	  NFmiSettings::Set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day1::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day2::speed::mean","3,0");
	  NFmiSettings::Set("a::fake::day2::speed::minimum","4,0");
	  NFmiSettings::Set("a::fake::day2::speed::maximum","5,0");

	  NFmiSettings::Set("a::fake::day1::direction::mean","0,0");
	  NFmiSettings::Set("a::fake::day2::direction::mean","90,0");
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,50");
	  REQUIRE(story,"fi",fun,"Huomenna pohjoistuulta 0...2 m/s, tiistaina itätuulta 4...5 m/s.");
	  REQUIRE(story,"sv",fun,"I morgon nordlig vind 0...2 m/s, på tisdagen ostlig vind 4...5 m/s.");
	  REQUIRE(story,"en",fun,"Tomorrow northerly wind 0...2 m/s, on Tuesday easterly wind 4...5 m/s.");

	  NFmiSettings::Set("a::fake::day1::direction::mean","0,0");
	  NFmiSettings::Set("a::fake::day2::direction::mean","90,0");
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,30");
	  REQUIRE(story,"fi",fun,"Huomenna koillisen puoleista tuulta 0...2 m/s, tiistaina 4...5 m/s.");
	  REQUIRE(story,"sv",fun,"I morgon vind omkring nordost 0...2 m/s, på tisdagen 4...5 m/s.");
	  REQUIRE(story,"en",fun,"Tomorrow approximately northeasterly wind 0...2 m/s, on Tuesday 4...5 m/s.");

	  NFmiSettings::Set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day1::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day2::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day2::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day2::speed::maximum","2,0");

	  NFmiSettings::Set("a::fake::day1::direction::mean","0,0");
	  NFmiSettings::Set("a::fake::day2::direction::mean","90,0");
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,50");
	  REQUIRE(story,"fi",fun,"Huomenna pohjoistuulta 0...2 m/s, tiistaina itätuulta.");
	  REQUIRE(story,"sv",fun,"I morgon nordlig vind 0...2 m/s, på tisdagen ostlig vind.");
	  REQUIRE(story,"en",fun,"Tomorrow northerly wind 0...2 m/s, on Tuesday easterly wind.");

	  NFmiSettings::Set("a::days::phrases","none,today,tomorrow,followingday,weekday");
	  NFmiSettings::Set("a::fake::day1::direction::mean","0,0");
	  NFmiSettings::Set("a::fake::day2::direction::mean","90,0");
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,30");
	  REQUIRE(story,"fi",fun,"Huomenna ja seuraavana päivänä koillisen puoleista tuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"I morgon och följande dag vind omkring nordost 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Tomorrow and the following day approximately northeasterly wind 0...2 m/s.");

	  NFmiSettings::Set("a::days::phrases","today,tomorrow,followingday,weekday,none");
	  REQUIRE(story,"fi",fun,"Huomenna ja seuraavana päivänä koillisen puoleista tuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"I morgon och följande dag vind omkring nordost 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Tomorrow and the following day approximately northeasterly wind 0...2 m/s.");

	  NFmiSettings::Set("a::days::phrases","weekday");
	  REQUIRE(story,"fi",fun,"Maanantaina ja tiistaina koillisen puoleista tuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"På måndagen och på tisdagen vind omkring nordost 0...2 m/s.");
	  REQUIRE(story,"en",fun,"On Monday and on Tuesday approximately northeasterly wind 0...2 m/s.");

	}


	// Test 3-day forecast starting today

	{
	  const WeatherPeriod period(time1,time4);
	  WindStory story(time1,sources,area,period,"a");

	  NFmiSettings::Set("a::today::phrases","none,today,tomorrow,weekday");
	  NFmiSettings::Set("a::next_day::phrases","tomorrow,following_day,weekday");
	  NFmiSettings::Set("a::days::phrases","none,today,tomorrow,followingday,weekday");

	  // W1<>W2<>W3

	  NFmiSettings::Set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day1::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day2::speed::minimum","3,0");
	  NFmiSettings::Set("a::fake::day2::speed::mean","4,0");
	  NFmiSettings::Set("a::fake::day2::speed::maximum","5,0");
	  NFmiSettings::Set("a::fake::day3::speed::minimum","6,0");
	  NFmiSettings::Set("a::fake::day3::speed::mean","7,0");
	  NFmiSettings::Set("a::fake::day3::speed::maximum","8,0");

	  // D1<>D2<>D3
	  NFmiSettings::Set("a::fake::day1::direction::mean","0,0");
	  NFmiSettings::Set("a::fake::day2::direction::mean","90,0");
	  NFmiSettings::Set("a::fake::day3::direction::mean","180,0");
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,50");
	  NFmiSettings::Set("a::fake::days2-3::direction::mean","135,50");
	  NFmiSettings::Set("a::fake::days1-3::direction::mean","90,90");
	  REQUIRE(story,"fi",fun,"Pohjoistuulta 0...2 m/s, huomenna itätuulta 3...5 m/s, tiistaina etelätuulta 6...8 m/s.");
	  REQUIRE(story,"sv",fun,"Nordlig vind 0...2 m/s, i morgon ostlig vind 3...5 m/s, på tisdagen sydlig vind 6...8 m/s.");
	  REQUIRE(story,"en",fun,"Northerly wind 0...2 m/s, tomorrow easterly wind 3...5 m/s, on Tuesday southerly wind 6...8 m/s.");

	  // D1==D2<>D3
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,30");
	  NFmiSettings::Set("a::fake::days2-3::direction::mean","135,50");
	  NFmiSettings::Set("a::fake::days1-3::direction::mean","90,90");
	  REQUIRE(story,"fi",fun,"Koillisen puoleista tuulta 0...2 m/s, huomenna 3...5 m/s, tiistaina etelätuulta 6...8 m/s.");
	  REQUIRE(story,"sv",fun,"Vind omkring nordost 0...2 m/s, i morgon 3...5 m/s, på tisdagen sydlig vind 6...8 m/s.");
	  REQUIRE(story,"en",fun,"Approximately northeasterly wind 0...2 m/s, tomorrow 3...5 m/s, on Tuesday southerly wind 6...8 m/s.");

	  // D1<>D2==D3
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,50");
	  NFmiSettings::Set("a::fake::days2-3::direction::mean","135,30");
	  NFmiSettings::Set("a::fake::days1-3::direction::mean","90,90");
	  REQUIRE(story,"fi",fun,"Pohjoistuulta 0...2 m/s, huomenna kaakon puoleista tuulta 3...5 m/s, tiistaina 6...8 m/s.");
	  REQUIRE(story,"sv",fun,"Nordlig vind 0...2 m/s, i morgon vind omkring sydost 3...5 m/s, på tisdagen 6...8 m/s.");
	  REQUIRE(story,"en",fun,"Northerly wind 0...2 m/s, tomorrow approximately southeasterly wind 3...5 m/s, on Tuesday 6...8 m/s.");

	  // D1==D2==D3
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,50");
	  NFmiSettings::Set("a::fake::days2-3::direction::mean","135,50");
	  NFmiSettings::Set("a::fake::days1-3::direction::mean","90,30");
	  REQUIRE(story,"fi",fun,"Idän puoleista tuulta 0...2 m/s, huomenna 3...5 m/s, tiistaina 6...8 m/s.");
	  REQUIRE(story,"sv",fun,"Vind omkring ost 0...2 m/s, i morgon 3...5 m/s, på tisdagen 6...8 m/s.");
	  REQUIRE(story,"en",fun,"Approximately easterly wind 0...2 m/s, tomorrow 3...5 m/s, on Tuesday 6...8 m/s.");

	  // W1==W2<>W3

	  NFmiSettings::Set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day1::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day2::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day2::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day2::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day3::speed::minimum","6,0");
	  NFmiSettings::Set("a::fake::day3::speed::mean","7,0");
	  NFmiSettings::Set("a::fake::day3::speed::maximum","8,0");

	  // D1<>D2<>D3
	  NFmiSettings::Set("a::fake::day1::direction::mean","0,0");
	  NFmiSettings::Set("a::fake::day2::direction::mean","90,0");
	  NFmiSettings::Set("a::fake::day3::direction::mean","180,0");
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,50");
	  NFmiSettings::Set("a::fake::days2-3::direction::mean","135,50");
	  NFmiSettings::Set("a::fake::days1-3::direction::mean","90,90");
	  REQUIRE(story,"fi",fun,"Pohjoistuulta 0...2 m/s, huomenna itätuulta, tiistaina etelätuulta 6...8 m/s.");
	  REQUIRE(story,"sv",fun,"Nordlig vind 0...2 m/s, i morgon ostlig vind, på tisdagen sydlig vind 6...8 m/s.");
	  REQUIRE(story,"en",fun,"Northerly wind 0...2 m/s, tomorrow easterly wind, on Tuesday southerly wind 6...8 m/s.");

	  // D1==D2<>D3
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,30");
	  NFmiSettings::Set("a::fake::days2-3::direction::mean","135,50");
	  NFmiSettings::Set("a::fake::days1-3::direction::mean","90,90");
	  REQUIRE(story,"fi",fun,"Koillisen puoleista tuulta 0...2 m/s, tiistaina etelätuulta 6...8 m/s.");
	  REQUIRE(story,"sv",fun,"Vind omkring nordost 0...2 m/s, på tisdagen sydlig vind 6...8 m/s.");
	  REQUIRE(story,"en",fun,"Approximately northeasterly wind 0...2 m/s, on Tuesday southerly wind 6...8 m/s.");

	  // D1<>D2==D3
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,50");
	  NFmiSettings::Set("a::fake::days2-3::direction::mean","135,30");
	  NFmiSettings::Set("a::fake::days1-3::direction::mean","90,90");
	  REQUIRE(story,"fi",fun,"Pohjoistuulta 0...2 m/s, huomenna kaakon puoleista tuulta, tiistaina 6...8 m/s.");
	  REQUIRE(story,"sv",fun,"Nordlig vind 0...2 m/s, i morgon vind omkring sydost, på tisdagen 6...8 m/s.");
	  REQUIRE(story,"en",fun,"Northerly wind 0...2 m/s, tomorrow approximately southeasterly wind, on Tuesday 6...8 m/s.");

	  // W1<>W2==W3

	  NFmiSettings::Set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day1::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day2::speed::minimum","3,0");
	  NFmiSettings::Set("a::fake::day2::speed::mean","4,0");
	  NFmiSettings::Set("a::fake::day2::speed::maximum","5,0");
	  NFmiSettings::Set("a::fake::day3::speed::minimum","3,0");
	  NFmiSettings::Set("a::fake::day3::speed::mean","4,0");
	  NFmiSettings::Set("a::fake::day3::speed::maximum","5,0");

	  // D1<>D2<>D3
	  NFmiSettings::Set("a::fake::day1::direction::mean","0,0");
	  NFmiSettings::Set("a::fake::day2::direction::mean","90,0");
	  NFmiSettings::Set("a::fake::day3::direction::mean","180,0");
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,50");
	  NFmiSettings::Set("a::fake::days2-3::direction::mean","135,50");
	  NFmiSettings::Set("a::fake::days1-3::direction::mean","90,90");
	  REQUIRE(story,"fi",fun,"Pohjoistuulta 0...2 m/s, huomenna itätuulta 3...5 m/s, tiistaina etelätuulta.");
	  REQUIRE(story,"sv",fun,"Nordlig vind 0...2 m/s, i morgon ostlig vind 3...5 m/s, på tisdagen sydlig vind.");
	  REQUIRE(story,"en",fun,"Northerly wind 0...2 m/s, tomorrow easterly wind 3...5 m/s, on Tuesday southerly wind.");

	  // D1==D2<>D3
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,30");
	  NFmiSettings::Set("a::fake::days2-3::direction::mean","135,50");
	  NFmiSettings::Set("a::fake::days1-3::direction::mean","90,90");
	  REQUIRE(story,"fi",fun,"Koillisen puoleista tuulta 0...2 m/s, huomenna 3...5 m/s, tiistaina etelätuulta.");
	  REQUIRE(story,"sv",fun,"Vind omkring nordost 0...2 m/s, i morgon 3...5 m/s, på tisdagen sydlig vind.");
	  REQUIRE(story,"en",fun,"Approximately northeasterly wind 0...2 m/s, tomorrow 3...5 m/s, on Tuesday southerly wind.");

	  // D1<>D2==D3
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,50");
	  NFmiSettings::Set("a::fake::days2-3::direction::mean","135,30");
	  NFmiSettings::Set("a::fake::days1-3::direction::mean","90,90");
	  REQUIRE(story,"fi",fun,"Pohjoistuulta 0...2 m/s, maanantaista alkaen kaakon puoleista tuulta 3...5 m/s.");
	  REQUIRE(story,"sv",fun,"Nordlig vind 0...2 m/s, från måndagen vind omkring sydost 3...5 m/s.");
	  REQUIRE(story,"en",fun,"Northerly wind 0...2 m/s, from Monday onwards approximately southeasterly wind 3...5 m/s.");

	  // D1==D2==D3
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,50");
	  NFmiSettings::Set("a::fake::days2-3::direction::mean","135,50");
	  NFmiSettings::Set("a::fake::days1-3::direction::mean","90,30");
	  REQUIRE(story,"fi",fun,"Idän puoleista tuulta 0...2 m/s, maanantaista alkaen 3...5 m/s.");
	  REQUIRE(story,"sv",fun,"Vind omkring ost 0...2 m/s, från måndagen 3...5 m/s.");
	  REQUIRE(story,"en",fun,"Approximately easterly wind 0...2 m/s, from Monday onwards 3...5 m/s.");

	  // W1==W2==W3

	  NFmiSettings::Set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day1::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day2::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day2::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day2::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day3::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day3::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day3::speed::maximum","2,0");

	  // D1<>D2<>D3
	  NFmiSettings::Set("a::fake::day1::direction::mean","0,0");
	  NFmiSettings::Set("a::fake::day2::direction::mean","90,0");
	  NFmiSettings::Set("a::fake::day3::direction::mean","180,0");
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,50");
	  NFmiSettings::Set("a::fake::days2-3::direction::mean","135,50");
	  NFmiSettings::Set("a::fake::days1-3::direction::mean","90,90");
	  REQUIRE(story,"fi",fun,"Pohjoistuulta 0...2 m/s, huomenna itätuulta, tiistaina etelätuulta.");
	  REQUIRE(story,"sv",fun,"Nordlig vind 0...2 m/s, i morgon ostlig vind, på tisdagen sydlig vind.");
	  REQUIRE(story,"en",fun,"Northerly wind 0...2 m/s, tomorrow easterly wind, on Tuesday southerly wind.");

	  // D1==D2<>D3
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,30");
	  NFmiSettings::Set("a::fake::days2-3::direction::mean","135,50");
	  NFmiSettings::Set("a::fake::days1-3::direction::mean","90,90");
	  REQUIRE(story,"fi",fun,"Koillisen puoleista tuulta 0...2 m/s, tiistaina etelätuulta.");
	  REQUIRE(story,"sv",fun,"Vind omkring nordost 0...2 m/s, på tisdagen sydlig vind.");
	  REQUIRE(story,"en",fun,"Approximately northeasterly wind 0...2 m/s, on Tuesday southerly wind.");

	  // D1<>D2==D3
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,50");
	  NFmiSettings::Set("a::fake::days2-3::direction::mean","135,30");
	  NFmiSettings::Set("a::fake::days1-3::direction::mean","90,90");
	  REQUIRE(story,"fi",fun,"Pohjoistuulta 0...2 m/s, maanantaista alkaen kaakon puoleista tuulta.");
	  REQUIRE(story,"sv",fun,"Nordlig vind 0...2 m/s, från måndagen vind omkring sydost.");
	  REQUIRE(story,"en",fun,"Northerly wind 0...2 m/s, from Monday onwards approximately southeasterly wind.");

	  // D1==D2==D3
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,50");
	  NFmiSettings::Set("a::fake::days2-3::direction::mean","135,50");
	  NFmiSettings::Set("a::fake::days1-3::direction::mean","90,30");
	  REQUIRE(story,"fi",fun,"Idän puoleista tuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"Vind omkring ost 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Approximately easterly wind 0...2 m/s.");

	}

	// SAME THING ALL OVER AGAIN, BUT WITH VARIABLE WINDS


	// Test 1-day forecasts
	{
	  const WeatherPeriod period(time1,time2);
	  WindStory story(time1,sources,area,period,"a");

	  NFmiSettings::Set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day1::speed::maximum","2,0");

	  NFmiSettings::Set("a::today::phrases","none,today,tomorrow,weekday");

	  NFmiSettings::Set("a::fake::day1::direction::mean","0,50");
	  REQUIRE(story,"fi",fun,"Suunnaltaan vaihtelevaa tuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"Varierande vind 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Variable wind 0...2 m/s.");

	  NFmiSettings::Set("a::today::phrases","today,tomorrow,weekday");

	  NFmiSettings::Set("a::fake::day1::direction::mean","0,50");
	  REQUIRE(story,"fi",fun,"Tänään suunnaltaan vaihtelevaa tuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"I dag varierande vind 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Today variable wind 0...2 m/s.");

	}

	// Test 1-day forecast for tomorrow

	{
	  const WeatherPeriod period(time2,time3);
	  WindStory story(time1,sources,area,period,"a");

	  NFmiSettings::Set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day1::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day1::direction::mean","0,50");

	  NFmiSettings::Set("a::today::phrases","none,today,tomorrow,weekday");
	  REQUIRE(story,"fi",fun,"Huomenna suunnaltaan vaihtelevaa tuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"I morgon varierande vind 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Tomorrow variable wind 0...2 m/s.");

	  NFmiSettings::Set("a::today::phrases","today,tomorrow,weekday");
	  REQUIRE(story,"fi",fun,"Huomenna suunnaltaan vaihtelevaa tuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"I morgon varierande vind 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Tomorrow variable wind 0...2 m/s.");

	}

	// Test 1-day forecast for a later day

	{
	  const WeatherPeriod period(time3,time4);
	  WindStory story(time1,sources,area,period,"a");

	  NFmiSettings::Set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day1::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day1::direction::mean","0,50");

	  NFmiSettings::Set("a::today::phrases","none,today,tomorrow,weekday");
	  REQUIRE(story,"fi",fun,"Tiistaina suunnaltaan vaihtelevaa tuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"På tisdagen varierande vind 0...2 m/s.");
	  REQUIRE(story,"en",fun,"On Tuesday variable wind 0...2 m/s.");

	}

	// Test 2-day forecast starting today

	{
	  const WeatherPeriod period(time1,time3);
	  WindStory story(time1,sources,area,period,"a");

	  NFmiSettings::Set("a::today::phrases","none,today,tomorrow,weekday");
	  NFmiSettings::Set("a::next_day::phrases","tomorrow,following_day,weekday");
	  NFmiSettings::Set("a::days::phrases","none,today,tomorrow,followingday,weekday");

	  NFmiSettings::Set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day1::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day2::speed::mean","3,0");
	  NFmiSettings::Set("a::fake::day2::speed::minimum","4,0");
	  NFmiSettings::Set("a::fake::day2::speed::maximum","5,0");

	  NFmiSettings::Set("a::fake::day1::direction::mean","0,50");
	  NFmiSettings::Set("a::fake::day2::direction::mean","90,50");
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,50");
	  REQUIRE(story,"fi",fun,"Suunnaltaan vaihtelevaa tuulta 0...2 m/s, huomenna 4...5 m/s.");
	  REQUIRE(story,"sv",fun,"Varierande vind 0...2 m/s, i morgon 4...5 m/s.");
	  REQUIRE(story,"en",fun,"Variable wind 0...2 m/s, tomorrow 4...5 m/s.");

	}

	// Test 3-day forecast starting today

	{
	  const WeatherPeriod period(time1,time4);
	  WindStory story(time1,sources,area,period,"a");

	  NFmiSettings::Set("a::today::phrases","none,today,tomorrow,weekday");
	  NFmiSettings::Set("a::next_day::phrases","tomorrow,following_day,weekday");
	  NFmiSettings::Set("a::days::phrases","none,today,tomorrow,followingday,weekday");

	  // W1<>W2<>W3

	  NFmiSettings::Set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day1::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day2::speed::minimum","3,0");
	  NFmiSettings::Set("a::fake::day2::speed::mean","4,0");
	  NFmiSettings::Set("a::fake::day2::speed::maximum","5,0");
	  NFmiSettings::Set("a::fake::day3::speed::minimum","6,0");
	  NFmiSettings::Set("a::fake::day3::speed::mean","7,0");
	  NFmiSettings::Set("a::fake::day3::speed::maximum","8,0");

	  NFmiSettings::Set("a::fake::day1::direction::mean","0,50");
	  NFmiSettings::Set("a::fake::day2::direction::mean","90,50");
	  NFmiSettings::Set("a::fake::day3::direction::mean","180,50");
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,50");
	  NFmiSettings::Set("a::fake::days2-3::direction::mean","135,50");
	  NFmiSettings::Set("a::fake::days1-3::direction::mean","90,50");
	  REQUIRE(story,"fi",fun,"Suunnaltaan vaihtelevaa tuulta 0...2 m/s, huomenna 3...5 m/s, tiistaina 6...8 m/s.");
	  REQUIRE(story,"sv",fun,"Varierande vind 0...2 m/s, i morgon 3...5 m/s, på tisdagen 6...8 m/s.");
	  REQUIRE(story,"en",fun,"Variable wind 0...2 m/s, tomorrow 3...5 m/s, on Tuesday 6...8 m/s.");

	  // W1==W2<>W3

	  NFmiSettings::Set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day1::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day2::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day2::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day2::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day3::speed::minimum","6,0");
	  NFmiSettings::Set("a::fake::day3::speed::mean","7,0");
	  NFmiSettings::Set("a::fake::day3::speed::maximum","8,0");

	  NFmiSettings::Set("a::fake::day1::direction::mean","0,50");
	  NFmiSettings::Set("a::fake::day2::direction::mean","90,50");
	  NFmiSettings::Set("a::fake::day3::direction::mean","180,50");
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,50");
	  NFmiSettings::Set("a::fake::days2-3::direction::mean","135,50");
	  NFmiSettings::Set("a::fake::days1-3::direction::mean","90,50");
	  REQUIRE(story,"fi",fun,"Suunnaltaan vaihtelevaa tuulta 0...2 m/s, tiistaina 6...8 m/s.");
	  REQUIRE(story,"sv",fun,"Varierande vind 0...2 m/s, på tisdagen 6...8 m/s.");
	  REQUIRE(story,"en",fun,"Variable wind 0...2 m/s, on Tuesday 6...8 m/s.");

	  // W1<>W2==W3

	  NFmiSettings::Set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day1::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day2::speed::minimum","3,0");
	  NFmiSettings::Set("a::fake::day2::speed::mean","4,0");
	  NFmiSettings::Set("a::fake::day2::speed::maximum","5,0");
	  NFmiSettings::Set("a::fake::day3::speed::minimum","3,0");
	  NFmiSettings::Set("a::fake::day3::speed::mean","4,0");
	  NFmiSettings::Set("a::fake::day3::speed::maximum","5,0");

	  NFmiSettings::Set("a::fake::day1::direction::mean","0,50");
	  NFmiSettings::Set("a::fake::day2::direction::mean","90,50");
	  NFmiSettings::Set("a::fake::day3::direction::mean","180,50");
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,50");
	  NFmiSettings::Set("a::fake::days2-3::direction::mean","135,50");
	  NFmiSettings::Set("a::fake::days1-3::direction::mean","90,50");

	  REQUIRE(story,"fi",fun,"Suunnaltaan vaihtelevaa tuulta 0...2 m/s, maanantaista alkaen 3...5 m/s.");
	  REQUIRE(story,"sv",fun,"Varierande vind 0...2 m/s, från måndagen 3...5 m/s.");
	  REQUIRE(story,"en",fun,"Variable wind 0...2 m/s, from Monday onwards 3...5 m/s.");

	  // W1==W2==W3

	  NFmiSettings::Set("a::fake::day1::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day1::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day1::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day2::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day2::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day2::speed::maximum","2,0");
	  NFmiSettings::Set("a::fake::day3::speed::minimum","0,0");
	  NFmiSettings::Set("a::fake::day3::speed::mean","1,0");
	  NFmiSettings::Set("a::fake::day3::speed::maximum","2,0");

	  NFmiSettings::Set("a::fake::day1::direction::mean","0,50");
	  NFmiSettings::Set("a::fake::day2::direction::mean","90,50");
	  NFmiSettings::Set("a::fake::day3::direction::mean","180,50");
	  NFmiSettings::Set("a::fake::days1-2::direction::mean","45,50");
	  NFmiSettings::Set("a::fake::days2-3::direction::mean","135,50");
	  NFmiSettings::Set("a::fake::days1-3::direction::mean","90,50");

	  REQUIRE(story,"fi",fun,"Suunnaltaan vaihtelevaa tuulta 0...2 m/s.");
	  REQUIRE(story,"sv",fun,"Varierande vind 0...2 m/s.");
	  REQUIRE(story,"en",fun,"Variable wind 0...2 m/s.");

	}

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WindStory::simple_overview
   */
  // ----------------------------------------------------------------------

  void wind_simple_overview()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WindStory::overview
   */
  // ----------------------------------------------------------------------

  void wind_overview()
  {
	TEST_NOT_IMPLEMENTED();
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
	  TEST(wind_simple_overview);
	  TEST(wind_overview);
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
