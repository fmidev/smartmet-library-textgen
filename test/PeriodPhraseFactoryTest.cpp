#include "regression/tframe.h"
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
	const NFmiTime day1(2003,06,29);
	const NFmiTime day2(2003,06,30);
	const NFmiTime day3(2003,07,01);

	const string mode = "until_tonight";
	const string var = mode;
	const string phrasesvar = var+"::"+mode+"::phrases";

	const NFmiTime ftime(day1);
	const WeatherAnalysis::WeatherPeriod period1(day1,day2);
	const WeatherAnalysis::WeatherPeriod period2(day2,day3);

	string result;

	REQUIRE(mode,var,ftime,period1,"fi","");
	REQUIRE(mode,var,ftime,period1,"sv","");
	REQUIRE(mode,var,ftime,period1,"en","");

	REQUIRE(mode,var,ftime,period2,"fi","Maanantaina.");
	REQUIRE(mode,var,ftime,period2,"sv","På måndagen.");
	REQUIRE(mode,var,ftime,period2,"en","On Monday.");

	NFmiSettings::Set(phrasesvar,"weekday");
	REQUIRE(mode,var,ftime,period1,"fi","Sunnuntaina.");
	REQUIRE(mode,var,ftime,period1,"sv","På söndagen.");
	REQUIRE(mode,var,ftime,period1,"en","On Sunday.");

	REQUIRE(mode,var,ftime,period2,"fi","Maanantaina.");
	REQUIRE(mode,var,ftime,period2,"sv","På måndagen.");
	REQUIRE(mode,var,ftime,period2,"en","On Monday.");

	NFmiSettings::Set(phrasesvar,"today");
	REQUIRE(mode,var,ftime,period1,"fi","Tänään.");
	REQUIRE(mode,var,ftime,period1,"sv","I dag.");
	REQUIRE(mode,var,ftime,period1,"en","Today.");

	REQUIRE(mode,var,ftime,period2,"fi","Maanantaina.");
	REQUIRE(mode,var,ftime,period2,"sv","På måndagen.");
	REQUIRE(mode,var,ftime,period2,"en","On Monday.");

	NFmiSettings::Set(phrasesvar,"atday");
	REQUIRE(mode,var,ftime,period1,"fi","Päivällä.");
	REQUIRE(mode,var,ftime,period1,"sv","På dagen.");
	REQUIRE(mode,var,ftime,period1,"en","During the day.");

	REQUIRE(mode,var,ftime,period2,"fi","Maanantaina.");
	REQUIRE(mode,var,ftime,period2,"sv","På måndagen.");
	REQUIRE(mode,var,ftime,period2,"en","On Monday.");

	NFmiSettings::Set(phrasesvar,"none");
	REQUIRE(mode,var,ftime,period1,"fi","");
	REQUIRE(mode,var,ftime,period1,"sv","");
	REQUIRE(mode,var,ftime,period1,"en","");

	REQUIRE(mode,var,ftime,period2,"fi","Maanantaina.");
	REQUIRE(mode,var,ftime,period2,"sv","På måndagen.");
	REQUIRE(mode,var,ftime,period2,"en","On Monday.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PeriodPhraseFactory::create("until_morning");
   */
  // ----------------------------------------------------------------------

  void until_morning()
  {
	const NFmiTime day1(2003,06,29);
	const NFmiTime day2(2003,06,30);
	const NFmiTime day3(2003,07,01);

	const string mode = "until_morning";
	const string var = mode;
	const string phrasesvar = var+"::"+mode+"::phrases";

	const NFmiTime ftime(day1);
	const WeatherAnalysis::WeatherPeriod period1(day1,day2);
	const WeatherAnalysis::WeatherPeriod period2(day2,day3);

	string result;

	REQUIRE(mode,var,ftime,period1,"fi","");
	REQUIRE(mode,var,ftime,period1,"sv","");
	REQUIRE(mode,var,ftime,period1,"en","");

	REQUIRE(mode,var,ftime,period2,"fi","Tiistain vastaisena yönä.");
	REQUIRE(mode,var,ftime,period2,"sv","Natten mot tisdagen.");
	REQUIRE(mode,var,ftime,period2,"en","On Tuesday night.");

	NFmiSettings::Set(phrasesvar,"weekday");
	REQUIRE(mode,var,ftime,period1,"fi","Maanantain vastaisena yönä.");
	REQUIRE(mode,var,ftime,period1,"sv","Natten mot måndagen.");
	REQUIRE(mode,var,ftime,period1,"en","On Monday night.");

	REQUIRE(mode,var,ftime,period2,"fi","Tiistain vastaisena yönä.");
	REQUIRE(mode,var,ftime,period2,"sv","Natten mot tisdagen.");
	REQUIRE(mode,var,ftime,period2,"en","On Tuesday night.");

	NFmiSettings::Set(phrasesvar,"tonight");
	REQUIRE(mode,var,ftime,period1,"fi","Ensi yönä.");
	REQUIRE(mode,var,ftime,period1,"sv","I natt.");
	REQUIRE(mode,var,ftime,period1,"en","At night.");

	REQUIRE(mode,var,ftime,period2,"fi","Tiistain vastaisena yönä.");
	REQUIRE(mode,var,ftime,period2,"sv","Natten mot tisdagen.");
	REQUIRE(mode,var,ftime,period2,"en","On Tuesday night.");

	NFmiSettings::Set(phrasesvar,"atnight");
	REQUIRE(mode,var,ftime,period1,"fi","Yöllä.");
	REQUIRE(mode,var,ftime,period1,"sv","På natten.");
	REQUIRE(mode,var,ftime,period1,"en","At night.");

	REQUIRE(mode,var,ftime,period2,"fi","Tiistain vastaisena yönä.");
	REQUIRE(mode,var,ftime,period2,"sv","Natten mot tisdagen.");
	REQUIRE(mode,var,ftime,period2,"en","On Tuesday night.");

	NFmiSettings::Set(phrasesvar,"none");
	REQUIRE(mode,var,ftime,period1,"fi","");
	REQUIRE(mode,var,ftime,period1,"sv","");
	REQUIRE(mode,var,ftime,period1,"en","");

	REQUIRE(mode,var,ftime,period2,"fi","Tiistain vastaisena yönä.");
	REQUIRE(mode,var,ftime,period2,"sv","Natten mot tisdagen.");
	REQUIRE(mode,var,ftime,period2,"en","On Tuesday night.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PeriodPhraseFactory::create("today");
   */
  // ----------------------------------------------------------------------

  void today()
  {
	const NFmiTime day1(2003,06,29);
	const NFmiTime day2(2003,06,30);
	const NFmiTime day3(2003,07,01);

	const string mode = "today";
	const string var = mode;
	const string phrasesvar = var+"::"+mode+"::phrases";

	const NFmiTime ftime(day1);
	const WeatherAnalysis::WeatherPeriod period1(day1,day2);
	const WeatherAnalysis::WeatherPeriod period2(day2,day3);

	string result;

	REQUIRE(mode,var,ftime,period1,"fi","");
	REQUIRE(mode,var,ftime,period1,"sv","");
	REQUIRE(mode,var,ftime,period1,"en","");

	REQUIRE(mode,var,ftime,period2,"fi","Huomenna.");
	REQUIRE(mode,var,ftime,period2,"sv","I morgon.");
	REQUIRE(mode,var,ftime,period2,"en","Tomorrow.");

	NFmiSettings::Set(phrasesvar,"today");
	REQUIRE(mode,var,ftime,period1,"fi","Tänään.");
	REQUIRE(mode,var,ftime,period1,"sv","I dag.");
	REQUIRE(mode,var,ftime,period1,"en","Today.");

	REQUIRE(mode,var,ftime,period2,"fi","Huomenna.");
	REQUIRE(mode,var,ftime,period2,"sv","I morgon.");
	REQUIRE(mode,var,ftime,period2,"en","Tomorrow.");

	NFmiSettings::Set(phrasesvar,"atday");
	REQUIRE(mode,var,ftime,period1,"fi","Päivällä.");
	REQUIRE(mode,var,ftime,period1,"sv","På dagen.");
	REQUIRE(mode,var,ftime,period1,"en","During the day.");

	NFmiSettings::Set(phrasesvar,"weekday");
	REQUIRE(mode,var,ftime,period1,"fi","Sunnuntaina.");
	REQUIRE(mode,var,ftime,period1,"sv","På söndagen.");
	REQUIRE(mode,var,ftime,period1,"en","On Sunday.");

	REQUIRE(mode,var,ftime,period2,"fi","Maanantaina.");
	REQUIRE(mode,var,ftime,period2,"sv","På måndagen.");
	REQUIRE(mode,var,ftime,period2,"en","On Monday.");

	NFmiSettings::Set(phrasesvar,"tomorrow");
	REQUIRE(mode,var,ftime,period1,"fi","");
	REQUIRE(mode,var,ftime,period1,"sv","");
	REQUIRE(mode,var,ftime,period1,"en","");

	REQUIRE(mode,var,ftime,period2,"fi","Huomenna.");
	REQUIRE(mode,var,ftime,period2,"sv","I morgon.");
	REQUIRE(mode,var,ftime,period2,"en","Tomorrow.");


	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PeriodPhraseFactory::create("tonight");
   */
  // ----------------------------------------------------------------------

  void tonight()
  {
	const NFmiTime day1(2003,06,29,18);
	const NFmiTime day2(2003,06,30,06);
	const NFmiTime day3(2003,07,01,06);

	const string mode = "tonight";
	const string var = mode;
	const string phrasesvar = var+"::"+mode+"::phrases";

	const NFmiTime ftime(day1);
	const WeatherAnalysis::WeatherPeriod period1(day1,day2);
	const WeatherAnalysis::WeatherPeriod period2(day2,day3);

	string result;

	REQUIRE(mode,var,ftime,period1,"fi","");
	REQUIRE(mode,var,ftime,period1,"sv","");
	REQUIRE(mode,var,ftime,period1,"en","");

	REQUIRE(mode,var,ftime,period2,"fi","Tiistain vastaisena yönä.");
	REQUIRE(mode,var,ftime,period2,"sv","Natten mot tisdagen.");
	REQUIRE(mode,var,ftime,period2,"en","On Tuesday night.");

	NFmiSettings::Set(phrasesvar,"tonight");
	REQUIRE(mode,var,ftime,period1,"fi","Ensi yönä.");
	REQUIRE(mode,var,ftime,period1,"sv","I natt.");
	REQUIRE(mode,var,ftime,period1,"en","At night.");

	REQUIRE(mode,var,ftime,period2,"fi","Tiistain vastaisena yönä.");
	REQUIRE(mode,var,ftime,period2,"sv","Natten mot tisdagen.");
	REQUIRE(mode,var,ftime,period2,"en","On Tuesday night.");

	NFmiSettings::Set(phrasesvar,"atnight");
	REQUIRE(mode,var,ftime,period1,"fi","Yöllä.");
	REQUIRE(mode,var,ftime,period1,"sv","På natten.");
	REQUIRE(mode,var,ftime,period1,"en","At night.");

	REQUIRE(mode,var,ftime,period2,"fi","Tiistain vastaisena yönä.");
	REQUIRE(mode,var,ftime,period2,"sv","Natten mot tisdagen.");
	REQUIRE(mode,var,ftime,period2,"en","On Tuesday night.");

	NFmiSettings::Set(phrasesvar,"weekday");
	REQUIRE(mode,var,ftime,period1,"fi","Maanantain vastaisena yönä.");
	REQUIRE(mode,var,ftime,period1,"sv","Natten mot måndagen.");
	REQUIRE(mode,var,ftime,period1,"en","On Monday night.");

	REQUIRE(mode,var,ftime,period2,"fi","Tiistain vastaisena yönä.");
	REQUIRE(mode,var,ftime,period2,"sv","Natten mot tisdagen.");
	REQUIRE(mode,var,ftime,period2,"en","On Tuesday night.");

	NFmiSettings::Set(phrasesvar,"none");
	REQUIRE(mode,var,ftime,period1,"fi","");
	REQUIRE(mode,var,ftime,period1,"sv","");
	REQUIRE(mode,var,ftime,period1,"en","");

	REQUIRE(mode,var,ftime,period2,"fi","Tiistain vastaisena yönä.");
	REQUIRE(mode,var,ftime,period2,"sv","Natten mot tisdagen.");
	REQUIRE(mode,var,ftime,period2,"en","On Tuesday night.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PeriodPhraseFactory::create("next_night");
   */
  // ----------------------------------------------------------------------

  void next_night()
  {
	const NFmiTime day1(2003,06,29);
	const NFmiTime day2(2003,06,30);

	const string mode = "next_night";
	const string var = mode;
	const string phrasesvar = var+"::"+mode+"::phrases";

	const NFmiTime ftime(day1);
	const WeatherAnalysis::WeatherPeriod period(day1,day2);

	string result;

	REQUIRE(mode,var,ftime,period,"fi","Ensi yönä.");
	REQUIRE(mode,var,ftime,period,"sv","I natt.");
	REQUIRE(mode,var,ftime,period,"en","At night.");

	NFmiSettings::Set(phrasesvar,"weekday");
	REQUIRE(mode,var,ftime,period,"fi","Maanantain vastaisena yönä.");
	REQUIRE(mode,var,ftime,period,"sv","Natten mot måndagen.");
	REQUIRE(mode,var,ftime,period,"en","On Monday night.");

	NFmiSettings::Set(phrasesvar,"followingnight");
	REQUIRE(mode,var,ftime,period,"fi","Seuraavana yönä.");
	REQUIRE(mode,var,ftime,period,"sv","Följande natt.");
	REQUIRE(mode,var,ftime,period,"en","The following night.");

	NFmiSettings::Set(phrasesvar,"tonight");
	REQUIRE(mode,var,ftime,period,"fi","Ensi yönä.");
	REQUIRE(mode,var,ftime,period,"sv","I natt.");
	REQUIRE(mode,var,ftime,period,"en","At night.");

	NFmiSettings::Set(phrasesvar,"atnight");
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
	const NFmiTime day1(2003,06,29);
	const NFmiTime day2(2003,06,30);
	const NFmiTime day3(2003,07,01);

	const string mode = "next_day";
	const string var = mode;
	const string phrasesvar = var+"::"+mode+"::phrases";

	const NFmiTime ftime(day1);
	const WeatherAnalysis::WeatherPeriod period1(day1,day2);
	const WeatherAnalysis::WeatherPeriod period2(day2,day3);

	string result;

	REQUIRE(mode,var,ftime,period1,"fi","Seuraavana päivänä.");
	REQUIRE(mode,var,ftime,period1,"sv","Följande dag.");
	REQUIRE(mode,var,ftime,period1,"en","The following day.");

	REQUIRE(mode,var,ftime,period2,"fi","Huomenna.");
	REQUIRE(mode,var,ftime,period2,"sv","I morgon.");
	REQUIRE(mode,var,ftime,period2,"en","Tomorrow.");

	NFmiSettings::Set(phrasesvar,"weekday");
	REQUIRE(mode,var,ftime,period1,"fi","Sunnuntaina.");
	REQUIRE(mode,var,ftime,period1,"sv","På söndagen.");
	REQUIRE(mode,var,ftime,period1,"en","On Sunday.");

	REQUIRE(mode,var,ftime,period2,"fi","Maanantaina.");
	REQUIRE(mode,var,ftime,period2,"sv","På måndagen.");
	REQUIRE(mode,var,ftime,period2,"en","On Monday.");

	NFmiSettings::Set(phrasesvar,"followingday");
	REQUIRE(mode,var,ftime,period1,"fi","Seuraavana päivänä.");
	REQUIRE(mode,var,ftime,period1,"sv","Följande dag.");
	REQUIRE(mode,var,ftime,period1,"en","The following day.");

	REQUIRE(mode,var,ftime,period2,"fi","Seuraavana päivänä.");
	REQUIRE(mode,var,ftime,period2,"sv","Följande dag.");
	REQUIRE(mode,var,ftime,period2,"en","The following day.");

	NFmiSettings::Set(phrasesvar,"tomorrow");
	REQUIRE(mode,var,ftime,period1,"fi","Seuraavana päivänä.");
	REQUIRE(mode,var,ftime,period1,"sv","Följande dag.");
	REQUIRE(mode,var,ftime,period1,"en","The following day.");

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
	const NFmiTime day1(2003,06,29);
	const NFmiTime day2(2003,06,30);
	const NFmiTime day3(2003,07,01);

	const string mode = "next_days";
	const string var = mode;
	const string phrasesvar = var+"::"+mode+"::phrases";

	const NFmiTime ftime(day1);
	const WeatherAnalysis::WeatherPeriod period1(day1,day2);
	const WeatherAnalysis::WeatherPeriod period2(day2,day3);

	string result;

	REQUIRE(mode,var,ftime,period1,"fi","Sunnuntaista alkaen.");
	REQUIRE(mode,var,ftime,period1,"sv","Från söndagen.");
	REQUIRE(mode,var,ftime,period1,"en","From Sunday onwards.");

	// REQUIRE(mode,var,ftime,period2,"fi","Huomisesta alkaen.");
	// REQUIRE(mode,var,ftime,period2,"sv","Från i morgon.");
	// REQUIRE(mode,var,ftime,period2,"en","Starting tomorrow.");
	REQUIRE(mode,var,ftime,period2,"fi","Maanantaista alkaen.");
	REQUIRE(mode,var,ftime,period2,"sv","Från måndagen.");
	REQUIRE(mode,var,ftime,period2,"en","From Monday onwards.");

	NFmiSettings::Set(phrasesvar,"weekday");
	REQUIRE(mode,var,ftime,period2,"fi","Maanantaista alkaen.");
	REQUIRE(mode,var,ftime,period2,"sv","Från måndagen.");
	REQUIRE(mode,var,ftime,period2,"en","From Monday onwards.");

	NFmiSettings::Set(phrasesvar,"tomorrow");
	// REQUIRE(mode,var,ftime,period2,"fi","Huomisesta alkaen.");
	// REQUIRE(mode,var,ftime,period2,"sv","Från i morgon.");
	// REQUIRE(mode,var,ftime,period2,"en","Starting tomorrow.");
	REQUIRE(mode,var,ftime,period2,"fi","Maanantaista alkaen.");
	REQUIRE(mode,var,ftime,period2,"sv","Från måndagen.");
	REQUIRE(mode,var,ftime,period2,"en","From Monday onwards.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PeriodPhraseFactory::create("days");
   */
  // ----------------------------------------------------------------------

  void days()
  {
	const NFmiTime day1(2003,06,29);
	const NFmiTime day2(2003,06,30);
	const NFmiTime day3(2003,07,01);
	const NFmiTime day4(2003,07,02);

	const string mode = "days";
	const string var = mode;
	const string phrasesvar = var+"::"+mode+"::phrases";

	const NFmiTime ftime(day1);
	string result;

	NFmiSettings::Set("days::day::starthour","06");
	NFmiSettings::Set("days::day::endhour","18");

	// 1 day, starting today
	{
	  const WeatherAnalysis::WeatherPeriod period(day1,day2);

	  NFmiSettings::Set(mode+"::days::phrases","none");
	  REQUIRE(mode,var,ftime,period,"fi","");
	  REQUIRE(mode,var,ftime,period,"sv","");
	  REQUIRE(mode,var,ftime,period,"en","");

	  NFmiSettings::Set(mode+"::days::phrases","today");
	  REQUIRE(mode,var,ftime,period,"fi","Tänään.");
	  REQUIRE(mode,var,ftime,period,"sv","I dag.");
	  REQUIRE(mode,var,ftime,period,"en","Today.");

	  NFmiSettings::Set(mode+"::days::phrases","tomorrow");
	  REQUIRE(mode,var,ftime,period,"fi","");
	  REQUIRE(mode,var,ftime,period,"sv","");
	  REQUIRE(mode,var,ftime,period,"en","");

	  NFmiSettings::Set(mode+"::days::phrases","followingday");
	  REQUIRE(mode,var,ftime,period,"fi","");
	  REQUIRE(mode,var,ftime,period,"sv","");
	  REQUIRE(mode,var,ftime,period,"en","");

	  NFmiSettings::Set(mode+"::days::phrases","weekday");
	  REQUIRE(mode,var,ftime,period,"fi","Sunnuntaina.");
	  REQUIRE(mode,var,ftime,period,"sv","På söndagen.");
	  REQUIRE(mode,var,ftime,period,"en","On Sunday.");
	  
	}

	// 1 day, starting tomorrow
	{
	  const WeatherAnalysis::WeatherPeriod period(day2,day3);

	  NFmiSettings::Set(mode+"::days::phrases","none");
	  REQUIRE(mode,var,ftime,period,"fi","");
	  REQUIRE(mode,var,ftime,period,"sv","");
	  REQUIRE(mode,var,ftime,period,"en","");

	  NFmiSettings::Set(mode+"::days::phrases","today");
	  REQUIRE(mode,var,ftime,period,"fi","");
	  REQUIRE(mode,var,ftime,period,"sv","");
	  REQUIRE(mode,var,ftime,period,"en","");

	  NFmiSettings::Set(mode+"::days::phrases","tomorrow");
	  REQUIRE(mode,var,ftime,period,"fi","Huomenna.");
	  REQUIRE(mode,var,ftime,period,"sv","I morgon.");
	  REQUIRE(mode,var,ftime,period,"en","Tomorrow.");

	  NFmiSettings::Set(mode+"::days::phrases","followingday");
	  REQUIRE(mode,var,ftime,period,"fi","");
	  REQUIRE(mode,var,ftime,period,"sv","");
	  REQUIRE(mode,var,ftime,period,"en","");

	  NFmiSettings::Set(mode+"::days::phrases","weekday");
	  REQUIRE(mode,var,ftime,period,"fi","Maanantaina.");
	  REQUIRE(mode,var,ftime,period,"sv","På måndagen.");
	  REQUIRE(mode,var,ftime,period,"en","On Monday.");
	  
	}

	// 2 days, starting today
	{
	  const WeatherAnalysis::WeatherPeriod period(day1,day3);

	  NFmiSettings::Set(mode+"::days::phrases","none");
	  REQUIRE(mode,var,ftime,period,"fi","");
	  REQUIRE(mode,var,ftime,period,"sv","");
	  REQUIRE(mode,var,ftime,period,"en","");

	  NFmiSettings::Set(mode+"::days::phrases","today");
	  REQUIRE(mode,var,ftime,period,"fi","Tänään ja huomenna.");
	  REQUIRE(mode,var,ftime,period,"sv","I dag och i morgon.");
	  REQUIRE(mode,var,ftime,period,"en","Today and tomorrow.");

	  NFmiSettings::Set(mode+"::days::phrases","tomorrow");
	  REQUIRE(mode,var,ftime,period,"fi","");
	  REQUIRE(mode,var,ftime,period,"sv","");
	  REQUIRE(mode,var,ftime,period,"en","");

	  NFmiSettings::Set(mode+"::days::phrases","followingday");
	  REQUIRE(mode,var,ftime,period,"fi","");
	  REQUIRE(mode,var,ftime,period,"sv","");
	  REQUIRE(mode,var,ftime,period,"en","");

	  NFmiSettings::Set(mode+"::days::phrases","weekday");
	  REQUIRE(mode,var,ftime,period,"fi","Sunnuntaina ja maanantaina.");
	  REQUIRE(mode,var,ftime,period,"sv","På söndagen och på måndagen.");
	  REQUIRE(mode,var,ftime,period,"en","On Sunday and on Monday.");
	  
	}

	// 2 days, starting tomorrow
	{
	  const WeatherAnalysis::WeatherPeriod period(day2,day4);

	  NFmiSettings::Set(mode+"::days::phrases","none");
	  REQUIRE(mode,var,ftime,period,"fi","");
	  REQUIRE(mode,var,ftime,period,"sv","");
	  REQUIRE(mode,var,ftime,period,"en","");

	  NFmiSettings::Set(mode+"::days::phrases","today");
	  REQUIRE(mode,var,ftime,period,"fi","");
	  REQUIRE(mode,var,ftime,period,"sv","");
	  REQUIRE(mode,var,ftime,period,"en","");

	  NFmiSettings::Set(mode+"::days::phrases","tomorrow");
	  REQUIRE(mode,var,ftime,period,"fi","Huomenna ja seuraavana päivänä.");
	  REQUIRE(mode,var,ftime,period,"sv","I morgon och följande dag.");
	  REQUIRE(mode,var,ftime,period,"en","Tomorrow and the following day.");

	  NFmiSettings::Set(mode+"::days::phrases","followingday");
	  REQUIRE(mode,var,ftime,period,"fi","");
	  REQUIRE(mode,var,ftime,period,"sv","");
	  REQUIRE(mode,var,ftime,period,"en","");

	  NFmiSettings::Set(mode+"::days::phrases","weekday");
	  REQUIRE(mode,var,ftime,period,"fi","Maanantaina ja tiistaina.");
	  REQUIRE(mode,var,ftime,period,"sv","På måndagen och på tisdagen.");
	  REQUIRE(mode,var,ftime,period,"en","On Monday and on Tuesday.");
	  
	}

	// 3 days, starting today
	{
	  const WeatherAnalysis::WeatherPeriod period(day1,day4);

	  NFmiSettings::Set(mode+"::days::phrases","none");
	  REQUIRE(mode,var,ftime,period,"fi","");
	  REQUIRE(mode,var,ftime,period,"sv","");
	  REQUIRE(mode,var,ftime,period,"en","");

	  NFmiSettings::Set(mode+"::days::phrases","today");
	  REQUIRE(mode,var,ftime,period,"fi","");
	  REQUIRE(mode,var,ftime,period,"sv","");
	  REQUIRE(mode,var,ftime,period,"en","");

	  NFmiSettings::Set(mode+"::days::phrases","tomorrow");
	  REQUIRE(mode,var,ftime,period,"fi","");
	  REQUIRE(mode,var,ftime,period,"sv","");
	  REQUIRE(mode,var,ftime,period,"en","");

	  NFmiSettings::Set(mode+"::days::phrases","followingday");
	  REQUIRE(mode,var,ftime,period,"fi","");
	  REQUIRE(mode,var,ftime,period,"sv","");
	  REQUIRE(mode,var,ftime,period,"en","");

	  NFmiSettings::Set(mode+"::days::phrases","weekday");
	  REQUIRE(mode,var,ftime,period,"fi","Sunnuntaista alkaen.");
	  REQUIRE(mode,var,ftime,period,"sv","Från söndagen.");
	  REQUIRE(mode,var,ftime,period,"en","From Sunday onwards.");
	  
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
	  TEST(until_tonight);
	  TEST(until_morning);
	  TEST(today);
	  TEST(tonight);
	  TEST(next_night);
	  TEST(next_day);
	  TEST(next_days);
	  TEST(days);
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
