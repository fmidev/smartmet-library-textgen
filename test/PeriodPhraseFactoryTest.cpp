#include "tframe.h"
#include "PeriodPhraseFactory.h"
#include "DictionaryFactory.h"
#include "PlainTextFormatter.h"
#include "Sentence.h"
#include "WeatherPeriod.h"

#include "NFmiSettings.h"
#include "NFmiTime.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;

namespace PeriodPhraseFactoryTest
{
  shared_ptr<TextGen::Dictionary> dict;
  TextGen::PlainTextFormatter formatter;

  string require(const string & theMode,
				 const string & theVariable,
				 const NFmiTime & theForecastTime,
				 const WeatherAnalysis::WeatherPeriod & thePeriod,
				 const string & theLanguage,
				 const string & theResult)
  {
	using TextGen::Sentence;
	using TextGen::PeriodPhraseFactory;

	dict->init(theLanguage);
	formatter.dictionary(dict);

	const Sentence sentence = PeriodPhraseFactory::create(theMode,
														  theVariable,
														  theForecastTime,
														  thePeriod);

	const string result = sentence.realize(formatter);
	if(result != theResult)
	  return (result + " <> " + theResult);
	else
	  return "";
  }

#define REQUIRE(mode,var,time,period,lang,expected) \
result = require(mode,var,time,period,lang,expected); \
if(!result.empty()) TEST_FAILED(result.c_str());

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PeriodPhraseFactory::create("until_tonight");
   */
  // ----------------------------------------------------------------------

  void until_tonight()
  {
	NFmiTime day1(2003,06,29);
	NFmiTime day2(2003,06,30);
	NFmiTime day3(2003,07,01);

	string mode = "until_tonight";
	string var = mode;

	NFmiTime ftime(day1);
	WeatherAnalysis::WeatherPeriod period1(day1,day2);
	WeatherAnalysis::WeatherPeriod period2(day2,day3);

	string result;

	REQUIRE(mode,var,ftime,period1,"fi","");
	REQUIRE(mode,var,ftime,period1,"sv","");
	REQUIRE(mode,var,ftime,period1,"en","");

	REQUIRE(mode,var,ftime,period2,"fi","Maanantaina.");
	REQUIRE(mode,var,ftime,period2,"sv","På måndagen.");
	REQUIRE(mode,var,ftime,period2,"en","On Monday.");

	NFmiSettings::instance().set(var+"::prefer_phrase_weekday","true");
	NFmiSettings::instance().set(var+"::prefer_phrase_today","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_atday","false");
	REQUIRE(mode,var,ftime,period1,"fi","Sunnuntaina.");
	REQUIRE(mode,var,ftime,period1,"sv","På söndagen.");
	REQUIRE(mode,var,ftime,period1,"en","On Sunday.");

	NFmiSettings::instance().set(var+"::prefer_phrase_weekday","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_today","true");
	NFmiSettings::instance().set(var+"::prefer_phrase_atday","false");
	REQUIRE(mode,var,ftime,period1,"fi","Tänään.");
	REQUIRE(mode,var,ftime,period1,"sv","I dag.");
	REQUIRE(mode,var,ftime,period1,"en","Today.");

	NFmiSettings::instance().set(var+"::prefer_phrase_weekday","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_today","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_atday","true");
	REQUIRE(mode,var,ftime,period1,"fi","Päivällä.");
	REQUIRE(mode,var,ftime,period1,"sv","På dagen.");
	REQUIRE(mode,var,ftime,period1,"en","During the day.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PeriodPhraseFactory::create("until_morning");
   */
  // ----------------------------------------------------------------------

  void until_morning()
  {
	NFmiTime day1(2003,06,29);
	NFmiTime day2(2003,06,30);
	NFmiTime day3(2003,07,01);

	string mode = "until_morning";
	string var = mode;

	NFmiTime ftime(day1);
	WeatherAnalysis::WeatherPeriod period1(day1,day2);
	WeatherAnalysis::WeatherPeriod period2(day2,day3);

	string result;

	REQUIRE(mode,var,ftime,period1,"fi","");
	REQUIRE(mode,var,ftime,period1,"sv","");
	REQUIRE(mode,var,ftime,period1,"en","");

	REQUIRE(mode,var,ftime,period2,"fi","Tiistain vastaisena yönä.");
	REQUIRE(mode,var,ftime,period2,"sv","Natten mot tisdagen.");
	REQUIRE(mode,var,ftime,period2,"en","On Tuesday night.");

	NFmiSettings::instance().set(var+"::prefer_phrase_weekday","true");
	NFmiSettings::instance().set(var+"::prefer_phrase_tonight","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_atnight","false");
	REQUIRE(mode,var,ftime,period1,"fi","Maanantain vastaisena yönä.");
	REQUIRE(mode,var,ftime,period1,"sv","Natten mot måndagen.");
	REQUIRE(mode,var,ftime,period1,"en","On Monday night.");

	NFmiSettings::instance().set(var+"::prefer_phrase_weekday","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_tonight","true");
	NFmiSettings::instance().set(var+"::prefer_phrase_atnight","false");
	REQUIRE(mode,var,ftime,period1,"fi","Ensi yönä.");
	REQUIRE(mode,var,ftime,period1,"sv","I natt.");
	REQUIRE(mode,var,ftime,period1,"en","At night.");

	NFmiSettings::instance().set(var+"::prefer_phrase_weekday","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_tonight","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_atnight","true");
	REQUIRE(mode,var,ftime,period1,"fi","Yöllä.");
	REQUIRE(mode,var,ftime,period1,"sv","På natten.");
	REQUIRE(mode,var,ftime,period1,"en","At night.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PeriodPhraseFactory::create("next_night");
   */
  // ----------------------------------------------------------------------

  void next_night()
  {
	NFmiTime day1(2003,06,29);
	NFmiTime day2(2003,06,30);

	string mode = "next_night";
	string var = mode;

	NFmiTime ftime(day1);
	WeatherAnalysis::WeatherPeriod period(day1,day2);

	string result;

	REQUIRE(mode,var,ftime,period,"fi","Yöllä.");
	REQUIRE(mode,var,ftime,period,"sv","På natten.");
	REQUIRE(mode,var,ftime,period,"en","At night.");

	NFmiSettings::instance().set(var+"::prefer_phrase_weekday","true");
	NFmiSettings::instance().set(var+"::prefer_phrase_followingnight","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_tonight","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_atnight","false");
	REQUIRE(mode,var,ftime,period,"fi","Maanantain vastaisena yönä.");
	REQUIRE(mode,var,ftime,period,"sv","Natten mot måndagen.");
	REQUIRE(mode,var,ftime,period,"en","On Monday night.");

	NFmiSettings::instance().set(var+"::prefer_phrase_weekday","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_followingnight","true");
	NFmiSettings::instance().set(var+"::prefer_phrase_tonight","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_atnight","false");
	REQUIRE(mode,var,ftime,period,"fi","Seuraavana yönä.");
	REQUIRE(mode,var,ftime,period,"sv","Följande natt.");
	REQUIRE(mode,var,ftime,period,"en","The following night.");

	NFmiSettings::instance().set(var+"::prefer_phrase_weekday","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_followingnight","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_tonight","true");
	NFmiSettings::instance().set(var+"::prefer_phrase_atnight","false");
	REQUIRE(mode,var,ftime,period,"fi","Ensi yönä.");
	REQUIRE(mode,var,ftime,period,"sv","I natt.");
	REQUIRE(mode,var,ftime,period,"en","At night.");

	NFmiSettings::instance().set(var+"::prefer_phrase_weekday","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_followingnight","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_tonight","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_atnight","true");
	REQUIRE(mode,var,ftime,period,"fi","Yöllä.");
	REQUIRE(mode,var,ftime,period,"sv","På natten.");
	REQUIRE(mode,var,ftime,period,"en","At night.");


	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PeriodPhraseFactory::create("next_day");
   */
  // ----------------------------------------------------------------------

  void next_day()
  {
	NFmiTime day1(2003,06,29);
	NFmiTime day2(2003,06,30);
	NFmiTime day3(2003,07,01);

	string mode = "next_day";
	string var = mode;

	NFmiTime ftime(day1);
	WeatherAnalysis::WeatherPeriod period1(day1,day2);
	WeatherAnalysis::WeatherPeriod period2(day2,day3);

	string result;

	REQUIRE(mode,var,ftime,period1,"fi","Sunnuntaina.");
	REQUIRE(mode,var,ftime,period1,"sv","På söndagen.");
	REQUIRE(mode,var,ftime,period1,"en","On Sunday.");

	REQUIRE(mode,var,ftime,period2,"fi","Huomenna.");
	REQUIRE(mode,var,ftime,period2,"sv","I morgon.");
	REQUIRE(mode,var,ftime,period2,"en","Tomorrow.");

	NFmiSettings::instance().set(var+"::prefer_phrase_weekday","true");
	NFmiSettings::instance().set(var+"::prefer_phrase_followingday","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_tomorrow","false");
	REQUIRE(mode,var,ftime,period2,"fi","Maanantaina.");
	REQUIRE(mode,var,ftime,period2,"sv","På måndagen.");
	REQUIRE(mode,var,ftime,period2,"en","On Monday.");

	NFmiSettings::instance().set(var+"::prefer_phrase_weekday","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_followingday","true");
	NFmiSettings::instance().set(var+"::prefer_phrase_tomorrow","false");
	REQUIRE(mode,var,ftime,period2,"fi","Seuraavana päivänä.");
	REQUIRE(mode,var,ftime,period2,"sv","Följande dag.");
	REQUIRE(mode,var,ftime,period2,"en","The following day.");

	NFmiSettings::instance().set(var+"::prefer_phrase_weekday","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_followingday","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_tomorrow","true");
	REQUIRE(mode,var,ftime,period2,"fi","Huomenna.");
	REQUIRE(mode,var,ftime,period2,"sv","I morgon.");
	REQUIRE(mode,var,ftime,period2,"en","Tomorrow.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PeriodPhraseFactory::create("next_days");
   */
  // ----------------------------------------------------------------------

  void next_days()
  {
	NFmiTime day1(2003,06,29);
	NFmiTime day2(2003,06,30);
	NFmiTime day3(2003,07,01);

	string mode = "next_days";
	string var = mode;

	NFmiTime ftime(day1);
	WeatherAnalysis::WeatherPeriod period1(day1,day2);
	WeatherAnalysis::WeatherPeriod period2(day2,day3);

	string result;

	REQUIRE(mode,var,ftime,period1,"fi","Sunnuntaista alkaen.");
	REQUIRE(mode,var,ftime,period1,"sv","Från söndagen.");
	REQUIRE(mode,var,ftime,period1,"en","From Sunday onwards.");

	REQUIRE(mode,var,ftime,period2,"fi","Huomisesta alkaen.");
	REQUIRE(mode,var,ftime,period2,"sv","Från i morgon.");
	REQUIRE(mode,var,ftime,period2,"en","Starting tomorrow.");

	NFmiSettings::instance().set(var+"::prefer_phrase_weekday","true");
	NFmiSettings::instance().set(var+"::prefer_phrase_tomorrow","false");
	REQUIRE(mode,var,ftime,period2,"fi","Maanantaista alkaen.");
	REQUIRE(mode,var,ftime,period2,"sv","Från måndagen.");
	REQUIRE(mode,var,ftime,period2,"en","From Monday onwards.");

	NFmiSettings::instance().set(var+"::prefer_phrase_weekday","false");
	NFmiSettings::instance().set(var+"::prefer_phrase_tomorrow","true");
	REQUIRE(mode,var,ftime,period2,"fi","Huomisesta alkaen.");
	REQUIRE(mode,var,ftime,period2,"sv","Från i morgon.");
	REQUIRE(mode,var,ftime,period2,"en","Starting tomorrow.");

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
	  TEST(until_tonight);
	  TEST(until_morning);
	  TEST(next_night);
	  TEST(next_day);
	  TEST(next_days);
	}

  }; // class tests

} // namespace PeriodPhraseFactoryTest


int main(void)
{
  using namespace PeriodPhraseFactoryTest;

  cout << endl
	   << "PeriodPhraseFactory tests" << endl
	   << "=========================" << endl;

  dict = TextGen::DictionaryFactory::create("multimysql");

  dict->init("fi");
  dict->init("sv");
  dict->init("en");

  tests t;
  return t.run();
}
