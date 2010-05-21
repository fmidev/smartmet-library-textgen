#include <regression/tframe.h>
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "PlainTextFormatter.h"
#include "TemperatureStory.h"
#include "Story.h"
#include "MessageLogger.h"
#include "Settings.h"
#include "AnalysisSources.h"
#include "UserWeatherSource.h"
#include "RegularMaskSource.h"

#include <newbase/NFmiStreamQueryData.h>
#include <newbase/NFmiSvgPath.h>
#include <newbase/NFmiGrid.h>
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
			//			cout << endl << msg << endl << endl;
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
	struct AnomalyTestParam
	{
	  AnomalyTestParam(const char* d1_temperature_min,
					   const char* d1_temperature_max,
					   const char* d1_temperature_mean,
					   const char* nite_temperature_min,
					   const char* nite_temperature_max,
					   const char* nite_temperature_mean,
					   const char* d2_temperature_min,
					   const char* d2_temperature_max,
					   const char* d2_temperature_mean,
					   const char* d2_windspeed_morning_min,
					   const char* d2_windspeed_morning_max,
					   const char* d2_windspeed_morning_mean,
					   const char* d2_windspeed_afternoon_min,
					   const char* d2_windspeed_afternoon_max,
					   const char* d2_windspeed_afternoon_mean,
					   const char* d2_windchill_min,
					   const char* d2_windchill_max,
					   const char* d2_windchill_mean,
					   const char* story) :
		anomaly_d1_temperature_min(d1_temperature_min),
		anomaly_d1_temperature_max(d1_temperature_max),
		anomaly_d1_temperature_mean(d1_temperature_mean),
		anomaly_nite_temperature_min(nite_temperature_min),
		anomaly_nite_temperature_max(nite_temperature_max),
		anomaly_nite_temperature_mean(nite_temperature_mean),
		anomaly_d2_temperature_min(d2_temperature_min),
		anomaly_d2_temperature_max(d2_temperature_max),
		anomaly_d2_temperature_mean(d2_temperature_mean),
		anomaly_d2_windspeed_morning_min(d2_windspeed_morning_min),
		anomaly_d2_windspeed_morning_max(d2_windspeed_morning_max),
		anomaly_d2_windspeed_morning_mean(d2_windspeed_morning_mean),
		anomaly_d2_windspeed_afternoon_min(d2_windspeed_afternoon_min),
		anomaly_d2_windspeed_afternoon_max(d2_windspeed_afternoon_max),
		anomaly_d2_windspeed_afternoon_mean(d2_windspeed_afternoon_mean),
		anomaly_d2_windchill_min(d2_windchill_min),
		anomaly_d2_windchill_max(d2_windchill_max),
		anomaly_d2_windchill_mean(d2_windchill_mean),
		anomaly_story(story)
	  {
	  }
	  const char* anomaly_d1_temperature_min;
	  const char* anomaly_d1_temperature_max;
	  const char* anomaly_d1_temperature_mean;
	  const char* anomaly_nite_temperature_min;
	  const char* anomaly_nite_temperature_max;
	  const char* anomaly_nite_temperature_mean;
	  const char* anomaly_d2_temperature_min;
	  const char* anomaly_d2_temperature_max;
	  const char* anomaly_d2_temperature_mean;
	  const char* anomaly_d2_windspeed_morning_min;
	  const char* anomaly_d2_windspeed_morning_max;
	  const char* anomaly_d2_windspeed_morning_mean;
	  const char* anomaly_d2_windspeed_afternoon_min;
	  const char* anomaly_d2_windspeed_afternoon_max;
	  const char* anomaly_d2_windspeed_afternoon_mean;
	  const char* anomaly_d2_windchill_min;
	  const char* anomaly_d2_windchill_max;
	  const char* anomaly_d2_windchill_mean;
	  const char* anomaly_story;
	};

	typedef std::map<int, AnomalyTestParam> AnomalyTestCases;


	struct Max36HoursTestParamStr 
	{
	  Max36HoursTestParamStr(const char* d1_inlandmin,
							 const char* d1_inlandmax,
							 const char* d1_inlandmean,
							 const char* d1_coastmin,
							 const char* d1_coastmax,
							 const char* d1_coastmean,
							 const char* d1_areamin,
							 const char* d1_areamax,
							 const char* d1_areamean,

							 const char* d1_morning_inlandmin,
							 const char* d1_morning_inlandmax,
							 const char* d1_morning_inlandmean,
							 const char* d1_morning_coastmin,
							 const char* d1_morning_coastmax,
							 const char* d1_morning_coastmean,
							 const char* d1_morning_areamin,
							 const char* d1_morning_areamax,
							 const char* d1_morning_areamean,

							 const char* d1_afternoon_inlandmin,
							 const char* d1_afternoon_inlandmax,
							 const char* d1_afternoon_inlandmean,
							 const char* d1_afternoon_coastmin,
							 const char* d1_afternoon_coastmax,
							 const char* d1_afternoon_coastmean,
							 const char* d1_afternoon_areamin,
							 const char* d1_afternoon_areamax,
							 const char* d1_afternoon_areamean,

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

							 const char* d2_morning_inlandmin,
							 const char* d2_morning_inlandmax,
							 const char* d2_morning_inlandmean,
							 const char* d2_morning_coastmin,
							 const char* d2_morning_coastmax,
							 const char* d2_morning_coastmean,
							 const char* d2_morning_areamin,
							 const char* d2_morning_areamax,
							 const char* d2_morning_areamean,

							 const char* d2_afternoon_inlandmin,
							 const char* d2_afternoon_inlandmax,
							 const char* d2_afternoon_inlandmean,
							 const char* d2_afternoon_coastmin,
							 const char* d2_afternoon_coastmax,
							 const char* d2_afternoon_coastmean,
							 const char* d2_afternoon_areamin,
							 const char* d2_afternoon_areamax,
							 const char* d2_afternoon_areamean,
							 const char* story) : 
		t36h_d1_inlandmin(d1_inlandmin),
		t36h_d1_inlandmax(d1_inlandmax),
		t36h_d1_inlandmean(d1_inlandmean),
		t36h_d1_coastmin(d1_coastmin),
		t36h_d1_coastmax(d1_coastmax),
		t36h_d1_coastmean(d1_coastmean),
		t36h_d1_areamin(d1_areamin),
		t36h_d1_areamax(d1_areamax),
		t36h_d1_areamean(d1_areamean),

		t36h_d1_morning_inlandmin(d1_morning_inlandmin),
		t36h_d1_morning_inlandmax(d1_morning_inlandmax),
		t36h_d1_morning_inlandmean(d1_morning_inlandmean),
		t36h_d1_morning_coastmin(d1_morning_coastmin),
		t36h_d1_morning_coastmax(d1_morning_coastmax),
		t36h_d1_morning_coastmean(d1_morning_coastmean),
		t36h_d1_morning_areamin(d1_morning_areamin),
		t36h_d1_morning_areamax(d1_morning_areamax),
		t36h_d1_morning_areamean(d1_morning_areamean),

		t36h_d1_afternoon_inlandmin(d1_afternoon_inlandmin),
		t36h_d1_afternoon_inlandmax(d1_afternoon_inlandmax),
		t36h_d1_afternoon_inlandmean(d1_afternoon_inlandmean),
		t36h_d1_afternoon_coastmin(d1_afternoon_coastmin),
		t36h_d1_afternoon_coastmax(d1_afternoon_coastmax),
		t36h_d1_afternoon_coastmean(d1_afternoon_coastmean),
		t36h_d1_afternoon_areamin(d1_afternoon_areamin),
		t36h_d1_afternoon_areamax(d1_afternoon_areamax),
		t36h_d1_afternoon_areamean(d1_afternoon_areamean),

		t36h_nite_inlandmin(nite_inlandmin),
		t36h_nite_inlandmax(nite_inlandmax),
		t36h_nite_inlandmean(nite_inlandmean),
		t36h_nite_coastmin(nite_coastmin),
		t36h_nite_coastmax(nite_coastmax),
		t36h_nite_coastmean(nite_coastmean),
		t36h_nite_areamin(nite_areamin),
		t36h_nite_areamax(nite_areamax),
		t36h_nite_areamean(nite_areamean),

		t36h_d2_inlandmin(d2_inlandmin),
		t36h_d2_inlandmax(d2_inlandmax),
		t36h_d2_inlandmean(d2_inlandmean),
		t36h_d2_coastmin(d2_coastmin),
		t36h_d2_coastmax(d2_coastmax),
		t36h_d2_coastmean(d2_coastmean),
		t36h_d2_areamin(d2_areamin),
		t36h_d2_areamax(d2_areamax),
		t36h_d2_areamean(d2_areamean),

		t36h_d2_morning_inlandmin(d2_morning_inlandmin),
		t36h_d2_morning_inlandmax(d2_morning_inlandmax),
		t36h_d2_morning_inlandmean(d2_morning_inlandmean),
		t36h_d2_morning_coastmin(d2_morning_coastmin),
		t36h_d2_morning_coastmax(d2_morning_coastmax),
		t36h_d2_morning_coastmean(d2_morning_coastmean),
		t36h_d2_morning_areamin(d2_morning_areamin),
		t36h_d2_morning_areamax(d2_morning_areamax),
		t36h_d2_morning_areamean(d2_morning_areamean),

		t36h_d2_afternoon_inlandmin(d2_afternoon_inlandmin),
		t36h_d2_afternoon_inlandmax(d2_afternoon_inlandmax),
		t36h_d2_afternoon_inlandmean(d2_afternoon_inlandmean),
		t36h_d2_afternoon_coastmin(d2_afternoon_coastmin),
		t36h_d2_afternoon_coastmax(d2_afternoon_coastmax),
		t36h_d2_afternoon_coastmean(d2_afternoon_coastmean),
		t36h_d2_afternoon_areamin(d2_afternoon_areamin),
		t36h_d2_afternoon_areamax(d2_afternoon_areamax),
		t36h_d2_afternoon_areamean(d2_afternoon_areamean),

		t36h_story(story)

	  {}

	  string t36h_d1_inlandmin;
	  string t36h_d1_inlandmax;
	  string t36h_d1_inlandmean;
	  string t36h_d1_coastmin;
	  string t36h_d1_coastmax;
	  string t36h_d1_coastmean;
	  string t36h_d1_areamin;
	  string t36h_d1_areamax;
	  string t36h_d1_areamean;

	  string t36h_d1_morning_inlandmin;
	  string t36h_d1_morning_inlandmax;
	  string t36h_d1_morning_inlandmean;
	  string t36h_d1_morning_coastmin;
	  string t36h_d1_morning_coastmax;
	  string t36h_d1_morning_coastmean;
	  string t36h_d1_morning_areamin;
	  string t36h_d1_morning_areamax;
	  string t36h_d1_morning_areamean;

	  string t36h_d1_afternoon_inlandmin;
	  string t36h_d1_afternoon_inlandmax;
	  string t36h_d1_afternoon_inlandmean;
	  string t36h_d1_afternoon_coastmin;
	  string t36h_d1_afternoon_coastmax;
	  string t36h_d1_afternoon_coastmean;
	  string t36h_d1_afternoon_areamin;
	  string t36h_d1_afternoon_areamax;
	  string t36h_d1_afternoon_areamean;

	  string t36h_nite_inlandmin;
	  string t36h_nite_inlandmax;
	  string t36h_nite_inlandmean;
	  string t36h_nite_coastmin;
	  string t36h_nite_coastmax;
	  string t36h_nite_coastmean;
	  string t36h_nite_areamin;
	  string t36h_nite_areamax;
	  string t36h_nite_areamean;

	  string t36h_d2_inlandmin;
	  string t36h_d2_inlandmax;
	  string t36h_d2_inlandmean;
	  string t36h_d2_coastmin;
	  string t36h_d2_coastmax;
	  string t36h_d2_coastmean;
	  string t36h_d2_areamin;
	  string t36h_d2_areamax;
	  string t36h_d2_areamean;

	  string t36h_d2_morning_inlandmin;
	  string t36h_d2_morning_inlandmax;
	  string t36h_d2_morning_inlandmean;
	  string t36h_d2_morning_coastmin;
	  string t36h_d2_morning_coastmax;
	  string t36h_d2_morning_coastmean;
	  string t36h_d2_morning_areamin;
	  string t36h_d2_morning_areamax;
	  string t36h_d2_morning_areamean;

	  string t36h_d2_afternoon_inlandmin;
	  string t36h_d2_afternoon_inlandmax;
	  string t36h_d2_afternoon_inlandmean;
	  string t36h_d2_afternoon_coastmin;
	  string t36h_d2_afternoon_coastmax;
	  string t36h_d2_afternoon_coastmean;
	  string t36h_d2_afternoon_areamin;
	  string t36h_d2_afternoon_areamax;
	  string t36h_d2_afternoon_areamean;

	  string t36h_story;
	};




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

						  const char* d1_morning_inlandmin,
						  const char* d1_morning_inlandmax,
						  const char* d1_morning_inlandmean,
						  const char* d1_morning_coastmin,
						  const char* d1_morning_coastmax,
						  const char* d1_morning_coastmean,
						  const char* d1_morning_areamin,
						  const char* d1_morning_areamax,
						  const char* d1_morning_areamean,

						  const char* d1_afternoon_inlandmin,
						  const char* d1_afternoon_inlandmax,
						  const char* d1_afternoon_inlandmean,
						  const char* d1_afternoon_coastmin,
						  const char* d1_afternoon_coastmax,
						  const char* d1_afternoon_coastmean,
						  const char* d1_afternoon_areamin,
						  const char* d1_afternoon_areamax,
						  const char* d1_afternoon_areamean,

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

						  const char* d2_morning_inlandmin,
						  const char* d2_morning_inlandmax,
						  const char* d2_morning_inlandmean,
						  const char* d2_morning_coastmin,
						  const char* d2_morning_coastmax,
						  const char* d2_morning_coastmean,
						  const char* d2_morning_areamin,
						  const char* d2_morning_areamax,
						  const char* d2_morning_areamean,

						  const char* d2_afternoon_inlandmin,
						  const char* d2_afternoon_inlandmax,
						  const char* d2_afternoon_inlandmean,
						  const char* d2_afternoon_coastmin,
						  const char* d2_afternoon_coastmax,
						  const char* d2_afternoon_coastmean,
						  const char* d2_afternoon_areamin,
						  const char* d2_afternoon_areamax,
						  const char* d2_afternoon_areamean,

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
	
		t36h_d1_morning_inlandmin = d1_morning_inlandmin;
		t36h_d1_morning_inlandmax = d1_morning_inlandmax;
		t36h_d1_morning_inlandmean = d1_morning_inlandmean;
		t36h_d1_morning_coastmin = d1_morning_coastmin;
		t36h_d1_morning_coastmax = d1_morning_coastmax;
		t36h_d1_morning_coastmean = d1_morning_coastmean;
		t36h_d1_morning_areamin = d1_morning_areamin;
		t36h_d1_morning_areamax = d1_morning_areamax;
		t36h_d1_morning_areamean = d1_morning_areamean;

		t36h_d1_afternoon_inlandmin = d1_afternoon_inlandmin;
		t36h_d1_afternoon_inlandmax = d1_afternoon_inlandmax;
		t36h_d1_afternoon_inlandmean = d1_afternoon_inlandmean;
		t36h_d1_afternoon_coastmin = d1_afternoon_coastmin;
		t36h_d1_afternoon_coastmax = d1_afternoon_coastmax;
		t36h_d1_afternoon_coastmean = d1_afternoon_coastmean;
		t36h_d1_afternoon_areamin = d1_afternoon_areamin;
		t36h_d1_afternoon_areamax = d1_afternoon_areamax;
		t36h_d1_afternoon_areamean = d1_afternoon_areamean;

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

		t36h_d2_morning_inlandmin = d2_morning_inlandmin;
		t36h_d2_morning_inlandmax = d2_morning_inlandmax;
		t36h_d2_morning_inlandmean = d2_morning_inlandmean;
		t36h_d2_morning_coastmin = d2_morning_coastmin;
		t36h_d2_morning_coastmax = d2_morning_coastmax;
		t36h_d2_morning_coastmean = d2_morning_coastmean;
		t36h_d2_morning_areamin = d2_morning_areamin;
		t36h_d2_morning_areamax = d2_morning_areamax;
		t36h_d2_morning_areamean = d2_morning_areamean;

		t36h_d2_afternoon_inlandmin = d2_afternoon_inlandmin;
		t36h_d2_afternoon_inlandmax = d2_afternoon_inlandmax;
		t36h_d2_afternoon_inlandmean = d2_afternoon_inlandmean;
		t36h_d2_afternoon_coastmin = d2_afternoon_coastmin;
		t36h_d2_afternoon_coastmax = d2_afternoon_coastmax;
		t36h_d2_afternoon_coastmean = d2_afternoon_coastmean;
		t36h_d2_afternoon_areamin = d2_afternoon_areamin;
		t36h_d2_afternoon_areamax = d2_afternoon_areamax;
		t36h_d2_afternoon_areamean = d2_afternoon_areamean;

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

	  const char* t36h_d1_morning_inlandmin;
	  const char* t36h_d1_morning_inlandmax;
	  const char* t36h_d1_morning_inlandmean;
	  const char* t36h_d1_morning_coastmin;
	  const char* t36h_d1_morning_coastmax;
	  const char* t36h_d1_morning_coastmean;
	  const char* t36h_d1_morning_areamin;
	  const char* t36h_d1_morning_areamax;
	  const char* t36h_d1_morning_areamean;

	  const char* t36h_d1_afternoon_inlandmin;
	  const char* t36h_d1_afternoon_inlandmax;
	  const char* t36h_d1_afternoon_inlandmean;
	  const char* t36h_d1_afternoon_coastmin;
	  const char* t36h_d1_afternoon_coastmax;
	  const char* t36h_d1_afternoon_coastmean;
	  const char* t36h_d1_afternoon_areamin;
	  const char* t36h_d1_afternoon_areamax;
	  const char* t36h_d1_afternoon_areamean;

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

	  const char* t36h_d2_morning_inlandmin;
	  const char* t36h_d2_morning_inlandmax;
	  const char* t36h_d2_morning_inlandmean;
	  const char* t36h_d2_morning_coastmin;
	  const char* t36h_d2_morning_coastmax;
	  const char* t36h_d2_morning_coastmean;
	  const char* t36h_d2_morning_areamin;
	  const char* t36h_d2_morning_areamax;
	  const char* t36h_d2_morning_areamean;

	  const char* t36h_d2_afternoon_inlandmin;
	  const char* t36h_d2_afternoon_inlandmax;
	  const char* t36h_d2_afternoon_inlandmean;
	  const char* t36h_d2_afternoon_coastmin;
	  const char* t36h_d2_afternoon_coastmax;
	  const char* t36h_d2_afternoon_coastmean;
	  const char* t36h_d2_afternoon_areamin;
	  const char* t36h_d2_afternoon_areamax;
	  const char* t36h_d2_afternoon_areamean;

	  const char* t36h_story;
	};

	typedef std::map<int, Max36HoursTestParam> Max36HoursTestCases;
	typedef std::map<int, Max36HoursTestParamStr> Max36HoursTestCasesStr;

  double random_d()
  {
	return ( (double)rand() / ((double)(RAND_MAX)+(double)(1)) );
  }

  int random_i(unsigned int scale)
  {
	return rand() % scale;
  }

std::ostream & operator<<(std::ostream & theOutput,
						  const Max36HoursTestParamStr& theParam)
{
  theOutput << "***** settings *******" << endl;
  theOutput << " Area minimum d1: " << theParam.t36h_d1_areamin << endl;
  theOutput << " Area maximum d1: " << theParam.t36h_d1_areamax << endl;
  theOutput << " Area mean d1: " << theParam.t36h_d1_areamean << endl;
  theOutput << " Area morning minimum d1: " << theParam.t36h_d1_morning_areamin << endl;
  theOutput << " Area morning maximum d1: " << theParam.t36h_d1_morning_areamax << endl;
  theOutput << " Area morning mean d1: " << theParam.t36h_d1_morning_areamean << endl;
  theOutput << " Area afternoon minimum d1: " << theParam.t36h_d1_afternoon_areamin << endl;
  theOutput << " Area afternoon maximum d1: " << theParam.t36h_d1_afternoon_areamax << endl;
  theOutput << " Area afternoon mean d1: " << theParam.t36h_d1_afternoon_areamean << endl;

  theOutput << " Inland minimum d1: " << theParam.t36h_d1_inlandmin << endl;
  theOutput << " Inland maximum d1: " << theParam.t36h_d1_inlandmax << endl;
  theOutput << " Inland mean d1: " << theParam.t36h_d1_inlandmean << endl;
  theOutput << " Inland morning minimum d1: " << theParam.t36h_d1_morning_inlandmin << endl;
  theOutput << " Inland morning maximum d1: " << theParam.t36h_d1_morning_inlandmax << endl;
  theOutput << " Inland morning mean d1: " << theParam.t36h_d1_morning_inlandmean << endl;
  theOutput << " Inland afternoon minimum d1: " << theParam.t36h_d1_afternoon_inlandmin << endl;
  theOutput << " Inland afternoon maximum d1: " << theParam.t36h_d1_afternoon_inlandmax << endl;
  theOutput << " Inland afternoon mean d1: " << theParam.t36h_d1_afternoon_inlandmean << endl;

  theOutput << " Coast minimum d1: " << theParam.t36h_d1_coastmin << endl;
  theOutput << " Coast maximum d1: " << theParam.t36h_d1_coastmax << endl;
  theOutput << " Coast mean d1: " << theParam.t36h_d1_coastmean << endl;
  theOutput << " Coast morning minimum d1: " << theParam.t36h_d1_morning_coastmin << endl;
  theOutput << " Coast morning maximum d1: " << theParam.t36h_d1_morning_coastmax << endl;
  theOutput << " Coast morning mean d1: " << theParam.t36h_d1_morning_coastmean << endl;
  theOutput << " Coast afternoon minimum d1: " << theParam.t36h_d1_afternoon_coastmin << endl;
  theOutput << " Coast afternoon maximum d1: " << theParam.t36h_d1_afternoon_coastmax << endl;
  theOutput << " Coast afternoon mean d1: " << theParam.t36h_d1_afternoon_coastmean << endl;

  theOutput << " Area minimum night: " << theParam.t36h_nite_areamin << endl;
  theOutput << " Área maximum night: " << theParam.t36h_nite_areamax << endl;
  theOutput << " Area mean night: " << theParam.t36h_nite_areamean << endl;

  theOutput << " Inland minimum night: " << theParam.t36h_nite_inlandmin << endl;
  theOutput << " Inland maximum night: " << theParam.t36h_nite_inlandmax << endl;
  theOutput << " Inland mean night: " << theParam.t36h_nite_inlandmean << endl;

  theOutput << " Coast minimum night: " << theParam.t36h_nite_coastmin << endl;
  theOutput << " Coast maximum night: " << theParam.t36h_nite_coastmax << endl;
  theOutput << " Coast mean night: " << theParam.t36h_nite_coastmean << endl;

  theOutput << " Area minimum d2: " << theParam.t36h_d2_areamin << endl;
  theOutput << " Area maximum d2: " << theParam.t36h_d2_areamax << endl;
  theOutput << " Area mean d2: " << theParam.t36h_d2_areamean << endl;
  theOutput << " Area morning minimum d2: " << theParam.t36h_d2_morning_areamin << endl;
  theOutput << " Area morning maximum d2: " << theParam.t36h_d2_morning_areamax << endl;
  theOutput << " Area morning mean d2: " << theParam.t36h_d2_morning_areamean << endl;
  theOutput << " Area afternoon minimum d2: " << theParam.t36h_d2_afternoon_areamin << endl;
  theOutput << " Area afternoon maximum d2: " << theParam.t36h_d2_afternoon_areamax << endl;
  theOutput << " Area afternoon mean d2: " << theParam.t36h_d2_afternoon_areamean << endl;

  theOutput << " Inland minimum d2: " << theParam.t36h_d2_inlandmin << endl;
  theOutput << " Inland maximum d2: " << theParam.t36h_d2_inlandmax << endl;
  theOutput << " Inland mean d2: " << theParam.t36h_d2_inlandmean << endl;
  theOutput << " Inland morning minimum d2: " << theParam.t36h_d2_morning_inlandmin << endl;
  theOutput << " Inland morning maximum d2: " << theParam.t36h_d2_morning_inlandmax << endl;
  theOutput << " Inland morning mean d2: " << theParam.t36h_d2_morning_inlandmean << endl;
  theOutput << " Inland afternoon minimum d2: " << theParam.t36h_d2_afternoon_inlandmin << endl;
  theOutput << " Inland afternoon maximum d2: " << theParam.t36h_d2_afternoon_inlandmax << endl;
  theOutput << " Inland afternoon mean d2: " << theParam.t36h_d2_afternoon_inlandmean << endl;

  theOutput << " Coast minimum d2: " << theParam.t36h_d2_coastmin << endl;
  theOutput << " Coast maximum d2: " << theParam.t36h_d2_coastmax << endl;
  theOutput << " Coast mean d2: " << theParam.t36h_d2_coastmean << endl;
  theOutput << " Coast morning minimum d2: " << theParam.t36h_d2_morning_coastmin << endl;
  theOutput << " Coast morning maximum d2: " << theParam.t36h_d2_morning_coastmax << endl;
  theOutput << " Coast morning mean d2: " << theParam.t36h_d2_morning_coastmean << endl;
  theOutput << " Coast afternoon minimum d2: " << theParam.t36h_d2_afternoon_coastmin << endl;
  theOutput << " Coast afternoon maximum d2: " << theParam.t36h_d2_afternoon_coastmax << endl;
  theOutput << " Coast afternoon mean d2: " << theParam.t36h_d2_afternoon_coastmean << endl;

  return theOutput;
}
std::ostream & operator<<(std::ostream & theOutput,
						  const AnomalyTestParam& theParam)
{
  theOutput << "***** settings *******" << endl;
  theOutput << " Minimum temperature d1: " << theParam.anomaly_d1_temperature_min << endl;
  theOutput << " Maximum temperature d1: " << theParam.anomaly_d1_temperature_max << endl;
  theOutput << " Mean temperature d1: " << theParam.anomaly_d1_temperature_mean << endl;
  theOutput << " Minimum temperature night: " << theParam.anomaly_nite_temperature_min << endl;
  theOutput << " Maximum temperature night: " << theParam.anomaly_nite_temperature_max << endl;
  theOutput << " Mean temperature night: " << theParam.anomaly_nite_temperature_mean << endl;
  theOutput << " Minimum temperature d2: " << theParam.anomaly_d2_temperature_min << endl;
  theOutput << " Maximum temperature d2: " << theParam.anomaly_d2_temperature_max << endl;
  theOutput << " Mean temperature d2: " << theParam.anomaly_d2_temperature_mean << endl;
  theOutput << " Minimum windspeed morning d2: " << theParam.anomaly_d2_windspeed_morning_min << endl;
  theOutput << " Maximum windspeed morning d2: " << theParam.anomaly_d2_windspeed_morning_max << endl;
  theOutput << " Mean windspeed morning d2: " << theParam.anomaly_d2_windspeed_morning_mean << endl;
  theOutput << " Minimum windspeed afternoon d2: " << theParam.anomaly_d2_windspeed_afternoon_min << endl;
  theOutput << " Maximum windspeed afternoon d2: " << theParam.anomaly_d2_windspeed_afternoon_max << endl;
  theOutput << " Mean windspeed afternoon d2: " << theParam.anomaly_d2_windspeed_afternoon_mean << endl;
  theOutput << " Minimum windchill d2: " << theParam.anomaly_d2_windchill_min << endl;
  theOutput << " Maximum windchill d2: " << theParam.anomaly_d2_windchill_max << endl;
  theOutput << " Mean windchill d2: " << theParam.anomaly_d2_windchill_mean << endl;

  return theOutput;
}


  void create_testcases2(Max36HoursTestCasesStr& testCases, const string& language)
  {
	srand(time(NULL));
	char d1_area_morning_min_str[15], 
	  d1_area_morning_max_str[15], 
	  d1_area_morning_mean_str[15], 
	  d1_area_afternoon_min_str[15], 
	  d1_area_afternoon_max_str[15], 
	  d1_area_afternoon_mean_str[15],
	  d1_inland_morning_min_str[15], 
	  d1_inland_morning_max_str[15], 
	  d1_inland_morning_mean_str[15], 
	  d1_inland_afternoon_min_str[15], 
	  d1_inland_afternoon_max_str[15], 
	  d1_inland_afternoon_mean_str[15],
	  d1_coast_morning_min_str[15], 
	  d1_coast_morning_max_str[15], 
	  d1_coast_morning_mean_str[15], 
	  d1_coast_afternoon_min_str[15], 
	  d1_coast_afternoon_max_str[15], 
	  d1_coast_afternoon_mean_str[15],
	  area_night_min_str[15], 
	  area_night_max_str[15], 
	  area_night_mean_str[15], 
	  inland_night_min_str[15], 
	  inland_night_max_str[15], 
	  inland_night_mean_str[15], 
	  coast_night_min_str[15], 
	  coast_night_max_str[15], 
	  coast_night_mean_str[15], 
	  d1_area_min_str[15], 
	  d1_area_max_str[15],
	  d1_area_mean_str[15],
	  d1_inland_min_str[15], 
	  d1_inland_max_str[15],
	  d1_inland_mean_str[15],
	  d1_coast_min_str[15], 
	  d1_coast_max_str[15],
	  d1_coast_mean_str[15],
	  d2_area_morning_min_str[15], 
	  d2_area_morning_max_str[15], 
	  d2_area_morning_mean_str[15], 
	  d2_area_afternoon_min_str[15], 
	  d2_area_afternoon_max_str[15], 
	  d2_area_afternoon_mean_str[15],
	  d2_inland_morning_min_str[15], 
	  d2_inland_morning_max_str[15], 
	  d2_inland_morning_mean_str[15], 
	  d2_inland_afternoon_min_str[15], 
	  d2_inland_afternoon_max_str[15], 
	  d2_inland_afternoon_mean_str[15],
	  d2_coast_morning_min_str[15], 
	  d2_coast_morning_max_str[15], 
	  d2_coast_morning_mean_str[15], 
	  d2_coast_afternoon_min_str[15], 
	  d2_coast_afternoon_max_str[15], 
	  d2_coast_afternoon_mean_str[15],
	  d2_area_min_str[15], 
	  d2_area_max_str[15],
	  d2_area_mean_str[15],
	  d2_inland_min_str[15], 
	  d2_inland_max_str[15],
	  d2_inland_mean_str[15],
	  d2_coast_min_str[15], 
	  d2_coast_max_str[15],
	  d2_coast_mean_str[15];



	double d1_area_morning_min, 
	  d1_area_morning_max, 
	  d1_area_morning_mean, 
	  d1_area_afternoon_min, 
	  d1_area_afternoon_max, 
	  d1_area_afternoon_mean,
	  d1_inland_morning_min, 
	  d1_inland_morning_max, 
	  d1_inland_morning_mean, 
	  d1_inland_afternoon_min, 
	  d1_inland_afternoon_max, 
	  d1_inland_afternoon_mean,
	  d1_coast_morning_min, 
	  d1_coast_morning_max, 
	  d1_coast_morning_mean, 
	  d1_coast_afternoon_min, 
	  d1_coast_afternoon_max, 
	  d1_coast_afternoon_mean,
	  area_night_min, 
	  area_night_max, 
	  area_night_mean, 
	  inland_night_min, 
	  inland_night_max, 
	  inland_night_mean, 
	  coast_night_min, 
	  coast_night_max, 
	  coast_night_mean, 
	  d1_area_min, 
	  d1_area_max,
	  d1_area_mean,
	  d1_inland_min, 
	  d1_inland_max,
	  d1_inland_mean,
	  d1_coast_min, 
	  d1_coast_max,
	  d1_coast_mean,
	  d2_area_morning_min, 
	  d2_area_morning_max, 
	  d2_area_morning_mean, 
	  d2_area_afternoon_min, 
	  d2_area_afternoon_max, 
	  d2_area_afternoon_mean,
	  d2_inland_morning_min, 
	  d2_inland_morning_max, 
	  d2_inland_morning_mean, 
	  d2_inland_afternoon_min, 
	  d2_inland_afternoon_max, 
	  d2_inland_afternoon_mean,
	  d2_coast_morning_min, 
	  d2_coast_morning_max, 
	  d2_coast_morning_mean, 
	  d2_coast_afternoon_min, 
	  d2_coast_afternoon_max, 
	  d2_coast_afternoon_mean,
	  d2_area_min, 
	  d2_area_max,
	  d2_area_mean,
	  d2_inland_min, 
	  d2_inland_max,
	  d2_inland_mean,
	  d2_coast_min, 
	  d2_coast_max,
	  d2_coast_mean;

	int i = 1;

	for(double d = -30.0 + random_d(); d <= 25.0; d += random_d())
	  {
		d1_area_morning_min = d + random_d();
		d1_area_morning_max = d1_area_morning_min + random_i(2) + random_d();
		d1_area_morning_mean = (d1_area_morning_min+d1_area_morning_max)/2.0;
		d1_inland_morning_min = d1_area_morning_min + random_d();
		d1_inland_morning_max = d1_inland_morning_min + random_d();
		d1_inland_morning_mean = (d1_inland_morning_min+d1_inland_morning_max)/2.0;
		d1_coast_morning_min = d1_inland_morning_min + random_d();
		d1_coast_morning_max = d1_coast_morning_min + random_i(2) + random_d();
		d1_coast_morning_mean = (d1_coast_morning_min+d1_coast_morning_max)/2.0;
		d1_area_afternoon_min = d1_area_morning_max + random_d();
		d1_area_afternoon_max = d1_area_afternoon_min + random_i(7) + random_d();
		d1_area_afternoon_mean = (d1_area_afternoon_min+d1_area_afternoon_max)/2.0;
		d1_inland_afternoon_min = d1_area_afternoon_min + random_d();
		d1_inland_afternoon_max = d1_inland_afternoon_min + random_d();
		d1_inland_afternoon_mean = (d1_inland_afternoon_min+d1_inland_afternoon_max)/2.0;
		d1_coast_afternoon_min = d1_inland_afternoon_min + random_d();
		d1_coast_afternoon_max = d1_coast_afternoon_min + random_i(7) + random_d();
		d1_coast_afternoon_mean = (d1_coast_afternoon_min+d1_coast_afternoon_max)/2.0;

		d1_area_min = d1_area_morning_min;
		d1_area_max = d1_area_afternoon_max;
		d1_area_mean = (d1_area_min+d1_area_max)/2.0;
		d1_inland_min = d1_inland_morning_min;
		d1_inland_max = d1_inland_afternoon_max;
		d1_inland_mean = (d1_inland_min+d1_inland_max)/2.0;
		d1_coast_min = d1_coast_morning_min;
		d1_coast_max = d1_coast_afternoon_max;
		d1_coast_mean = (d1_coast_min+d1_coast_max)/2.0;

		area_night_min = d1_area_morning_min - random_i(5) - random_d();
		area_night_max = area_night_min + random_i(2)+ random_d();
		area_night_mean = (area_night_min+area_night_max)/2.0;
		inland_night_min = d1_inland_morning_min - random_i(5) - random_d();
		inland_night_max = inland_night_min + random_i(2)+ random_d();
		inland_night_mean = (inland_night_min+inland_night_max)/2.0;
		coast_night_min = d1_coast_morning_min - random_i(5) - random_d();
		coast_night_max = coast_night_min + random_i(2)+ random_d();
		coast_night_mean = (coast_night_min+coast_night_max)/2.0;


		d2_area_morning_min = d1_area_morning_min - d + (random_d() >= 0.5 ? (1 + random_i(7) + random_d()) : (-1 - random_i(7) - random_d()));
		d2_area_morning_max = d2_area_morning_min + + random_i(2) + random_d();
		d2_area_morning_mean = (d2_area_morning_min+d2_area_morning_max)/2.0;
		d2_inland_morning_min = d2_area_morning_min + random_d();
		d2_inland_morning_max = d2_inland_morning_min + random_d();
		d2_inland_morning_mean = (d2_inland_morning_min+d2_inland_morning_max)/2.0;
		d2_coast_morning_min = d2_inland_morning_min + random_d();
		d2_coast_morning_max = d2_coast_morning_min + random_i(2) + random_d();
		d2_coast_morning_mean = (d2_coast_morning_min+d2_coast_morning_max)/2.0;
		d2_area_afternoon_min = d2_area_morning_max + random_i(5) + random_d();
		d2_area_afternoon_max = d2_area_afternoon_min + random_i(7) + random_d();
		d2_area_afternoon_mean = (d2_area_afternoon_min+d2_area_afternoon_max)/2.0;
		d2_inland_afternoon_min = d2_area_afternoon_min + random_d();
		d2_inland_afternoon_max = d2_inland_afternoon_min + random_d();
		d2_inland_afternoon_mean = (d2_inland_afternoon_min+d2_inland_afternoon_max)/2.0;
		d2_coast_afternoon_min = d2_inland_afternoon_min + random_d();
		d2_coast_afternoon_max = d2_coast_afternoon_min + random_i(7) + random_d();
		d2_coast_afternoon_mean = (d2_coast_afternoon_min+d2_coast_afternoon_max)/2.0;

		d2_area_min = d2_area_morning_min - random_d();
		d2_area_max = d2_area_afternoon_max + random_d();
		d2_area_mean = (d2_area_min+d2_area_max)/2.0;
		d2_inland_min = d2_inland_morning_min - random_d();
		d2_inland_max = d2_inland_afternoon_max + random_d();
		d2_inland_mean = (d2_inland_min+d2_inland_max)/2.0;
		d2_coast_min = d2_coast_morning_min - random_d();
		d2_coast_max = d2_coast_afternoon_max + random_d();
		d2_coast_mean = (d2_coast_min+d2_coast_max)/2.0;


		sprintf(d1_area_morning_min_str, "%.02f,0", d1_area_morning_min); 
		sprintf(d1_area_morning_max_str, "%.02f,0", d1_area_morning_max); 
		sprintf(d1_area_morning_mean_str, "%.02f,0", d1_area_morning_mean); 
		sprintf(d1_area_afternoon_min_str, "%.02f,0", d1_area_afternoon_min); 
		sprintf(d1_area_afternoon_max_str, "%.02f,0", d1_area_afternoon_max); 
		sprintf(d1_area_afternoon_mean_str, "%.02f,0", d1_area_afternoon_mean);
		sprintf(d1_inland_morning_min_str, "%.02f,0", d1_inland_morning_min); 
		sprintf(d1_inland_morning_max_str, "%.02f,0", d1_inland_morning_max); 
		sprintf(d1_inland_morning_mean_str, "%.02f,0", d1_inland_morning_mean); 
		sprintf(d1_inland_afternoon_min_str, "%.02f,0", d1_inland_afternoon_min); 
		sprintf(d1_inland_afternoon_max_str, "%.02f,0", d1_inland_afternoon_max); 
		sprintf(d1_inland_afternoon_mean_str, "%.02f,0", d1_inland_afternoon_mean);
		sprintf(d1_coast_morning_min_str, "%.02f,0", d1_coast_morning_min); 
		sprintf(d1_coast_morning_max_str, "%.02f,0", d1_coast_morning_max); 
		sprintf(d1_coast_morning_mean_str, "%.02f,0", d1_coast_morning_mean); 
		sprintf(d1_coast_afternoon_min_str, "%.02f,0", d1_coast_afternoon_min); 
		sprintf(d1_coast_afternoon_max_str, "%.02f,0", d1_coast_afternoon_max); 
		sprintf(d1_coast_afternoon_mean_str, "%.02f,0", d1_coast_afternoon_mean);
		sprintf(area_night_min_str, "%.02f,0", area_night_min); 
		sprintf(area_night_max_str, "%.02f,0", area_night_max); 
		sprintf(area_night_mean_str, "%.02f,0", area_night_mean); 
		sprintf(inland_night_min_str, "%.02f,0", inland_night_min); 
		sprintf(inland_night_max_str, "%.02f,0", inland_night_max); 
		sprintf(inland_night_mean_str, "%.02f,0", inland_night_mean); 
		sprintf(coast_night_min_str, "%.02f,0", coast_night_min); 
		sprintf(coast_night_max_str, "%.02f,0", coast_night_max); 
		sprintf(coast_night_mean_str, "%.02f,0", coast_night_mean); 
		sprintf(d1_area_min_str, "%.02f,0", d1_area_min); 
		sprintf(d1_area_max_str, "%.02f,0", d1_area_max);
		sprintf(d1_area_mean_str, "%.02f,0", d1_area_mean);
		sprintf(d1_inland_min_str, "%.02f,0", d1_inland_min); 
		sprintf(d1_inland_max_str, "%.02f,0", d1_inland_max);
		sprintf(d1_inland_mean_str, "%.02f,0", d1_inland_mean);
		sprintf(d1_coast_min_str, "%.02f,0", d1_coast_min); 
		sprintf(d1_coast_max_str, "%.02f,0", d1_coast_max);
		sprintf(d1_coast_mean_str, "%.02f,0", d1_coast_mean);
		sprintf(d2_area_morning_min_str, "%.02f,0", d2_area_morning_min); 
		sprintf(d2_area_morning_max_str, "%.02f,0", d2_area_morning_max); 
		sprintf(d2_area_morning_mean_str, "%.02f,0", d2_area_morning_mean); 
		sprintf(d2_area_afternoon_min_str, "%.02f,0", d2_area_afternoon_min); 
		sprintf(d2_area_afternoon_max_str, "%.02f,0", d2_area_afternoon_max); 
		sprintf(d2_area_afternoon_mean_str, "%.02f,0", d2_area_afternoon_mean);
		sprintf(d2_inland_morning_min_str, "%.02f,0", d2_inland_morning_min); 
		sprintf(d2_inland_morning_max_str, "%.02f,0", d2_inland_morning_max); 
		sprintf(d2_inland_morning_mean_str, "%.02f,0", d2_inland_morning_mean); 
		sprintf(d2_inland_afternoon_min_str, "%.02f,0", d2_inland_afternoon_min); 
		sprintf(d2_inland_afternoon_max_str, "%.02f,0", d2_inland_afternoon_max); 
		sprintf(d2_inland_afternoon_mean_str, "%.02f,0", d2_inland_afternoon_mean);
		sprintf(d2_coast_morning_min_str, "%.02f,0", d2_coast_morning_min); 
		sprintf(d2_coast_morning_max_str, "%.02f,0", d2_coast_morning_max); 
		sprintf(d2_coast_morning_mean_str, "%.02f,0", d2_coast_morning_mean); 
		sprintf(d2_coast_afternoon_min_str, "%.02f,0",d2_coast_afternoon_min ); 
		sprintf(d2_coast_afternoon_max_str, "%.02f,0", d2_coast_afternoon_max); 
		sprintf(d2_coast_afternoon_mean_str, "%.02f,0", d2_coast_afternoon_mean);
		sprintf(d2_area_min_str, "%.02f,0", d2_area_min); 
		sprintf(d2_area_max_str, "%.02f,0", d2_area_max);
		sprintf(d2_area_mean_str, "%.02f,0", d2_area_mean);
		sprintf(d2_inland_min_str, "%.02f,0", d2_inland_min); 
		sprintf(d2_inland_max_str, "%.02f,0", d2_inland_max);
		sprintf(d2_inland_mean_str, "%.02f,0", d2_inland_mean);
		sprintf(d2_coast_min_str, "%.02f,0", d2_coast_min); 
		sprintf(d2_coast_max_str, "%.02f,0", d2_coast_max);
		sprintf(d2_coast_mean_str, "%.02f,0", d2_coast_mean);

	  Max36HoursTestParamStr param(
								   d1_inland_min_str,
								   d1_inland_max_str,
								   d1_inland_mean_str,

								   d1_coast_min_str,
								   d1_coast_max_str,
								   d1_coast_mean_str,

								   d1_area_min_str,
								   d1_area_max_str,
								   d1_area_mean_str,

								   d1_inland_morning_min_str,
								   d1_inland_morning_max_str,
								   d1_inland_morning_mean_str,

								   d1_coast_morning_min_str,
								   d1_coast_morning_max_str,
								   d1_coast_morning_mean_str,

								   d1_area_morning_min_str,
								   d1_area_morning_max_str,
								   d1_area_morning_mean_str,


								   d1_inland_afternoon_min_str,
								   d1_inland_afternoon_max_str,
								   d1_inland_afternoon_mean_str,

								   d1_coast_afternoon_min_str,
								   d1_coast_afternoon_max_str,
								   d1_coast_afternoon_mean_str,

								   d1_area_afternoon_min_str,
								   d1_area_afternoon_max_str,
								   d1_area_afternoon_mean_str,

								   inland_night_min_str,
								   inland_night_max_str,
								   inland_night_mean_str,

								   coast_night_min_str,
								   coast_night_max_str,
								   coast_night_mean_str,

								   area_night_min_str,
								   area_night_max_str,
								   area_night_mean_str,

								   d2_inland_min_str,
								   d2_inland_max_str,
								   d2_inland_mean_str,

								   d2_coast_min_str,
								   d2_coast_max_str,
								   d2_coast_mean_str,

								   d2_area_min_str,
								   d2_area_max_str,
								   d2_area_mean_str,

								   d2_inland_morning_min_str,
								   d2_inland_morning_max_str,
								   d2_inland_morning_mean_str,

								   d2_coast_morning_min_str,
								   d2_coast_morning_max_str,
								   d2_coast_morning_mean_str,

								   d2_area_morning_min_str,
								   d2_area_morning_max_str,
								   d2_area_morning_mean_str,


								   d2_inland_afternoon_min_str,
								   d2_inland_afternoon_max_str,
								   d2_inland_afternoon_mean_str,

								   d2_coast_afternoon_min_str,
								   d2_coast_afternoon_max_str,
								   d2_coast_afternoon_mean_str,

								   d2_area_afternoon_min_str,
								   d2_area_afternoon_max_str,
								   d2_area_afternoon_mean_str,
								   "");

	  testCases.insert(make_pair(i++,param));
	  }
}


void create_anomaly_testcases(AnomalyTestCases& testCases, const string& language, const bool& isWinter)
  {
	int i = 0;

	if(isWinter)
	  {
		if(language == "fi")
		  {
			// pakkanen kiristyy
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"-6.0,0","-7.0,0","-6.5,0", // d1 min, max, mean
															"-6.0,0","-7.0,0","-6.5,0", // night min, max, mean
															"-26.0,0","-27.0,0","-26.5,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Pakkanen kiristyy.")));

			// kireä pakkanen jatkuu
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"-26.0,0","-27.0,0","-26.5,0", // d1 min, max, mean
															"-6.0,0","-7.0,0","-6.5,0", // night min, max, mean
															"-26.0,0","-27.0,0","-26.5,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Sää on poikkeuksellisen kylmää. Kireä pakkanen jatkuu.")));

			// sää on poikkeuksellisen kylmää ja pakkanen kiristyy
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"-27.0,0","-28.0,0","-27.5,0", // d1 min, max, mean
															"-27.0,0","-28.0,0","-27.5,0", // night min, max, mean
															"-32.0,0","-33.0,0","-32.5,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Sää on poikkeuksellisen kylmää. Pakkanen kiristyy.")));
			// kireä pakkanen heikkenee
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"-27.0,0","-28.0,0","-27.5,0", // d1 min, max, mean
															"-27.0,0","-28.0,0","-27.5,0", // night min, max, mean
															"-6.0,0","-7.0,0","-6.5,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Sää on poikkeuksellisen kylmää. Kireä pakkanen heikkenee.")));
			// pakkanen heikkenee
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"-18.0,0","-19.0,0","-18.5,0", // d1 min, max, mean
															"-18.0,0","-19.0,0","-18.5,0", // night min, max, mean
															"-6.0,0","-7.0,0","-6.5,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Pakkanen heikkenee.")));
			/*
			// pakkanen hellittää (sää lauhtuu)
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"-12.0,0","-14.0,0","-13.5,0", // d1 min, max, mean
															"-12.0,0","-14.0,0","-13.5,0", // night min, max, mean
															"-2.0,0","-6.0,0","-4.5,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Pakkanen hellittää.")));
			*/

			// sää lauhtuu
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"-18.0,0","-19.0,0","-18.5,0", // d1 min, max, mean
															"-18.0,0","-19.0,0","-18.5,0", // night min, max, mean
															"-4.0,0","-6.0,0","-4.9,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Sää lauhtuu.")));
			// sää lauhtuu
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"-27.0,0","-28.0,0","-27.5,0", // d1 min, max, mean
															"-27.0,0","-28.0,0","-27.5,0", // night min, max, mean
															"-4.0,0","-6.0,0","-4.9,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Sää on poikkeuksellisen kylmää. Sää lauhtuu.")));


			// sää one edelleen lauhaa
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"-2.0,0","-3.0,0","-2.5,0", // d1 min, max, mean
															"-2.0,0","-3.0,0","-2.5,0", // night min, max, mean
															"-2.0,0","-3.0,0","-2.5,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Sää on edelleen lauhaa.")));

			// sää one edelleen lauhaa. sää on erittäin tuulista
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"-2.0,0","-3.0,0","-2.5,0", // d1 min, max, mean
															"-2.0,0","-3.0,0","-2.5,0", // night min, max, mean
															"-2.0,0","-3.0,0","-2.5,0", // d2 min, max, mean
															"10.0,0","12.0,0","11.0,0", //d2 windspeed morning min,max,mean
															"10.0,0","12.0,0","11.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Sää on edelleen lauhaa. Sää on erittäin tuulinen.")));

			// sää on hyvin kylmää. kireä pakkanen jatkuu. sää on tuulinen. pakkanen on purevaa
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"-22.0,0","-23.0,0","-22.5,0", // d1 min, max, mean
															"-22.0,0","-23.0,0","-22.5,0", // night min, max, mean
															"-22.0,0","-23.0,0","-22.5,0", // d2 min, max, mean
															"8.0,0","10.0,0","9.0,0", //d2 windspeed morning min,max,mean
															"9.0,0","11.0,0","9.6,0", //d2 windspeed afternoon min,max,me															    
															"-26.0,0","-29.0,0","-27.0,0", //d2 windchill min,max,mean
															"Sää on hyvin kylmää. Kireä pakkanen jatkuu. Sää on tuulinen. Pakkanen on purevaa.")));

			// sää on hyvin kylmää. kireä pakkanen jatkuu. sää on erittäin tuulinen. pakkanen on erittäin purevaa
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"-22.0,0","-23.0,0","-22.5,0", // d1 min, max, mean
															"-22.0,0","-23.0,0","-22.5,0", // night min, max, mean
															"-22.0,0","-23.0,0","-22.5,0", // d2 min, max, mean
															"10.0,0","12.0,0","11.0,0", //d2 windspeed morning min,max,mean
															"10.0,0","12.0,0","11.0,0", //d2 windspeed afternoon min,max,me															    
															"-36.0,0","-39.0,0","-37.0,0", //d2 windchill min,max,mean
															"Sää on hyvin kylmää. Kireä pakkanen jatkuu. Sää on erittäin tuulinen. Pakkanen on erittäin purevaa.")));

		  }
	  }
	else
	  {
		if(language == "fi")
		  {
			// sää lämpenee huomattavasti ja  muuttuu helteiseksi
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"18.0,0","19.0,0","18.5,0", // d1 min, max, mean
															"15.0,0","16.0,0","15.5,0", // night min, max, mean
															"25.0,0","26.0,0","25.5,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Sää lämpenee huomattavasti ja muuttuu helteiseksi.")));

			// sää on poikkeuksellisen lämmintä. sää lämpenee vähän ja muuttuu helteiseksi
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"23.0,0","24.8,0","23.5,0", // d1 min, max, mean
															"15.0,0","16.0,0","15.5,0", // night min, max, mean
															"25.0,0","26.0,0","25.5,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Sää on poikkeuksellisen lämmintä. Sää lämpenee vähän ja sää on helteistä.")));

			// sää viilenee, mutta jatkuu helteisenä (vaikka lämpötila laskee)
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"29.0,0","30.0,0","29.5,0", // d1 min, max, mean
															"20.0,0","21.0,0","20.5,0", // night min, max, mean
															"25.0,0","26.0,0","25.5,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Sää viilenee, mutta jatkuu helteisenä.")));
			// helteinen sää jatkuu (lämpötila pysyy samana)
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"29.0,0","30.0,0","29.5,0", // d1 min, max, mean
															"20.0,0","21.0,0","20.5,0", // night min, max, mean
															"29.0,0","30.0,0","29.5,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Helteinen sää jatkuu.")));
			// sää lämpenee ja jatkuu helteisenä(lämpötila nousee)
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"25.0,0","26.0,0","25.5,0", // d1 min, max, mean
															"20.0,0","21.0,0","20.5,0", // night min, max, mean
															"29.0,0","30.0,0","29.5,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Sää lämpenee ja jatkuu helteisenä.")));
			// sää on koleaa. sää lämpenee vähän, mutta jatkuu koleana
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"8.0,0","9.0,0","8.5,0", // d1 min, max, mean
															"5.0,0","6.0,0","5.5,0", // night min, max, mean
															"10.0,0","11.0,0","10.5,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Sää on koleaa. Sää lämpenee vähän, mutta jatkuu koleana.")));

			// sää on koleaa. kolea sää jatkuu
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"8.0,0","9.0,0","8.5,0", // d1 min, max, mean
															"5.0,0","6.0,0","5.5,0", // night min, max, mean
															"7.5,0","8.5,0","8.0,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Sää on koleaa. Kolea sää jatkuu.")));

			// sää on koleaa. sää lämpenee, mutta jatkuu viileänä
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"8.0,0","9.0,0","8.5,0", // d1 min, max, mean
															"5.0,0","6.0,0","5.5,0", // night min, max, mean
															"12.0,0","14.0,0","13.5,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Sää on koleaa. Sää lämpenee, mutta jatkuu viileänä.")));

			// sää viilenee vähän
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"17.0,0","18.0,0","17.5,0", // d1 min, max, mean
															"15.0,0","16.0,0","15.5,0", // night min, max, mean
															"14.0,0","15.0,0","14.6,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Sää viilenee vähän.")));
			// sää viilenee
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"17.0,0","18.0,0","17.5,0", // d1 min, max, mean
															"15.0,0","16.0,0","15.5,0", // night min, max, mean
															"14.0,0","15.0,0","14.5,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Sää viilenee.")));
			// sää viilenee huomattavasti
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"17.0,0","18.0,0","17.5,0", // d1 min, max, mean
															"15.0,0","16.0,0","15.5,0", // night min, max, mean
															"10.0,0","11.0,0","10.5,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Sää viilenee huomattavasti.")));
			// sää lämpenee vähän
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"14.0,0","15.0,0","14.6,0", // d1 min, max, mean
															"15.0,0","16.0,0","15.5,0", // night min, max, mean
															"17.0,0","18.0,0","17.5,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Sää lämpenee vähän.")));
			// sää lämpenee
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"14.0,0","15.0,0","14.5,0", // d1 min, max, mean
															"15.0,0","16.0,0","15.5,0", // night min, max, mean
															"17.0,0","18.0,0","17.5,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Sää lämpenee.")));
			// sää on koleaa, sää lämpenee huomattavasti
			testCases.insert(make_pair(i++,AnomalyTestParam(
															"10.0,0","11.0,0","10.5,0", // d1 min, max, mean
															"15.0,0","16.0,0","15.5,0", // night min, max, mean
															"17.0,0","18.0,0","17.5,0", // d2 min, max, mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
															"2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
															"0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
															"Sää on koleaa. Sää lämpenee huomattavasti.")));
		  }
	  }
  }

  void create_testcases(Max36HoursTestCases& testCases, const string& language)
  {
	int i = 1;
	if(language == "fi")
	  {
				/*
		// - testin tarkoitus: päivän maksimilämpötiloissa suuri haarukka (> 5°C)
		// - Summertime
		// - both inland and coast included, but no difference in temperature
		// - big difference (7°C) among maximum day temperatures in the area
		// - 36 hours:  Day1, night, day2
		// - Day1: the maximum temperature is in the afternoon
		// - Night: Night temperature is about 10 degrees lower than day temperatures
		// - Day2: the temperature is somewhat lower than on previous day
		testCases.insert(make_pair(i++,Max36HoursTestParam(
														 "22.5,0","29.7,0","26.1,0", // d1 inland
														 "22.6,0","29.4,0","26.0,0", // d1 coast
														 "22.5,0","29.7,0","26.1,0", // d1 area

														 "21.5,0","29.4,0","25.1,0", // d1 morning inland
														 "21.4,0","29.3,0","25.0,0", // d1 morning coast
														 "21.5,0","29.4,0","25.1,0", // d1 morning area

														 "22.5,0","29.7,0","26.1,0", // d1 afternoon inland
														 "22.6,0","29.4,0","26.0,0", // d1 afternoon coast
														 "22.5,0","29.7,0","26.1,0", // d1 afternoon area

														 "12.1,0","15.3,0","13.7,0", // night inland
														 "10.1,0","13.6,0","11.4,0", // night coast
														 "10.1,0","15.3,0","12.7,0", // night area

														 "23.5,0","26.7,0","24.1,0", // d2 inland
														 "23.6,0","26.4,0","24.0,0", // d2 coast
														 "23.5,0","26.7,0","24.1,0", // d2 area

														 "22.5,0","25.4,0","24.1,0", // d2 morning inland
														 "22.4,0","25.3,0","24.0,0", // d2 morning coast
														 "22.5,0","25.4,0","24.1,0", // d2 morning area

														 "23.5,0","26.7,0","25.1,0", // d2 afternoon inland
														 "23.6,0","26.4,0","25.0,0", // d2 afternoon coast
														 "23.5,0","26.7,0","25.1,0", // d2 afternoon area
														 "")));

		// - testin tarkoitus: päivän maksimilämpötiloissa pieni haarukka (< 2°C)
		// - Summertime
		// - both inland and coast included, but no difference in temperature
		// - small difference (1,5°C) among maximum day temperatures in the area
		// - 36 hours:  Day1, night, day2
		// - Day1: the maximum temperature is in the afternoon
		// - Night: Night temperature is about 10 degrees lower than day temperatures
		// - Day2: the temperature is about the same as on previous day
		testCases.insert(make_pair(i++,Max36HoursTestParam(
														 "22.5,0","23.7,0","23.1,0", // d1 inland
														 "22.6,0","23.4,0","23.0,0", // d1 coast
														 "22.5,0","23.7,0","23.1,0", // d1 area

														 "21.5,0","23.4,0","23.1,0", // d1 morning inland
														 "21.4,0","23.3,0","23.0,0", // d1 morning coast
														 "21.5,0","23.4,0","23.1,0", // d1 morning area

														 "22.5,0","23.7,0","24.1,0", // d1 afternoon inland
														 "22.6,0","23.4,0","24.0,0", // d1 afternoon coast
														 "22.5,0","23.7,0","24.1,0", // d1 afternoon area

														 "12.1,0","15.3,0","13.7,0", // night inland
														 "10.1,0","13.6,0","11.4,0", // night coast
														 "10.1,0","15.3,0","12.7,0", // night area

														 "23.5,0","26.7,0","24.1,0", // d2 inland
														 "23.6,0","26.4,0","24.0,0", // d2 coast
														 "23.5,0","26.7,0","24.1,0", // d2 area

														 "22.5,0","25.4,0","24.1,0", // d2 morning inland
														 "22.4,0","25.3,0","24.0,0", // d2 morning coast
														 "22.5,0","25.4,0","24.1,0", // d2 morning area

														 "23.5,0","26.7,0","25.1,0", // d2 afternoon inland
														 "23.6,0","26.4,0","25.0,0", // d2 afternoon coast
														 "23.5,0","26.7,0","25.1,0", // d2 afternoon area
														 "")));

		// - testin tarkoitus: sisämaata ja rannikkoa ei erotella, päivän maksimilämpötiloissa 
		// keskisuuri haarukka (2°C <= X <= 5°C)
		// - Summertime
		// - both inland and coast included, but no difference in temperature
		// - moderate difference (~3°C) among maximum day temperatures in the area
		// - 36 hours:  Day1, night, day2
		// - Day1: the maximum temperature is in the afternoon
		// - Night: Night temperature is about 10 degrees lower than day temperatures
		// - Day2: the temperature is about the same as on previous day
		testCases.insert(make_pair(i++,Max36HoursTestParam(
														 "22.5,0","25.7,0","23.1,0", // d1 inland
														 "22.6,0","25.4,0","23.0,0", // d1 coast
														 "22.5,0","25.7,0","23.1,0", // d1 area

														 "21.5,0","24.4,0","23.1,0", // d1 morning inland
														 "21.4,0","24.3,0","23.0,0", // d1 morning coast
														 "21.5,0","24.4,0","23.1,0", // d1 morning area

														 "22.5,0","25.7,0","24.1,0", // d1 afternoon inland
														 "22.6,0","25.4,0","24.0,0", // d1 afternoon coast
														 "22.5,0","25.7,0","24.1,0", // d1 afternoon area

														 "12.1,0","15.3,0","13.7,0", // night inland
														 "10.1,0","13.6,0","11.4,0", // night coast
														 "10.1,0","15.3,0","12.7,0", // night area

														 "23.5,0","26.7,0","24.1,0", // d2 inland
														 "23.6,0","26.4,0","24.0,0", // d2 coast
														 "23.5,0","26.7,0","24.1,0", // d2 area

														 "22.5,0","25.4,0","24.1,0", // d2 morning inland
														 "22.4,0","25.3,0","24.0,0", // d2 morning coast
														 "22.5,0","25.4,0","24.1,0", // d2 morning area

														 "23.5,0","26.7,0","25.1,0", // d2 afternoon inland
														 "23.6,0","26.4,0","25.0,0", // d2 afternoon coast
														 "23.5,0","26.7,0","25.1,0", // d2 afternoon area
														 "")));

		// - testin tarkoitus: sisämaa/rannikko erotellaan, peräkkäisten päivien vertailu. aamu/iltapäivää ei erotella
		// - Summertime
		// - both inland and coast included
		// - 36 hours:  Day1, night, day2
		// - Day1: the maximum temperature is in the afternoon
		// - Night: Night temperature is about 7 degrees lower than day temperatures
		// - Day2: the temperature is about the same on inland area and a bit lower 
		// on coastal area than in the previous day
		testCases.insert(make_pair(i++,Max36HoursTestParam("21.5,0","26.7,0","24.1,0", // d1 inland
														 "19.6,0","23.4,0","22.1,0", // d1 coast
														 "19.6,0","26.7,0","23.5,0", // d1 area

														 "21.5,0","24.4,0","23.1,0", // d1 morning inland
														 "19.6,0","22.1,0","21.6,0", // d1 morning coast
														 "19.6,0","24.4,0","22.1,0", // d1 morning area

														 "23.5,0","26.7,0","25.1,0", // d1 afternoon inland
														 "21.1,0","24.4,0","22.5,0", // d1 afternoon coast
														 "22.6,0","25.2,0","24.0,0", // d1 afternoon area

														 "12.1,0","15.3,0","13.7,0", // night inland
														 "10.1,0","13.6,0","11.4,0", // night coast
														 "10.1,0","15.3,0","12.7,0", // night area

														 "21.7,0","26.9,0","24.3,0", // d2 inland
														 "18.1,0","22.2,0","20.0,0", // d2 coast
														 "18.1,0","26.9,0","22.5,0", // d2 area

														 "16.3,0","25.4,0","20.8,0", // d2 morning inland
														 "14.6,0","23.1,0","18.6,0", // d2 morning coast
														 "14.6,0","25.4,0","19.7,0", // d2 morning area

														 "23.4,0","27.7,0","25.5,0", // d2 afternoon inland
														 "23.1,0","25.4,0","24.0,0", // d2 afternoon coast
														 "23.1,0","27.7,0","25.3,0", // d2 afternoon area
														 "")));

		// - testin tarkoitus: aamu/iltapäivä, sisämaa/rannikko erotellaan, peräkkäisten päivien vertailu,
		// yön alin lämpötila kerrotaan
		// - Summertime
		// - both inland and coast included
		// - 36 hours:  Day1, night, day2
		// - Day1: the maximum temperature is in the morning in the inland area, and in the afternoon in the coastal area 
		// - Night: Night temperature is about 10 degrees lower than day temperatures
		// - Day2: the maximum temperature is in the afternoon on coastal area, but not on inland area
		// the temperature is about the same inland area and a bit lower on coastal area than in the previous day
		testCases.insert(make_pair(i++,Max36HoursTestParam("21.5,0","26.7,0","24.1,0", // d1 inland
														 "19.6,0","23.4,0","22.1,0", // d1 coast
														 "19.6,0","26.7,0","23.5,0", // d1 area

														 "22.5,0","26.4,0","24.6,0", // d1 morning inland
														 "19.6,0","22.1,0","21.6,0", // d1 morning coast
														 "19.6,0","24.4,0","22.1,0", // d1 morning area

														 "21.5,0","25.7,0","23.5,0", // d1 afternoon inland
														 "21.1,0","24.4,0","22.5,0", // d1 afternoon coast
														 "22.6,0","25.2,0","24.0,0", // d1 afternoon area

														 "12.1,0","15.3,0","13.7,0", // night inland
														 "10.1,0","13.6,0","11.4,0", // night coast
														 "10.1,0","15.3,0","12.7,0", // night area

														 "21.7,0","26.9,0","24.3,0", // d2 inland
														 "18.1,0","22.2,0","20.0,0", // d2 coast
														 "18.1,0","26.9,0","22.5,0", // d2 area

														 "16.3,0","25.4,0","20.8,0", // d2 morning inland
														 "23.1,0","25.4,0","24.0,0", // d2 morning coast
														 "14.6,0","25.4,0","19.7,0", // d2 morning area

														 "23.4,0","27.7,0","25.5,0", // d2 afternoon inland
														 "14.6,0","23.1,0","18.6,0", // d2 afternoon coast
														 "23.1,0","27.7,0","25.3,0", // d2 afternoon area
														 "")));

		// - testin tarkoitus: aamu/iltapäivä erotellaan molempina päivinä, 
		// sisämaa/rannikko erotellaan kaikilla periodeilla (d1, yö, d2)
		// yön alin lämpötila kerrotaan
		// - Summertime
		// - both inland and coast included
		// - 36 hours:  Day1, night, day2
		// - Day1: the maximum temperature is in the morning in the inland area, and in the afternoon in the coastal area 
		// - Night: Night temperature is about 10 degrees lower than day temperatures
		// - Day2: the maximum temperature is in the afternoon on coastal area, but not on inland area
		// the temperature is about the same inland area and a bit lower on coastal area than in the previous day
		testCases.insert(make_pair(i++,Max36HoursTestParam("21.5,0","26.7,0","24.1,0", // d1 inland
														 "19.6,0","23.4,0","22.1,0", // d1 coast
														 "19.6,0","26.7,0","23.5,0", // d1 area

														 "22.5,0","26.4,0","24.6,0", // d1 morning inland
														 "19.6,0","22.1,0","21.6,0", // d1 morning coast
														 "19.6,0","24.4,0","22.1,0", // d1 morning area

														 "21.5,0","25.7,0","23.5,0", // d1 afternoon inland
														 "18.1,0","21.4,0","19.8,0", // d1 afternoon coast
														 "18.1,0","25.7,0","21.5,0", // d1 afternoon area

														 "12.1,0","15.3,0","13.7,0", // night inland
														 "10.1,0","13.6,0","11.4,0", // night coast
														 "10.1,0","15.3,0","12.7,0", // night area

														 "19.6,0","23.4,0","22.1,0", // d2 inland
														 "21.5,0","26.7,0","24.1,0", // d2 coast
														 "19.6,0","26.7,0","23.5,0", // d2 area

														 "21.3,0","24.4,0","22.5,0", // d2 morning inland
														 "16.1,0","18.4,0","17.3,0", // d2 morning coast
														 "16.1,0","24.4,0","20.2,0", // d2 morning area

														 "19.4,0","23.7,0","21.5,0", // d2 afternoon inland
														 "15.6,0","17.1,0","17.1,0", // d2 afternoon coast
														 "15.6,0","23.7,0","19.8,0", // d2 afternoon area
														 "")));

		// - testin tarkoitus: päivän maksimilämpötiloissa suuri haarukka (> 5°C), yöllä lämpötila nousee
		// - Wintertime
		// - both inland and coast included, but no difference in temperature
		// - big difference (7°C) among maximum day temperatures in the area
		// - 36 hours:  Day1, night, day2
		// - Day1: the maximum temperature is in the afternoon
		// - Night: Night temperature is about 2 degrees lower than day temperatures
		// - Day2: the temperature is somewhat lower than on previous day
		testCases.insert(make_pair(i++,Max36HoursTestParam(
														 "-29.5,0","-22.7,0","-26.1,0", // d1 inland
														 "-29.6,0","-22.4,0","-26.0,0", // d1 coast
														 "-29.5,0","-22.7,0","-26.1,0", // d1 area

														 "-29.5,0","-21.4,0","-25.1,0", // d1 morning inland
														 "-29.4,0","-21.3,0","-25.0,0", // d1 morning coast
														 "-29.5,0","-21.4,0","-25.1,0", // d1 morning area

														 "-29.5,0","-22.7,0","-26.1,0", // d1 afternoon inland
														 "-29.6,0","-22.4,0","-26.0,0", // d1 afternoon coast
														 "-29.5,0","-22.7,0","-26.1,0", // d1 afternoon area

														 "-23.1,0","-16.3,0","-19.7,0", // night inland
														 "-22.1,0","-15.6,0","-18.4,0", // night coast
														 "-23.1,0","-15.6,0","-19.1,0", // night area

														 "-30.5,0","-23.7,0","-26.1,0", // d2 inland
														 "-30.6,0","-23.4,0","-26.0,0", // d2 coast
														 "-30.6,0","-23.7,0","-26.1,0", // d2 area

														 "-30.5,0","-23.4,0","26.1,0", // d2 morning inland
														 "-30.4,0","-23.4,0","26.0,0", // d2 morning coast
														 "-30.5,0","-23.7,0","26.1,0", // d2 morning area

														 "-30.5,0","-23.4,0","26.1,0", // d2 afternoon inland
														 "-30.4,0","-23.4,0","26.0,0", // d2 afternoon coast
														 "-30.5,0","-23.7,0","26.1,0", // d2 afternoon area

														 "")));
		
		// - testin tarkoitus: päivän maksimilämpötiloissa suuri haarukka (> 5°C)
		// - Wintertime
		// - both inland and coast included, but no difference in temperature
		// - big difference (7°C) among maximum day temperatures in the area
		// - 36 hours:  Day1, night, day2
		// - Day1: the maximum temperature is in the afternoon
		// - Night: Night temperature is about 2 degrees lower than day temperatures
		// - Day2: the temperature is somewhat lower than on previous day
		testCases.insert(make_pair(i++,Max36HoursTestParam(
														 "-29.5,0","-22.7,0","-26.1,0", // d1 inland
														 "-29.6,0","-22.4,0","-26.0,0", // d1 coast
														 "-29.5,0","-22.7,0","-26.1,0", // d1 area

														 "-29.5,0","-21.4,0","-25.1,0", // d1 morning inland
														 "-29.4,0","-21.3,0","-25.0,0", // d1 morning coast
														 "-29.5,0","-21.4,0","-25.1,0", // d1 morning area

														 "-29.5,0","-22.7,0","-26.1,0", // d1 afternoon inland
														 "-29.6,0","-22.4,0","-26.0,0", // d1 afternoon coast
														 "-29.5,0","-22.7,0","-26.1,0", // d1 afternoon area

														 "-31.1,0","-26.3,0","-28.7,0", // night inland
														 "-30.1,0","-25.6,0","-27.4,0", // night coast
														 "-31.1,0","-25.6,0","-28.1,0", // night area

														 "-30.5,0","-23.7,0","-26.1,0", // d2 inland
														 "-30.6,0","-23.4,0","-26.0,0", // d2 coast
														 "-30.6,0","-23.7,0","-26.1,0", // d2 area

														 "-30.5,0","-23.4,0","26.1,0", // d2 morning inland
														 "-30.4,0","-23.4,0","26.0,0", // d2 morning coast
														 "-30.5,0","-23.7,0","26.1,0", // d2 morning area

														 "-30.5,0","-23.4,0","26.1,0", // d2 afternoon inland
														 "-30.4,0","-23.4,0","26.0,0", // d2 afternoon coast
														 "-30.5,0","-23.7,0","26.1,0", // d2 afternoon area

														 "")));
		


		// - testin tarkoitus: lämpötila nollan tienoilla
		// - Wintertime
		// - 36 hours:  Day1, night, day2
		// - Day1: the maximum temperature is in the afternoon
		// - Night: Night temperature is about 2 degrees lower than day temperatures
		// - Day2: the temperature is somewhat lower than on previous day
		testCases.insert(make_pair(i++,Max36HoursTestParam(
														 "0.1,0","2.49,0","1.2,0", // d1 inland
														 "-2.0,0","2.0,0","0.0,0", // d1 coast
														 "0.1,0","2.49,0","1.2,0", // d1 area

														 "0.1,0","2.49,0","1.2,0", // d1 morning inland
														 "-2.0,0","2.0,0","0.0,0", // d1 morning coast
														 "0.1,0","2.49,0","1.2,0", // d1 morning area

														 "0.1,0","2.49,0","1.2,0", // d1 afternoon inland
														 "-2.0,0","2.0,0","0.0,0", // d1 afternoon coast
														 "0.1,0","2.49,0","1.2,0", // d1 afternoon area

														 "-0.49,0","2.49,0","1.2,0", // night inland
														 "-0.49,0","2.49,0","1.2,0", // night coast
														 "-0.49,0","2.49,0","1.2,0", // night area

														 "-2.49,0","0.0,0","-1.1,0", // d2 inland
														 "-2.49,0","0.0,0","-1.1,0", // d2 coast
														 "-2.49,0","0.0,0","-1.1,0", // d2 area

														 "-2.49,0","0.0,0","-1.1,0", // d2 morning inland
														 "-2.49,0","0.0,0","-1.1,0", // d2 morning coast
														 "-2.49,0","0.0,0","-1.1,0", // d2 morning area

														 "-2.49,0","0.0,0","-1.1,0", // d2 afternoon inland
														 "-2.49,0","0.0,0","-1.1,0", // d2 afternoon coast
														 "-2.49,0","0.0,0","-1.1,0", // d2 afternoon area
														 "")));

		// - testin tarkoitus: lämpötila nollan tienoilla
		// - Wintertime
		// - 36 hours:  Day1, night, day2
		// - Day1: the maximum temperature is in the afternoon
		// - Night: Night temperature is about 2 degrees lower than day temperatures
		// - Day2: the temperature is somewhat lower than on previous day
		testCases.insert(make_pair(i++,Max36HoursTestParam(
														 "0.1,0","2.49,0","1.2,0", // d1 inland
														 "-2.0,0","2.0,0","0.0,0", // d1 coast
														 "0.1,0","2.49,0","1.2,0", // d1 area

														 "0.1,0","2.49,0","1.2,0", // d1 morning inland
														 "-2.0,0","2.0,0","0.0,0", // d1 morning coast
														 "0.1,0","2.49,0","1.2,0", // d1 morning area

														 "0.1,0","2.49,0","1.2,0", // d1 afternoon inland
														 "-2.0,0","2.0,0","0.0,0", // d1 afternoon coast
														 "0.1,0","2.49,0","1.2,0", // d1 afternoon area

														 "-2.49,0","0.0,0","-1.2,0", // night inland
														 "-2.49,0","0.0,0","-1.2,0", // night coast
														 "-2.49,0","0.0,0","-1.2,0", // night area

														 "-2.49,0","0.0,0","-1.1,0", // d2 inland
														 "-2.49,0","0.0,0","-1.1,0", // d2 coast
														 "-2.49,0","0.0,0","-1.1,0", // d2 area

														 "-2.49,0","0.0,0","-1.1,0", // d2 morning inland
														 "-2.49,0","0.0,0","-1.1,0", // d2 morning coast
														 "-2.49,0","0.0,0","-1.1,0", // d2 morning area

														 "-2.49,0","0.0,0","-1.1,0", // d2 afternoon inland
														 "-2.49,0","0.0,0","-1.1,0", // d2 afternoon coast
														 "-2.49,0","0.0,0","-1.1,0", // d2 afternoon area
														 "")));

		// - testin tarkoitus: päivän maksimilämpötiloissa suuri haarukka (> 5°C)
		// - Wintertime
		// - both inland and coast included, but no difference in temperature
		// - big difference (7°C) among maximum day temperatures in the area
		// - 36 hours:  Day1, night, day2
		// - Day1: the maximum temperature is in the afternoon
		// - Night: Night temperature is the samea as day1 temperature
		// - Day2: the temperature is about the same as on previous day
		testCases.insert(make_pair(i++,Max36HoursTestParam(
														 "-29.5,0","-22.7,0","-26.1,0", // d1 inland
														 "-29.6,0","-22.4,0","-26.0,0", // d1 coast
														 "-29.5,0","-22.7,0","-26.1,0", // d1 area

														 "-29.5,0","-21.4,0","-25.1,0", // d1 morning inland
														 "-29.4,0","-21.3,0","-25.0,0", // d1 morning coast
														 "-29.5,0","-21.4,0","-25.1,0", // d1 morning area

														 "-29.5,0","-22.7,0","-26.1,0", // d1 afternoon inland
														 "-29.6,0","-22.4,0","-26.0,0", // d1 afternoon coast
														 "-29.5,0","-22.7,0","-26.1,0", // d1 afternoon area

														 "-29.5,0","-22.7,0","-26.1,0", // night inland
														 "-29.6,0","-22.4,0","-26.0,0", // night coast
														 "-29.5,0","-22.7,0","-26.1,0", // night area

														 "-30.5,0","-23.7,0","-26.1,0", // d2 inland
														 "-30.6,0","-23.4,0","-26.0,0", // d2 coast
														 "-30.6,0","-23.7,0","-26.1,0", // d2 area

														 "-30.5,0","-23.4,0","26.1,0", // d2 morning inland
														 "-30.4,0","-23.4,0","26.0,0", // d2 morning coast
														 "-30.5,0","-23.7,0","26.1,0", // d2 morning area

														 "-30.5,0","-23.4,0","26.1,0", // d2 afternoon inland
														 "-30.4,0","-23.4,0","26.0,0", // d2 afternoon coast
														 "-30.5,0","-23.7,0","26.1,0", // d2 afternoon area

														 "")));


		testCases.insert(make_pair(i++,Max36HoursTestParam(
														 "25.6,0","26.6,0","26.1,0", // d1 inland
														 "25.4,0","26.2,0","25.8,0", // d1 coast
														 "24.2,0","33.2,0","28.7,0", // d1 area

														 "25.0,0","25.4,0","25.2,0", // d1 morning inland
														 "26.2,0","28.1,0","27.1,0", // d1 morning coast
														 "25.1,0","25.4,0","25.2,0", // d1 morning area

														 "25.7,0","26.3,0","26.0,0", // d1 afternoon inland
														 "25.8,0","26.2,0","26.0,0", // d1 afternoon coast
														 "25.6,0","32.3,0","25.6,0", // d1 afternoon area // HUOM mean

														 "24.8,0","26.5,0","25.6,0", // night inland
														 "18.4,0","19.5,0","19.0,0", // night coast
														 "20.1,0","21.8,0","21.0,0", // night area

														 "28.6,0","32.4,0","30.5,0", // d2 inland
														 "29.2,0","38.6,0","33.9,0", // d2 coast
														 "28.1,0","34.4,0","31.2,0", // d2 area

														 "29.1,0","29.3,0","29.2,0", // d2 morning inland
														 "29.5,0","31.2,0","30.4,0", // d2 morning coast
														 "28.3,0","29.0,0","28.7,0", // d2 morning area

														 "31.3,0","31.5,0","31.4,0", // d2 afternoon inland
														 "31.9,0","38.2,0","35.1,0", // d2 afternoon coast
														 "31.2,0","33.5,0","31.2,0", // d2 afternoon area

														 "")));
				*/
		testCases.insert(make_pair(i++,Max36HoursTestParam(
														 "7.3,0","5.2,0","5.3,0", // d1 inland
														 "6.4,0","4.6,0","5.5,0", // d1 coast
														 "-16.01,0","-15.00,0", "-15.49,0", // d1 area

														 "17.6,0","15.2,0","14.3,0", // d1 morning inland
														 "16.4,0","14.6,0","15.5,0", // d1 morning coast
														 "17.6,0","15.7,0","15.0,0", // d1 morning area

														 "4.54,0","9.70,0","7.12,0", // d1 afternoon inland
														 "4.54,0","9.70,0","7.12,0", // d1 afternoon coast
														 "4.54,0","9.70,0","7.12,0", // d1 afternoon area

														 "4.54,0","9.70,0","7.12,0", // night inland
														 "4.54,0","9.70,0","7.12,0", // night coast
														 "4.54,0","9.70,0","7.12,0", // night area

														 "-10.9,0","-4.9,0","-7.9,0", // d2 inland
														 "-10.8,0","-3.4,0","-7.1,0", // d2 coast
														 "-4.49,0","-4.1,0","-4.3,0", // d2 area

														 "29.1,0","29.3,0","29.2,0", // d2 morning inland
														 "29.5,0","31.2,0","30.4,0", // d2 morning coast
														 "28.3,0","29.0,0","28.7,0", // d2 morning area

														 "31.3,0","31.5,0","31.4,0", // d2 afternoon inland
														 "31.9,0","38.2,0","35.1,0", // d2 afternoon coast
														 "31.2,0","33.5,0","31.2,0", // d2 afternoon area

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

	NFmiSettings::Set("textgen::fractiles_climatology","/home/reponen/work/testdata/tmax.sqd");
	NFmiSettings::Set("max36hours::fake::fractile02_limit", "2.0");
	NFmiSettings::Set("max36hours::day::starthour","6");
	NFmiSettings::Set("max36hours::day::maxstarthour","11");
	NFmiSettings::Set("max36hours::day::endhour","18");
	NFmiSettings::Set("max36hours::night::starthour","18");
	NFmiSettings::Set("max36hours::night::endhour","6");
	//	NFmiSettings::Set("qdtext::forecasttime","200901010615");
	//NFmiSettings::Set("textgen::main_forecast", "/home/reponen/work/testdata/200904300654_pal_skandinavia_pinta.sqd");

	//qdtext::forecasttime = 200904301200

	NFmiTime time1(2009,4,1,6,0,0);
	NFmiTime time2(2009,4,1,18,0,0);
	NFmiTime time3(2009,4,2,6,0,0);
	NFmiTime time4(2009,4,2,18,0,0);
	WeatherPeriod period_day1_night_day2(time1,time4);
	WeatherPeriod period_day1_night(time1,time3);
	WeatherPeriod period_night_day2(time2,time4);
	WeatherPeriod period_day1(time1,time2);
	TemperatureStory story1(time1,sources,area,period_day1_night_day2,"max36hours");
	TemperatureStory story2(time1,sources,area,period_day1_night,"max36hours");
	TemperatureStory story3(time2,sources,area,period_night_day2,"max36hours");
	TemperatureStory story4(time1,sources,area,period_day1,"max36hours");

	Max36HoursTestCases testCases;
	Max36HoursTestCasesStr testCasesStr;
	Max36HoursTestCasesStr::iterator iter;


	//	#ifdef LATER

	const char* languages [] = {"fi", "sv", "en"};

	for(int i = 0; i < 1; i++)
	  {
		create_testcases2(testCasesStr, languages[i]);
		for(iter = testCasesStr.begin(); iter != testCasesStr.end(); iter++)
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

			NFmiSettings::Set("max36hours::fake::day1::morning::inland::min",iter->second.t36h_d1_morning_inlandmin);
			NFmiSettings::Set("max36hours::fake::day1::morning::inland::max",iter->second.t36h_d1_morning_inlandmax);
			NFmiSettings::Set("max36hours::fake::day1::morning::inland::mean",iter->second.t36h_d1_morning_inlandmean);
			NFmiSettings::Set("max36hours::fake::day1::morning::coast::min",iter->second.t36h_d1_morning_coastmin);
			NFmiSettings::Set("max36hours::fake::day1::morning::coast::max",iter->second.t36h_d1_morning_coastmax);
			NFmiSettings::Set("max36hours::fake::day1::morning::coast::mean",iter->second.t36h_d1_morning_coastmean);
			NFmiSettings::Set("max36hours::fake::day1::morning::area::min",iter->second.t36h_d1_morning_areamin);
			NFmiSettings::Set("max36hours::fake::day1::morning::area::max",iter->second.t36h_d1_morning_areamax);
			NFmiSettings::Set("max36hours::fake::day1::morning::area::mean",iter->second.t36h_d1_morning_areamean);

			NFmiSettings::Set("max36hours::fake::day1::afternoon::inland::min",iter->second.t36h_d1_afternoon_inlandmin);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::inland::max",iter->second.t36h_d1_afternoon_inlandmax);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::inland::mean",iter->second.t36h_d1_afternoon_inlandmean);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::coast::min",iter->second.t36h_d1_afternoon_coastmin);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::coast::max",iter->second.t36h_d1_afternoon_coastmax);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::coast::mean",iter->second.t36h_d1_afternoon_coastmean);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::area::min",iter->second.t36h_d1_afternoon_areamin);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::area::max",iter->second.t36h_d1_afternoon_areamax);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::area::mean",iter->second.t36h_d1_afternoon_areamean);

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

			NFmiSettings::Set("max36hours::fake::day2::morning::inland::min",iter->second.t36h_d2_morning_inlandmin);
			NFmiSettings::Set("max36hours::fake::day2::morning::inland::max",iter->second.t36h_d2_morning_inlandmax);
			NFmiSettings::Set("max36hours::fake::day2::morning::inland::mean",iter->second.t36h_d2_morning_inlandmean);
			NFmiSettings::Set("max36hours::fake::day2::morning::coast::min",iter->second.t36h_d2_morning_coastmin);
			NFmiSettings::Set("max36hours::fake::day2::morning::coast::max",iter->second.t36h_d2_morning_coastmax);
			NFmiSettings::Set("max36hours::fake::day2::morning::coast::mean",iter->second.t36h_d2_morning_coastmean);
			NFmiSettings::Set("max36hours::fake::day2::morning::area::min",iter->second.t36h_d2_morning_areamin);
			NFmiSettings::Set("max36hours::fake::day2::morning::area::max",iter->second.t36h_d2_morning_areamax);
			NFmiSettings::Set("max36hours::fake::day2::morning::area::mean",iter->second.t36h_d2_morning_areamean);

			NFmiSettings::Set("max36hours::fake::day2::afternoon::inland::min",iter->second.t36h_d2_afternoon_inlandmin);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::inland::max",iter->second.t36h_d2_afternoon_inlandmax);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::inland::mean",iter->second.t36h_d2_afternoon_inlandmean);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::coast::min",iter->second.t36h_d2_afternoon_coastmin);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::coast::max",iter->second.t36h_d2_afternoon_coastmax);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::coast::mean",iter->second.t36h_d2_afternoon_coastmean);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::area::min",iter->second.t36h_d2_afternoon_areamin);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::area::max",iter->second.t36h_d2_afternoon_areamax);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::area::mean",iter->second.t36h_d2_afternoon_areamean);

			cout << iter->second << endl;

			cout << "**** päivä-yö-huominen ****" << endl;
			require(story1,languages[i],fun,iter->second.t36h_story);
			cout << "**** päivä-yö ****" << endl;
			require(story2,languages[i],fun,iter->second.t36h_story);
			cout << "**** yö-huominen ****" << endl;
			require(story3,languages[i],fun,iter->second.t36h_story);
			cout << "**** päivä ****" << endl;
			require(story4,languages[i],fun,iter->second.t36h_story);
		  }
		testCases.clear();
	  }
	//	#endif



#ifdef LATER

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

			NFmiSettings::Set("max36hours::fake::day1::morning::inland::min",iter->second.t36h_d1_morning_inlandmin);
			NFmiSettings::Set("max36hours::fake::day1::morning::inland::max",iter->second.t36h_d1_morning_inlandmax);
			NFmiSettings::Set("max36hours::fake::day1::morning::inland::mean",iter->second.t36h_d1_morning_inlandmean);
			NFmiSettings::Set("max36hours::fake::day1::morning::coast::min",iter->second.t36h_d1_morning_coastmin);
			NFmiSettings::Set("max36hours::fake::day1::morning::coast::max",iter->second.t36h_d1_morning_coastmax);
			NFmiSettings::Set("max36hours::fake::day1::morning::coast::mean",iter->second.t36h_d1_morning_coastmean);
			NFmiSettings::Set("max36hours::fake::day1::morning::area::min",iter->second.t36h_d1_morning_areamin);
			NFmiSettings::Set("max36hours::fake::day1::morning::area::max",iter->second.t36h_d1_morning_areamax);
			NFmiSettings::Set("max36hours::fake::day1::morning::area::mean",iter->second.t36h_d1_morning_areamean);

			NFmiSettings::Set("max36hours::fake::day1::afternoon::inland::min",iter->second.t36h_d1_afternoon_inlandmin);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::inland::max",iter->second.t36h_d1_afternoon_inlandmax);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::inland::mean",iter->second.t36h_d1_afternoon_inlandmean);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::coast::min",iter->second.t36h_d1_afternoon_coastmin);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::coast::max",iter->second.t36h_d1_afternoon_coastmax);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::coast::mean",iter->second.t36h_d1_afternoon_coastmean);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::area::min",iter->second.t36h_d1_afternoon_areamin);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::area::max",iter->second.t36h_d1_afternoon_areamax);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::area::mean",iter->second.t36h_d1_afternoon_areamean);

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

			NFmiSettings::Set("max36hours::fake::day2::morning::inland::min",iter->second.t36h_d2_morning_inlandmin);
			NFmiSettings::Set("max36hours::fake::day2::morning::inland::max",iter->second.t36h_d2_morning_inlandmax);
			NFmiSettings::Set("max36hours::fake::day2::morning::inland::mean",iter->second.t36h_d2_morning_inlandmean);
			NFmiSettings::Set("max36hours::fake::day2::morning::coast::min",iter->second.t36h_d2_morning_coastmin);
			NFmiSettings::Set("max36hours::fake::day2::morning::coast::max",iter->second.t36h_d2_morning_coastmax);
			NFmiSettings::Set("max36hours::fake::day2::morning::coast::mean",iter->second.t36h_d2_morning_coastmean);
			NFmiSettings::Set("max36hours::fake::day2::morning::area::min",iter->second.t36h_d2_morning_areamin);
			NFmiSettings::Set("max36hours::fake::day2::morning::area::max",iter->second.t36h_d2_morning_areamax);
			NFmiSettings::Set("max36hours::fake::day2::morning::area::mean",iter->second.t36h_d2_morning_areamean);

			NFmiSettings::Set("max36hours::fake::day2::afternoon::inland::min",iter->second.t36h_d2_afternoon_inlandmin);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::inland::max",iter->second.t36h_d2_afternoon_inlandmax);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::inland::mean",iter->second.t36h_d2_afternoon_inlandmean);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::coast::min",iter->second.t36h_d2_afternoon_coastmin);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::coast::max",iter->second.t36h_d2_afternoon_coastmax);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::coast::mean",iter->second.t36h_d2_afternoon_coastmean);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::area::min",iter->second.t36h_d2_afternoon_areamin);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::area::max",iter->second.t36h_d2_afternoon_areamax);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::area::mean",iter->second.t36h_d2_afternoon_areamean);

			/*
			require(story1,languages[i],fun,"Lämpötila on aamulla sisämaassa 12...17 astetta, iltapäivällä 12...16 astetta, huomenna suunnilleen sama, yön alin lämpötila on 0...5 astetta, lämpötila on aamulla rannikolla 8...13 astetta, iltapäivällä 8...11 astetta, huomenna suunnilleen sama, yön alin lämpötila on 6...11 astetta.");
			require(story2,languages[i],fun,"Lämpötila on aamulla sisämaassa 12...17 astetta, iltapäivällä 12...16 astetta, yön alin lämpötila on 0...5 astetta, lämpötila on aamulla rannikolla 8...13 astetta, iltapäivällä 8...11 astetta, yön alin lämpötila on suunnilleen sama.");
			require(story3,languages[i],fun,"Yön alin lämpötila on sisämaassa 0...5 astetta, huomenna päivän ylin lämpötila on 12...17 astetta, yön alin lämpötila on rannikolla 6...11 astetta, huomenna päivän ylin lämpötila on 9...14 astetta.");
*/
			require(story1,languages[i],fun,iter->second.t36h_story);
			//require(story2,languages[i],fun,iter->second.t36h_story);
			//require(story3,languages[i],fun,iter->second.t36h_story);
			//require(story4,languages[i],fun,iter->second.t36h_story);
		  }
		testCases.clear();
	  }
#endif

	TEST_PASSED();
  }

  shared_ptr<NFmiStreamQueryData> theQD;

  void read_querydata(const std::string & theFilename)
  {
	theQD.reset(new NFmiStreamQueryData());
	if(!theQD->ReadData(theFilename.c_str()))
	  throw runtime_error("Failed to read "+theFilename);
  }

  void temperature_anomaly()
  {

	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	NFmiSettings::Set("textgen::fractiles_climatology","/home/reponen/work/testdata/tmax.sqd");
	NFmiSettings::Set("anomaly::fake::fractile02_limit", "2.0");
	NFmiSettings::Set("anomaly::day::starthour","6");
	NFmiSettings::Set("anomaly::day::maxstarthour","11");
	NFmiSettings::Set("anomaly::day::endhour","18");
	NFmiSettings::Set("anomaly::night::starthour","18");
	NFmiSettings::Set("anomaly::night::endhour","6");

	// winter fractiles
	NFmiSettings::Set("anomaly::fake::fractile::winter::F02", "-25.0");
	NFmiSettings::Set("anomaly::fake::fractile::winter::F12", "-20.0");
	NFmiSettings::Set("anomaly::fake::fractile::winter::F37", "-15.0");
	NFmiSettings::Set("anomaly::fake::fractile::winter::F50", "-10.0");
	NFmiSettings::Set("anomaly::fake::fractile::winter::F63", "-5.0");
	NFmiSettings::Set("anomaly::fake::fractile::winter::F88", "0.0");
	NFmiSettings::Set("anomaly::fake::fractile::winter::F98", "3.0");
	// summer fractiles
	NFmiSettings::Set("anomaly::fake::fractile::summer::F02", "5.0");
	NFmiSettings::Set("anomaly::fake::fractile::summer::F12", "12.0");
	NFmiSettings::Set("anomaly::fake::fractile::summer::F37", "14.0");
	NFmiSettings::Set("anomaly::fake::fractile::summer::F50", "16.0");
	NFmiSettings::Set("anomaly::fake::fractile::summer::F63", "19.0");
	NFmiSettings::Set("anomaly::fake::fractile::summer::F88", "22.0");
	NFmiSettings::Set("anomaly::fake::fractile::summer::F98", "25.0");

	AnalysisSources sources;
	WeatherArea area("25,60");
	const string fun = "temperature_anomaly";

	AnomalyTestCases testCases;
	AnomalyTestCases::iterator iter;
	const char* languages [] = {"fi", "sv", "en"};

	// winter
	for(int k = 0; k < 2; k++)
	  {
		NFmiTime time1(2009, k == 0 ? 1 : 6, 1, 6, 0, 0);
		NFmiTime time2(2009, k == 0 ? 1 : 6, 2, 18, 0, 0);
		WeatherPeriod period_day1_night_day2(time1,time2);
		TemperatureStory story(time1,sources, area, period_day1_night_day2,"anomaly");

		for(int i = 0; i < 1; i++)
		  {
			create_anomaly_testcases(testCases, languages[i], k == 0);
			for(iter = testCases.begin(); iter != testCases.end(); iter++)
			  {
				NFmiSettings::Set("anomaly::fake::temperature::day1::area::min",iter->second.anomaly_d1_temperature_min);
				NFmiSettings::Set("anomaly::fake::temperature::day1::area::max",iter->second.anomaly_d1_temperature_max);
				NFmiSettings::Set("anomaly::fake::temperature::day1::area::mean",iter->second.anomaly_d1_temperature_mean);

				NFmiSettings::Set("anomaly::fake::temperature::night::area::min",iter->second.anomaly_nite_temperature_min);
				NFmiSettings::Set("anomaly::fake::temperature::night::area::max",iter->second.anomaly_nite_temperature_max);
				NFmiSettings::Set("anomaly::fake::temperature::night::area::mean",iter->second.anomaly_nite_temperature_mean);

				NFmiSettings::Set("anomaly::fake::temperature::day2::area::min",iter->second.anomaly_d2_temperature_min);
				NFmiSettings::Set("anomaly::fake::temperature::day2::area::max",iter->second.anomaly_d2_temperature_max);
				NFmiSettings::Set("anomaly::fake::temperature::day2::area::mean",iter->second.anomaly_d2_temperature_mean);
				NFmiSettings::Set("anomaly::fake::windspeed::morning::min",iter->second.anomaly_d2_windspeed_morning_min);
				NFmiSettings::Set("anomaly::fake::windspeed::morning::max",iter->second.anomaly_d2_windspeed_morning_max);
				NFmiSettings::Set("anomaly::fake::windspeed::morning::mean",iter->second.anomaly_d2_windspeed_morning_mean);
				NFmiSettings::Set("anomaly::fake::windspeed::afternoon::min",iter->second.anomaly_d2_windspeed_afternoon_min);
				NFmiSettings::Set("anomaly::fake::windspeed::afternoon::max",iter->second.anomaly_d2_windspeed_afternoon_max);
				NFmiSettings::Set("anomaly::fake::windspeed::afternoon::mean",iter->second.anomaly_d2_windspeed_afternoon_mean);
				NFmiSettings::Set("anomaly::fake::windchill::min",iter->second.anomaly_d2_windchill_min);
				NFmiSettings::Set("anomaly::fake::windchill::max",iter->second.anomaly_d2_windchill_max);
				NFmiSettings::Set("anomaly::fake::windchill::mean",iter->second.anomaly_d2_windchill_mean);

				require(story,languages[i],fun,iter->second.anomaly_story);
			  }
			testCases.clear();
		  }
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
	  // TEST(temperature_max36hours);
	  TEST(temperature_anomaly);
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
  MessageLogger::open("my.log");

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
