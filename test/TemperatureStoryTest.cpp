#include <regression/tframe.h>
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "PlainTextFormatter.h"
#include "TemperatureStory.h"
#include "Story.h"

#include <newbase/NFmiSettings.h>

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;


namespace TemperatureStoryTest
{
  shared_ptr<TextGen::Dictionary> dict;
  TextGen::PlainTextFormatter formatter;
  

  void require(const TextGen::Story & theStory,
			   const string & theLanguage,
			   const string & theName,
			   const string & theExpected)
  {
	dict->init(theLanguage);
	formatter.dictionary(dict);

	TextGen::Paragraph para = theStory.makeStory(theName);
	const string value = para.realize(formatter);

	if(value != theExpected)
	  {
		const string msg = value + " <> " + theExpected;
		TEST_FAILED(msg.c_str());
	  }
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TemperatureStory::day()
   */
  // ----------------------------------------------------------------------

  void temperature_day()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("25,60");

	// 1 day forecast

	{
	  NFmiTime time1(2000,1,1,6);
	  NFmiTime time2(2000,1,2,6);
	  WeatherPeriod period(time1,time2);
	  TemperatureStory story(time1,sources,area,period,"day");
	  
	  const string fun = "temperature_day";
	  
	  NFmiSettings::Set("day::day::starthour","6");
	  NFmiSettings::Set("day::day::endhour","18");
	  NFmiSettings::Set("day::night::starthour","18");
	  NFmiSettings::Set("day::night::endhour","6");
	  
	  NFmiSettings::Set("day::comparison::significantly_higher","5");
	  NFmiSettings::Set("day::comparison::higher","3");
	  NFmiSettings::Set("day::comparison::somewhat_higher","2");
	  NFmiSettings::Set("day::comparison::somewhat_lower","2");
	  NFmiSettings::Set("day::comparison::lower","3");
	  NFmiSettings::Set("day::comparison::significantly_lower","5");
	  
	  
	  NFmiSettings::Set("day::fake::day1::area::mean","15,0");
	  NFmiSettings::Set("day::fake::day1::area::min","15,0");
	  NFmiSettings::Set("day::fake::day1::area::max","15,0");
	  
	  NFmiSettings::Set("day::fake::day1::coast::mean","15,0");
	  NFmiSettings::Set("day::fake::day1::coast::min","15,0");
	  NFmiSettings::Set("day::fake::day1::coast::max","15,0");
	  
	  NFmiSettings::Set("day::fake::day1::inland::mean","15,0");
	  NFmiSettings::Set("day::fake::day1::inland::min","15,0");
	  NFmiSettings::Set("day::fake::day1::inland::max","15,0");
	  
	  NFmiSettings::Set("day::fake::night1::area::mean","15,0");
	  NFmiSettings::Set("day::fake::night1::area::min","15,0");
	  NFmiSettings::Set("day::fake::night1::area::max","15,0");
	  
	  NFmiSettings::Set("day::fake::night1::coast::mean","15,0");
	  NFmiSettings::Set("day::fake::night1::coast::min","15,0");
	  NFmiSettings::Set("day::fake::night1::coast::max","15,0");
	  
	  NFmiSettings::Set("day::fake::night1::inland::mean","15,0");
	  NFmiSettings::Set("day::fake::night1::inland::min","15,0");
	  NFmiSettings::Set("day::fake::night1::inland::max","15,0");
	  
	  require(story,"fi",fun,"Lämpötila on noin 15 astetta.");
	  require(story,"sv",fun,"Temperatur är cirka 15 grader.");
	  require(story,"en",fun,"Temperature is about 15 degrees.");
	  
	  NFmiSettings::Set("day::fake::day1::coast::mean","10,0");
	  NFmiSettings::Set("day::fake::day1::coast::min","8,0");
	  NFmiSettings::Set("day::fake::day1::coast::max","12,0");
	  
	  NFmiSettings::Set("day::fake::day1::inland::mean","16,0");
	  NFmiSettings::Set("day::fake::day1::inland::min","15,0");
	  NFmiSettings::Set("day::fake::day1::inland::max","14,0");
	  
	  require(story,"fi",fun,"Lämpötila on noin 16 astetta, rannikolla alempi.");
	  require(story,"sv",fun,"Temperatur är cirka 16 grader, vid kusten lägre.");
	  require(story,"en",fun,"Temperature is about 16 degrees, on the coastal area lower.");
	  
	  NFmiSettings::Set("day::fake::night1::area::mean","10,0");
	  NFmiSettings::Set("day::fake::night1::area::min","10,0");
	  NFmiSettings::Set("day::fake::night1::area::max","10,0");
	  
	  NFmiSettings::Set("day::fake::night1::coast::mean","10,0");
	  NFmiSettings::Set("day::fake::night1::coast::min","10,0");
	  NFmiSettings::Set("day::fake::night1::coast::max","10,0");
	  
	  NFmiSettings::Set("day::fake::night1::inland::mean","10,0");
	  NFmiSettings::Set("day::fake::night1::inland::min","10,0");
	  NFmiSettings::Set("day::fake::night1::inland::max","10,0");
	  
	  require(story,"fi",fun,"Päivän ylin lämpötila on noin 16 astetta, rannikolla huomattavasti alempi. Yön alin lämpötila on noin 10 astetta.");
	  require(story,"sv",fun,"Dagens högsta temperatur är cirka 16 grader, vid kusten betydligt lägre. Nattens lägsta temperatur är cirka 10 grader.");
	  require(story,"en",fun,"The maximum day temperature is about 16 degrees, on the coastal area significantly lower. The minimum night temperature is about 10 degrees.");
	  
	  NFmiSettings::Set("day::fake::day1::coast::mean","-10,0");
	  NFmiSettings::Set("day::fake::day1::coast::min","-12,0");
	  NFmiSettings::Set("day::fake::day1::coast::max","-8,0");
	  
	  NFmiSettings::Set("day::fake::night1::coast::mean","7,0");
	  NFmiSettings::Set("day::fake::night1::coast::min","7,0");
	  NFmiSettings::Set("day::fake::night1::coast::max","7,0");
	  
	  require(story,"fi",fun,"Päivän ylin lämpötila on noin 16 astetta, rannikolla -8...-12 astetta. Yön alin lämpötila on noin 10 astetta, rannikolla alempi.");
	  require(story,"sv",fun,"Dagens högsta temperatur är cirka 16 grader, vid kusten -8...-12 grader. Nattens lägsta temperatur är cirka 10 grader, vid kusten lägre.");
	  require(story,"en",fun,"The maximum day temperature is about 16 degrees, on the coastal area -8...-12 degrees. The minimum night temperature is about 10 degrees, on the coastal area lower.");

	}

	// 1 1/2 days

	{
	  NFmiTime time1(2000,1,1,6);
	  NFmiTime time2(2000,1,2,18);
	  WeatherPeriod period(time1,time2);
	  TemperatureStory story(time1,sources,area,period,"day");
	  
	  const string fun = "temperature_day";

	  NFmiSettings::Set("day::today::phrases","today,tomorrow,weekday");
	  
	  NFmiSettings::Set("day::day::starthour","6");
	  NFmiSettings::Set("day::day::endhour","18");
	  NFmiSettings::Set("day::night::starthour","18");
	  NFmiSettings::Set("day::night::endhour","6");
	  
	  NFmiSettings::Set("day::comparison::significantly_higher","5");
	  NFmiSettings::Set("day::comparison::higher","3");
	  NFmiSettings::Set("day::comparison::somewhat_higher","2");
	  NFmiSettings::Set("day::comparison::somewhat_lower","2");
	  NFmiSettings::Set("day::comparison::lower","3");
	  NFmiSettings::Set("day::comparison::significantly_lower","5");
	  
	  
	  NFmiSettings::Set("day::fake::day1::area::mean","15,0");
	  NFmiSettings::Set("day::fake::day1::area::min","15,0");
	  NFmiSettings::Set("day::fake::day1::area::max","15,0");
	  
	  NFmiSettings::Set("day::fake::day1::coast::mean","15,0");
	  NFmiSettings::Set("day::fake::day1::coast::min","15,0");
	  NFmiSettings::Set("day::fake::day1::coast::max","15,0");
	  
	  NFmiSettings::Set("day::fake::day1::inland::mean","15,0");
	  NFmiSettings::Set("day::fake::day1::inland::min","15,0");
	  NFmiSettings::Set("day::fake::day1::inland::max","15,0");
	  
	  NFmiSettings::Set("day::fake::night1::area::mean","15,0");
	  NFmiSettings::Set("day::fake::night1::area::min","15,0");
	  NFmiSettings::Set("day::fake::night1::area::max","15,0");
	  
	  NFmiSettings::Set("day::fake::night1::coast::mean","15,0");
	  NFmiSettings::Set("day::fake::night1::coast::min","15,0");
	  NFmiSettings::Set("day::fake::night1::coast::max","15,0");
	  
	  NFmiSettings::Set("day::fake::night1::inland::mean","15,0");
	  NFmiSettings::Set("day::fake::night1::inland::min","15,0");
	  NFmiSettings::Set("day::fake::night1::inland::max","15,0");
	  
	  NFmiSettings::Set("day::fake::day2::area::mean","11,0");
	  NFmiSettings::Set("day::fake::day2::area::min","10,0");
	  NFmiSettings::Set("day::fake::day2::area::max","13,0");
	  
	  NFmiSettings::Set("day::fake::day2::coast::mean","5,0");
	  NFmiSettings::Set("day::fake::day2::coast::min","0,0");
	  NFmiSettings::Set("day::fake::day2::coast::max","15,0");
	  
	  NFmiSettings::Set("day::fake::day2::inland::mean","20,0");
	  NFmiSettings::Set("day::fake::day2::inland::min","15,0");
	  NFmiSettings::Set("day::fake::day2::inland::max","25,0");

	  require(story,"fi",fun,"Lämpötila on tänään noin 15 astetta. Päivän ylin lämpötila on huomenna 15...25 astetta, rannikolla 0...15 astetta.");
	  require(story,"sv",fun,"Temperatur är i dag cirka 15 grader. Dagens högsta temperatur är i morgon 15...25 grader, vid kusten 0...15 grader.");
	  require(story,"en",fun,"Temperature is today about 15 degrees. The maximum day temperature is tomorrow 15...25 degrees, on the coastal area 0...15 degrees.");
	  
	  NFmiSettings::Set("day::fake::day1::coast::mean","10,0");
	  NFmiSettings::Set("day::fake::day1::coast::min","8,0");
	  NFmiSettings::Set("day::fake::day1::coast::max","12,0");
	  
	  NFmiSettings::Set("day::fake::day1::inland::mean","16,0");
	  NFmiSettings::Set("day::fake::day1::inland::min","15,0");
	  NFmiSettings::Set("day::fake::day1::inland::max","14,0");
	  
	  require(story,"fi",fun,"Lämpötila on tänään noin 16 astetta, rannikolla alempi. Päivän ylin lämpötila on huomenna 15...25 astetta, rannikolla 0...15 astetta.");
	  require(story,"sv",fun,"Temperatur är i dag cirka 16 grader, vid kusten lägre. Dagens högsta temperatur är i morgon 15...25 grader, vid kusten 0...15 grader.");
	  require(story,"en",fun,"Temperature is today about 16 degrees, on the coastal area lower. The maximum day temperature is tomorrow 15...25 degrees, on the coastal area 0...15 degrees.");
	  
	  NFmiSettings::Set("day::fake::night1::area::mean","10,0");
	  NFmiSettings::Set("day::fake::night1::area::min","10,0");
	  NFmiSettings::Set("day::fake::night1::area::max","10,0");
	  
	  NFmiSettings::Set("day::fake::night1::coast::mean","10,0");
	  NFmiSettings::Set("day::fake::night1::coast::min","10,0");
	  NFmiSettings::Set("day::fake::night1::coast::max","10,0");
	  
	  NFmiSettings::Set("day::fake::night1::inland::mean","10,0");
	  NFmiSettings::Set("day::fake::night1::inland::min","10,0");
	  NFmiSettings::Set("day::fake::night1::inland::max","10,0");
	  
	  require(story,"fi",fun,"Päivän ylin lämpötila on tänään noin 16 astetta, rannikolla huomattavasti alempi. Yön alin lämpötila on noin 10 astetta. Päivän ylin lämpötila on huomenna 15...25 astetta, rannikolla 0...15 astetta.");
	  require(story,"sv",fun,"Dagens högsta temperatur är i dag cirka 16 grader, vid kusten betydligt lägre. Nattens lägsta temperatur är cirka 10 grader. Dagens högsta temperatur är i morgon 15...25 grader, vid kusten 0...15 grader.");
	  require(story,"en",fun,"The maximum day temperature is today about 16 degrees, on the coastal area significantly lower. The minimum night temperature is about 10 degrees. The maximum day temperature is tomorrow 15...25 degrees, on the coastal area 0...15 degrees.");
	  
	  NFmiSettings::Set("day::fake::day1::coast::mean","-10,0");
	  NFmiSettings::Set("day::fake::day1::coast::min","-12,0");
	  NFmiSettings::Set("day::fake::day1::coast::max","-8,0");
	  
	  NFmiSettings::Set("day::fake::night1::coast::mean","7,0");
	  NFmiSettings::Set("day::fake::night1::coast::min","7,0");
	  NFmiSettings::Set("day::fake::night1::coast::max","7,0");
	  
	  require(story,"fi",fun,"Päivän ylin lämpötila on tänään noin 16 astetta, rannikolla -8...-12 astetta. Yön alin lämpötila on noin 10 astetta, rannikolla alempi. Päivän ylin lämpötila on huomenna 15...25 astetta, rannikolla 0...15 astetta.");
	  require(story,"sv",fun,"Dagens högsta temperatur är i dag cirka 16 grader, vid kusten -8...-12 grader. Nattens lägsta temperatur är cirka 10 grader, vid kusten lägre. Dagens högsta temperatur är i morgon 15...25 grader, vid kusten 0...15 grader.");
	  require(story,"en",fun,"The maximum day temperature is today about 16 degrees, on the coastal area -8...-12 degrees. The minimum night temperature is about 10 degrees, on the coastal area lower. The maximum day temperature is tomorrow 15...25 degrees, on the coastal area 0...15 degrees.");

	}

	TEST_PASSED();

  }
  // ----------------------------------------------------------------------
  /*!
   * \brief Test TemperatureStory::mean()
   */
  // ----------------------------------------------------------------------

  void temperature_mean()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("25,60");
	NFmiTime time1(2000,1,1);
	NFmiTime time2(2000,1,2);
	WeatherPeriod period(time1,time2);
	TemperatureStory story(time1,sources,area,period,"mean");

	const string fun = "temperature_mean";

	NFmiSettings::Set("mean::fake::mean","0.1,0");
	require(story,"fi",fun,"Keskilämpötila 0 astetta.");
	require(story,"sv",fun,"Medeltemperaturen 0 grader.");
	require(story,"en",fun,"Mean temperature 0 degrees.");

	NFmiSettings::Set("mean::fake::mean","0.5,0");
	require(story,"fi",fun,"Keskilämpötila 1 astetta.");
	require(story,"sv",fun,"Medeltemperaturen 1 grader.");
	require(story,"en",fun,"Mean temperature 1 degrees.");

	NFmiSettings::Set("mean::fake::mean","10,0");
	require(story,"fi",fun,"Keskilämpötila 10 astetta.");
	require(story,"sv",fun,"Medeltemperaturen 10 grader.");
	require(story,"en",fun,"Mean temperature 10 degrees.");

	NFmiSettings::Set("mean::fake::mean","-10.5,0");
	require(story,"fi",fun,"Keskilämpötila -11 astetta.");
	require(story,"sv",fun,"Medeltemperaturen -11 grader.");
	require(story,"en",fun,"Mean temperature -11 degrees.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TemperatureStory::meanmax()
   */
  // ----------------------------------------------------------------------

  void temperature_meanmax()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("25,60");
	NFmiTime time1(2000,1,1);
	NFmiTime time2(2000,1,2);
	WeatherPeriod period(time1,time2);
	TemperatureStory story(time1,sources,area,period,"meanmax");

	const string fun = "temperature_meanmax";

	NFmiSettings::Set("meanmax::fake::mean","0.1,0");
	require(story,"fi",fun,"Keskimääräinen ylin lämpötila 0 astetta.");
	require(story,"sv",fun,"Maximitemperaturen i medeltal 0 grader.");
	require(story,"en",fun,"Mean maximum temperature 0 degrees.");

	NFmiSettings::Set("meanmax::fake::mean","0.5,0");
	require(story,"fi",fun,"Keskimääräinen ylin lämpötila 1 astetta.");
	require(story,"sv",fun,"Maximitemperaturen i medeltal 1 grader.");
	require(story,"en",fun,"Mean maximum temperature 1 degrees.");

	NFmiSettings::Set("meanmax::fake::mean","10,0");
	require(story,"fi",fun,"Keskimääräinen ylin lämpötila 10 astetta.");
	require(story,"sv",fun,"Maximitemperaturen i medeltal 10 grader.");
	require(story,"en",fun,"Mean maximum temperature 10 degrees.");

	NFmiSettings::Set("meanmax::fake::mean","-10.5,0");
	require(story,"fi",fun,"Keskimääräinen ylin lämpötila -11 astetta.");
	require(story,"sv",fun,"Maximitemperaturen i medeltal -11 grader.");
	require(story,"en",fun,"Mean maximum temperature -11 degrees.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TemperatureStory::meanmin()
   */
  // ----------------------------------------------------------------------

  void temperature_meanmin()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("25,60");
	NFmiTime time1(2000,1,1);
	NFmiTime time2(2000,1,2);
	WeatherPeriod period(time1,time2);
	TemperatureStory story(time1,sources,area,period,"meanmin");

	const string fun = "temperature_meanmin";

	NFmiSettings::Set("meanmin::fake::mean","0.1,0");
	require(story,"fi",fun,"Keskimääräinen alin lämpötila 0 astetta.");
	require(story,"sv",fun,"Minimitemperaturen i medeltal 0 grader.");
	require(story,"en",fun,"Mean minimum temperature 0 degrees.");

	NFmiSettings::Set("meanmin::fake::mean","0.5,0");
	require(story,"fi",fun,"Keskimääräinen alin lämpötila 1 astetta.");
	require(story,"sv",fun,"Minimitemperaturen i medeltal 1 grader.");
	require(story,"en",fun,"Mean minimum temperature 1 degrees.");

	NFmiSettings::Set("meanmin::fake::mean","10,0");
	require(story,"fi",fun,"Keskimääräinen alin lämpötila 10 astetta.");
	require(story,"sv",fun,"Minimitemperaturen i medeltal 10 grader.");
	require(story,"en",fun,"Mean minimum temperature 10 degrees.");

	NFmiSettings::Set("meanmin::fake::mean","-10.5,0");
	require(story,"fi",fun,"Keskimääräinen alin lämpötila -11 astetta.");
	require(story,"sv",fun,"Minimitemperaturen i medeltal -11 grader.");
	require(story,"en",fun,"Mean minimum temperature -11 degrees.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TemperatureStory::dailymax()
   */
  // ----------------------------------------------------------------------

  void temperature_dailymax()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("25,60");
	const string fun = "temperature_dailymax";

	NFmiSettings::Set("dailymax::comparison::significantly_higher","6");
	NFmiSettings::Set("dailymax::comparison::higher","4");
	NFmiSettings::Set("dailymax::comparison::somewhat_higher","2");
	NFmiSettings::Set("dailymax::comparison::somewhat_lower","2");
	NFmiSettings::Set("dailymax::comparison::lower","4");
	NFmiSettings::Set("dailymax::comparison::significantly_lower","6");
	NFmiSettings::Set("dailymax::mininterval","2");
	NFmiSettings::Set("dailymax::always_interval_zero","true");
	NFmiSettings::Set("dailymax::day::starthour","6");
	NFmiSettings::Set("dailymax::day::endhour","18");
	NFmiSettings::Set("dailymax::today::phrases","weekday");
	NFmiSettings::Set("dailymax::next_day::phrases","followingday");

	// Test the part concerning day 1
	{
	  NFmiTime time1(2003,6,1);
	  NFmiTime time2(2003,6,2);
	  WeatherPeriod period(time1,time2);
	  TemperatureStory story(time1,sources,area,period,"dailymax");
	
	  NFmiSettings::Set("dailymax::fake::day1::minimum","5,0");
	  NFmiSettings::Set("dailymax::fake::day1::mean","5,0");
	  NFmiSettings::Set("dailymax::fake::day1::maximum","5,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina noin 5 astetta.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen cirka 5 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday about 5 degrees.");

	  NFmiSettings::Set("dailymax::fake::day1::minimum","5,0");
	  NFmiSettings::Set("dailymax::fake::day1::mean","6,0");
	  NFmiSettings::Set("dailymax::fake::day1::maximum","6,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina noin 6 astetta.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen cirka 6 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday about 6 degrees.");

	  NFmiSettings::Set("dailymax::fake::day1::minimum","5,0");
	  NFmiSettings::Set("dailymax::fake::day1::mean","6,0");
	  NFmiSettings::Set("dailymax::fake::day1::maximum","7,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina 5...7 astetta.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen 5...7 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees.");

	  NFmiSettings::Set("dailymax::fake::day1::minimum","0,0");
	  NFmiSettings::Set("dailymax::fake::day1::mean","0,0");
	  NFmiSettings::Set("dailymax::fake::day1::maximum","0,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina noin 0 astetta.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen cirka 0 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday about 0 degrees.");

	  NFmiSettings::Set("dailymax::fake::day1::minimum","0,0");
	  NFmiSettings::Set("dailymax::fake::day1::mean","0,0");
	  NFmiSettings::Set("dailymax::fake::day1::maximum","1,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina 0...1 astetta.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen 0...1 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 0...1 degrees.");

	  NFmiSettings::Set("dailymax::fake::day1::minimum","-1,0");
	  NFmiSettings::Set("dailymax::fake::day1::mean","0,0");
	  NFmiSettings::Set("dailymax::fake::day1::maximum","0,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina -1...0 astetta.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen -1...0 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday -1...0 degrees.");

	  NFmiSettings::Set("dailymax::fake::day1::minimum","-1,0");
	  NFmiSettings::Set("dailymax::fake::day1::mean","0,0");
	  NFmiSettings::Set("dailymax::fake::day1::maximum","1,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina -1...1 astetta.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen -1...1 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday -1...1 degrees.");

	}

	// Test the part concerning day 2
	{
	  NFmiTime time1(2003,6,1);
	  NFmiTime time2(2003,6,3);
	  WeatherPeriod period(time1,time2);
	  TemperatureStory story(time1,sources,area,period,"dailymax");
	
	  NFmiSettings::Set("dailymax::fake::day1::minimum","5,0");
	  NFmiSettings::Set("dailymax::fake::day1::mean","6,0");
	  NFmiSettings::Set("dailymax::fake::day1::maximum","7,0");

	  // change 0 degrees
	  NFmiSettings::Set("dailymax::fake::day2::minimum","5,0");
	  NFmiSettings::Set("dailymax::fake::day2::mean","6,0");
	  NFmiSettings::Set("dailymax::fake::day2::maximum","7,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina 5...7 astetta, seuraavana päivänä suunnilleen sama.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen 5...7 grader, följande dag ungefär densamma.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day about the same.");

	  // change 1 degrees
	  NFmiSettings::Set("dailymax::fake::day2::minimum","6,0");
	  NFmiSettings::Set("dailymax::fake::day2::mean","7,0");
	  NFmiSettings::Set("dailymax::fake::day2::maximum","8,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina 5...7 astetta, seuraavana päivänä suunnilleen sama.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen 5...7 grader, följande dag ungefär densamma.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day about the same.");

	  // change 2 degrees
	  NFmiSettings::Set("dailymax::fake::day2::minimum","7,0");
	  NFmiSettings::Set("dailymax::fake::day2::mean","8,0");
	  NFmiSettings::Set("dailymax::fake::day2::maximum","9,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina 5...7 astetta, seuraavana päivänä hieman korkeampi.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen 5...7 grader, följande dag något högre.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day somewhat higher.");

	  // change 4 degrees
	  NFmiSettings::Set("dailymax::fake::day2::minimum","9,0");
	  NFmiSettings::Set("dailymax::fake::day2::mean","10,0");
	  NFmiSettings::Set("dailymax::fake::day2::maximum","11,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina 5...7 astetta, seuraavana päivänä korkeampi.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen 5...7 grader, följande dag högre.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day higher.");

	  // change 6 degrees
	  NFmiSettings::Set("dailymax::fake::day2::minimum","11,0");
	  NFmiSettings::Set("dailymax::fake::day2::mean","12,0");
	  NFmiSettings::Set("dailymax::fake::day2::maximum","13,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina 5...7 astetta, seuraavana päivänä huomattavasti korkeampi.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen 5...7 grader, följande dag betydligt högre.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day significantly higher.");

	  // change - 2 degrees
	  NFmiSettings::Set("dailymax::fake::day2::minimum","3,0");
	  NFmiSettings::Set("dailymax::fake::day2::mean","4,0");
	  NFmiSettings::Set("dailymax::fake::day2::maximum","5,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina 5...7 astetta, seuraavana päivänä hieman alempi.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen 5...7 grader, följande dag något lägre.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day somewhat lower.");

	  // change -4 degrees
	  NFmiSettings::Set("dailymax::fake::day2::minimum","1,0");
	  NFmiSettings::Set("dailymax::fake::day2::mean","2,0");
	  NFmiSettings::Set("dailymax::fake::day2::maximum","3,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina 5...7 astetta, seuraavana päivänä alempi.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen 5...7 grader, följande dag lägre.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day lower.");

	  // change -6 degrees
	  NFmiSettings::Set("dailymax::fake::day2::minimum","-1,0");
	  NFmiSettings::Set("dailymax::fake::day2::mean","0,0");
	  NFmiSettings::Set("dailymax::fake::day2::maximum","1,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina 5...7 astetta, seuraavana päivänä huomattavasti alempi.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen 5...7 grader, följande dag betydligt lägre.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day significantly lower.");

	}

	// Test the part concerning days 3 and 4
	{
	  NFmiTime time1(2003,6,1);
	  NFmiTime time2(2003,6,5);
	  WeatherPeriod period(time1,time2);
	  TemperatureStory story(time1,sources,area,period,"dailymax");
	
	  NFmiSettings::Set("dailymax::fake::day1::minimum","5,0");
	  NFmiSettings::Set("dailymax::fake::day1::mean","6,0");
	  NFmiSettings::Set("dailymax::fake::day1::maximum","7,0");

	  NFmiSettings::Set("dailymax::fake::day2::minimum","5,0");
	  NFmiSettings::Set("dailymax::fake::day2::mean","6,0");
	  NFmiSettings::Set("dailymax::fake::day2::maximum","7,0");

	  NFmiSettings::Set("dailymax::fake::day3::minimum","8,0");
	  NFmiSettings::Set("dailymax::fake::day3::mean","10,0");
	  NFmiSettings::Set("dailymax::fake::day3::maximum","12,0");

	  NFmiSettings::Set("dailymax::fake::day4::minimum","3,0");
	  NFmiSettings::Set("dailymax::fake::day4::mean","4,0");
	  NFmiSettings::Set("dailymax::fake::day4::maximum","4,0");

	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina 5...7 astetta, seuraavana päivänä suunnilleen sama, tiistaina 8...12 astetta, keskiviikkona noin 4 astetta.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen 5...7 grader, följande dag ungefär densamma, på tisdagen 8...12 grader, på onsdagen cirka 4 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day about the same, on Tuesday 8...12 degrees, on Wednesday about 4 degrees.");

	}

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TemperatureStory::weekly_minmax()
   */
  // ----------------------------------------------------------------------

  void temperature_weekly_minmax()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("25,60");
	NFmiTime time1(2000,1,1);
	NFmiTime time2(2000,1,5);
	WeatherPeriod period(time1,time2);
	TemperatureStory story(time1,sources,area,period,"weekly_minmax");

	const string fun = "temperature_weekly_minmax";

	NFmiSettings::Set("weekly_minmax::day::starthour","6");
	NFmiSettings::Set("weekly_minmax::day::endhour","18");
	NFmiSettings::Set("weekly_minmax::night::starthour","18");
	NFmiSettings::Set("weekly_minmax::night::endhour","6");

	NFmiSettings::Set("weekly_minmax::fake::day::minimum","10,0");
	NFmiSettings::Set("weekly_minmax::fake::day::mean","12,0");
	NFmiSettings::Set("weekly_minmax::fake::day::maximum","15,0");
	NFmiSettings::Set("weekly_minmax::fake::night::minimum","2,0");
	NFmiSettings::Set("weekly_minmax::fake::night::mean","4,0");
	NFmiSettings::Set("weekly_minmax::fake::night::maximum","5,0");
	require(story,"fi",fun,"Päivien ylin lämpötila on 10...15 astetta, öiden alin lämpötila 2...5 astetta.");
	require(story,"sv",fun,"Dagens maximi temperaturer är 10...15 grader, nattens minimi temperaturer 2...5 grader.");
	require(story,"en",fun,"Daily maximum temperature is 10...15 degrees, nightly minimum temperature 2...5 degrees.");

	NFmiSettings::Set("weekly_minmax::night::mininterval","4");
	require(story,"fi",fun,"Päivien ylin lämpötila on 10...15 astetta, öiden alin lämpötila noin 4 astetta.");
	require(story,"sv",fun,"Dagens maximi temperaturer är 10...15 grader, nattens minimi temperaturer cirka 4 grader.");
	require(story,"en",fun,"Daily maximum temperature is 10...15 degrees, nightly minimum temperature about 4 degrees.");

	NFmiSettings::Set("weekly_minmax::day::mininterval","6");
	NFmiSettings::Set("weekly_minmax::night::mininterval","2");
	require(story,"fi",fun,"Päivien ylin lämpötila on noin 12 astetta, öiden alin lämpötila 2...5 astetta.");
	require(story,"sv",fun,"Dagens maximi temperaturer är cirka 12 grader, nattens minimi temperaturer 2...5 grader.");
	require(story,"en",fun,"Daily maximum temperature is about 12 degrees, nightly minimum temperature 2...5 degrees.");

	NFmiSettings::Set("weekly_minmax::emphasize_night_minimum","true");
	require(story,"fi",fun,"Päivien ylin lämpötila on noin 12 astetta, öiden alin lämpötila noin 2 astetta.");
	require(story,"sv",fun,"Dagens maximi temperaturer är cirka 12 grader, nattens minimi temperaturer cirka 2 grader.");
	require(story,"en",fun,"Daily maximum temperature is about 12 degrees, nightly minimum temperature about 2 degrees.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TemperatureStory::weekly_averages()
   */
  // ----------------------------------------------------------------------

  void temperature_weekly_averages()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("25,60");
	NFmiTime time1(2000,1,1);
	NFmiTime time2(2000,1,5);
	WeatherPeriod period(time1,time2);
	TemperatureStory story(time1,sources,area,period,"weekly_averages");

	const string fun = "temperature_weekly_averages";

	NFmiSettings::Set("weekly_averages::day::starthour","6");
	NFmiSettings::Set("weekly_averages::day::endhour","18");
	NFmiSettings::Set("weekly_averages::night::starthour","18");
	NFmiSettings::Set("weekly_averages::night::endhour","6");

	NFmiSettings::Set("weekly_averages::fake::day::minimum","10,0");
	NFmiSettings::Set("weekly_averages::fake::day::mean","12,0");
	NFmiSettings::Set("weekly_averages::fake::day::maximum","15,0");
	NFmiSettings::Set("weekly_averages::fake::night::minimum","2,0");
	NFmiSettings::Set("weekly_averages::fake::night::mean","4,0");
	NFmiSettings::Set("weekly_averages::fake::night::maximum","5,0");
	require(story,"fi",fun,"Päivälämpötila on 10...15 astetta, yölämpötila 2...5 astetta.");
	require(story,"sv",fun,"Dagstemperaturen är 10...15 grader, nattemperaturen 2...5 grader.");
	require(story,"en",fun,"Daily temperature is 10...15 degrees, nightly temperature 2...5 degrees.");

	NFmiSettings::Set("weekly_averages::night::mininterval","4");
	require(story,"fi",fun,"Päivälämpötila on 10...15 astetta, yölämpötila noin 4 astetta.");
	require(story,"sv",fun,"Dagstemperaturen är 10...15 grader, nattemperaturen cirka 4 grader.");
	require(story,"en",fun,"Daily temperature is 10...15 degrees, nightly temperature about 4 degrees.");

	NFmiSettings::Set("weekly_averages::day::mininterval","6");
	NFmiSettings::Set("weekly_averages::night::mininterval","2");
	require(story,"fi",fun,"Päivälämpötila on noin 12 astetta, yölämpötila 2...5 astetta.");
	require(story,"sv",fun,"Dagstemperaturen är cirka 12 grader, nattemperaturen 2...5 grader.");
	require(story,"en",fun,"Daily temperature is about 12 degrees, nightly temperature 2...5 degrees.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TemperatureStory::range()
   */
  // ----------------------------------------------------------------------

  void temperature_range()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("25,60");
	const string fun = "temperature_range";

	NFmiSettings::Set("range::mininterval","2");
	NFmiSettings::Set("range::always_interval_zero","true");

	NFmiTime time1(2003,6,1);
	NFmiTime time2(2003,6,2);
	WeatherPeriod period(time1,time2);
	TemperatureStory story(time1,sources,area,period,"range");
	
	NFmiSettings::Set("range::fake::minimum","5,0");
	NFmiSettings::Set("range::fake::mean","5,0");
	NFmiSettings::Set("range::fake::maximum","5,0");
	require(story,"fi",fun,"Lämpötila on noin 5 astetta.");
	require(story,"sv",fun,"Temperatur är cirka 5 grader.");
	require(story,"en",fun,"Temperature is about 5 degrees.");
	
	NFmiSettings::Set("range::fake::minimum","5,0");
	NFmiSettings::Set("range::fake::mean","6,0");
	NFmiSettings::Set("range::fake::maximum","6,0");
	require(story,"fi",fun,"Lämpötila on noin 6 astetta.");
	require(story,"sv",fun,"Temperatur är cirka 6 grader.");
	require(story,"en",fun,"Temperature is about 6 degrees.");
	
	NFmiSettings::Set("range::fake::minimum","5,0");
	NFmiSettings::Set("range::fake::mean","6,0");
	NFmiSettings::Set("range::fake::maximum","7,0");
	require(story,"fi",fun,"Lämpötila on 5...7 astetta.");
	require(story,"sv",fun,"Temperatur är 5...7 grader.");
	require(story,"en",fun,"Temperature is 5...7 degrees.");
	
	NFmiSettings::Set("range::fake::minimum","0,0");
	NFmiSettings::Set("range::fake::mean","0,0");
	NFmiSettings::Set("range::fake::maximum","1,0");
	require(story,"fi",fun,"Lämpötila on 0...1 astetta.");
	require(story,"sv",fun,"Temperatur är 0...1 grader.");
	require(story,"en",fun,"Temperature is 0...1 degrees.");
	
	NFmiSettings::Set("range::fake::minimum","-1,0");
	NFmiSettings::Set("range::fake::mean","0,0");
	NFmiSettings::Set("range::fake::maximum","0,0");
	require(story,"fi",fun,"Lämpötila on -1...0 astetta.");
	require(story,"sv",fun,"Temperatur är -1...0 grader.");
	require(story,"en",fun,"Temperature is -1...0 degrees.");
	
	NFmiSettings::Set("range::fake::minimum","-1,0");
	NFmiSettings::Set("range::fake::mean","0,0");
	NFmiSettings::Set("range::fake::maximum","1,0");
	require(story,"fi",fun,"Lämpötila on -1...1 astetta.");
	require(story,"sv",fun,"Temperatur är -1...1 grader.");
	require(story,"en",fun,"Temperature is -1...1 degrees.");

	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TemperatureStory::max36_hours()
   */
  // ----------------------------------------------------------------------
	struct Max36HoursTestParam 
	{
	  Max36HoursTestParam(const char* d1_inlandmin,
						  const char* d1_inlandmax,
						  const char* d1_inlandmean,
						  const char* d1_coastmin,
						  const char* d1_coastmax,
						  const char* d1_coastmean,
						  const char* d1_areamin,
						  const char* d1_areamax,
						  const char* d1_areamean,

						  const char* nite_inlandmin,
						  const char* nite_inlandmax,
						  const char* nite_inlandmean,
						  const char* nite_coastmin,
						  const char* nite_coastmax,
						  const char* nite_coastmean,
						  const char* nite_areamin,
						  const char* nite_areamax,
						  const char* nite_areamean,

						  const char* d2_inlandmin,
						  const char* d2_inlandmax,
						  const char* d2_inlandmean,
						  const char* d2_coastmin,
						  const char* d2_coastmax,
						  const char* d2_coastmean,
						  const char* d2_areamin,
						  const char* d2_areamax,
						  const char* d2_areamean,

						  const char* story)
	  {
		t36h_d1_inlandmin = d1_inlandmin;
		t36h_d1_inlandmax = d1_inlandmax;
		t36h_d1_inlandmean = d1_inlandmean;
		t36h_d1_coastmin = d1_coastmin;
		t36h_d1_coastmax = d1_coastmax;
		t36h_d1_coastmean = d1_coastmean;
		t36h_d1_areamin = d1_areamin;
		t36h_d1_areamax = d1_areamax;
		t36h_d1_areamean = d1_areamean;

		t36h_nite_inlandmin = nite_inlandmin;
		t36h_nite_inlandmax = nite_inlandmax;
		t36h_nite_inlandmean = nite_inlandmean;
		t36h_nite_coastmin = nite_coastmin;
		t36h_nite_coastmax = nite_coastmax;
		t36h_nite_coastmean = nite_coastmean;
		t36h_nite_areamin = nite_areamin;
		t36h_nite_areamax = nite_areamax;
		t36h_nite_areamean = nite_areamean;

		t36h_d2_inlandmin = d2_inlandmin;
		t36h_d2_inlandmax = d2_inlandmax;
		t36h_d2_inlandmean = d2_inlandmean;
		t36h_d2_coastmin = d2_coastmin;
		t36h_d2_coastmax = d2_coastmax;
		t36h_d2_coastmean = d2_coastmean;
		t36h_d2_areamin = d2_areamin;
		t36h_d2_areamax = d2_areamax;
		t36h_d2_areamean = d2_areamean;

		t36h_story = story;
	  }

	  const char* t36h_d1_inlandmin;
	  const char* t36h_d1_inlandmax;
	  const char* t36h_d1_inlandmean;
	  const char* t36h_d1_coastmin;
	  const char* t36h_d1_coastmax;
	  const char* t36h_d1_coastmean;
	  const char* t36h_d1_areamin;
	  const char* t36h_d1_areamax;
	  const char* t36h_d1_areamean;

	  const char* t36h_nite_inlandmin;
	  const char* t36h_nite_inlandmax;
	  const char* t36h_nite_inlandmean;
	  const char* t36h_nite_coastmin;
	  const char* t36h_nite_coastmax;
	  const char* t36h_nite_coastmean;
	  const char* t36h_nite_areamin;
	  const char* t36h_nite_areamax;
	  const char* t36h_nite_areamean;

	  const char* t36h_d2_inlandmin;
	  const char* t36h_d2_inlandmax;
	  const char* t36h_d2_inlandmean;
	  const char* t36h_d2_coastmin;
	  const char* t36h_d2_coastmax;
	  const char* t36h_d2_coastmean;
	  const char* t36h_d2_areamin;
	  const char* t36h_d2_areamax;
	  const char* t36h_d2_areamean;

	  const char* t36h_story;
	};

	typedef std::map<int, Max36HoursTestParam> Max36HoursTestCases;

  void create_testcases(Max36HoursTestCases& testCases, const string& language)
  {
	if(language == "fi")
	  {
		// no areas and no periods included
		/*
		testCases.insert(make_pair(1,Max36HoursTestParam("32700,0","32700,0","32700,0",
														 "","","",
														 "","","",
														 "32700,0","32700,0","32700,0",
														 "","","",
														 "","","",
														 "32700,0","32700,0","32700,0",
														 "","","",
														 "","","",
														 "")));
		*/
		testCases.insert(make_pair(1,Max36HoursTestParam("6.0,0","13.2,0","8.1,0",
														 "32700,0","32700,0","32700,0",
														 "32700,0","32700,0","32700,0",
														 "1.0,0","5.2,0","3.3,0",
														 "32700,0","32700,0","32700,0",
														 "32700,0","32700,0","32700,0",
														 "5.0,0","11.1,0","7.7,0",
														 "","","",
														 "","","",
														 "")));

	  }
	else if(language == "sv")
	  {
	  }
	else if(language == "en")
	  {
	  }

  }


  void temperature_max36hours()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("25,60");
	const string fun = "temperature_max36hours";

	NFmiSettings::Set("max36hours::day::starthour","6");
	NFmiSettings::Set("max36hours::day::maxstarthour","11");
	NFmiSettings::Set("max36hours::day::endhour","18");
	NFmiSettings::Set("max36hours::night::starthour","18");
	NFmiSettings::Set("max36hours::night::endhour","6");
	NFmiSettings::Set("textgen::main_forecast", "/home/reponen/work/testdata/200904300654_pal_skandinavia_pinta.sqd");

	NFmiTime time1(2009,5,1,6,0,0);
	NFmiTime time2(2009,5,3,18,0,0);
	WeatherPeriod period(time1,time2);
	TemperatureStory story(time1,sources,area,period,"max36hours");


	Max36HoursTestCases testCases;
	Max36HoursTestCases::iterator iter;

	const char* languages [] = {"fi", "sv", "en"};

	for(int i = 0; i < 3; i++)
	  {
		create_testcases(testCases, languages[i]);
		for(iter = testCases.begin(); iter != testCases.end(); iter++)
		  {
			NFmiSettings::Set("max36hours::fake::day1::inland::min",iter->second.t36h_d1_inlandmin);
			NFmiSettings::Set("max36hours::fake::day1::inland::max",iter->second.t36h_d1_inlandmax);
			NFmiSettings::Set("max36hours::fake::day1::inland::mean",iter->second.t36h_d1_inlandmean);
			NFmiSettings::Set("max36hours::fake::day1::coast::min",iter->second.t36h_d1_coastmin);
			NFmiSettings::Set("max36hours::fake::day1::coast::max",iter->second.t36h_d1_coastmax);
			NFmiSettings::Set("max36hours::fake::day1::coast::mean",iter->second.t36h_d1_coastmean);
			NFmiSettings::Set("max36hours::fake::day1::area::min",iter->second.t36h_d1_areamin);
			NFmiSettings::Set("max36hours::fake::day1::area::max",iter->second.t36h_d1_areamax);
			NFmiSettings::Set("max36hours::fake::day1::area::mean",iter->second.t36h_d1_areamean);

			NFmiSettings::Set("max36hours::fake::night::inland::min",iter->second.t36h_nite_inlandmin);
			NFmiSettings::Set("max36hours::fake::night::inland::max",iter->second.t36h_nite_inlandmax);
			NFmiSettings::Set("max36hours::fake::night::inland::mean",iter->second.t36h_nite_inlandmean);
			NFmiSettings::Set("max36hours::fake::night::coast::min",iter->second.t36h_nite_coastmin);
			NFmiSettings::Set("max36hours::fake::night::coast::max",iter->second.t36h_nite_coastmax);
			NFmiSettings::Set("max36hours::fake::night::coast::mean",iter->second.t36h_nite_coastmean);
			NFmiSettings::Set("max36hours::fake::night::area::min",iter->second.t36h_nite_areamin);
			NFmiSettings::Set("max36hours::fake::night::area::max",iter->second.t36h_nite_areamax);
			NFmiSettings::Set("max36hours::fake::night::area::mean",iter->second.t36h_nite_areamean);

			NFmiSettings::Set("max36hours::fake::day2::inland::min",iter->second.t36h_d2_inlandmin);
			NFmiSettings::Set("max36hours::fake::day2::inland::max",iter->second.t36h_d2_inlandmax);
			NFmiSettings::Set("max36hours::fake::day2::inland::mean",iter->second.t36h_d2_inlandmean);
			NFmiSettings::Set("max36hours::fake::day2::coast::min",iter->second.t36h_d2_coastmin);
			NFmiSettings::Set("max36hours::fake::day2::coast::max",iter->second.t36h_d2_coastmax);
			NFmiSettings::Set("max36hours::fake::day2::coast::mean",iter->second.t36h_d2_coastmean);
			NFmiSettings::Set("max36hours::fake::day2::area::min",iter->second.t36h_d2_areamin);
			NFmiSettings::Set("max36hours::fake::day2::area::max",iter->second.t36h_d2_areamax);
			NFmiSettings::Set("max36hours::fake::day2::area::mean",iter->second.t36h_d2_areamean);

			require(story,languages[i],fun,iter->second.t36h_story);
		  }
		testCases.clear();
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
	  TEST(temperature_max36hours);
	  /*
	  TEST(temperature_day);
	  TEST(temperature_mean);
	  TEST(temperature_meanmax);
	  TEST(temperature_meanmin);
	  TEST(temperature_dailymax);
	  TEST(temperature_weekly_minmax);
	  TEST(temperature_weekly_averages);
	  TEST(temperature_range);
	  */
	}

  }; // class tests

} // namespace TemperatureStoryTest


int main(void)
{
  using namespace TemperatureStoryTest;

  cout << endl
	   << "TemperatureStory tests" << endl
	   << "======================" << endl;

  dict = TextGen::DictionaryFactory::create("multimysql");

  NFmiSettings::Init();
  NFmiSettings::Set("textgen::units::celsius::format","phrase");

  dict->init("fi");
  dict->init("sv");
  dict->init("en");

  tests t;
  return t.run();
}
