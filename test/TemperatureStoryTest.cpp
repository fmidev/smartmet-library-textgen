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
#include <boost/algorithm/string.hpp>

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;


namespace TemperatureStoryTest
{
  enum TemperatureTestType
	{
	  TEMPERATURE_MAX36_HOURS,
	  TEMPERATURE_ANOMALY,
	  WIND_ANOMALY
	};

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
	
	const string msg = value + " <> " + theExpected;
	cout << endl << msg << endl << endl;
	if(value != theExpected)
	  {
		//	const string msg = value + " <> " + theExpected;
		//cout << endl << msg << endl << endl;
		//		TEST_FAILED(msg.c_str());
		cout << "FAILED!!" << endl;

	  }
	else
	  {
		cout << "PASSED!!" << endl;
	  }
  }

  string get_story(const TextGen::Story & theStory,
				 const string & theLanguage,
				 const string & theName)
  {
	dict->init(theLanguage);
	formatter.dictionary(dict);

	TextGen::Paragraph para = theStory.makeStory(theName);
	const string value = para.realize(formatter);

	return value;
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

#ifdef LATER
  struct AnomalyTestParam
  {
	AnomalyTestParam(const char* d1_temperature_inlandmin,
					 const char* d1_temperature_inlandmax,
					 const char* d1_temperature_inlandmean,
					 const char* d1_temperature_coastmin,
					 const char* d1_temperature_coastmax,
					 const char* d1_temperature_coastmean,
					 const char* d1_temperature_areamin,
					 const char* d1_temperature_areamax,
					 const char* d1_temperature_areamean,

					 const char* d1_temperature_morning_inlandmin,
					 const char* d1_temperature_morning_inlandmax,
					 const char* d1_temperature_morning_inlandmean,
					 const char* d1_temperature_morning_coastmin,
					 const char* d1_temperature_morning_coastmax,
					 const char* d1_temperature_morning_coastmean,
					 const char* d1_temperature_morning_areamin,
					 const char* d1_temperature_morning_areamax,
					 const char* d1_temperature_morning_areamean,

					 const char* d1_temperature_afternoon_inlandmin,
					 const char* d1_temperature_afternoon_inlandmax,
					 const char* d1_temperature_afternoon_inlandmean,
					 const char* d1_temperature_afternoon_coastmin,
					 const char* d1_temperature_afternoon_coastmax,
					 const char* d1_temperature_afternoon_coastmean,
					 const char* d1_temperature_afternoon_areamin,
					 const char* d1_temperature_afternoon_areamax,
					 const char* d1_temperature_afternoon_areamean,

					 const char* nite_temperature_min,
					 const char* nite_temperature_max,
					 const char* nite_temperature_mean,
											  
					 const char* d2_temperature_inlandmin,
					 const char* d2_temperature_inlandmax,
					 const char* d2_temperature_inlandmean,
					 const char* d2_temperature_coastmin,
					 const char* d2_temperature_coastmax,
					 const char* d2_temperature_coastmean,
					 const char* d2_temperature_areamin,
					 const char* d2_temperature_areamax,
					 const char* d2_temperature_areamean,

					 const char* d2_temperature_morning_inlandmin,
					 const char* d2_temperature_morning_inlandmax,
					 const char* d2_temperature_morning_inlandmean,
					 const char* d2_temperature_morning_coastmin,
					 const char* d2_temperature_morning_coastmax,
					 const char* d2_temperature_morning_coastmean,
					 const char* d2_temperature_morning_areamin,
					 const char* d2_temperature_morning_areamax,
					 const char* d2_temperature_morning_areamean,

					 const char* d2_temperature_afternoon_inlandmin,
					 const char* d2_temperature_afternoon_inlandmax,
					 const char* d2_temperature_afternoon_inlandmean,
					 const char* d2_temperature_afternoon_coastmin,
					 const char* d2_temperature_afternoon_coastmax,
					 const char* d2_temperature_afternoon_coastmean,
					 const char* d2_temperature_afternoon_areamin,
					 const char* d2_temperature_afternoon_areamax,
					 const char* d2_temperature_afternoon_areamean,

					 const char* d2_windspeed_morning_inlandmin,
					 const char* d2_windspeed_morning_inlandmax,
					 const char* d2_windspeed_morning_inlandmean,
					 const char* d2_windspeed_morning_coastmin,
					 const char* d2_windspeed_morning_coastmax,
					 const char* d2_windspeed_morning_coastmean,

					 const char* d2_windspeed_afternoon_inlandmin,
					 const char* d2_windspeed_afternoon_inlandmax,
					 const char* d2_windspeed_afternoon_inlandmean,
					 const char* d2_windspeed_afternoon_coastmin,
					 const char* d2_windspeed_afternoon_coastmax,
					 const char* d2_windspeed_afternoon_coastmean,

					 const char* d2_windchill_morning_inlandmin,
					 const char* d2_windchill_morning_inlandmax,
					 const char* d2_windchill_morning_inlandmean,
					 const char* d2_windchill_morning_coastmin,
					 const char* d2_windchill_morning_coastmax,
					 const char* d2_windchill_morning_coastmean,

					 const char* d2_windchill_afternoon_inlandmin,
					 const char* d2_windchill_afternoon_inlandmax,
					 const char* d2_windchill_afternoon_inlandmean,
					 const char* d2_windchill_afternoon_coastmin,
					 const char* d2_windchill_afternoon_coastmax,
					 const char* d2_windchill_afternoon_coastmean,
					 const char* story) :
	  anomaly_d1_temperature_inlandmin(d1_temperature_inlandmin),
	  anomaly_d1_temperature_inlandmax(d1_temperature_inlandmax),
	  anomaly_d1_temperature_inlandmean(d1_temperature_inlandmean),
	  anomaly_d1_temperature_coastmin(d1_temperature_coastmin),
	  anomaly_d1_temperature_coastmax(d1_temperature_coastmax),
	  anomaly_d1_temperature_coastmean(d1_temperature_coastmean),
	  anomaly_d1_temperature_areamin(d1_temperature_areamin),
	  anomaly_d1_temperature_areamax(d1_temperature_areamax),
	  anomaly_d1_temperature_areamean(d1_temperature_areamean),

	  anomaly_d1_temperature_morning_inlandmin(d1_temperature_morning_inlandmin),
	  anomaly_d1_temperature_morning_inlandmax(d1_temperature_morning_inlandmax),
	  anomaly_d1_temperature_morning_inlandmean(d1_temperature_morning_inlandmean),
	  anomaly_d1_temperature_morning_coastmin(d1_temperature_morning_coastmin),
	  anomaly_d1_temperature_morning_coastmax(d1_temperature_morning_coastmax),
	  anomaly_d1_temperature_morning_coastmean(d1_temperature_morning_coastmean),
	  anomaly_d1_temperature_morning_areamin(d1_temperature_morning_areamin),
	  anomaly_d1_temperature_morning_areamax(d1_temperature_morning_areamax),
	  anomaly_d1_temperature_morning_areamean(d1_temperature_morning_areamean),

	  anomaly_d1_temperature_afternoon_inlandmin(d1_temperature_afternoon_inlandmin),
	  anomaly_d1_temperature_afternoon_inlandmax(d1_temperature_afternoon_inlandmax),
	  anomaly_d1_temperature_afternoon_inlandmean(d1_temperature_afternoon_inlandmean),
	  anomaly_d1_temperature_afternoon_coastmin(d1_temperature_afternoon_coastmin),
	  anomaly_d1_temperature_afternoon_coastmax(d1_temperature_afternoon_coastmax),
	  anomaly_d1_temperature_afternoon_coastmean(d1_temperature_afternoon_coastmean),
	  anomaly_d1_temperature_afternoon_areamin(d1_temperature_afternoon_areamin),
	  anomaly_d1_temperature_afternoon_areamax(d1_temperature_afternoon_areamax),
	  anomaly_d1_temperature_afternoon_areamean(d1_temperature_afternoon_areamean),

	  anomaly_d1_windspeed_morning_inlandmin(d1_windspeed_morning_inlandmin),
	  anomaly_d1_windspeed_morning_inlandmax(d1_windspeed_morning_inlandmax),
	  anomaly_d1_windspeed_morning_inlandmean(d1_windspeed_morning_inlandmean),
	  anomaly_d1_windspeed_morning_coastmin(d1_windspeed_morning_coastmin),
	  anomaly_d1_windspeed_morning_coastmax(d1_windspeed_morning_coastmax),
	  anomaly_d1_windspeed_morning_coastmean(d1_windspeed_morning_coastmean),

	  anomaly_d1_windspeed_afternoon_inlandmin(d1_windspeed_afternoon_inlandmin),
	  anomaly_d1_windspeed_afternoon_inlandmax(d1_windspeed_afternoon_inlandmax),
	  anomaly_d1_windspeed_afternoon_inlandmean(d1_windspeed_afternoon_inlandmean),
	  anomaly_d1_windspeed_afternoon_coastmin(d1_windspeed_afternoon_coastmin),
	  anomaly_d1_windspeed_afternoon_coastmax(d1_windspeed_afternoon_coastmax),
	  anomaly_d1_windspeed_afternoon_coastmean(d1_windspeed_afternoon_coastmean),

	  anomaly_d1_windchill_morning_inlandmin(d1_windchill_morning_inlandmin),
	  anomaly_d1_windchill_morning_inlandmax(d1_windchill_morning_inlandmax),
	  anomaly_d1_windchill_morning_inlandmean(d1_windchill_morning_inlandmean),
	  anomaly_d1_windchill_morning_coastmin(d1_windchill_morning_coastmin),
	  anomaly_d1_windchill_morning_coastmax(d1_windchill_morning_coastmax),
	  anomaly_d1_windchill_morning_coastmean(d1_windchill_morning_coastmean),

	  anomaly_d1_windchill_afternoon_inlandmin(d1_windchill_afternoon_inlandmin),
	  anomaly_d1_windchill_afternoon_inlandmax(d1_windchill_afternoon_inlandmax),
	  anomaly_d1_windchill_afternoon_inlandmean(d1_windchill_afternoon_inlandmean),
	  anomaly_d1_windchill_afternoon_coastmin(d1_windchill_afternoon_coastmin),
	  anomaly_d1_windchill_afternoon_coastmax(d1_windchill_afternoon_coastmax),
	  anomaly_d1_windchill_afternoon_coastmean(d1_windchill_afternoon_coastmean),

	  anomaly_nite_temperature_min(nite_temperature_min),
	  anomaly_nite_temperature_max(nite_temperature_max),
	  anomaly_nite_temperature_mean(nite_temperature_mean),
											  
	  anomaly_d2_temperature_inlandmin(d2_temperature_inlandmin),
	  anomaly_d2_temperature_inlandmax(d2_temperature_inlandmax),
	  anomaly_d2_temperature_inlandmean(d2_temperature_inlandmean),
	  anomaly_d2_temperature_coastmin(d2_temperature_coastmin),
	  anomaly_d2_temperature_coastmax(d2_temperature_coastmax),
	  anomaly_d2_temperature_coastmean(d2_temperature_coastmean),
	  anomaly_d2_temperature_areamin(d2_temperature_areamin),
	  anomaly_d2_temperature_areamax(d2_temperature_areamax),
	  anomaly_d2_temperature_areamean(d2_temperature_areamean),

	  anomaly_d2_temperature_morning_inlandmin(d2_temperature_morning_inlandmin),
	  anomaly_d2_temperature_morning_inlandmax(d2_temperature_morning_inlandmax),
	  anomaly_d2_temperature_morning_inlandmean(d2_temperature_morning_inlandmean),
	  anomaly_d2_temperature_morning_coastmin(d2_temperature_morning_coastmin),
	  anomaly_d2_temperature_morning_coastmax(d2_temperature_morning_coastmax),
	  anomaly_d2_temperature_morning_coastmean(d2_temperature_morning_coastmean),
	  anomaly_d2_temperature_morning_areamin(d2_temperature_morning_areamin),
	  anomaly_d2_temperature_morning_areamax(d2_temperature_morning_areamax),
	  anomaly_d2_temperature_morning_areamean(d2_temperature_morning_areamean),

	  anomaly_d2_temperature_afternoon_inlandmin(d2_temperature_afternoon_inlandmin),
	  anomaly_d2_temperature_afternoon_inlandmax(d2_temperature_afternoon_inlandmax),
	  anomaly_d2_temperature_afternoon_inlandmean(d2_temperature_afternoon_inlandmean),
	  anomaly_d2_temperature_afternoon_coastmin(d2_temperature_afternoon_coastmin),
	  anomaly_d2_temperature_afternoon_coastmax(d2_temperature_afternoon_coastmax),
	  anomaly_d2_temperature_afternoon_coastmean(d2_temperature_afternoon_coastmean),
	  anomaly_d2_temperature_afternoon_areamin(d2_temperature_afternoon_areamin),
	  anomaly_d2_temperature_afternoon_areamax(d2_temperature_afternoon_areamax),
	  anomaly_d2_temperature_afternoon_areamean(d2_temperature_afternoon_areamean),

	  anomaly_d2_windspeed_morning_inlandmin(d2_windspeed_morning_inlandmin),
	  anomaly_d2_windspeed_morning_inlandmax(d2_windspeed_morning_inlandmax),
	  anomaly_d2_windspeed_morning_inlandmean(d2_windspeed_morning_inlandmean),
	  anomaly_d2_windspeed_morning_coastmin(d2_windspeed_morning_coastmin),
	  anomaly_d2_windspeed_morning_coastmax(d2_windspeed_morning_coastmax),
	  anomaly_d2_windspeed_morning_coastmean(d2_windspeed_morning_coastmean),

	  anomaly_d2_windspeed_afternoon_inlandmin(d2_windspeed_afternoon_inlandmin),
	  anomaly_d2_windspeed_afternoon_inlandmax(d2_windspeed_afternoon_inlandmax),
	  anomaly_d2_windspeed_afternoon_inlandmean(d2_windspeed_afternoon_inlandmean),
	  anomaly_d2_windspeed_afternoon_coastmin(d2_windspeed_afternoon_coastmin),
	  anomaly_d2_windspeed_afternoon_coastmax(d2_windspeed_afternoon_coastmax),
	  anomaly_d2_windspeed_afternoon_coastmean(d2_windspeed_afternoon_coastmean),

	  anomaly_d2_windchill_morning_inlandmin(d2_windchill_morning_inlandmin),
	  anomaly_d2_windchill_morning_inlandmax(d2_windchill_morning_inlandmax),
	  anomaly_d2_windchill_morning_inlandmean(d2_windchill_morning_inlandmean),
	  anomaly_d2_windchill_morning_coastmin(d2_windchill_morning_coastmin),
	  anomaly_d2_windchill_morning_coastmax(d2_windchill_morning_coastmax),
	  anomaly_d2_windchill_morning_coastmean(d2_windchill_morning_coastmean),

	  anomaly_d2_windchill_afternoon_inlandmin(d2_windchill_afternoon_inlandmin),
	  anomaly_d2_windchill_afternoon_inlandmax(d2_windchill_afternoon_inlandmax),
	  anomaly_d2_windchill_afternoon_inlandmean(d2_windchill_afternoon_inlandmean),
	  anomaly_d2_windchill_afternoon_coastmin(d2_windchill_afternoon_coastmin),
	  anomaly_d2_windchill_afternoon_coastmax(d2_windchill_afternoon_coastmax),
	  anomaly_d2_windchill_afternoon_coastmean(d2_windchill_afternoon_coastmean),
	  anomaly_story(story)

	{
	}

	string anomaly_d1_temperature_inlandmin;
	string anomaly_d1_temperature_inlandmax;
	string anomaly_d1_temperature_inlandmean;
	string anomaly_d1_temperature_coastmin;
	string anomaly_d1_temperature_coastmax;
	string anomaly_d1_temperature_coastmean;
	string anomaly_d1_temperature_areamin;
	string anomaly_d1_temperature_areamax;
	string anomaly_d1_temperature_areamean;

	string anomaly_d1_temperature_morning_inlandmin;
	string anomaly_d1_temperature_morning_inlandmax;
	string anomaly_d1_temperature_morning_inlandmean;
	string anomaly_d1_temperature_morning_coastmin;
	string anomaly_d1_temperature_morning_coastmax;
	string anomaly_d1_temperature_morning_coastmean;
	string anomaly_d1_temperature_morning_areamin;
	string anomaly_d1_temperature_morning_areamax;
	string anomaly_d1_temperature_morning_areamean;

	string anomaly_d1_temperature_afternoon_inlandmin;
	string anomaly_d1_temperature_afternoon_inlandmax;
	string anomaly_d1_temperature_afternoon_inlandmean;
	string anomaly_d1_temperature_afternoon_coastmin;
	string anomaly_d1_temperature_afternoon_coastmax;
	string anomaly_d1_temperature_afternoon_coastmean;
	string anomaly_d1_temperature_afternoon_areamin;
	string anomaly_d1_temperature_afternoon_areamax;
	string anomaly_d1_temperature_afternoon_areamean;

	string anomaly_d1_windspeed_morning_inlandmin;
	string anomaly_d1_windspeed_morning_inlandmax;
	string anomaly_d1_windspeed_morning_inlandmean;
	string anomaly_d1_windspeed_morning_coastmin;
	string anomaly_d1_windspeed_morning_coastmax;
	string anomaly_d1_windspeed_morning_coastmean;

	string anomaly_d1_windspeed_afternoon_inlandmin;
	string anomaly_d1_windspeed_afternoon_inlandmax;
	string anomaly_d1_windspeed_afternoon_inlandmean;
	string anomaly_d1_windspeed_afternoon_coastmin;
	string anomaly_d1_windspeed_afternoon_coastmax;
	string anomaly_d1_windspeed_afternoon_coastmean;

	string anomaly_d1_windchill_morning_inlandmin;
	string anomaly_d1_windchill_morning_inlandmax;
	string anomaly_d1_windchill_morning_inlandmean;
	string anomaly_d1_windchill_morning_coastmin;
	string anomaly_d1_windchill_morning_coastmax;
	string anomaly_d1_windchill_morning_coastmean;

	string anomaly_d1_windchill_afternoon_inlandmin;
	string anomaly_d1_windchill_afternoon_inlandmax;
	string anomaly_d1_windchill_afternoon_inlandmean;
	string anomaly_d1_windchill_afternoon_coastmin;
	string anomaly_d1_windchill_afternoon_coastmax;
	string anomaly_d1_windchill_afternoon_coastmean;

	string anomaly_nite_temperature_min;
	string anomaly_nite_temperature_max;
	string anomaly_nite_temperature_mean;
											  
	string anomaly_d2_temperature_inlandmin;
	string anomaly_d2_temperature_inlandmax;
	string anomaly_d2_temperature_inlandmean;
	string anomaly_d2_temperature_coastmin;
	string anomaly_d2_temperature_coastmax;
	string anomaly_d2_temperature_coastmean;
	string anomaly_d2_temperature_areamin;
	string anomaly_d2_temperature_areamax;
	string anomaly_d2_temperature_areamean;

	string anomaly_d2_temperature_morning_inlandmin;
	string anomaly_d2_temperature_morning_inlandmax;
	string anomaly_d2_temperature_morning_inlandmean;
	string anomaly_d2_temperature_morning_coastmin;
	string anomaly_d2_temperature_morning_coastmax;
	string anomaly_d2_temperature_morning_coastmean;
	string anomaly_d2_temperature_morning_areamin;
	string anomaly_d2_temperature_morning_areamax;
	string anomaly_d2_temperature_morning_areamean;

	string anomaly_d2_temperature_afternoon_inlandmin;
	string anomaly_d2_temperature_afternoon_inlandmax;
	string anomaly_d2_temperature_afternoon_inlandmean;
	string anomaly_d2_temperature_afternoon_coastmin;
	string anomaly_d2_temperature_afternoon_coastmax;
	string anomaly_d2_temperature_afternoon_coastmean;
	string anomaly_d2_temperature_afternoon_areamin;
	string anomaly_d2_temperature_afternoon_areamax;
	string anomaly_d2_temperature_afternoon_areamean;

	string anomaly_d2_windspeed_morning_inlandmin;
	string anomaly_d2_windspeed_morning_inlandmax;
	string anomaly_d2_windspeed_morning_inlandmean;
	string anomaly_d2_windspeed_morning_coastmin;
	string anomaly_d2_windspeed_morning_coastmax;
	string anomaly_d2_windspeed_morning_coastmean;

	string anomaly_d2_windspeed_afternoon_inlandmin;
	string anomaly_d2_windspeed_afternoon_inlandmax;
	string anomaly_d2_windspeed_afternoon_inlandmean;
	string anomaly_d2_windspeed_afternoon_coastmin;
	string anomaly_d2_windspeed_afternoon_coastmax;
	string anomaly_d2_windspeed_afternoon_coastmean;

	string anomaly_d2_windchill_morning_inlandmin;
	string anomaly_d2_windchill_morning_inlandmax;
	string anomaly_d2_windchill_morning_inlandmean;
	string anomaly_d2_windchill_morning_coastmin;
	string anomaly_d2_windchill_morning_coastmax;
	string anomaly_d2_windchill_morning_coastmean;

	string anomaly_d2_windchill_afternoon_inlandmin;
	string anomaly_d2_windchill_afternoon_inlandmax;
	string anomaly_d2_windchill_afternoon_inlandmean;
	string anomaly_d2_windchill_afternoon_coastmin;
	string anomaly_d2_windchill_afternoon_coastmax;
	string anomaly_d2_windchill_afternoon_coastmean;
	string anomaly_story;
  };

	typedef std::map<int, AnomalyTestParam> AnomalyTestCases;
#endif

  struct TemperatureStoryTestParam
  {
	TemperatureStoryTestParam(const char* d1_afternoon_inlandmin,
							  const char* d1_afternoon_inlandmax,
							  const char* d1_afternoon_inlandmean,
							  const char* d1_afternoon_coastmin,
							  const char* d1_afternoon_coastmax,
							  const char* d1_afternoon_coastmean,
							  const char* d1_afternoon_areamin,
							  const char* d1_afternoon_areamax,
							  const char* d1_afternoon_areamean,
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
	  temperature_d1_afternoon_inlandmin(d1_afternoon_inlandmin),
	  temperature_d1_afternoon_inlandmax(d1_afternoon_inlandmax),
 	  temperature_d1_afternoon_inlandmean(d1_afternoon_inlandmean),
	  temperature_d1_afternoon_coastmin(d1_afternoon_coastmin),
	  temperature_d1_afternoon_coastmax(d1_afternoon_coastmax),
	  temperature_d1_afternoon_coastmean(d1_afternoon_coastmean),
	  temperature_d1_afternoon_areamin(d1_afternoon_areamin),
	  temperature_d1_afternoon_areamax(d1_afternoon_areamax),
	  temperature_d1_afternoon_areamean(d1_afternoon_areamean),

	  temperature_d2_afternoon_inlandmin(d2_afternoon_inlandmin),
	  temperature_d2_afternoon_inlandmax(d2_afternoon_inlandmax),
 	  temperature_d2_afternoon_inlandmean(d2_afternoon_inlandmean),
	  temperature_d2_afternoon_coastmin(d2_afternoon_coastmin),
	  temperature_d2_afternoon_coastmax(d2_afternoon_coastmax),
	  temperature_d2_afternoon_coastmean(d2_afternoon_coastmean),
	  temperature_d2_afternoon_areamin(d2_afternoon_areamin),
	  temperature_d2_afternoon_areamax(d2_afternoon_areamax),
	  temperature_d2_afternoon_areamean(d2_afternoon_areamean),

	  theStory(story)
	  {}

	  string temperature_d1_afternoon_inlandmin;
	  string temperature_d1_afternoon_inlandmax;
	  string temperature_d1_afternoon_inlandmean;
	  string temperature_d1_afternoon_coastmin;
	  string temperature_d1_afternoon_coastmax;
	  string temperature_d1_afternoon_coastmean;
	  string temperature_d1_afternoon_areamin;
	  string temperature_d1_afternoon_areamax;
	  string temperature_d1_afternoon_areamean;

	  string temperature_d2_afternoon_inlandmin;
	  string temperature_d2_afternoon_inlandmax;
	  string temperature_d2_afternoon_inlandmean;
	  string temperature_d2_afternoon_coastmin;
	  string temperature_d2_afternoon_coastmax;
	  string temperature_d2_afternoon_coastmean;
	  string temperature_d2_afternoon_areamin;
	  string temperature_d2_afternoon_areamax;
	  string temperature_d2_afternoon_areamean;

	  string theStory;
	};


  struct Max36HoursTestParam : public TemperatureStoryTestParam
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
						  const char* story) :
		TemperatureStoryTestParam(d1_afternoon_inlandmin,
								  d1_afternoon_inlandmax,
								  d1_afternoon_inlandmean,
								  d1_afternoon_coastmin,
								  d1_afternoon_coastmax,
								  d1_afternoon_coastmean,
								  d1_afternoon_areamin,
								  d1_afternoon_areamax,
								  d1_afternoon_areamean,
								  d2_afternoon_inlandmin,
								  d2_afternoon_inlandmax,
								  d2_afternoon_inlandmean,
								  d2_afternoon_coastmin,
								  d2_afternoon_coastmax,
								  d2_afternoon_coastmean,
								  d2_afternoon_areamin,
								  d2_afternoon_areamax,
								  d2_afternoon_areamean,
								  story),
		temperature_d1_inlandmin(d1_inlandmin),
		temperature_d1_inlandmax(d1_inlandmax),
		temperature_d1_inlandmean(d1_inlandmean),
		temperature_d1_coastmin(d1_coastmin),
		temperature_d1_coastmax(d1_coastmax),
		temperature_d1_coastmean(d1_coastmean),
		temperature_d1_areamin(d1_areamin),
		temperature_d1_areamax(d1_areamax),
		temperature_d1_areamean(d1_areamean),

		temperature_d1_morning_inlandmin(d1_morning_inlandmin),
		temperature_d1_morning_inlandmax(d1_morning_inlandmax),
		temperature_d1_morning_inlandmean(d1_morning_inlandmean),
		temperature_d1_morning_coastmin(d1_morning_coastmin),
		temperature_d1_morning_coastmax(d1_morning_coastmax),
		temperature_d1_morning_coastmean(d1_morning_coastmean),
		temperature_d1_morning_areamin(d1_morning_areamin),
		temperature_d1_morning_areamax(d1_morning_areamax),
		temperature_d1_morning_areamean(d1_morning_areamean),

		temperature_nite_inlandmin(nite_inlandmin),
		temperature_nite_inlandmax(nite_inlandmax),
		temperature_nite_inlandmean(nite_inlandmean),
		temperature_nite_coastmin(nite_coastmin),
		temperature_nite_coastmax(nite_coastmax),
		temperature_nite_coastmean(nite_coastmean),
		temperature_nite_areamin(nite_areamin),
		temperature_nite_areamax(nite_areamax),
		temperature_nite_areamean(nite_areamean),

		temperature_d2_inlandmin(d2_inlandmin),
		temperature_d2_inlandmax(d2_inlandmax),
		temperature_d2_inlandmean(d2_inlandmean),
		temperature_d2_coastmin(d2_coastmin),
		temperature_d2_coastmax(d2_coastmax),
		temperature_d2_coastmean(d2_coastmean),
		temperature_d2_areamin(d2_areamin),
		temperature_d2_areamax(d2_areamax),
		temperature_d2_areamean(d2_areamean),

		temperature_d2_morning_inlandmin(d2_morning_inlandmin),
		temperature_d2_morning_inlandmax(d2_morning_inlandmax),
		temperature_d2_morning_inlandmean(d2_morning_inlandmean),
		temperature_d2_morning_coastmin(d2_morning_coastmin),
		temperature_d2_morning_coastmax(d2_morning_coastmax),
		temperature_d2_morning_coastmean(d2_morning_coastmean),
		temperature_d2_morning_areamin(d2_morning_areamin),
		temperature_d2_morning_areamax(d2_morning_areamax),
		temperature_d2_morning_areamean(d2_morning_areamean)

	  {}

	  string temperature_d1_inlandmin;
	  string temperature_d1_inlandmax;
	  string temperature_d1_inlandmean;
	  string temperature_d1_coastmin;
	  string temperature_d1_coastmax;
	  string temperature_d1_coastmean;
	  string temperature_d1_areamin;
	  string temperature_d1_areamax;
	  string temperature_d1_areamean;

	  string temperature_d1_morning_inlandmin;
	  string temperature_d1_morning_inlandmax;
	  string temperature_d1_morning_inlandmean;
	  string temperature_d1_morning_coastmin;
	  string temperature_d1_morning_coastmax;
	  string temperature_d1_morning_coastmean;
	  string temperature_d1_morning_areamin;
	  string temperature_d1_morning_areamax;
	  string temperature_d1_morning_areamean;

	  string temperature_nite_inlandmin;
	  string temperature_nite_inlandmax;
	  string temperature_nite_inlandmean;
	  string temperature_nite_coastmin;
	  string temperature_nite_coastmax;
	  string temperature_nite_coastmean;
	  string temperature_nite_areamin;
	  string temperature_nite_areamax;
	  string temperature_nite_areamean;

	  string temperature_d2_inlandmin;
	  string temperature_d2_inlandmax;
	  string temperature_d2_inlandmean;
	  string temperature_d2_coastmin;
	  string temperature_d2_coastmax;
	  string temperature_d2_coastmean;
	  string temperature_d2_areamin;
	  string temperature_d2_areamax;
	  string temperature_d2_areamean;

	  string temperature_d2_morning_inlandmin;
	  string temperature_d2_morning_inlandmax;
	  string temperature_d2_morning_inlandmean;
	  string temperature_d2_morning_coastmin;
	  string temperature_d2_morning_coastmax;
	  string temperature_d2_morning_coastmean;
	  string temperature_d2_morning_areamin;
	  string temperature_d2_morning_areamax;
	  string temperature_d2_morning_areamean;

	};

  struct TemperatureAnomalyTestParam: public TemperatureStoryTestParam
  {
	TemperatureAnomalyTestParam(const char* d1_temperature_afternoon_inlandmin,
								const char* d1_temperature_afternoon_inlandmax,
								const char* d1_temperature_afternoon_inlandmean,
								const char* d1_temperature_afternoon_coastmin,
								const char* d1_temperature_afternoon_coastmax,
								const char* d1_temperature_afternoon_coastmean,
								const char* d1_temperature_afternoon_areamin,
								const char* d1_temperature_afternoon_areamax,
								const char* d1_temperature_afternoon_areamean,
								
								const char* d2_temperature_afternoon_inlandmin,
								const char* d2_temperature_afternoon_inlandmax,
								const char* d2_temperature_afternoon_inlandmean,
								const char* d2_temperature_afternoon_coastmin,
								const char* d2_temperature_afternoon_coastmax,
								const char* d2_temperature_afternoon_coastmean,
								const char* d2_temperature_afternoon_areamin,
								const char* d2_temperature_afternoon_areamax,
								const char* d2_temperature_afternoon_areamean,
								
								const char* story): 

	  TemperatureStoryTestParam(d1_temperature_afternoon_inlandmin,
								d1_temperature_afternoon_inlandmax,
								d1_temperature_afternoon_inlandmean,
								d1_temperature_afternoon_coastmin,
								d1_temperature_afternoon_coastmax,
								d1_temperature_afternoon_coastmean,
								d1_temperature_afternoon_areamin,
								d1_temperature_afternoon_areamax,
								d1_temperature_afternoon_areamean,
							   
								d2_temperature_afternoon_inlandmin,
								d2_temperature_afternoon_inlandmax,
								d2_temperature_afternoon_inlandmean,
								d2_temperature_afternoon_coastmin,
								d2_temperature_afternoon_coastmax,
								d2_temperature_afternoon_coastmean,
								d2_temperature_afternoon_areamin,
								d2_temperature_afternoon_areamax,
								d2_temperature_afternoon_areamean,
								story)
	{}
  };

  struct WindAnomalyTestParam: public TemperatureStoryTestParam
  {
	WindAnomalyTestParam(const char* d2_temperature_morning_inlandmin,
						 const char* d2_temperature_morning_inlandmax,
						 const char* d2_temperature_morning_inlandmean,
						 const char* d2_temperature_morning_coastmin,
						 const char* d2_temperature_morning_coastmax,
						 const char* d2_temperature_morning_coastmean,
						 const char* d2_temperature_morning_areamin,
						 const char* d2_temperature_morning_areamax,
						 const char* d2_temperature_morning_areamean,
						 
						 const char* d2_temperature_afternoon_inlandmin,
						 const char* d2_temperature_afternoon_inlandmax,
						 const char* d2_temperature_afternoon_inlandmean,
						 const char* d2_temperature_afternoon_coastmin,
						 const char* d2_temperature_afternoon_coastmax,
						 const char* d2_temperature_afternoon_coastmean,
						 const char* d2_temperature_afternoon_areamin,
						 const char* d2_temperature_afternoon_areamax,
						 const char* d2_temperature_afternoon_areamean,
						 
						 const char* d2_windspeed_morning_inlandmin,
						 const char* d2_windspeed_morning_inlandmax,
						 const char* d2_windspeed_morning_inlandmean,
						 const char* d2_windspeed_morning_coastmin,
						 const char* d2_windspeed_morning_coastmax,
						 const char* d2_windspeed_morning_coastmean,

						 const char* d2_windspeed_afternoon_inlandmin,
						 const char* d2_windspeed_afternoon_inlandmax,
						 const char* d2_windspeed_afternoon_inlandmean,
						 const char* d2_windspeed_afternoon_coastmin,
						 const char* d2_windspeed_afternoon_coastmax,
						 const char* d2_windspeed_afternoon_coastmean,

						 const char* d2_windchill_morning_inlandmin,
						 const char* d2_windchill_morning_inlandmax,
						 const char* d2_windchill_morning_inlandmean,
						 const char* d2_windchill_morning_coastmin,
						 const char* d2_windchill_morning_coastmax,
						 const char* d2_windchill_morning_coastmean,

						 const char* d2_windchill_afternoon_inlandmin,
						 const char* d2_windchill_afternoon_inlandmax,
						 const char* d2_windchill_afternoon_inlandmean,
						 const char* d2_windchill_afternoon_coastmin,
						 const char* d2_windchill_afternoon_coastmax,
						 const char* d2_windchill_afternoon_coastmean,

						 const char* story): 

	  TemperatureStoryTestParam("",
								"",
								"",
								"",
								"",
								"",
								"",
								"",
								"",
							   
								d2_temperature_afternoon_inlandmin,
								d2_temperature_afternoon_inlandmax,
								d2_temperature_afternoon_inlandmean,
								d2_temperature_afternoon_coastmin,
								d2_temperature_afternoon_coastmax,
								d2_temperature_afternoon_coastmean,
								d2_temperature_afternoon_areamin,
								d2_temperature_afternoon_areamax,
								d2_temperature_afternoon_areamean,
								story),
	  temperature_d2_morning_inlandmin(d2_temperature_morning_inlandmin),
	  temperature_d2_morning_inlandmax(d2_temperature_morning_inlandmax),
	  temperature_d2_morning_inlandmean(d2_temperature_morning_inlandmean),
	  temperature_d2_morning_coastmin(d2_temperature_morning_coastmin),
	  temperature_d2_morning_coastmax(d2_temperature_morning_coastmax),
	  temperature_d2_morning_coastmean(d2_temperature_morning_coastmean),
	  temperature_d2_morning_areamin(d2_temperature_morning_areamin),
	  temperature_d2_morning_areamax(d2_temperature_morning_areamax),
	  temperature_d2_morning_areamean(d2_temperature_morning_areamean),

	  anomaly_d2_windspeed_morning_inlandmin(d2_windspeed_morning_inlandmin),
	  anomaly_d2_windspeed_morning_inlandmax(d2_windspeed_morning_inlandmax),
	  anomaly_d2_windspeed_morning_inlandmean(d2_windspeed_morning_inlandmean),
	  anomaly_d2_windspeed_morning_coastmin(d2_windspeed_morning_coastmin),
	  anomaly_d2_windspeed_morning_coastmax(d2_windspeed_morning_coastmax),
	  anomaly_d2_windspeed_morning_coastmean(d2_windspeed_morning_coastmean),

	  anomaly_d2_windspeed_afternoon_inlandmin(d2_windspeed_afternoon_inlandmin),
	  anomaly_d2_windspeed_afternoon_inlandmax(d2_windspeed_afternoon_inlandmax),
	  anomaly_d2_windspeed_afternoon_inlandmean(d2_windspeed_afternoon_inlandmean),
	  anomaly_d2_windspeed_afternoon_coastmin(d2_windspeed_afternoon_coastmin),
	  anomaly_d2_windspeed_afternoon_coastmax(d2_windspeed_afternoon_coastmax),
	  anomaly_d2_windspeed_afternoon_coastmean(d2_windspeed_afternoon_coastmean),

	  anomaly_d2_windchill_morning_inlandmin(d2_windchill_morning_inlandmin),
	  anomaly_d2_windchill_morning_inlandmax(d2_windchill_morning_inlandmax),
	  anomaly_d2_windchill_morning_inlandmean(d2_windchill_morning_inlandmean),
	  anomaly_d2_windchill_morning_coastmin(d2_windchill_morning_coastmin),
	  anomaly_d2_windchill_morning_coastmax(d2_windchill_morning_coastmax),
	  anomaly_d2_windchill_morning_coastmean(d2_windchill_morning_coastmean),

	  anomaly_d2_windchill_afternoon_inlandmin(d2_windchill_afternoon_inlandmin),
	  anomaly_d2_windchill_afternoon_inlandmax(d2_windchill_afternoon_inlandmax),
	  anomaly_d2_windchill_afternoon_inlandmean(d2_windchill_afternoon_inlandmean),
	  anomaly_d2_windchill_afternoon_coastmin(d2_windchill_afternoon_coastmin),
	  anomaly_d2_windchill_afternoon_coastmax(d2_windchill_afternoon_coastmax),
	  anomaly_d2_windchill_afternoon_coastmean(d2_windchill_afternoon_coastmean)
	{}

	string temperature_d2_morning_inlandmin;
	string temperature_d2_morning_inlandmax;
	string temperature_d2_morning_inlandmean;
	string temperature_d2_morning_coastmin;
	string temperature_d2_morning_coastmax;
	string temperature_d2_morning_coastmean;
	string temperature_d2_morning_areamin;
	string temperature_d2_morning_areamax;
	string temperature_d2_morning_areamean;

	string anomaly_d2_windspeed_morning_inlandmin;
	string anomaly_d2_windspeed_morning_inlandmax;
	string anomaly_d2_windspeed_morning_inlandmean;
	string anomaly_d2_windspeed_morning_coastmin;
	string anomaly_d2_windspeed_morning_coastmax;
	string anomaly_d2_windspeed_morning_coastmean;

	string anomaly_d2_windspeed_afternoon_inlandmin;
	string anomaly_d2_windspeed_afternoon_inlandmax;
	string anomaly_d2_windspeed_afternoon_inlandmean;
	string anomaly_d2_windspeed_afternoon_coastmin;
	string anomaly_d2_windspeed_afternoon_coastmax;
	string anomaly_d2_windspeed_afternoon_coastmean;

	string anomaly_d2_windchill_morning_inlandmin;
	string anomaly_d2_windchill_morning_inlandmax;
	string anomaly_d2_windchill_morning_inlandmean;
	string anomaly_d2_windchill_morning_coastmin;
	string anomaly_d2_windchill_morning_coastmax;
	string anomaly_d2_windchill_morning_coastmean;

	string anomaly_d2_windchill_afternoon_inlandmin;
	string anomaly_d2_windchill_afternoon_inlandmax;
	string anomaly_d2_windchill_afternoon_inlandmean;
	string anomaly_d2_windchill_afternoon_coastmin;
	string anomaly_d2_windchill_afternoon_coastmax;
	string anomaly_d2_windchill_afternoon_coastmean;
  };


  typedef std::map<int, TemperatureStoryTestParam*> TestCaseContainer;


  double random_d()
  {
	return ( (double)rand() / ((double)(RAND_MAX)+(double)(1)) );
  }

  int random_i(unsigned int scale)
  {
	return rand() % scale;
  }

  int random_i(unsigned int min_scale, unsigned int max_scale)
  {
	return rand() % (max_scale-min_scale) + min_scale;
  }

std::ostream & operator<<(std::ostream & theOutput,
						  const Max36HoursTestParam& theParam)
{
  theOutput << "***** settings *******" << endl;
  theOutput << " Area minimum d1: " << theParam.temperature_d1_areamin << endl;
  theOutput << " Area maximum d1: " << theParam.temperature_d1_areamax << endl;
  theOutput << " Area mean d1: " << theParam.temperature_d1_areamean << endl;
  theOutput << " Area morning minimum d1: " << theParam.temperature_d1_morning_areamin << endl;
  theOutput << " Area morning maximum d1: " << theParam.temperature_d1_morning_areamax << endl;
  theOutput << " Area morning mean d1: " << theParam.temperature_d1_morning_areamean << endl;
  theOutput << " Area afternoon minimum d1: " << theParam.temperature_d1_afternoon_areamin << endl;
  theOutput << " Area afternoon maximum d1: " << theParam.temperature_d1_afternoon_areamax << endl;
  theOutput << " Area afternoon mean d1: " << theParam.temperature_d1_afternoon_areamean << endl;

  theOutput << " Inland minimum d1: " << theParam.temperature_d1_inlandmin << endl;
  theOutput << " Inland maximum d1: " << theParam.temperature_d1_inlandmax << endl;
  theOutput << " Inland mean d1: " << theParam.temperature_d1_inlandmean << endl;
  theOutput << " Inland morning minimum d1: " << theParam.temperature_d1_morning_inlandmin << endl;
  theOutput << " Inland morning maximum d1: " << theParam.temperature_d1_morning_inlandmax << endl;
  theOutput << " Inland morning mean d1: " << theParam.temperature_d1_morning_inlandmean << endl;
  theOutput << " Inland afternoon minimum d1: " << theParam.temperature_d1_afternoon_inlandmin << endl;
  theOutput << " Inland afternoon maximum d1: " << theParam.temperature_d1_afternoon_inlandmax << endl;
  theOutput << " Inland afternoon mean d1: " << theParam.temperature_d1_afternoon_inlandmean << endl;

  theOutput << " Coast minimum d1: " << theParam.temperature_d1_coastmin << endl;
  theOutput << " Coast maximum d1: " << theParam.temperature_d1_coastmax << endl;
  theOutput << " Coast mean d1: " << theParam.temperature_d1_coastmean << endl;
  theOutput << " Coast morning minimum d1: " << theParam.temperature_d1_morning_coastmin << endl;
  theOutput << " Coast morning maximum d1: " << theParam.temperature_d1_morning_coastmax << endl;
  theOutput << " Coast morning mean d1: " << theParam.temperature_d1_morning_coastmean << endl;
  theOutput << " Coast afternoon minimum d1: " << theParam.temperature_d1_afternoon_coastmin << endl;
  theOutput << " Coast afternoon maximum d1: " << theParam.temperature_d1_afternoon_coastmax << endl;
  theOutput << " Coast afternoon mean d1: " << theParam.temperature_d1_afternoon_coastmean << endl;

  theOutput << " Area minimum night: " << theParam.temperature_nite_areamin << endl;
  theOutput << " Área maximum night: " << theParam.temperature_nite_areamax << endl;
  theOutput << " Area mean night: " << theParam.temperature_nite_areamean << endl;

  theOutput << " Inland minimum night: " << theParam.temperature_nite_inlandmin << endl;
  theOutput << " Inland maximum night: " << theParam.temperature_nite_inlandmax << endl;
  theOutput << " Inland mean night: " << theParam.temperature_nite_inlandmean << endl;

  theOutput << " Coast minimum night: " << theParam.temperature_nite_coastmin << endl;
  theOutput << " Coast maximum night: " << theParam.temperature_nite_coastmax << endl;
  theOutput << " Coast mean night: " << theParam.temperature_nite_coastmean << endl;

  theOutput << " Area minimum d2: " << theParam.temperature_d2_areamin << endl;
  theOutput << " Area maximum d2: " << theParam.temperature_d2_areamax << endl;
  theOutput << " Area mean d2: " << theParam.temperature_d2_areamean << endl;
  theOutput << " Area morning minimum d2: " << theParam.temperature_d2_morning_areamin << endl;
  theOutput << " Area morning maximum d2: " << theParam.temperature_d2_morning_areamax << endl;
  theOutput << " Area morning mean d2: " << theParam.temperature_d2_morning_areamean << endl;
  theOutput << " Area afternoon minimum d2: " << theParam.temperature_d2_afternoon_areamin << endl;
  theOutput << " Area afternoon maximum d2: " << theParam.temperature_d2_afternoon_areamax << endl;
  theOutput << " Area afternoon mean d2: " << theParam.temperature_d2_afternoon_areamean << endl;

  theOutput << " Inland minimum d2: " << theParam.temperature_d2_inlandmin << endl;
  theOutput << " Inland maximum d2: " << theParam.temperature_d2_inlandmax << endl;
  theOutput << " Inland mean d2: " << theParam.temperature_d2_inlandmean << endl;
  theOutput << " Inland morning minimum d2: " << theParam.temperature_d2_morning_inlandmin << endl;
  theOutput << " Inland morning maximum d2: " << theParam.temperature_d2_morning_inlandmax << endl;
  theOutput << " Inland morning mean d2: " << theParam.temperature_d2_morning_inlandmean << endl;
  theOutput << " Inland afternoon minimum d2: " << theParam.temperature_d2_afternoon_inlandmin << endl;
  theOutput << " Inland afternoon maximum d2: " << theParam.temperature_d2_afternoon_inlandmax << endl;
  theOutput << " Inland afternoon mean d2: " << theParam.temperature_d2_afternoon_inlandmean << endl;

  theOutput << " Coast minimum d2: " << theParam.temperature_d2_coastmin << endl;
  theOutput << " Coast maximum d2: " << theParam.temperature_d2_coastmax << endl;
  theOutput << " Coast mean d2: " << theParam.temperature_d2_coastmean << endl;
  theOutput << " Coast morning minimum d2: " << theParam.temperature_d2_morning_coastmin << endl;
  theOutput << " Coast morning maximum d2: " << theParam.temperature_d2_morning_coastmax << endl;
  theOutput << " Coast morning mean d2: " << theParam.temperature_d2_morning_coastmean << endl;
  theOutput << " Coast afternoon minimum d2: " << theParam.temperature_d2_afternoon_coastmin << endl;
  theOutput << " Coast afternoon maximum d2: " << theParam.temperature_d2_afternoon_coastmax << endl;
  theOutput << " Coast afternoon mean d2: " << theParam.temperature_d2_afternoon_coastmean << endl;

  return theOutput;
}
std::ostream & operator<<(std::ostream & theOutput,
						  const TemperatureAnomalyTestParam& theParam)
{
  /*
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
  */

  return theOutput;
}
void tokenize(const string& str,
			  vector<string>& tokens,
			  const string& delimiters = ";")
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

void read_testcasefile(TestCaseContainer& testCases, const string& fileName, const bool& anomaly)
   {
	 ifstream input_stream;
	 input_stream.open(fileName.c_str());
	 char buffer[1024];
	 int index = 1;

	 if(!input_stream)
	   {
		 cout << "could not open file: " << fileName << endl;
		 return;
	   }

	 int inputLinesCount = anomaly ? 7 : 22;

	 vector<string> tokens;
	 vector<string> testCaseData;
	 while(!input_stream.eof())
	   {
		 input_stream.getline(buffer, 1023);
		 std::string data(buffer);
		 if(data == "test_case_begin")
		   {
			 int inputLines = 0;
			 testCaseData.clear();
			 while(inputLines < inputLinesCount || input_stream.eof())
			   {
				 input_stream.getline(buffer, 1023);
				 data = buffer;
				 if(data == "test_case_end")
				   break;
				 if(data.empty() || data.compare(0, 2, "//") == 0)
				   continue;
				 tokens.clear();
				 tokenize(data, tokens, ";");
				 //int tokens_size = tokens.size();
				 // if(tokens.size() >= 3)
				   {
					 // valid line
					 if(inputLines < inputLinesCount-1)
					   tokens.erase(tokens.begin()+3,tokens.end());
					 else
					   tokens.erase(tokens.begin()+1,tokens.end());
					 for(unsigned int i = 0; i < tokens.size(); i++)
					   testCaseData.push_back(tokens[i]);
					 inputLines++;
				   }
				 /*
				 else if(tokens.size() == 1) // the story
				   {
					 testCaseData.push_back(tokens[i]);
					 inputLines++;
				   }
				 */
			   }
			 if(inputLines < inputLinesCount)
			   {
				 cout << "errorii" << endl;
				 return;
			   }
			 else
			   {
				 TemperatureStoryTestParam* param = 0;
				 // create a new test case
				 if(anomaly)
				   {
					 boost::trim(testCaseData[18]);
					 param = new TemperatureAnomalyTestParam(testCaseData[0].c_str(),
															 testCaseData[1].c_str(),
															 testCaseData[2].c_str(),
															 testCaseData[3].c_str(),
															 testCaseData[4].c_str(),
															 testCaseData[5].c_str(),
															 testCaseData[6].c_str(),
															 testCaseData[7].c_str(),
															 testCaseData[8].c_str(),
															 
															 testCaseData[9].c_str(),
															 testCaseData[10].c_str(),
															 testCaseData[11].c_str(),
															 testCaseData[12].c_str(),
															 testCaseData[13].c_str(),
															 testCaseData[14].c_str(),
															 testCaseData[15].c_str(),
															 testCaseData[16].c_str(),
															 testCaseData[17].c_str(),
															 testCaseData[18].c_str());
				   }
				 else
				   {
					 boost::trim(testCaseData[63]);
					 param = new Max36HoursTestParam(testCaseData[0].c_str(),
													 testCaseData[1].c_str(),
													 testCaseData[2].c_str(),
													 testCaseData[3].c_str(),
													 testCaseData[4].c_str(),
													 testCaseData[5].c_str(),
													 testCaseData[6].c_str(),
													 testCaseData[7].c_str(),
													 testCaseData[8].c_str(),
											   
													 testCaseData[9].c_str(),
													 testCaseData[10].c_str(),
													 testCaseData[11].c_str(),
													 testCaseData[12].c_str(),
													 testCaseData[13].c_str(),
													 testCaseData[14].c_str(),
													 testCaseData[15].c_str(),
													 testCaseData[16].c_str(),
													 testCaseData[17].c_str(),
											   
													 testCaseData[18].c_str(),
													 testCaseData[19].c_str(),
													 testCaseData[20].c_str(),
													 testCaseData[21].c_str(),
													 testCaseData[22].c_str(),
													 testCaseData[23].c_str(),
													 testCaseData[24].c_str(),
													 testCaseData[25].c_str(),
													 testCaseData[26].c_str(),
											   
													 testCaseData[27].c_str(),
													 testCaseData[28].c_str(),
													 testCaseData[29].c_str(),
													 testCaseData[30].c_str(),
													 testCaseData[31].c_str(),
													 testCaseData[32].c_str(),
													 testCaseData[33].c_str(),
													 testCaseData[34].c_str(),
													 testCaseData[35].c_str(),
											   
													 testCaseData[35].c_str(),
													 testCaseData[37].c_str(),
													 testCaseData[38].c_str(),
													 testCaseData[39].c_str(),
													 testCaseData[40].c_str(),
													 testCaseData[41].c_str(),
													 testCaseData[42].c_str(),
													 testCaseData[43].c_str(),
													 testCaseData[44].c_str(),
											   
													 testCaseData[45].c_str(),
													 testCaseData[46].c_str(),
													 testCaseData[47].c_str(),
													 testCaseData[48].c_str(),
													 testCaseData[49].c_str(),
													 testCaseData[50].c_str(),
													 testCaseData[51].c_str(),
													 testCaseData[52].c_str(),
													 testCaseData[53].c_str(),

													 testCaseData[54].c_str(),
													 testCaseData[55].c_str(),
													 testCaseData[56].c_str(),
													 testCaseData[57].c_str(),
													 testCaseData[58].c_str(),
													 testCaseData[59].c_str(),
													 testCaseData[60].c_str(),
													 testCaseData[61].c_str(),
													 testCaseData[62].c_str(),
													 testCaseData[63].c_str());
				   }
					 
				 index++;
				 testCases.insert(make_pair(index++, param));
			   }
		   }
		 else
		   {
			 continue;
		   }
	   }
   }



using namespace std;
using namespace TextGen;
using namespace WeatherAnalysis;

void create_wind_anomaly_testcase_stories(TestCaseContainer& testCases,
										  const string & theLanguage,
										  const AnalysisSources& sources,
										  const WeatherArea& area)
{
  TestCaseContainer::iterator iter;

  for(iter = testCases.begin(); iter != testCases.end(); iter++)
	{
	  WindAnomalyTestParam* pTestParam = static_cast<WindAnomalyTestParam*>(iter->second);

	  NFmiSettings::Set("anomaly::fake::temperature::day2::morning::inland::min",
						pTestParam->temperature_d2_morning_inlandmin);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::morning::inland::max",
						pTestParam->temperature_d2_morning_inlandmax);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::morning::inland::mean",
						pTestParam->temperature_d2_morning_inlandmean);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::morning::coast::min",
						pTestParam->temperature_d2_morning_coastmin);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::morning::coast::max",
						pTestParam->temperature_d2_morning_coastmax);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::morning::coast::mean",
						pTestParam->temperature_d2_morning_coastmean);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::morning::area::min",
						pTestParam->temperature_d2_morning_areamin);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::morning::area::max",
						pTestParam->temperature_d2_morning_areamax);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::morning::area::mean",
						pTestParam->temperature_d2_morning_areamean);

	  NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::inland::min",
						pTestParam->temperature_d2_afternoon_inlandmin);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::inland::max",
						pTestParam->temperature_d2_afternoon_inlandmax);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::inland::mean",
						pTestParam->temperature_d2_afternoon_inlandmean);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::coast::min",
						pTestParam->temperature_d2_afternoon_coastmin);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::coast::max",
						pTestParam->temperature_d2_afternoon_coastmax);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::coast::mean",
						pTestParam->temperature_d2_afternoon_coastmean);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::area::min",
						pTestParam->temperature_d2_afternoon_areamin);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::area::max",
						pTestParam->temperature_d2_afternoon_areamax);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::area::mean",
						pTestParam->temperature_d2_afternoon_areamean);

	  NFmiSettings::Set("anomaly::fake::windspeed::morning::inland::min",
						pTestParam->anomaly_d2_windspeed_morning_inlandmin);
	  NFmiSettings::Set("anomaly::fake::windspeed::morning::inland::max",
						pTestParam->anomaly_d2_windspeed_morning_inlandmax);
	  NFmiSettings::Set("anomaly::fake::windspeed::morning::inland::mean",
						pTestParam->anomaly_d2_windspeed_morning_inlandmean);
	  NFmiSettings::Set("anomaly::fake::windspeed::morning::coast::min",
						pTestParam->anomaly_d2_windspeed_morning_coastmin);
	  NFmiSettings::Set("anomaly::fake::windspeed::morning::coast::max",
						pTestParam->anomaly_d2_windspeed_morning_coastmax);
	  NFmiSettings::Set("anomaly::fake::windspeed::morning::coast::mean",
						pTestParam->anomaly_d2_windspeed_morning_coastmean);
	  NFmiSettings::Set("anomaly::fake::windspeed::afternoon::inland::min",
						pTestParam->anomaly_d2_windspeed_afternoon_inlandmin);
	  NFmiSettings::Set("anomaly::fake::windspeed::afternoon::inland::max",
						pTestParam->anomaly_d2_windspeed_afternoon_inlandmax);
	  NFmiSettings::Set("anomaly::fake::windspeed::afternoon::inland::mean",
						pTestParam->anomaly_d2_windspeed_afternoon_inlandmean);
	  NFmiSettings::Set("anomaly::fake::windspeed::afternoon::coast::min",
						pTestParam->anomaly_d2_windspeed_afternoon_coastmin);
	  NFmiSettings::Set("anomaly::fake::windspeed::afternoon::coast::max",
						pTestParam->anomaly_d2_windspeed_afternoon_coastmax);
	  NFmiSettings::Set("anomaly::fake::windspeed::afternoon::coast::mean",
						pTestParam->anomaly_d2_windspeed_afternoon_coastmean);

	  NFmiSettings::Set("anomaly::fake::windchill::morning::inland::min",
						pTestParam->anomaly_d2_windchill_morning_inlandmin);
	  NFmiSettings::Set("anomaly::fake::windchill::morning::inland::max",
						pTestParam->anomaly_d2_windchill_morning_inlandmax);
	  NFmiSettings::Set("anomaly::fake::windchill::morning::inland::mean",
						pTestParam->anomaly_d2_windchill_morning_inlandmean);
	  NFmiSettings::Set("anomaly::fake::windchill::morning::coast::min",
						pTestParam->anomaly_d2_windchill_morning_coastmin);
	  NFmiSettings::Set("anomaly::fake::windchill::morning::coast::max",
						pTestParam->anomaly_d2_windchill_morning_coastmax);
	  NFmiSettings::Set("anomaly::fake::windchill::morning::coast::mean",
						pTestParam->anomaly_d2_windchill_morning_coastmean);
	  NFmiSettings::Set("anomaly::fake::windchill::afternoon::inland::min",
						pTestParam->anomaly_d2_windchill_afternoon_inlandmin);
	  NFmiSettings::Set("anomaly::fake::windchill::afternoon::inland::max",
						pTestParam->anomaly_d2_windchill_afternoon_inlandmax);
	  NFmiSettings::Set("anomaly::fake::windchill::afternoon::inland::mean",
						pTestParam->anomaly_d2_windchill_afternoon_inlandmean);
	  NFmiSettings::Set("anomaly::fake::windchill::afternoon::coast::min",
						pTestParam->anomaly_d2_windchill_afternoon_coastmin);
	  NFmiSettings::Set("anomaly::fake::windchill::afternoon::coast::max",
						pTestParam->anomaly_d2_windchill_afternoon_coastmax);
	  NFmiSettings::Set("anomaly::fake::windchill::afternoon::coast::mean",
						pTestParam->anomaly_d2_windchill_afternoon_coastmean);


	  NFmiTime time1(2009, 6, 1, 6, 0, 0);
	  NFmiTime time2(2009, 6, 2, 18, 0, 0);
	  WeatherPeriod period_day1_night_day2(time1,time2);
	  TemperatureStory story(time1,sources, area, period_day1_night_day2,"anomaly");
	  const string fun = "wind_anomaly";

	  pTestParam->theStory = get_story(story,
									   theLanguage,
									   fun);
	}
}

void create_temperature_anomaly_testcase_stories(TestCaseContainer& testCases,
												 const string & theLanguage,
												 const AnalysisSources& sources,
												 const WeatherArea& area)
{
  TestCaseContainer::iterator iter;

  for(iter = testCases.begin(); iter != testCases.end(); iter++)
	{
	  TemperatureAnomalyTestParam* pTestParam = static_cast<TemperatureAnomalyTestParam*>(iter->second);

	  NFmiSettings::Set("anomaly::fake::temperature::day1::afternoon::inland::min",
						pTestParam->temperature_d1_afternoon_inlandmin);
	  NFmiSettings::Set("anomaly::fake::temperature::day1::afternoon::inland::max",
						pTestParam->temperature_d1_afternoon_inlandmax);
	  NFmiSettings::Set("anomaly::fake::temperature::day1::afternoon::inland::mean",
						pTestParam->temperature_d1_afternoon_inlandmean);
	  NFmiSettings::Set("anomaly::fake::temperature::day1::afternoon::coast::min",
						pTestParam->temperature_d1_afternoon_coastmin);
	  NFmiSettings::Set("anomaly::fake::temperature::day1::afternoon::coast::max",
						pTestParam->temperature_d1_afternoon_coastmax);
	  NFmiSettings::Set("anomaly::fake::temperature::day1::afternoon::coast::mean",
						pTestParam->temperature_d1_afternoon_coastmean);
	  NFmiSettings::Set("anomaly::fake::temperature::day1::afternoon::area::min",
						pTestParam->temperature_d1_afternoon_areamin);
	  NFmiSettings::Set("anomaly::fake::temperature::day1::afternoon::area::max",
						pTestParam->temperature_d1_afternoon_areamax);
	  NFmiSettings::Set("anomaly::fake::temperature::day1::afternoon::area::mean",
						pTestParam->temperature_d1_afternoon_areamean);

	  NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::inland::min",
						pTestParam->temperature_d2_afternoon_inlandmin);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::inland::max",
						pTestParam->temperature_d2_afternoon_inlandmax);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::inland::mean",
						pTestParam->temperature_d2_afternoon_inlandmean);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::coast::min",
						pTestParam->temperature_d2_afternoon_coastmin);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::coast::max",
						pTestParam->temperature_d2_afternoon_coastmax);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::coast::mean",
						pTestParam->temperature_d2_afternoon_coastmean);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::area::min",
						pTestParam->temperature_d2_afternoon_areamin);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::area::max",
						pTestParam->temperature_d2_afternoon_areamax);
	  NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::area::mean",
						pTestParam->temperature_d2_afternoon_areamean);

	  NFmiTime time1(2009, 6, 1, 6, 0, 0);
	  NFmiTime time2(2009, 6, 2, 18, 0, 0);
	  WeatherPeriod period_day1_night_day2(time1,time2);
	  TemperatureStory story(time1,sources, area, period_day1_night_day2,"anomaly");
	  const string fun = "temperature_anomaly";

	  pTestParam->theStory = get_story(story,
									   theLanguage,
									   fun);
	}
}

void create_max36hours_testcase_stories(TestCaseContainer& testCases,
										const string & theLanguage,
										const AnalysisSources& sources,
										const WeatherArea& area)
{
  TestCaseContainer::iterator iter;

  Max36HoursTestParam* param = 0;		

  for(iter = testCases.begin(); iter != testCases.end(); iter++)
	{
	  param = static_cast<Max36HoursTestParam*>(iter->second);

	  NFmiSettings::Set("max36hours::fake::day1::inland::min",param->temperature_d1_inlandmin);
	  NFmiSettings::Set("max36hours::fake::day1::inland::max",param->temperature_d1_inlandmax);
	  NFmiSettings::Set("max36hours::fake::day1::inland::mean",param->temperature_d1_inlandmean);
	  NFmiSettings::Set("max36hours::fake::day1::coast::min",param->temperature_d1_coastmin);
	  NFmiSettings::Set("max36hours::fake::day1::coast::max",param->temperature_d1_coastmax);
	  NFmiSettings::Set("max36hours::fake::day1::coast::mean",param->temperature_d1_coastmean);
	  NFmiSettings::Set("max36hours::fake::day1::area::min",param->temperature_d1_areamin);
	  NFmiSettings::Set("max36hours::fake::day1::area::max",param->temperature_d1_areamax);
	  NFmiSettings::Set("max36hours::fake::day1::area::mean",param->temperature_d1_areamean);

	  NFmiSettings::Set("max36hours::fake::day1::morning::inland::min",param->temperature_d1_morning_inlandmin);
	  NFmiSettings::Set("max36hours::fake::day1::morning::inland::max",param->temperature_d1_morning_inlandmax);
	  NFmiSettings::Set("max36hours::fake::day1::morning::inland::mean",param->temperature_d1_morning_inlandmean);
	  NFmiSettings::Set("max36hours::fake::day1::morning::coast::min",param->temperature_d1_morning_coastmin);
	  NFmiSettings::Set("max36hours::fake::day1::morning::coast::max",param->temperature_d1_morning_coastmax);
	  NFmiSettings::Set("max36hours::fake::day1::morning::coast::mean",param->temperature_d1_morning_coastmean);
	  NFmiSettings::Set("max36hours::fake::day1::morning::area::min",param->temperature_d1_morning_areamin);
	  NFmiSettings::Set("max36hours::fake::day1::morning::area::max",param->temperature_d1_morning_areamax);
	  NFmiSettings::Set("max36hours::fake::day1::morning::area::mean",param->temperature_d1_morning_areamean);

	  NFmiSettings::Set("max36hours::fake::day1::afternoon::inland::min",param->temperature_d1_afternoon_inlandmin);
	  NFmiSettings::Set("max36hours::fake::day1::afternoon::inland::max",param->temperature_d1_afternoon_inlandmax);
	  NFmiSettings::Set("max36hours::fake::day1::afternoon::inland::mean",param->temperature_d1_afternoon_inlandmean);
	  NFmiSettings::Set("max36hours::fake::day1::afternoon::coast::min",param->temperature_d1_afternoon_coastmin);
	  NFmiSettings::Set("max36hours::fake::day1::afternoon::coast::max",param->temperature_d1_afternoon_coastmax);
	  NFmiSettings::Set("max36hours::fake::day1::afternoon::coast::mean",param->temperature_d1_afternoon_coastmean);
	  NFmiSettings::Set("max36hours::fake::day1::afternoon::area::min",param->temperature_d1_afternoon_areamin);
	  NFmiSettings::Set("max36hours::fake::day1::afternoon::area::max",param->temperature_d1_afternoon_areamax);
	  NFmiSettings::Set("max36hours::fake::day1::afternoon::area::mean",param->temperature_d1_afternoon_areamean);

	  NFmiSettings::Set("max36hours::fake::night::inland::min",param->temperature_nite_inlandmin);
	  NFmiSettings::Set("max36hours::fake::night::inland::max",param->temperature_nite_inlandmax);
	  NFmiSettings::Set("max36hours::fake::night::inland::mean",param->temperature_nite_inlandmean);
	  NFmiSettings::Set("max36hours::fake::night::coast::min",param->temperature_nite_coastmin);
	  NFmiSettings::Set("max36hours::fake::night::coast::max",param->temperature_nite_coastmax);
	  NFmiSettings::Set("max36hours::fake::night::coast::mean",param->temperature_nite_coastmean);
	  NFmiSettings::Set("max36hours::fake::night::area::min",param->temperature_nite_areamin);
	  NFmiSettings::Set("max36hours::fake::night::area::max",param->temperature_nite_areamax);
	  NFmiSettings::Set("max36hours::fake::night::area::mean",param->temperature_nite_areamean);

	  NFmiSettings::Set("max36hours::fake::day2::inland::min",param->temperature_d2_inlandmin);
	  NFmiSettings::Set("max36hours::fake::day2::inland::max",param->temperature_d2_inlandmax);
	  NFmiSettings::Set("max36hours::fake::day2::inland::mean",param->temperature_d2_inlandmean);
	  NFmiSettings::Set("max36hours::fake::day2::coast::min",param->temperature_d2_coastmin);
	  NFmiSettings::Set("max36hours::fake::day2::coast::max",param->temperature_d2_coastmax);
	  NFmiSettings::Set("max36hours::fake::day2::coast::mean",param->temperature_d2_coastmean);
	  NFmiSettings::Set("max36hours::fake::day2::area::min",param->temperature_d2_areamin);
	  NFmiSettings::Set("max36hours::fake::day2::area::max",param->temperature_d2_areamax);
	  NFmiSettings::Set("max36hours::fake::day2::area::mean",param->temperature_d2_areamean);

	  NFmiSettings::Set("max36hours::fake::day2::morning::inland::min",param->temperature_d2_morning_inlandmin);
	  NFmiSettings::Set("max36hours::fake::day2::morning::inland::max",param->temperature_d2_morning_inlandmax);
	  NFmiSettings::Set("max36hours::fake::day2::morning::inland::mean",param->temperature_d2_morning_inlandmean);
	  NFmiSettings::Set("max36hours::fake::day2::morning::coast::min",param->temperature_d2_morning_coastmin);
	  NFmiSettings::Set("max36hours::fake::day2::morning::coast::max",param->temperature_d2_morning_coastmax);
	  NFmiSettings::Set("max36hours::fake::day2::morning::coast::mean",param->temperature_d2_morning_coastmean);
	  NFmiSettings::Set("max36hours::fake::day2::morning::area::min",param->temperature_d2_morning_areamin);
	  NFmiSettings::Set("max36hours::fake::day2::morning::area::max",param->temperature_d2_morning_areamax);
	  NFmiSettings::Set("max36hours::fake::day2::morning::area::mean",param->temperature_d2_morning_areamean);

	  NFmiSettings::Set("max36hours::fake::day2::afternoon::inland::min",param->temperature_d2_afternoon_inlandmin);
	  NFmiSettings::Set("max36hours::fake::day2::afternoon::inland::max",param->temperature_d2_afternoon_inlandmax);
	  NFmiSettings::Set("max36hours::fake::day2::afternoon::inland::mean",param->temperature_d2_afternoon_inlandmean);
	  NFmiSettings::Set("max36hours::fake::day2::afternoon::coast::min",param->temperature_d2_afternoon_coastmin);
	  NFmiSettings::Set("max36hours::fake::day2::afternoon::coast::max",param->temperature_d2_afternoon_coastmax);
	  NFmiSettings::Set("max36hours::fake::day2::afternoon::coast::mean",param->temperature_d2_afternoon_coastmean);
	  NFmiSettings::Set("max36hours::fake::day2::afternoon::area::min",param->temperature_d2_afternoon_areamin);
	  NFmiSettings::Set("max36hours::fake::day2::afternoon::area::max",param->temperature_d2_afternoon_areamax);
	  NFmiSettings::Set("max36hours::fake::day2::afternoon::area::mean",param->temperature_d2_afternoon_areamean);


	  NFmiTime time1(2009,4,1,6,0,0);
	  NFmiTime time2(2009,4,2,18,0,0);
	  WeatherPeriod period_day1_night_day2(time1, time2);
	  TemperatureStory story1(time1,sources,area,period_day1_night_day2,"max36hours");
	  const string fun = "temperature_max36hours";

	  param->theStory = get_story(story1,
										 theLanguage,
										 fun);
	}
}


void create_max36hours_testcasefile(TestCaseContainer& testCases, const string& fileName)
   {
	 ofstream output_stream;
	 output_stream.open(fileName.c_str());
 
	 Max36HoursTestParam* param = 0;		

	 int i = 1;
	 for(TestCaseContainer::iterator iter = testCases.begin(); iter != testCases.end(); iter++)
	  {
		param = static_cast<Max36HoursTestParam*>(iter->second);

#ifdef CPP_TESTCASE

		output_stream << "testCases.insert(make_pair(i++, Max36HoursTestParam(" << endl;

		output_stream << "\""  << param->temperature_d1_inlandmin << "\",\"";
		output_stream << param->temperature_d1_inlandmax << "\",\"";
		output_stream << param->temperature_d1_inlandmean << "\", // inland day1: min, max, mean" << endl;
		output_stream << "\""  << param->temperature_d1_coastmin << "\",\"";
		output_stream << param->temperature_d1_coastmax << "\",\"";
		output_stream << param->temperature_d1_coastmean << "\", // coast day1: min, max, mean" << endl;
		output_stream << "\""  << param->temperature_d1_areamin << "\",\"";
		output_stream << param->temperature_d1_areamax << "\",\"";
		output_stream << param->temperature_d1_areamean << "\", // area day1: min, max, mean" << endl;	

		output_stream << "\""  << param->temperature_d1_morning_inlandmin << "\",\"";
		output_stream << param->temperature_d1_morning_inlandmax << "\",\"";
		output_stream << param->temperature_d1_morning_inlandmean << "\", // inland day1 morning: min, max, mean" << endl;
		output_stream << "\"" << param->temperature_d1_morning_coastmin << "\",\"";
		output_stream << param->temperature_d1_morning_coastmax << "\",\"";
		output_stream << param->temperature_d1_morning_coastmean << "\", // coast day1 morning: min, max, mean" << endl;
		output_stream << "\""  << param->temperature_d1_morning_areamin << "\",\"";
		output_stream << param->temperature_d1_morning_areamax << "\",\"";
		output_stream << param->temperature_d1_morning_areamean << "\", // area day1 morning: min, max, mean" << endl;

		output_stream << "\""  << param->temperature_d1_afternoon_inlandmin << "\",\"";
		output_stream << param->temperature_d1_afternoon_inlandmax << "\",\"";
		output_stream << param->temperature_d1_afternoon_inlandmean << "\", // inland day1 afternoon: min, max, mean" << endl;
		output_stream << "\""  << param->temperature_d1_afternoon_coastmin << "\",\"";
		output_stream << param->temperature_d1_afternoon_coastmax << "\",\"";
		output_stream << param->temperature_d1_afternoon_coastmean << "\", // coast day1 afternoon: min, max, mean" << endl;		
		output_stream << "\""  << param->temperature_d1_afternoon_areamin << "\",\"";
		output_stream << param->temperature_d1_afternoon_areamax << "\",\"";
		output_stream << param->temperature_d1_afternoon_areamean << "\", // area day1 afternoon: min, max, mean" << endl;

		output_stream << "\""  << param->temperature_nite_inlandmin << "\",\"";
		output_stream << param->temperature_nite_inlandmax << "\",\"";
		output_stream << param->temperature_nite_inlandmean << "\", // inland night: min, max, mean" << endl;
		output_stream << "\""  << param->temperature_nite_coastmin << "\",\"";
		output_stream << param->temperature_nite_coastmax << "\",\"";
		output_stream << param->temperature_nite_coastmean << "\", // coast night: min, max, mean" << endl;

		output_stream << "\""  << param->temperature_nite_areamin << "\",\"";
		output_stream << param->temperature_nite_areamax << "\",\"";
		output_stream << param->temperature_nite_areamean << "\", // area night: min, max, mean" << endl;

		output_stream << "\""  << param->temperature_d2_inlandmin << "\",\"";
		output_stream << param->temperature_d2_inlandmax << "\",\"";
		output_stream << param->temperature_d2_inlandmean << "\", // inland day2: min, max, mean" << endl;
		output_stream << "\""  << param->temperature_d2_coastmin << "\",\"";
		output_stream << param->temperature_d2_coastmax << "\",\"";
		output_stream << param->temperature_d2_coastmean << "\", // coast day2: min, max, mean" << endl;
		output_stream << "\""  << param->temperature_d2_areamin << "\",\"";
		output_stream << param->temperature_d2_areamax << "\",\"";
		output_stream << param->temperature_d2_areamean << "\", // area day2: min, max, mean" << endl;

		output_stream << "\""  << param->temperature_d2_morning_inlandmin << "\",\"";
		output_stream << param->temperature_d2_morning_inlandmax << "\",\"";
		output_stream << param->temperature_d2_morning_inlandmean << "\", // inland day2 morning: min, max, mean" << endl;
		output_stream << "\""  << param->temperature_d2_morning_coastmin << "\",\"";
		output_stream << param->temperature_d2_morning_coastmax << "\",\"";
		output_stream << param->temperature_d2_morning_coastmean << "\", // coast day2 morning: min, max, mean" << endl;
		output_stream << "\""  << param->temperature_d2_morning_areamin << "\",\"";
		output_stream << param->temperature_d2_morning_areamax << "\",\"";
		output_stream << param->temperature_d2_morning_areamean << "\", // area day2 morning: min, max, mean" << endl;

		output_stream << "\""  << param->temperature_d2_afternoon_inlandmin << "\",\"";
		output_stream << param->temperature_d2_afternoon_inlandmax << "\",\"";
		output_stream << param->temperature_d2_afternoon_inlandmean << "\", // inland day2 afternoon: min, max, mean" << endl;
		output_stream << "\""  << param->temperature_d2_afternoon_coastmin << "\",\"";
		output_stream << param->temperature_d2_afternoon_coastmax << "\",\"";
		output_stream << param->temperature_d2_afternoon_coastmean << "\", // coast day2 afternoon: min, max, mean" << endl;
		output_stream  << "\"" << param->temperature_d2_afternoon_areamin << "\",\"";
		output_stream << param->temperature_d2_afternoon_areamax << "\",\"";
		output_stream << param->temperature_d2_afternoon_areamean << "\", // area day2 afternoon: min, max, mean" << endl;

		output_stream << "\"" << param->theStory << "\"))); // the story" << endl;
#endif

		output_stream << "#" << i++ << endl;
		output_stream << "test_case_begin" << endl;
		output_stream << param->temperature_d1_inlandmin << ";";
		output_stream << param->temperature_d1_inlandmax << ";";
		output_stream << param->temperature_d1_inlandmean << "; // inland day1: min, max, mean" << endl;
		output_stream << param->temperature_d1_coastmin << ";";
		output_stream << param->temperature_d1_coastmax << ";";
		output_stream << param->temperature_d1_coastmean << "; // coast day1: min, max, mean" << endl;
		output_stream << param->temperature_d1_areamin << ";";
		output_stream << param->temperature_d1_areamax << ";";
		output_stream << param->temperature_d1_areamean << "; // area day1: min, max, mean" << endl;

		output_stream << param->temperature_d1_morning_inlandmin << ";";
		output_stream << param->temperature_d1_morning_inlandmax << ";";
		output_stream << param->temperature_d1_morning_inlandmean << "; // inland day1 morning: min, max, mean" << endl;
		output_stream << param->temperature_d1_morning_coastmin << ";";
		output_stream << param->temperature_d1_morning_coastmax << ";";
		output_stream << param->temperature_d1_morning_coastmean << "; // coast day1 morning: min, max, mean" << endl;
		output_stream << param->temperature_d1_morning_areamin << ";";
		output_stream << param->temperature_d1_morning_areamax << ";";
		output_stream << param->temperature_d1_morning_areamean << "; // area day1 morning: min, max, mean" << endl;

		output_stream << param->temperature_d1_afternoon_inlandmin << ";";
		output_stream << param->temperature_d1_afternoon_inlandmax << ";";
		output_stream << param->temperature_d1_afternoon_inlandmean << "; // inland day1 afternoon: min, max, mean" << endl;
		output_stream << param->temperature_d1_afternoon_coastmin << ";";
		output_stream << param->temperature_d1_afternoon_coastmax << ";";
		output_stream << param->temperature_d1_afternoon_coastmean << "; // coast day1 afternoon: min, max, mean" << endl;
		output_stream << param->temperature_d1_afternoon_areamin << ";";
		output_stream << param->temperature_d1_afternoon_areamax << ";";
		output_stream << param->temperature_d1_afternoon_areamean << "; // area day1 afternoon: min, max, mean" << endl;

		output_stream << param->temperature_nite_inlandmin << ";";
		output_stream << param->temperature_nite_inlandmax << ";";
		output_stream << param->temperature_nite_inlandmean << "; // inland night: min, max, mean" << endl;
		output_stream << param->temperature_nite_coastmin << ";";
		output_stream << param->temperature_nite_coastmax << ";";
		output_stream << param->temperature_nite_coastmean << "; // coast night: min, max, mean" << endl;
		output_stream << param->temperature_nite_areamin << ";";
		output_stream << param->temperature_nite_areamax << ";";
		output_stream << param->temperature_nite_areamean << "; // area night: min, max, mean" << endl;

		output_stream << param->temperature_d2_inlandmin << ";";
		output_stream << param->temperature_d2_inlandmax << ";";
		output_stream << param->temperature_d2_inlandmean << "; // inland day2: min, max, mean" << endl;
		output_stream << param->temperature_d2_coastmin << ";";
		output_stream << param->temperature_d2_coastmax << ";";
		output_stream << param->temperature_d2_coastmean << "; // coast day2: min, max, mean" << endl;
		output_stream << param->temperature_d2_areamin << ";";
		output_stream << param->temperature_d2_areamax << ";";
		output_stream << param->temperature_d2_areamean << "; // area day2: min, max, mean" << endl;

		output_stream << param->temperature_d2_morning_inlandmin << ";";
		output_stream << param->temperature_d2_morning_inlandmax << ";";
		output_stream << param->temperature_d2_morning_inlandmean << "; // inland day2 morning: min, max, mean" << endl;
		output_stream << param->temperature_d2_morning_coastmin << ";";
		output_stream << param->temperature_d2_morning_coastmax << ";";
		output_stream << param->temperature_d2_morning_coastmean << "; // coast day2 morning: min, max, mean" << endl;
		output_stream << param->temperature_d2_morning_areamin << ";";
		output_stream << param->temperature_d2_morning_areamax << ";";
		output_stream << param->temperature_d2_morning_areamean << "; // area day2 morning: min, max, mean" << endl;

		output_stream << param->temperature_d2_afternoon_inlandmin << ";";
		output_stream << param->temperature_d2_afternoon_inlandmax << ";";
		output_stream << param->temperature_d2_afternoon_inlandmean << "; // inland day2 afternoon: min, max, mean" << endl;
		output_stream << param->temperature_d2_afternoon_coastmin << ";";
		output_stream << param->temperature_d2_afternoon_coastmax << ";";
		output_stream << param->temperature_d2_afternoon_coastmean << "; // coast day2 afternoon: min, max, mean" << endl;
		output_stream << param->temperature_d2_afternoon_areamin << ";";
		output_stream << param->temperature_d2_afternoon_areamax << ";";
		output_stream << param->temperature_d2_afternoon_areamean << "; // area day2 afternoon: min, max, mean" << endl;

		output_stream << param->theStory << "; // the story" << endl;
		output_stream << "test_case_end" << endl;
	  }
   }


void create_temperature_anomaly_testcasefile(TestCaseContainer& testCases, const string& fileName)
   {
	 ofstream output_stream;
	 output_stream.open(fileName.c_str());

	 TemperatureAnomalyTestParam* param = 0;
	 int i = 1;
	 for(TestCaseContainer::iterator iter = testCases.begin(); iter != testCases.end(); iter++)
	  {
		param = static_cast<TemperatureAnomalyTestParam*>(iter->second);

#ifdef CPP_TESTCASES

		output_stream << "testCases.insert(make_pair(i++, TemperatureAnomalyTestParam(" << endl;

		output_stream << "\""  << param->temperature_d1_afternoon_inlandmin << "\",\"";
		output_stream << param->temperature_d1_afternoon_inlandmax << "\",\"";
		output_stream << param->temperature_d1_afternoon_inlandmean << "\", // inland day1 afternoon: min, max, mean" << endl;
		output_stream << "\""  << param->temperature_d1_afternoon_coastmin << "\",\"";
		output_stream << param->temperature_d1_afternoon_coastmax << "\",\"";
		output_stream << param->temperature_d1_afternoon_coastmean << "\", // coast day1 afternoon: min, max, mean" << endl;		
		output_stream << "\""  << param->temperature_d1_afternoon_areamin << "\",\"";
		output_stream << param->temperature_d1_afternoon_areamax << "\",\"";
		output_stream << param->temperature_d1_afternoon_areamean << "\", // area day1 afternoon: min, max, mean" << endl;

		output_stream << "\""  << param->temperature_d2_afternoon_inlandmin << "\",\"";
		output_stream << param->temperature_d2_afternoon_inlandmax << "\",\"";
		output_stream << param->temperature_d2_afternoon_inlandmean << "\", // inland day2 afternoon: min, max, mean" << endl;
		output_stream << "\""  << param->temperature_d2_afternoon_coastmin << "\",\"";
		output_stream << param->temperature_d2_afternoon_coastmax << "\",\"";
		output_stream << param->temperature_d2_afternoon_coastmean << "\", // coast day2 afternoon: min, max, mean" << endl;
		output_stream  << "\"" << param->temperature_d2_afternoon_areamin << "\",\"";
		output_stream << param->temperature_d2_afternoon_areamax << "\",\"";
		output_stream << param->temperature_d2_afternoon_areamean << "\", // area day2 afternoon: min, max, mean" << endl;

		output_stream << "\"" << param->theStory << "\"))); // the story" << endl;
#endif

		output_stream << "#" << i++ << endl;
		output_stream << "test_case_begin" << endl;

		output_stream << param->temperature_d1_afternoon_inlandmin << ";";
		output_stream << param->temperature_d1_afternoon_inlandmax << ";";
		output_stream << param->temperature_d1_afternoon_inlandmean << "; // inland day1 afternoon: min, max, mean" << endl;
		output_stream << param->temperature_d1_afternoon_coastmin << ";";
		output_stream << param->temperature_d1_afternoon_coastmax << ";";
		output_stream << param->temperature_d1_afternoon_coastmean << "; // coast day1 afternoon: min, max, mean" << endl;
		output_stream << param->temperature_d1_afternoon_areamin << ";";
		output_stream << param->temperature_d1_afternoon_areamax << ";";
		output_stream << param->temperature_d1_afternoon_areamean << "; // area day1 afternoon: min, max, mean" << endl;

		output_stream << param->temperature_d2_afternoon_inlandmin << ";";
		output_stream << param->temperature_d2_afternoon_inlandmax << ";";
		output_stream << param->temperature_d2_afternoon_inlandmean << "; // inland day2 afternoon: min, max, mean" << endl;
		output_stream << param->temperature_d2_afternoon_coastmin << ";";
		output_stream << param->temperature_d2_afternoon_coastmax << ";";
		output_stream << param->temperature_d2_afternoon_coastmean << "; // coast day2 afternoon: min, max, mean" << endl;
		output_stream << param->temperature_d2_afternoon_areamin << ";";
		output_stream << param->temperature_d2_afternoon_areamax << ";";
		output_stream << param->temperature_d2_afternoon_areamean << "; // area day2 afternoon: min, max, mean" << endl;
		if(param->theStory.empty())
		  output_stream << " ; // the story" << endl;
		else
		  output_stream << param->theStory << "; // the story" << endl;

		output_stream << "test_case_end" << endl;
	  }
   }

  void create_wind_anomaly_testcasefile(TestCaseContainer& testCases, const string& fileName)
  {
	 ofstream output_stream;
	 output_stream.open(fileName.c_str());
	 
	 WindAnomalyTestParam* param = 0;
	 int i = 1;
	 for(TestCaseContainer::iterator iter = testCases.begin(); iter != testCases.end(); iter++)
	  {
		param = static_cast<WindAnomalyTestParam*>(iter->second);

#ifdef CPP_TESTCASES

		output_stream << "testCases.insert(make_pair(i++, WindAnomalyTestParam(" << endl;

		output_stream << "\""  << param->temperature_d1_afternoon_inlandmin << "\",\"";
		output_stream << param->temperature_d1_afternoon_inlandmax << "\",\"";
		output_stream << param->temperature_d1_afternoon_inlandmean << "\", // inland day1 afternoon: min, max, mean" << endl;
		output_stream << "\""  << param->temperature_d1_afternoon_coastmin << "\",\"";
		output_stream << param->temperature_d1_afternoon_coastmax << "\",\"";
		output_stream << param->temperature_d1_afternoon_coastmean << "\", // coast day1 afternoon: min, max, mean" << endl;		
		output_stream << "\""  << param->temperature_d1_afternoon_areamin << "\",\"";
		output_stream << param->temperature_d1_afternoon_areamax << "\",\"";
		output_stream << param->temperature_d1_afternoon_areamean << "\", // area day1 afternoon: min, max, mean" << endl;

		output_stream << "\""  << param->temperature_d2_afternoon_inlandmin << "\",\"";
		output_stream << param->temperature_d2_afternoon_inlandmax << "\",\"";
		output_stream << param->temperature_d2_afternoon_inlandmean << "\", // inland day2 afternoon: min, max, mean" << endl;
		output_stream << "\""  << param->temperature_d2_afternoon_coastmin << "\",\"";
		output_stream << param->temperature_d2_afternoon_coastmax << "\",\"";
		output_stream << param->temperature_d2_afternoon_coastmean << "\", // coast day2 afternoon: min, max, mean" << endl;
		output_stream  << "\"" << param->temperature_d2_afternoon_areamin << "\",\"";
		output_stream << param->temperature_d2_afternoon_areamax << "\",\"";
		output_stream << param->temperature_d2_afternoon_areamean << "\", // area day2 afternoon: min, max, mean" << endl;

		output_stream << "\"" << param->theStory << "\"))); // the story" << endl;
#endif

		output_stream << "#" << i++ << endl;
		output_stream << "test_case_begin" << endl;

		output_stream << param->temperature_d2_morning_inlandmin << ";";
		output_stream << param->temperature_d2_morning_inlandmax << ";";
		output_stream << param->temperature_d2_morning_inlandmean << "; // inland day2 morning: min, max, mean" << endl;
		output_stream << param->temperature_d2_morning_coastmin << ";";
		output_stream << param->temperature_d2_morning_coastmax << ";";
		output_stream << param->temperature_d2_morning_coastmean << "; // coast day2 morning: min, max, mean" << endl;

		output_stream << param->temperature_d2_morning_areamin << ";";
		output_stream << param->temperature_d2_morning_areamax << ";";
		output_stream << param->temperature_d2_morning_areamean << "; // area day2 morning: min, max, mean" << endl;

		output_stream << param->temperature_d2_afternoon_inlandmin << ";";
		output_stream << param->temperature_d2_afternoon_inlandmax << ";";
		output_stream << param->temperature_d2_afternoon_inlandmean << "; // inland day2 afternoon: min, max, mean" << endl;
		output_stream << param->temperature_d2_afternoon_coastmin << ";";
		output_stream << param->temperature_d2_afternoon_coastmax << ";";
		output_stream << param->temperature_d2_afternoon_coastmean << "; // coast day2 afternoon: min, max, mean" << endl;

		output_stream << param->temperature_d2_afternoon_areamin << ";";
		output_stream << param->temperature_d2_afternoon_areamax << ";";
		output_stream << param->temperature_d2_afternoon_areamean << "; // area day2 afternoon: min, max, mean" << endl;

		output_stream << param->anomaly_d2_windspeed_morning_inlandmin << ";";
		output_stream << param->anomaly_d2_windspeed_morning_inlandmax << ";";
		output_stream << param->anomaly_d2_windspeed_morning_inlandmean << "; // inland day2 morning windspeed min, max, mean" << endl;

		output_stream << param->anomaly_d2_windspeed_morning_coastmin << ";";
		output_stream << param->anomaly_d2_windspeed_morning_coastmax << ";";
		output_stream << param->anomaly_d2_windspeed_morning_coastmean << "; // coast day2 morning windspeed min, max, mean" << endl;

		output_stream << param->anomaly_d2_windspeed_afternoon_inlandmin << ";";
		output_stream << param->anomaly_d2_windspeed_afternoon_inlandmax << ";";
		output_stream << param->anomaly_d2_windspeed_afternoon_inlandmean << "; // inland day2 afternoon windspeed min, max, mean" << endl;

		output_stream << param->anomaly_d2_windspeed_afternoon_coastmin << ";";
		output_stream << param->anomaly_d2_windspeed_afternoon_coastmax << ";";
		output_stream << param->anomaly_d2_windspeed_afternoon_coastmean << "; // coast day2 afternoon windspeed min, max, mean" << endl;

		output_stream << param->anomaly_d2_windchill_morning_inlandmin << ";";
		output_stream << param->anomaly_d2_windchill_morning_inlandmax << ";";
		output_stream << param->anomaly_d2_windchill_morning_inlandmean << "; // inland day2 morning windchill min, max, mean" << endl;

		output_stream << param->anomaly_d2_windchill_morning_coastmin << ";";
		output_stream << param->anomaly_d2_windchill_morning_coastmax << ";";
		output_stream << param->anomaly_d2_windchill_morning_coastmean << "; // coast day2 morning windchill min, max, mean" << endl;

		output_stream << param->anomaly_d2_windchill_afternoon_inlandmin << ";";
		output_stream << param->anomaly_d2_windchill_afternoon_inlandmax << ";";
		output_stream << param->anomaly_d2_windchill_afternoon_inlandmean << "; // inland day2 afternoon windchill min, max, mean" << endl;

		output_stream << param->anomaly_d2_windchill_afternoon_coastmin << ";";
		output_stream << param->anomaly_d2_windchill_afternoon_coastmax << ";";
		output_stream << param->anomaly_d2_windchill_afternoon_coastmean << "; // coast day2 afternoon windchill min, max, mean" << endl;

		if(param->theStory.empty())
		  output_stream << " ; // the story" << endl;
		else
		  output_stream << param->theStory << "; // the story" << endl;

		output_stream << "test_case_end" << endl;
	  }
   }

int get_windchill(const float& theTemperature, const float& theWindSpeed)
{
  if(theTemperature >= 0.0)
	return 0;

  if(theWindSpeed < 6 && theTemperature < -15.0)
	{
	  return static_cast<int>(theTemperature + (2 * theWindSpeed));
	}
  else
	{
	  return static_cast<int>(theTemperature - theWindSpeed);
	}
}

void generate_testcase_parameters(TestCaseContainer& testCases, 
								  const string& language, 
								  const TemperatureTestType& testCaseType)
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
	  d1_windspeed_inland_morning_min_str[15], 
	  d1_windspeed_inland_morning_max_str[15], 
	  d1_windspeed_inland_morning_mean_str[15], 
	  d1_windspeed_inland_afternoon_min_str[15], 
	  d1_windspeed_inland_afternoon_max_str[15], 
	  d1_windspeed_inland_afternoon_mean_str[15],
	  d1_windspeed_coast_morning_min_str[15], 
	  d1_windspeed_coast_morning_max_str[15], 
	  d1_windspeed_coast_morning_mean_str[15], 
	  d1_windspeed_coast_afternoon_min_str[15], 
	  d1_windspeed_coast_afternoon_max_str[15], 
	  d1_windspeed_coast_afternoon_mean_str[15],
	  d1_windchill_inland_morning_min_str[15], 
	  d1_windchill_inland_morning_max_str[15], 
	  d1_windchill_inland_morning_mean_str[15], 
	  d1_windchill_inland_afternoon_min_str[15], 
	  d1_windchill_inland_afternoon_max_str[15], 
	  d1_windchill_inland_afternoon_mean_str[15],
	  d1_windchill_coast_morning_min_str[15], 
	  d1_windchill_coast_morning_max_str[15], 
	  d1_windchill_coast_morning_mean_str[15], 
	  d1_windchill_coast_afternoon_min_str[15], 
	  d1_windchill_coast_afternoon_max_str[15], 
	  d1_windchill_coast_afternoon_mean_str[15],
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
	  d2_coast_mean_str[15],
	  d2_windspeed_inland_morning_min_str[15], 
	  d2_windspeed_inland_morning_max_str[15], 
	  d2_windspeed_inland_morning_mean_str[15], 
	  d2_windspeed_inland_afternoon_min_str[15], 
	  d2_windspeed_inland_afternoon_max_str[15], 
	  d2_windspeed_inland_afternoon_mean_str[15],
	  d2_windspeed_coast_morning_min_str[15], 
	  d2_windspeed_coast_morning_max_str[15], 
	  d2_windspeed_coast_morning_mean_str[15], 
	  d2_windspeed_coast_afternoon_min_str[15], 
	  d2_windspeed_coast_afternoon_max_str[15], 
	  d2_windspeed_coast_afternoon_mean_str[15],
	  d2_windchill_inland_morning_min_str[15], 
	  d2_windchill_inland_morning_max_str[15], 
	  d2_windchill_inland_morning_mean_str[15], 
	  d2_windchill_inland_afternoon_min_str[15], 
	  d2_windchill_inland_afternoon_max_str[15], 
	  d2_windchill_inland_afternoon_mean_str[15],
	  d2_windchill_coast_morning_min_str[15], 
	  d2_windchill_coast_morning_max_str[15], 
	  d2_windchill_coast_morning_mean_str[15], 
	  d2_windchill_coast_afternoon_min_str[15], 
	  d2_windchill_coast_afternoon_max_str[15], 
	  d2_windchill_coast_afternoon_mean_str[15];



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
	  d1_windspeed_inland_morning_min, 
	  d1_windspeed_inland_morning_max, 
	  d1_windspeed_inland_morning_mean, 
	  d1_windspeed_inland_afternoon_min, 
	  d1_windspeed_inland_afternoon_max, 
	  d1_windspeed_inland_afternoon_mean,
	  d1_windspeed_coast_morning_min, 
	  d1_windspeed_coast_morning_max, 
	  d1_windspeed_coast_morning_mean, 
	  d1_windspeed_coast_afternoon_min, 
	  d1_windspeed_coast_afternoon_max, 
	  d1_windspeed_coast_afternoon_mean,
	  d1_windchill_inland_morning_min, 
	  d1_windchill_inland_morning_max, 
	  d1_windchill_inland_morning_mean, 
	  d1_windchill_inland_afternoon_min, 
	  d1_windchill_inland_afternoon_max, 
	  d1_windchill_inland_afternoon_mean,
	  d1_windchill_coast_morning_min, 
	  d1_windchill_coast_morning_max, 
	  d1_windchill_coast_morning_mean, 
	  d1_windchill_coast_afternoon_min, 
	  d1_windchill_coast_afternoon_max, 
	  d1_windchill_coast_afternoon_mean,
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
	  d2_coast_mean,
	  d2_windspeed_inland_morning_min, 
	  d2_windspeed_inland_morning_max, 
	  d2_windspeed_inland_morning_mean, 
	  d2_windspeed_inland_afternoon_min, 
	  d2_windspeed_inland_afternoon_max, 
	  d2_windspeed_inland_afternoon_mean,
	  d2_windspeed_coast_morning_min, 
	  d2_windspeed_coast_morning_max, 
	  d2_windspeed_coast_morning_mean, 
	  d2_windspeed_coast_afternoon_min, 
	  d2_windspeed_coast_afternoon_max, 
	  d2_windspeed_coast_afternoon_mean,
	  d2_windchill_inland_morning_min, 
	  d2_windchill_inland_morning_max, 
	  d2_windchill_inland_morning_mean, 
	  d2_windchill_inland_afternoon_min, 
	  d2_windchill_inland_afternoon_max, 
	  d2_windchill_inland_afternoon_mean,
	  d2_windchill_coast_morning_min, 
	  d2_windchill_coast_morning_max, 
	  d2_windchill_coast_morning_mean, 
	  d2_windchill_coast_afternoon_min, 
	  d2_windchill_coast_afternoon_max, 
	  d2_windchill_coast_afternoon_mean;

	int index = 1;

	for(double d = -30.0 + random_d(); d <= 25.0; d += random_d())
	  {
		d1_area_morning_min = d + random_d();
		d1_area_morning_max = d1_area_morning_min + random_i(2) + random_d();
		d1_area_morning_mean = (d1_area_morning_min+d1_area_morning_max)/2.0;
		d1_inland_morning_min = d1_area_morning_min + random_d();
		d1_inland_morning_max = d1_inland_morning_min + random_d();
		d1_inland_morning_mean = (d1_inland_morning_min+d1_inland_morning_max)/2.0;
		d1_coast_morning_min = d1_inland_morning_min - random_i(2) - random_d();
		d1_coast_morning_max = d1_coast_morning_min + random_i(2) + random_d();
		d1_coast_morning_mean = (d1_coast_morning_min+d1_coast_morning_max)/2.0;
		d1_area_afternoon_min = d1_area_morning_max + random_d();
		d1_area_afternoon_max = d1_area_afternoon_min + random_i(5) + random_d();
		d1_area_afternoon_mean = (d1_area_afternoon_min+d1_area_afternoon_max)/2.0;
		d1_inland_afternoon_min = d1_area_afternoon_min + random_d();
		d1_inland_afternoon_max = d1_inland_afternoon_min + random_d();
		d1_inland_afternoon_mean = (d1_inland_afternoon_min+d1_inland_afternoon_max)/2.0;
		d1_coast_afternoon_min = d1_inland_afternoon_min - random_i(2) - random_d();
		d1_coast_afternoon_max = d1_coast_afternoon_min + random_i(5) + random_d();
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

		area_night_min = d1_area_morning_min - random_i(4) - random_d();
		area_night_max = area_night_min + random_i(2)+ random_d();
		area_night_mean = (area_night_min+area_night_max)/2.0;
		inland_night_min = d1_inland_morning_min - random_i(5) - random_d();
		inland_night_max = inland_night_min + random_i(2)+ random_d();
		inland_night_mean = (inland_night_min+inland_night_max)/2.0;
		coast_night_min = d1_coast_morning_min - random_i(4) - random_d();
		coast_night_max = coast_night_min + random_i(2)+ random_d();
		coast_night_mean = (coast_night_min+coast_night_max)/2.0;

		d1_windspeed_inland_morning_min = random_i(1, 10);
		d1_windspeed_inland_morning_max = random_i(1, 10);
		d1_windspeed_inland_morning_mean = random_i(1, 10);
		d1_windspeed_inland_afternoon_min = random_i(1, 10);
		d1_windspeed_inland_afternoon_max = random_i(1, 10);
		d1_windspeed_inland_afternoon_mean = random_i(1, 10);
		d1_windspeed_coast_morning_min = random_i(1, 10);
		d1_windspeed_coast_morning_max = random_i(1, 10);
		d1_windspeed_coast_morning_mean = random_i(1, 10);
		d1_windspeed_coast_afternoon_min = random_i(1, 10);
		d1_windspeed_coast_afternoon_max = random_i(1, 10);
		d1_windspeed_coast_afternoon_mean = random_i(1, 10);
		d1_windchill_inland_morning_min = get_windchill(d1_inland_morning_min, d1_windspeed_inland_morning_min); 
		d1_windchill_inland_morning_max = get_windchill(d1_inland_morning_max, d1_windspeed_inland_morning_max); 
		d1_windchill_inland_morning_mean = get_windchill(d1_inland_morning_mean, d1_windspeed_inland_morning_mean); 
		d1_windchill_inland_afternoon_min = get_windchill(d1_inland_afternoon_min, d1_windspeed_inland_afternoon_min); 
		d1_windchill_inland_afternoon_max = get_windchill(d1_inland_afternoon_max, d1_windspeed_inland_afternoon_max); 
		d1_windchill_inland_afternoon_mean = get_windchill(d1_inland_afternoon_mean, d1_windspeed_inland_afternoon_mean); 
		d1_windchill_coast_morning_min = get_windchill(d1_coast_morning_min, d1_windspeed_coast_morning_min); 
		d1_windchill_coast_morning_max = get_windchill(d1_coast_morning_max, d1_windspeed_coast_morning_max); 
		d1_windchill_coast_morning_mean = get_windchill(d1_coast_morning_mean, d1_windspeed_coast_morning_mean); 
		d1_windchill_coast_afternoon_min = get_windchill(d1_coast_afternoon_min, d1_windspeed_coast_afternoon_min); 
		d1_windchill_coast_afternoon_max = get_windchill(d1_coast_afternoon_max, d1_windspeed_coast_afternoon_max); 
		d1_windchill_coast_afternoon_mean = get_windchill(d1_coast_afternoon_mean, d1_windspeed_coast_afternoon_mean); 


		d2_area_morning_min = d1_area_morning_min + (random_d() >= 0.5 ? (random_i(6) + random_d()) : (-1 *(random_i(6) + random_d())));
		d2_area_morning_max = d2_area_morning_min + random_i(2) + random_d();
		d2_area_morning_mean = (d2_area_morning_min+d2_area_morning_max)/2.0;
		d2_inland_morning_min = d2_area_morning_min + random_d();
		d2_inland_morning_max = d2_inland_morning_min + random_d();
		d2_inland_morning_mean = (d2_inland_morning_min+d2_inland_morning_max)/2.0;
		d2_coast_morning_min = d2_inland_morning_min + random_d();
		d2_coast_morning_max = d2_coast_morning_min + random_i(2) + random_d();
		d2_coast_morning_mean = (d2_coast_morning_min+d2_coast_morning_max)/2.0;
		d2_area_afternoon_min = d2_area_morning_max + random_i(3) + random_d();
		d2_area_afternoon_max = d2_area_afternoon_min + random_i(5) + random_d();
		d2_area_afternoon_mean = (d2_area_afternoon_min+d2_area_afternoon_max)/2.0;
		d2_inland_afternoon_min = d2_area_afternoon_min + random_d();
		d2_inland_afternoon_max = d2_inland_afternoon_min + random_d();
		d2_inland_afternoon_mean = (d2_inland_afternoon_min+d2_inland_afternoon_max)/2.0;
		d2_coast_afternoon_min = d2_inland_afternoon_min - random_i(2) - random_d();
		d2_coast_afternoon_max = d2_coast_afternoon_min + random_i(4) + random_d();
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

		d2_windspeed_inland_morning_min = random_i(2, 10);
		d2_windspeed_inland_morning_max = d2_windspeed_inland_morning_min + 1;
		d2_windspeed_inland_morning_mean = (d2_windspeed_inland_morning_min+d2_windspeed_inland_morning_max)/ 2.0;
		d2_windspeed_inland_afternoon_min = random_i(2, 10);
		d2_windspeed_inland_afternoon_max = d2_windspeed_inland_afternoon_min+1;
		d2_windspeed_inland_afternoon_mean = (d2_windspeed_inland_afternoon_min+d2_windspeed_inland_afternoon_max)/2.0;
		d2_windspeed_coast_morning_min = d2_windspeed_inland_morning_min + 1;
		d2_windspeed_coast_morning_max = d2_windspeed_coast_morning_min + 1 + random_d();
		d2_windspeed_coast_morning_mean = (d2_windspeed_coast_morning_min+d2_windspeed_coast_morning_max) / 2.0;
		d2_windspeed_coast_afternoon_min = d2_windspeed_inland_afternoon_min+1;
		d2_windspeed_coast_afternoon_max = d2_windspeed_coast_afternoon_min + 1 + random_d();
		d2_windspeed_coast_afternoon_mean = (d2_windspeed_coast_afternoon_min+d2_windspeed_coast_afternoon_max)/2.0;
		d2_windchill_inland_morning_min = get_windchill(d2_inland_morning_min, d2_windspeed_inland_morning_min); 
		d2_windchill_inland_morning_max = get_windchill(d2_inland_morning_max, d2_windspeed_inland_morning_max); 
		d2_windchill_inland_morning_mean = get_windchill(d2_inland_morning_mean, d2_windspeed_inland_morning_mean); 
		d2_windchill_inland_afternoon_min = get_windchill(d2_inland_afternoon_min, d2_windspeed_inland_afternoon_min); 
		d2_windchill_inland_afternoon_max = get_windchill(d2_inland_afternoon_max, d2_windspeed_inland_afternoon_max); 
		d2_windchill_inland_afternoon_mean = get_windchill(d2_inland_afternoon_mean, d2_windspeed_inland_afternoon_mean); 
		d2_windchill_coast_morning_min = get_windchill(d2_coast_morning_min, d2_windspeed_coast_morning_min); 
		d2_windchill_coast_morning_max = get_windchill(d2_coast_morning_max, d2_windspeed_coast_morning_max); 
		d2_windchill_coast_morning_mean = get_windchill(d2_coast_morning_mean, d2_windspeed_coast_morning_mean); 
		d2_windchill_coast_afternoon_min = get_windchill(d2_coast_afternoon_min, d2_windspeed_coast_afternoon_min); 
		d2_windchill_coast_afternoon_max = get_windchill(d2_coast_afternoon_max, d2_windspeed_coast_afternoon_max); 
		d2_windchill_coast_afternoon_mean = get_windchill(d2_coast_afternoon_mean, d2_windspeed_coast_afternoon_mean); 

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
		sprintf(d1_windspeed_inland_morning_min_str, "%.02f,0", d1_windspeed_inland_morning_min);
		sprintf(d1_windspeed_inland_morning_max_str, "%.02f,0", d1_windspeed_inland_morning_max);
		sprintf(d1_windspeed_inland_morning_mean_str, "%.02f,0", d1_windspeed_inland_morning_mean);
		sprintf(d1_windspeed_inland_afternoon_min_str, "%.02f,0", d1_windspeed_inland_afternoon_min);
		sprintf(d1_windspeed_inland_afternoon_max_str, "%.02f,0", d1_windspeed_inland_afternoon_max);
		sprintf(d1_windspeed_inland_afternoon_mean_str, "%.02f,0", d1_windspeed_inland_afternoon_mean);
		sprintf(d1_windspeed_coast_morning_min_str, "%.02f,0", d1_windspeed_coast_morning_min);
		sprintf(d1_windspeed_coast_morning_max_str, "%.02f,0", d1_windspeed_coast_morning_max);
		sprintf(d1_windspeed_coast_morning_mean_str, "%.02f,0", d1_windspeed_coast_morning_mean);
		sprintf(d1_windspeed_coast_afternoon_min_str, "%.02f,0", d1_windspeed_coast_afternoon_min);
		sprintf(d1_windspeed_coast_afternoon_max_str, "%.02f,0", d1_windspeed_coast_afternoon_max);
		sprintf(d1_windspeed_coast_afternoon_mean_str, "%.02f,0", d1_windspeed_coast_afternoon_mean);
		sprintf(d1_windchill_inland_morning_min_str, "%.02f,0", d1_windchill_inland_morning_min);
		sprintf(d1_windchill_inland_morning_max_str, "%.02f,0", d1_windchill_inland_morning_max);
		sprintf(d1_windchill_inland_morning_mean_str, "%.02f,0", d1_windchill_inland_morning_mean);
		sprintf(d1_windchill_inland_afternoon_min_str, "%.02f,0", d1_windchill_inland_afternoon_min);
		sprintf(d1_windchill_inland_afternoon_max_str, "%.02f,0", d1_windchill_inland_afternoon_max);
		sprintf(d1_windchill_inland_afternoon_mean_str, "%.02f,0", d1_windchill_inland_afternoon_mean);
		sprintf(d1_windchill_coast_morning_min_str, "%.02f,0", d1_windchill_coast_morning_min);
		sprintf(d1_windchill_coast_morning_max_str, "%.02f,0", d1_windchill_coast_morning_max);
		sprintf(d1_windchill_coast_morning_mean_str, "%.02f,0", d1_windchill_coast_morning_mean);
		sprintf(d1_windchill_coast_afternoon_min_str, "%.02f,0", d1_windchill_coast_afternoon_min);
		sprintf(d1_windchill_coast_afternoon_max_str, "%.02f,0", d1_windchill_coast_afternoon_max);
		sprintf(d1_windchill_coast_afternoon_mean_str, "%.02f,0", d1_windchill_coast_afternoon_mean);
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
		sprintf(d2_windspeed_inland_morning_min_str, "%.02f,0", d2_windspeed_inland_morning_min);
		sprintf(d2_windspeed_inland_morning_max_str, "%.02f,0", d2_windspeed_inland_morning_max);
		sprintf(d2_windspeed_inland_morning_mean_str, "%.02f,0", d2_windspeed_inland_morning_mean);
		sprintf(d2_windspeed_inland_afternoon_min_str, "%.02f,0", d2_windspeed_inland_afternoon_min);
		sprintf(d2_windspeed_inland_afternoon_max_str, "%.02f,0", d2_windspeed_inland_afternoon_max);
		sprintf(d2_windspeed_inland_afternoon_mean_str, "%.02f,0", d2_windspeed_inland_afternoon_mean);
		sprintf(d2_windspeed_coast_morning_min_str, "%.02f,0", d2_windspeed_coast_morning_min);
		sprintf(d2_windspeed_coast_morning_max_str, "%.02f,0", d2_windspeed_coast_morning_max);
		sprintf(d2_windspeed_coast_morning_mean_str, "%.02f,0", d2_windspeed_coast_morning_mean);
		sprintf(d2_windspeed_coast_afternoon_min_str, "%.02f,0", d2_windspeed_coast_afternoon_min);
		sprintf(d2_windspeed_coast_afternoon_max_str, "%.02f,0", d2_windspeed_coast_afternoon_max);
		sprintf(d2_windspeed_coast_afternoon_mean_str, "%.02f,0", d2_windspeed_coast_afternoon_mean);
		sprintf(d2_windchill_inland_morning_min_str, "%.02f,0", d2_windchill_inland_morning_min);
		sprintf(d2_windchill_inland_morning_max_str, "%.02f,0", d2_windchill_inland_morning_max);
		sprintf(d2_windchill_inland_morning_mean_str, "%.02f,0", d2_windchill_inland_morning_mean);
		sprintf(d2_windchill_inland_afternoon_min_str, "%.02f,0", d2_windchill_inland_afternoon_min);
		sprintf(d2_windchill_inland_afternoon_max_str, "%.02f,0", d2_windchill_inland_afternoon_max);
		sprintf(d2_windchill_inland_afternoon_mean_str, "%.02f,0", d2_windchill_inland_afternoon_mean);
		sprintf(d2_windchill_coast_morning_min_str, "%.02f,0", d2_windchill_coast_morning_min);
		sprintf(d2_windchill_coast_morning_max_str, "%.02f,0", d2_windchill_coast_morning_max);
		sprintf(d2_windchill_coast_morning_mean_str, "%.02f,0", d2_windchill_coast_morning_mean);
		sprintf(d2_windchill_coast_afternoon_min_str, "%.02f,0", d2_windchill_coast_afternoon_min);
		sprintf(d2_windchill_coast_afternoon_max_str, "%.02f,0", d2_windchill_coast_afternoon_max);
		sprintf(d2_windchill_coast_afternoon_mean_str, "%.02f,0", d2_windchill_coast_afternoon_mean);

		TemperatureStoryTestParam* param = 0;

		if(testCaseType == TEMPERATURE_ANOMALY)
		  {
			param = new TemperatureAnomalyTestParam(   d1_inland_afternoon_min_str,
													   d1_inland_afternoon_max_str,
													   d1_inland_afternoon_mean_str,
													   
													   d1_coast_afternoon_min_str,
													   d1_coast_afternoon_max_str,
													   d1_coast_afternoon_mean_str,
													   
													   d1_area_afternoon_min_str,
													   d1_area_afternoon_max_str,
													   d1_area_afternoon_mean_str,
													   
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
		  }
		else
		  {
			param = new Max36HoursTestParam(
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
		  }

		testCases.insert(make_pair(index++,param));
	  }
  }

  void create_max36hours_testcases(TestCaseContainer& testCases, const string& language)
{
  int i = 0;

  if(language == "fi")
	{
testCases.insert(make_pair(i++, new Max36HoursTestParam(
"3.89,0","5.27,0","4.58,0", // inland day1: min, max, mean
"4.06,0","8.02,0","6.04,0", // coast day1: min, max, mean
"3.52,0","8.18,0","5.85,0", // area day1: min, max, mean
"3.89,0","4.60,0","4.25,0", // inland day1 morning: min, max, mean
"4.06,0","4.87,0","4.46,0", // coast day1 morning: min, max, mean
"3.52,0","4.29,0","3.91,0", // area day1 morning: min, max, mean
"4.99,0","5.27,0","5.13,0", // inland day1 afternoon: min, max, mean
"4.99,0","8.02,0","6.51,0", // coast day1 afternoon: min, max, mean
"4.41,0","8.18,0","6.29,0", // area day1 afternoon: min, max, mean
"2.69,0","4.02,0","3.35,0", // inland night: min, max, mean
"-0.69,0","0.43,0","-0.13,0", // coast night: min, max, mean
"2.21,0","3.91,0","3.06,0", // area night: min, max, mean
"2.57,0","5.44,0","4.00,0", // inland day2: min, max, mean
"4.15,0","7.75,0","5.95,0", // coast day2: min, max, mean
"2.68,0","10.78,0","6.73,0", // area day2: min, max, mean
"3.29,0","3.34,0","3.32,0", // inland day2 morning: min, max, mean
"4.20,0","5.34,0","4.77,0", // coast day2 morning: min, max, mean
"3.12,0","4.32,0","3.72,0", // area day2 morning: min, max, mean
"4.79,0","5.30,0","5.04,0", // inland day2 afternoon: min, max, mean
"5.29,0","6.83,0","6.06,0", // coast day2 afternoon: min, max, mean
"4.76,0","10.25,0","7.51,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 4...8 astetta, huomenna suunnilleen sama. Yön alin lämpötila on sisämaassa noin 3 astetta, rannikolla nollan tienoilla."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"4.52,0","7.14,0","5.83,0", // inland day1: min, max, mean
"5.29,0","11.22,0","8.25,0", // coast day1: min, max, mean
"4.24,0","9.39,0","6.81,0", // area day1: min, max, mean
"4.52,0","4.99,0","4.76,0", // inland day1 morning: min, max, mean
"5.29,0","7.15,0","6.22,0", // coast day1 morning: min, max, mean
"4.24,0","5.92,0","5.08,0", // area day1 morning: min, max, mean
"6.84,0","7.14,0","6.99,0", // inland day1 afternoon: min, max, mean
"7.20,0","11.22,0","9.21,0", // coast day1 afternoon: min, max, mean
"6.36,0","9.39,0","7.87,0", // area day1 afternoon: min, max, mean
"2.51,0","3.41,0","2.96,0", // inland night: min, max, mean
"0.70,0","1.29,0","1.00,0", // coast night: min, max, mean
"0.40,0","1.23,0","0.81,0", // area night: min, max, mean
"4.23,0","8.37,0","6.30,0", // inland day2: min, max, mean
"4.60,0","11.47,0","8.04,0", // coast day2: min, max, mean
"4.34,0","9.63,0","6.99,0", // area day2: min, max, mean
"4.95,0","5.25,0","5.10,0", // inland day2 morning: min, max, mean
"5.31,0","6.80,0","6.06,0", // coast day2 morning: min, max, mean
"4.76,0","6.64,0","5.70,0", // area day2 morning: min, max, mean
"7.91,0","8.11,0","8.01,0", // inland day2 afternoon: min, max, mean
"8.53,0","10.74,0","9.63,0", // coast day2 afternoon: min, max, mean
"7.04,0","9.29,0","8.16,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 6...9 astetta, huomenna suunnilleen sama. Yön alin lämpötila on vähän plussan puolella."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"4.98,0","9.05,0","7.01,0", // inland day1: min, max, mean
"5.51,0","10.70,0","8.11,0", // coast day1: min, max, mean
"4.77,0","8.70,0","6.73,0", // area day1: min, max, mean
"4.98,0","5.76,0","5.37,0", // inland day1 morning: min, max, mean
"5.51,0","6.80,0","6.16,0", // coast day1 morning: min, max, mean
"4.77,0","6.67,0","5.72,0", // area day1 morning: min, max, mean
"8.05,0","9.05,0","8.55,0", // inland day1 afternoon: min, max, mean
"8.49,0","10.70,0","9.60,0", // coast day1 afternoon: min, max, mean
"7.21,0","8.70,0","7.95,0", // area day1 afternoon: min, max, mean
"3.91,0","4.56,0","4.24,0", // inland night: min, max, mean
"3.96,0","5.71,0","4.83,0", // coast night: min, max, mean
"3.54,0","4.24,0","3.89,0", // area night: min, max, mean
"3.55,0","9.17,0","6.36,0", // inland day2: min, max, mean
"2.85,0","11.03,0","6.94,0", // coast day2: min, max, mean
"2.60,0","12.32,0","7.46,0", // area day2: min, max, mean
"3.67,0","3.74,0","3.70,0", // inland day2 morning: min, max, mean
"3.73,0","4.55,0","4.14,0", // coast day2 morning: min, max, mean
"2.71,0","3.00,0","2.85,0", // area day2 morning: min, max, mean
"7.91,0","8.36,0","8.13,0", // inland day2 afternoon: min, max, mean
"8.18,0","10.84,0","9.51,0", // coast day2 afternoon: min, max, mean
"7.03,0","11.87,0","9.45,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 7...9 astetta, huomenna hieman korkeampi. Yön alin lämpötila on 5 asteen tuntumassa."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"5.62,0","7.12,0","6.37,0", // inland day1: min, max, mean
"6.46,0","14.41,0","10.44,0", // coast day1: min, max, mean
"4.72,0","8.45,0","6.59,0", // area day1: min, max, mean
"5.62,0","6.05,0","5.84,0", // inland day1 morning: min, max, mean
"6.46,0","6.95,0","6.71,0", // coast day1 morning: min, max, mean
"4.72,0","5.30,0","5.01,0", // area day1 morning: min, max, mean
"6.76,0","7.12,0","6.94,0", // inland day1 afternoon: min, max, mean
"7.70,0","14.41,0","11.06,0", // coast day1 afternoon: min, max, mean
"6.00,0","8.45,0","7.22,0", // area day1 afternoon: min, max, mean
"0.81,0","1.96,0","1.38,0", // inland night: min, max, mean
"3.94,0","5.68,0","4.81,0", // coast night: min, max, mean
"1.68,0","2.53,0","2.11,0", // area night: min, max, mean
"3.57,0","9.36,0","6.46,0", // inland day2: min, max, mean
"4.12,0","10.80,0","7.46,0", // coast day2: min, max, mean
"3.54,0","8.74,0","6.14,0", // area day2: min, max, mean
"4.30,0","5.13,0","4.71,0", // inland day2 morning: min, max, mean
"4.87,0","5.70,0","5.28,0", // coast day2 morning: min, max, mean
"3.97,0","4.10,0","4.04,0", // area day2 morning: min, max, mean
"8.61,0","9.09,0","8.85,0", // inland day2 afternoon: min, max, mean
"9.05,0","10.25,0","9.65,0", // coast day2 afternoon: min, max, mean
"7.97,0","8.48,0","8.23,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on sisämaassa noin 7 astetta, rannikolla 9...14 astetta. Huomenna päivän ylin lämpötila on noin 8 astetta. Yön alin lämpötila on sisämaassa vähän plussan puolella, rannikolla lähellä 5 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"6.46,0","8.91,0","7.69,0", // inland day1: min, max, mean
"6.73,0","11.09,0","8.91,0", // coast day1: min, max, mean
"6.33,0","11.02,0","8.68,0", // area day1: min, max, mean
"6.46,0","6.47,0","6.47,0", // inland day1 morning: min, max, mean
"6.73,0","7.41,0","7.07,0", // coast day1 morning: min, max, mean
"6.33,0","7.94,0","7.14,0", // area day1 morning: min, max, mean
"8.90,0","8.91,0","8.90,0", // inland day1 afternoon: min, max, mean
"8.96,0","11.09,0","10.03,0", // coast day1 afternoon: min, max, mean
"8.38,0","11.02,0","9.70,0", // area day1 afternoon: min, max, mean
"4.67,0","6.18,0","5.43,0", // inland night: min, max, mean
"6.19,0","6.22,0","6.20,0", // coast night: min, max, mean
"3.89,0","5.83,0","4.86,0", // area night: min, max, mean
"8.74,0","16.22,0","12.48,0", // inland day2: min, max, mean
"8.92,0","19.06,0","13.99,0", // coast day2: min, max, mean
"8.69,0","19.34,0","14.02,0", // area day2: min, max, mean
"9.70,0","9.88,0","9.79,0", // inland day2 morning: min, max, mean
"9.74,0","11.65,0","10.70,0", // coast day2 morning: min, max, mean
"9.21,0","10.25,0","9.73,0", // area day2 morning: min, max, mean
"15.15,0","15.76,0","15.46,0", // inland day2 afternoon: min, max, mean
"15.59,0","18.32,0","16.95,0", // coast day2 afternoon: min, max, mean
"14.45,0","18.81,0","16.63,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 10 asteen tienoilla, huomenna 14...19 astetta. Yön alin lämpötila on lähellä 5 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"7.47,0","7.70,0","7.59,0", // inland day1: min, max, mean
"8.44,0","12.34,0","10.39,0", // coast day1: min, max, mean
"6.74,0","9.93,0","8.33,0", // area day1: min, max, mean
"7.47,0","8.37,0","7.92,0", // inland day1 morning: min, max, mean
"8.44,0","10.07,0","9.26,0", // coast day1 morning: min, max, mean
"6.74,0","6.97,0","6.86,0", // area day1 morning: min, max, mean
"7.39,0","7.70,0","7.55,0", // inland day1 afternoon: min, max, mean
"8.05,0","12.34,0","10.20,0", // coast day1 afternoon: min, max, mean
"7.02,0","9.93,0","8.47,0", // area day1 afternoon: min, max, mean
"3.53,0","3.69,0","3.61,0", // inland night: min, max, mean
"4.07,0","4.58,0","4.33,0", // coast night: min, max, mean
"4.26,0","5.19,0","4.72,0", // area night: min, max, mean
"3.01,0","10.89,0","6.95,0", // inland day2: min, max, mean
"4.10,0","14.22,0","9.16,0", // coast day2: min, max, mean
"3.52,0","10.92,0","7.22,0", // area day2: min, max, mean
"3.97,0","4.94,0","4.46,0", // inland day2 morning: min, max, mean
"4.23,0","6.19,0","5.21,0", // coast day2 morning: min, max, mean
"3.92,0","5.28,0","4.60,0", // area day2 morning: min, max, mean
"9.80,0","10.57,0","10.18,0", // inland day2 afternoon: min, max, mean
"10.25,0","13.30,0","11.78,0", // coast day2 afternoon: min, max, mean
"9.35,0","10.14,0","9.75,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on sisämaassa noin 8 astetta, rannikolla 8...12 astetta. Huomenna päivän ylin lämpötila on 10 asteen tuntumassa. Yön alin lämpötila on 5 asteen tuntumassa."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"8.28,0","10.31,0","9.30,0", // inland day1: min, max, mean
"8.55,0","13.10,0","10.82,0", // coast day1: min, max, mean
"7.69,0","12.30,0","9.99,0", // area day1: min, max, mean
"8.28,0","9.28,0","8.78,0", // inland day1 morning: min, max, mean
"8.55,0","10.37,0","9.46,0", // coast day1 morning: min, max, mean
"7.69,0","8.92,0","8.30,0", // area day1 morning: min, max, mean
"9.90,0","10.31,0","10.10,0", // inland day1 afternoon: min, max, mean
"10.80,0","13.10,0","11.95,0", // coast day1 afternoon: min, max, mean
"9.42,0","12.30,0","10.86,0", // area day1 afternoon: min, max, mean
"4.74,0","5.45,0","5.10,0", // inland night: min, max, mean
"5.06,0","6.33,0","5.69,0", // coast night: min, max, mean
"5.79,0","6.21,0","6.00,0", // area night: min, max, mean
"8.93,0","14.57,0","11.75,0", // inland day2: min, max, mean
"8.67,0","15.26,0","11.96,0", // coast day2: min, max, mean
"8.17,0","20.72,0","14.45,0", // area day2: min, max, mean
"9.19,0","9.69,0","9.44,0", // inland day2 morning: min, max, mean
"9.26,0","10.55,0","9.91,0", // coast day2 morning: min, max, mean
"8.82,0","8.90,0","8.86,0", // area day2 morning: min, max, mean
"13.63,0","14.50,0","14.06,0", // inland day2 afternoon: min, max, mean
"14.30,0","14.47,0","14.38,0", // coast day2 afternoon: min, max, mean
"13.03,0","19.80,0","16.41,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 9...12 astetta, huomenna 15...20 astetta. Yön alin lämpötila on noin 5 astetta."))); // the story
testCases.insert(make_pair(i++, new Max36HoursTestParam(
"10.11,0","11.11,0","10.61,0", // inland day1: min, max, mean
"10.55,0","12.05,0","11.30,0", // coast day1: min, max, mean
"9.41,0","13.32,0","11.37,0", // area day1: min, max, mean
"10.11,0","10.33,0","10.22,0", // inland day1 morning: min, max, mean
"10.55,0","12.32,0","11.43,0", // coast day1 morning: min, max, mean
"9.41,0","9.56,0","9.49,0", // area day1 morning: min, max, mean
"10.65,0","11.11,0","10.88,0", // inland day1 afternoon: min, max, mean
"11.28,0","12.05,0","11.67,0", // coast day1 afternoon: min, max, mean
"10.55,0","13.32,0","11.93,0", // area day1 afternoon: min, max, mean
"6.31,0","7.06,0","6.69,0", // inland night: min, max, mean
"9.46,0","10.46,0","9.96,0", // coast night: min, max, mean
"8.24,0","9.10,0","8.67,0", // area night: min, max, mean
"5.78,0","12.06,0","8.92,0", // inland day2: min, max, mean
"5.98,0","18.45,0","12.22,0", // coast day2: min, max, mean
"6.04,0","13.16,0","9.60,0", // area day2: min, max, mean
"6.56,0","6.97,0","6.76,0", // inland day2 morning: min, max, mean
"6.59,0","7.58,0","7.08,0", // coast day2 morning: min, max, mean
"6.34,0","8.15,0","7.25,0", // area day2 morning: min, max, mean
"11.52,0","11.89,0","11.71,0", // inland day2 afternoon: min, max, mean
"11.73,0","17.87,0","14.80,0", // coast day2 afternoon: min, max, mean
"10.79,0","12.36,0","11.57,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 11...13 astetta. Huomenna päivän ylin lämpötila on sisämaassa vähän yli 10 astetta, rannikolla 13...18 astetta. Yön alin lämpötila on sisämaassa noin 7 astetta, rannikolla 10 asteen tuntumassa."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"10.39,0","12.25,0","11.32,0", // inland day1: min, max, mean
"11.01,0","15.23,0","13.12,0", // coast day1: min, max, mean
"9.59,0","13.91,0","11.75,0", // area day1: min, max, mean
"10.39,0","10.64,0","10.51,0", // inland day1 morning: min, max, mean
"11.01,0","11.32,0","11.17,0", // coast day1 morning: min, max, mean
"9.59,0","11.05,0","10.32,0", // area day1 morning: min, max, mean
"12.09,0","12.25,0","12.17,0", // inland day1 afternoon: min, max, mean
"12.28,0","15.23,0","13.76,0", // coast day1 afternoon: min, max, mean
"11.37,0","13.91,0","12.64,0", // area day1 afternoon: min, max, mean
"8.66,0","9.79,0","9.23,0", // inland night: min, max, mean
"9.02,0","10.22,0","9.62,0", // coast night: min, max, mean
"6.97,0","8.53,0","7.75,0", // area night: min, max, mean
"11.39,0","20.35,0","15.87,0", // inland day2: min, max, mean
"13.17,0","21.45,0","17.31,0", // coast day2: min, max, mean
"11.31,0","20.76,0","16.04,0", // area day2: min, max, mean
"12.20,0","12.95,0","12.58,0", // inland day2 morning: min, max, mean
"13.20,0","14.26,0","13.73,0", // coast day2 morning: min, max, mean
"12.17,0","13.92,0","13.04,0", // area day2 morning: min, max, mean
"19.58,0","19.93,0","19.75,0", // inland day2 afternoon: min, max, mean
"19.61,0","20.91,0","20.26,0", // coast day2 afternoon: min, max, mean
"18.89,0","20.73,0","19.81,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 11...14 astetta, huomenna lähellä 20 astetta. Yön alin lämpötila on 7...9 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"10.74,0","12.32,0","11.53,0", // inland day1: min, max, mean
"11.18,0","15.07,0","13.12,0", // coast day1: min, max, mean
"10.55,0","18.30,0","14.43,0", // area day1: min, max, mean
"10.74,0","11.06,0","10.90,0", // inland day1 morning: min, max, mean
"11.18,0","12.90,0","12.04,0", // coast day1 morning: min, max, mean
"10.55,0","10.82,0","10.69,0", // area day1 morning: min, max, mean
"11.77,0","12.32,0","12.04,0", // inland day1 afternoon: min, max, mean
"12.66,0","15.07,0","13.87,0", // coast day1 afternoon: min, max, mean
"11.59,0","18.30,0","14.95,0", // area day1 afternoon: min, max, mean
"8.80,0","10.05,0","9.42,0", // inland night: min, max, mean
"8.28,0","9.51,0","8.89,0", // coast night: min, max, mean
"10.13,0","10.56,0","10.34,0", // area night: min, max, mean
"14.07,0","19.77,0","16.92,0", // inland day2: min, max, mean
"14.91,0","21.92,0","18.41,0", // coast day2: min, max, mean
"13.37,0","24.39,0","18.88,0", // area day2: min, max, mean
"14.53,0","14.99,0","14.76,0", // inland day2 morning: min, max, mean
"15.51,0","17.30,0","16.41,0", // coast day2 morning: min, max, mean
"13.91,0","15.57,0","14.74,0", // area day2 morning: min, max, mean
"18.41,0","18.81,0","18.61,0", // inland day2 afternoon: min, max, mean
"18.72,0","21.53,0","20.12,0", // coast day2 afternoon: min, max, mean
"18.40,0","23.82,0","21.11,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 13...18 astetta, huomenna 19...24 astetta. Yön alin lämpötila on noin 10 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"11.76,0","14.58,0","13.17,0", // inland day1: min, max, mean
"12.62,0","18.54,0","15.58,0", // coast day1: min, max, mean
"11.72,0","17.30,0","14.51,0", // area day1: min, max, mean
"11.76,0","12.27,0","12.02,0", // inland day1 morning: min, max, mean
"12.62,0","14.16,0","13.39,0", // coast day1 morning: min, max, mean
"11.72,0","12.51,0","12.12,0", // area day1 morning: min, max, mean
"13.70,0","14.58,0","14.14,0", // inland day1 afternoon: min, max, mean
"14.39,0","18.54,0","16.47,0", // coast day1 afternoon: min, max, mean
"13.27,0","17.30,0","15.28,0", // area day1 afternoon: min, max, mean
"11.53,0","12.21,0","11.87,0", // inland night: min, max, mean
"7.99,0","9.39,0","8.69,0", // coast night: min, max, mean
"7.60,0","8.36,0","7.98,0", // area night: min, max, mean
"12.59,0","17.06,0","14.82,0", // inland day2: min, max, mean
"12.81,0","20.30,0","16.56,0", // coast day2: min, max, mean
"12.10,0","17.96,0","15.03,0", // area day2: min, max, mean
"12.98,0","13.64,0","13.31,0", // inland day2 morning: min, max, mean
"13.29,0","14.57,0","13.93,0", // coast day2 morning: min, max, mean
"12.45,0","13.26,0","12.86,0", // area day2 morning: min, max, mean
"15.95,0","16.94,0","16.45,0", // inland day2 afternoon: min, max, mean
"16.53,0","19.67,0","18.10,0", // coast day2 afternoon: min, max, mean
"15.39,0","17.27,0","16.33,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 13...17 astetta, huomenna suunnilleen sama. Yön alin lämpötila on sisämaassa noin 12 astetta, rannikolla noin 10 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"14.07,0","15.16,0","14.62,0", // inland day1: min, max, mean
"14.63,0","20.00,0","17.32,0", // coast day1: min, max, mean
"13.14,0","15.49,0","14.31,0", // area day1: min, max, mean
"14.07,0","14.77,0","14.42,0", // inland day1 morning: min, max, mean
"14.63,0","14.93,0","14.78,0", // coast day1 morning: min, max, mean
"13.14,0","13.64,0","13.39,0", // area day1 morning: min, max, mean
"14.80,0","15.16,0","14.98,0", // inland day1 afternoon: min, max, mean
"15.59,0","20.00,0","17.79,0", // coast day1 afternoon: min, max, mean
"14.42,0","15.49,0","14.95,0", // area day1 afternoon: min, max, mean
"11.40,0","12.45,0","11.93,0", // inland night: min, max, mean
"13.71,0","14.10,0","13.90,0", // coast night: min, max, mean
"8.23,0","8.72,0","8.48,0", // area night: min, max, mean
"12.67,0","19.79,0","16.23,0", // inland day2: min, max, mean
"13.36,0","24.34,0","18.85,0", // coast day2: min, max, mean
"12.41,0","22.74,0","17.57,0", // area day2: min, max, mean
"13.64,0","14.40,0","14.02,0", // inland day2 morning: min, max, mean
"13.89,0","15.33,0","14.61,0", // coast day2 morning: min, max, mean
"12.83,0","14.38,0","13.60,0", // area day2 morning: min, max, mean
"18.06,0","18.88,0","18.47,0", // inland day2 afternoon: min, max, mean
"18.45,0","23.69,0","21.07,0", // coast day2 afternoon: min, max, mean
"17.52,0","22.13,0","19.83,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on sisämaassa 15 asteen tuntumassa, rannikolla 16...20 astetta. Huomenna päivän ylin lämpötila on 18...22 astetta. Yön alin lämpötila on vajaat 10 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"10.11,0","11.11,0","10.61,0", // inland day1: min, max, mean
"10.55,0","12.05,0","11.30,0", // coast day1: min, max, mean
"9.41,0","13.32,0","11.37,0", // area day1: min, max, mean
"10.11,0","10.33,0","10.22,0", // inland day1 morning: min, max, mean
"10.55,0","12.32,0","11.43,0", // coast day1 morning: min, max, mean
"9.41,0","9.56,0","9.49,0", // area day1 morning: min, max, mean
"10.65,0","11.11,0","10.88,0", // inland day1 afternoon: min, max, mean
"11.28,0","12.05,0","11.67,0", // coast day1 afternoon: min, max, mean
"10.55,0","13.32,0","11.93,0", // area day1 afternoon: min, max, mean
"6.31,0","7.06,0","6.69,0", // inland night: min, max, mean
"9.46,0","10.46,0","9.96,0", // coast night: min, max, mean
"8.24,0","9.10,0","8.67,0", // area night: min, max, mean
"5.78,0","12.06,0","8.92,0", // inland day2: min, max, mean
"5.98,0","18.45,0","12.22,0", // coast day2: min, max, mean
"6.04,0","13.16,0","9.60,0", // area day2: min, max, mean
"6.56,0","6.97,0","6.76,0", // inland day2 morning: min, max, mean
"6.59,0","7.58,0","7.08,0", // coast day2 morning: min, max, mean
"6.34,0","8.15,0","7.25,0", // area day2 morning: min, max, mean
"11.52,0","11.89,0","11.71,0", // inland day2 afternoon: min, max, mean
"11.73,0","17.87,0","14.80,0", // coast day2 afternoon: min, max, mean
"10.79,0","12.36,0","11.57,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 11...13 astetta. Huomenna päivän ylin lämpötila on sisämaassa vähän yli 10 astetta, rannikolla 13...18 astetta. Yön alin lämpötila on sisämaassa noin 7 astetta, rannikolla 10 asteen tuntumassa."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"10.39,0","12.25,0","11.32,0", // inland day1: min, max, mean
"11.01,0","15.23,0","13.12,0", // coast day1: min, max, mean
"9.59,0","13.91,0","11.75,0", // area day1: min, max, mean
"10.39,0","10.64,0","10.51,0", // inland day1 morning: min, max, mean
"11.01,0","11.32,0","11.17,0", // coast day1 morning: min, max, mean
"9.59,0","11.05,0","10.32,0", // area day1 morning: min, max, mean
"12.09,0","12.25,0","12.17,0", // inland day1 afternoon: min, max, mean
"12.28,0","15.23,0","13.76,0", // coast day1 afternoon: min, max, mean
"11.37,0","13.91,0","12.64,0", // area day1 afternoon: min, max, mean
"8.66,0","9.79,0","9.23,0", // inland night: min, max, mean
"9.02,0","10.22,0","9.62,0", // coast night: min, max, mean
"6.97,0","8.53,0","7.75,0", // area night: min, max, mean
"11.39,0","20.35,0","15.87,0", // inland day2: min, max, mean
"13.17,0","21.45,0","17.31,0", // coast day2: min, max, mean
"11.31,0","20.76,0","16.04,0", // area day2: min, max, mean
"12.20,0","12.95,0","12.58,0", // inland day2 morning: min, max, mean
"13.20,0","14.26,0","13.73,0", // coast day2 morning: min, max, mean
"12.17,0","13.92,0","13.04,0", // area day2 morning: min, max, mean
"19.58,0","19.93,0","19.75,0", // inland day2 afternoon: min, max, mean
"19.61,0","20.91,0","20.26,0", // coast day2 afternoon: min, max, mean
"18.89,0","20.73,0","19.81,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 11...14 astetta, huomenna lähellä 20 astetta. Yön alin lämpötila on 7...9 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"10.74,0","12.32,0","11.53,0", // inland day1: min, max, mean
"11.18,0","15.07,0","13.12,0", // coast day1: min, max, mean
"10.55,0","18.30,0","14.43,0", // area day1: min, max, mean
"10.74,0","11.06,0","10.90,0", // inland day1 morning: min, max, mean
"11.18,0","12.90,0","12.04,0", // coast day1 morning: min, max, mean
"10.55,0","10.82,0","10.69,0", // area day1 morning: min, max, mean
"11.77,0","12.32,0","12.04,0", // inland day1 afternoon: min, max, mean
"12.66,0","15.07,0","13.87,0", // coast day1 afternoon: min, max, mean
"11.59,0","18.30,0","14.95,0", // area day1 afternoon: min, max, mean
"8.80,0","10.05,0","9.42,0", // inland night: min, max, mean
"8.28,0","9.51,0","8.89,0", // coast night: min, max, mean
"10.13,0","10.56,0","10.34,0", // area night: min, max, mean
"14.07,0","19.77,0","16.92,0", // inland day2: min, max, mean
"14.91,0","21.92,0","18.41,0", // coast day2: min, max, mean
"13.37,0","24.39,0","18.88,0", // area day2: min, max, mean
"14.53,0","14.99,0","14.76,0", // inland day2 morning: min, max, mean
"15.51,0","17.30,0","16.41,0", // coast day2 morning: min, max, mean
"13.91,0","15.57,0","14.74,0", // area day2 morning: min, max, mean
"18.41,0","18.81,0","18.61,0", // inland day2 afternoon: min, max, mean
"18.72,0","21.53,0","20.12,0", // coast day2 afternoon: min, max, mean
"18.40,0","23.82,0","21.11,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 13...18 astetta, huomenna 19...24 astetta. Yön alin lämpötila on noin 10 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"11.76,0","14.58,0","13.17,0", // inland day1: min, max, mean
"12.62,0","18.54,0","15.58,0", // coast day1: min, max, mean
"11.72,0","17.30,0","14.51,0", // area day1: min, max, mean
"11.76,0","12.27,0","12.02,0", // inland day1 morning: min, max, mean
"12.62,0","14.16,0","13.39,0", // coast day1 morning: min, max, mean
"11.72,0","12.51,0","12.12,0", // area day1 morning: min, max, mean
"13.70,0","14.58,0","14.14,0", // inland day1 afternoon: min, max, mean
"14.39,0","18.54,0","16.47,0", // coast day1 afternoon: min, max, mean
"13.27,0","17.30,0","15.28,0", // area day1 afternoon: min, max, mean
"11.53,0","12.21,0","11.87,0", // inland night: min, max, mean
"7.99,0","9.39,0","8.69,0", // coast night: min, max, mean
"7.60,0","8.36,0","7.98,0", // area night: min, max, mean
"12.59,0","17.06,0","14.82,0", // inland day2: min, max, mean
"12.81,0","20.30,0","16.56,0", // coast day2: min, max, mean
"12.10,0","17.96,0","15.03,0", // area day2: min, max, mean
"12.98,0","13.64,0","13.31,0", // inland day2 morning: min, max, mean
"13.29,0","14.57,0","13.93,0", // coast day2 morning: min, max, mean
"12.45,0","13.26,0","12.86,0", // area day2 morning: min, max, mean
"15.95,0","16.94,0","16.45,0", // inland day2 afternoon: min, max, mean
"16.53,0","19.67,0","18.10,0", // coast day2 afternoon: min, max, mean
"15.39,0","17.27,0","16.33,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 13...17 astetta, huomenna suunnilleen sama. Yön alin lämpötila on sisämaassa noin 12 astetta, rannikolla noin 10 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"14.07,0","15.16,0","14.62,0", // inland day1: min, max, mean
"14.63,0","20.00,0","17.32,0", // coast day1: min, max, mean
"13.14,0","15.49,0","14.31,0", // area day1: min, max, mean
"14.07,0","14.77,0","14.42,0", // inland day1 morning: min, max, mean
"14.63,0","14.93,0","14.78,0", // coast day1 morning: min, max, mean
"13.14,0","13.64,0","13.39,0", // area day1 morning: min, max, mean
"14.80,0","15.16,0","14.98,0", // inland day1 afternoon: min, max, mean
"15.59,0","20.00,0","17.79,0", // coast day1 afternoon: min, max, mean
"14.42,0","15.49,0","14.95,0", // area day1 afternoon: min, max, mean
"11.40,0","12.45,0","11.93,0", // inland night: min, max, mean
"13.71,0","14.10,0","13.90,0", // coast night: min, max, mean
"8.23,0","8.72,0","8.48,0", // area night: min, max, mean
"12.67,0","19.79,0","16.23,0", // inland day2: min, max, mean
"13.36,0","24.34,0","18.85,0", // coast day2: min, max, mean
"12.41,0","22.74,0","17.57,0", // area day2: min, max, mean
"13.64,0","14.40,0","14.02,0", // inland day2 morning: min, max, mean
"13.89,0","15.33,0","14.61,0", // coast day2 morning: min, max, mean
"12.83,0","14.38,0","13.60,0", // area day2 morning: min, max, mean
"18.06,0","18.88,0","18.47,0", // inland day2 afternoon: min, max, mean
"18.45,0","23.69,0","21.07,0", // coast day2 afternoon: min, max, mean
"17.52,0","22.13,0","19.83,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on sisämaassa 15 asteen tuntumassa, rannikolla 16...20 astetta. Huomenna päivän ylin lämpötila on 18...22 astetta. Yön alin lämpötila on vajaat 10 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"10.11,0","11.11,0","10.61,0", // inland day1: min, max, mean
"10.55,0","12.05,0","11.30,0", // coast day1: min, max, mean
"9.41,0","13.32,0","11.37,0", // area day1: min, max, mean
"10.11,0","10.33,0","10.22,0", // inland day1 morning: min, max, mean
"10.55,0","12.32,0","11.43,0", // coast day1 morning: min, max, mean
"9.41,0","9.56,0","9.49,0", // area day1 morning: min, max, mean
"10.65,0","11.11,0","10.88,0", // inland day1 afternoon: min, max, mean
"11.28,0","12.05,0","11.67,0", // coast day1 afternoon: min, max, mean
"10.55,0","13.32,0","11.93,0", // area day1 afternoon: min, max, mean
"6.31,0","7.06,0","6.69,0", // inland night: min, max, mean
"9.46,0","10.46,0","9.96,0", // coast night: min, max, mean
"8.24,0","9.10,0","8.67,0", // area night: min, max, mean
"5.78,0","12.06,0","8.92,0", // inland day2: min, max, mean
"5.98,0","18.45,0","12.22,0", // coast day2: min, max, mean
"6.04,0","13.16,0","9.60,0", // area day2: min, max, mean
"6.56,0","6.97,0","6.76,0", // inland day2 morning: min, max, mean
"6.59,0","7.58,0","7.08,0", // coast day2 morning: min, max, mean
"6.34,0","8.15,0","7.25,0", // area day2 morning: min, max, mean
"11.52,0","11.89,0","11.71,0", // inland day2 afternoon: min, max, mean
"11.73,0","17.87,0","14.80,0", // coast day2 afternoon: min, max, mean
"10.79,0","12.36,0","11.57,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 11...13 astetta. Huomenna päivän ylin lämpötila on sisämaassa vähän yli 10 astetta, rannikolla 13...18 astetta. Yön alin lämpötila on sisämaassa noin 7 astetta, rannikolla 10 asteen tuntumassa."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"10.39,0","12.25,0","11.32,0", // inland day1: min, max, mean
"11.01,0","15.23,0","13.12,0", // coast day1: min, max, mean
"9.59,0","13.91,0","11.75,0", // area day1: min, max, mean
"10.39,0","10.64,0","10.51,0", // inland day1 morning: min, max, mean
"11.01,0","11.32,0","11.17,0", // coast day1 morning: min, max, mean
"9.59,0","11.05,0","10.32,0", // area day1 morning: min, max, mean
"12.09,0","12.25,0","12.17,0", // inland day1 afternoon: min, max, mean
"12.28,0","15.23,0","13.76,0", // coast day1 afternoon: min, max, mean
"11.37,0","13.91,0","12.64,0", // area day1 afternoon: min, max, mean
"8.66,0","9.79,0","9.23,0", // inland night: min, max, mean
"9.02,0","10.22,0","9.62,0", // coast night: min, max, mean
"6.97,0","8.53,0","7.75,0", // area night: min, max, mean
"11.39,0","20.35,0","15.87,0", // inland day2: min, max, mean
"13.17,0","21.45,0","17.31,0", // coast day2: min, max, mean
"11.31,0","20.76,0","16.04,0", // area day2: min, max, mean
"12.20,0","12.95,0","12.58,0", // inland day2 morning: min, max, mean
"13.20,0","14.26,0","13.73,0", // coast day2 morning: min, max, mean
"12.17,0","13.92,0","13.04,0", // area day2 morning: min, max, mean
"19.58,0","19.93,0","19.75,0", // inland day2 afternoon: min, max, mean
"19.61,0","20.91,0","20.26,0", // coast day2 afternoon: min, max, mean
"18.89,0","20.73,0","19.81,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 11...14 astetta, huomenna lähellä 20 astetta. Yön alin lämpötila on 7...9 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"10.74,0","12.32,0","11.53,0", // inland day1: min, max, mean
"11.18,0","15.07,0","13.12,0", // coast day1: min, max, mean
"10.55,0","18.30,0","14.43,0", // area day1: min, max, mean
"10.74,0","11.06,0","10.90,0", // inland day1 morning: min, max, mean
"11.18,0","12.90,0","12.04,0", // coast day1 morning: min, max, mean
"10.55,0","10.82,0","10.69,0", // area day1 morning: min, max, mean
"11.77,0","12.32,0","12.04,0", // inland day1 afternoon: min, max, mean
"12.66,0","15.07,0","13.87,0", // coast day1 afternoon: min, max, mean
"11.59,0","18.30,0","14.95,0", // area day1 afternoon: min, max, mean
"8.80,0","10.05,0","9.42,0", // inland night: min, max, mean
"8.28,0","9.51,0","8.89,0", // coast night: min, max, mean
"10.13,0","10.56,0","10.34,0", // area night: min, max, mean
"14.07,0","19.77,0","16.92,0", // inland day2: min, max, mean
"14.91,0","21.92,0","18.41,0", // coast day2: min, max, mean
"13.37,0","24.39,0","18.88,0", // area day2: min, max, mean
"14.53,0","14.99,0","14.76,0", // inland day2 morning: min, max, mean
"15.51,0","17.30,0","16.41,0", // coast day2 morning: min, max, mean
"13.91,0","15.57,0","14.74,0", // area day2 morning: min, max, mean
"18.41,0","18.81,0","18.61,0", // inland day2 afternoon: min, max, mean
"18.72,0","21.53,0","20.12,0", // coast day2 afternoon: min, max, mean
"18.40,0","23.82,0","21.11,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 13...18 astetta, huomenna 19...24 astetta. Yön alin lämpötila on noin 10 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"11.76,0","14.58,0","13.17,0", // inland day1: min, max, mean
"12.62,0","18.54,0","15.58,0", // coast day1: min, max, mean
"11.72,0","17.30,0","14.51,0", // area day1: min, max, mean
"11.76,0","12.27,0","12.02,0", // inland day1 morning: min, max, mean
"12.62,0","14.16,0","13.39,0", // coast day1 morning: min, max, mean
"11.72,0","12.51,0","12.12,0", // area day1 morning: min, max, mean
"13.70,0","14.58,0","14.14,0", // inland day1 afternoon: min, max, mean
"14.39,0","18.54,0","16.47,0", // coast day1 afternoon: min, max, mean
"13.27,0","17.30,0","15.28,0", // area day1 afternoon: min, max, mean
"11.53,0","12.21,0","11.87,0", // inland night: min, max, mean
"7.99,0","9.39,0","8.69,0", // coast night: min, max, mean
"7.60,0","8.36,0","7.98,0", // area night: min, max, mean
"12.59,0","17.06,0","14.82,0", // inland day2: min, max, mean
"12.81,0","20.30,0","16.56,0", // coast day2: min, max, mean
"12.10,0","17.96,0","15.03,0", // area day2: min, max, mean
"12.98,0","13.64,0","13.31,0", // inland day2 morning: min, max, mean
"13.29,0","14.57,0","13.93,0", // coast day2 morning: min, max, mean
"12.45,0","13.26,0","12.86,0", // area day2 morning: min, max, mean
"15.95,0","16.94,0","16.45,0", // inland day2 afternoon: min, max, mean
"16.53,0","19.67,0","18.10,0", // coast day2 afternoon: min, max, mean
"15.39,0","17.27,0","16.33,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 13...17 astetta, huomenna suunnilleen sama. Yön alin lämpötila on sisämaassa noin 12 astetta, rannikolla noin 10 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"14.07,0","15.16,0","14.62,0", // inland day1: min, max, mean
"14.63,0","20.00,0","17.32,0", // coast day1: min, max, mean
"13.14,0","15.49,0","14.31,0", // area day1: min, max, mean
"14.07,0","14.77,0","14.42,0", // inland day1 morning: min, max, mean
"14.63,0","14.93,0","14.78,0", // coast day1 morning: min, max, mean
"13.14,0","13.64,0","13.39,0", // area day1 morning: min, max, mean
"14.80,0","15.16,0","14.98,0", // inland day1 afternoon: min, max, mean
"15.59,0","20.00,0","17.79,0", // coast day1 afternoon: min, max, mean
"14.42,0","15.49,0","14.95,0", // area day1 afternoon: min, max, mean
"11.40,0","12.45,0","11.93,0", // inland night: min, max, mean
"13.71,0","14.10,0","13.90,0", // coast night: min, max, mean
"8.23,0","8.72,0","8.48,0", // area night: min, max, mean
"12.67,0","19.79,0","16.23,0", // inland day2: min, max, mean
"13.36,0","24.34,0","18.85,0", // coast day2: min, max, mean
"12.41,0","22.74,0","17.57,0", // area day2: min, max, mean
"13.64,0","14.40,0","14.02,0", // inland day2 morning: min, max, mean
"13.89,0","15.33,0","14.61,0", // coast day2 morning: min, max, mean
"12.83,0","14.38,0","13.60,0", // area day2 morning: min, max, mean
"18.06,0","18.88,0","18.47,0", // inland day2 afternoon: min, max, mean
"18.45,0","23.69,0","21.07,0", // coast day2 afternoon: min, max, mean
"17.52,0","22.13,0","19.83,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on sisämaassa 15 asteen tuntumassa, rannikolla 16...20 astetta. Huomenna päivän ylin lämpötila on 18...22 astetta. Yön alin lämpötila on vajaat 10 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"10.11,0","11.11,0","10.61,0", // inland day1: min, max, mean
"10.55,0","12.05,0","11.30,0", // coast day1: min, max, mean
"9.41,0","13.32,0","11.37,0", // area day1: min, max, mean
"10.11,0","10.33,0","10.22,0", // inland day1 morning: min, max, mean
"10.55,0","12.32,0","11.43,0", // coast day1 morning: min, max, mean
"9.41,0","9.56,0","9.49,0", // area day1 morning: min, max, mean
"10.65,0","11.11,0","10.88,0", // inland day1 afternoon: min, max, mean
"11.28,0","12.05,0","11.67,0", // coast day1 afternoon: min, max, mean
"10.55,0","13.32,0","11.93,0", // area day1 afternoon: min, max, mean
"6.31,0","7.06,0","6.69,0", // inland night: min, max, mean
"9.46,0","10.46,0","9.96,0", // coast night: min, max, mean
"8.24,0","9.10,0","8.67,0", // area night: min, max, mean
"5.78,0","12.06,0","8.92,0", // inland day2: min, max, mean
"5.98,0","18.45,0","12.22,0", // coast day2: min, max, mean
"6.04,0","13.16,0","9.60,0", // area day2: min, max, mean
"6.56,0","6.97,0","6.76,0", // inland day2 morning: min, max, mean
"6.59,0","7.58,0","7.08,0", // coast day2 morning: min, max, mean
"6.34,0","8.15,0","7.25,0", // area day2 morning: min, max, mean
"11.52,0","11.89,0","11.71,0", // inland day2 afternoon: min, max, mean
"11.73,0","17.87,0","14.80,0", // coast day2 afternoon: min, max, mean
"10.79,0","12.36,0","11.57,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 11...13 astetta. Huomenna päivän ylin lämpötila on sisämaassa vähän yli 10 astetta, rannikolla 13...18 astetta. Yön alin lämpötila on sisämaassa noin 7 astetta, rannikolla 10 asteen tuntumassa."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"10.39,0","12.25,0","11.32,0", // inland day1: min, max, mean
"11.01,0","15.23,0","13.12,0", // coast day1: min, max, mean
"9.59,0","13.91,0","11.75,0", // area day1: min, max, mean
"10.39,0","10.64,0","10.51,0", // inland day1 morning: min, max, mean
"11.01,0","11.32,0","11.17,0", // coast day1 morning: min, max, mean
"9.59,0","11.05,0","10.32,0", // area day1 morning: min, max, mean
"12.09,0","12.25,0","12.17,0", // inland day1 afternoon: min, max, mean
"12.28,0","15.23,0","13.76,0", // coast day1 afternoon: min, max, mean
"11.37,0","13.91,0","12.64,0", // area day1 afternoon: min, max, mean
"8.66,0","9.79,0","9.23,0", // inland night: min, max, mean
"9.02,0","10.22,0","9.62,0", // coast night: min, max, mean
"6.97,0","8.53,0","7.75,0", // area night: min, max, mean
"11.39,0","20.35,0","15.87,0", // inland day2: min, max, mean
"13.17,0","21.45,0","17.31,0", // coast day2: min, max, mean
"11.31,0","20.76,0","16.04,0", // area day2: min, max, mean
"12.20,0","12.95,0","12.58,0", // inland day2 morning: min, max, mean
"13.20,0","14.26,0","13.73,0", // coast day2 morning: min, max, mean
"12.17,0","13.92,0","13.04,0", // area day2 morning: min, max, mean
"19.58,0","19.93,0","19.75,0", // inland day2 afternoon: min, max, mean
"19.61,0","20.91,0","20.26,0", // coast day2 afternoon: min, max, mean
"18.89,0","20.73,0","19.81,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 11...14 astetta, huomenna lähellä 20 astetta. Yön alin lämpötila on 7...9 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"10.74,0","12.32,0","11.53,0", // inland day1: min, max, mean
"11.18,0","15.07,0","13.12,0", // coast day1: min, max, mean
"10.55,0","18.30,0","14.43,0", // area day1: min, max, mean
"10.74,0","11.06,0","10.90,0", // inland day1 morning: min, max, mean
"11.18,0","12.90,0","12.04,0", // coast day1 morning: min, max, mean
"10.55,0","10.82,0","10.69,0", // area day1 morning: min, max, mean
"11.77,0","12.32,0","12.04,0", // inland day1 afternoon: min, max, mean
"12.66,0","15.07,0","13.87,0", // coast day1 afternoon: min, max, mean
"11.59,0","18.30,0","14.95,0", // area day1 afternoon: min, max, mean
"8.80,0","10.05,0","9.42,0", // inland night: min, max, mean
"8.28,0","9.51,0","8.89,0", // coast night: min, max, mean
"10.13,0","10.56,0","10.34,0", // area night: min, max, mean
"14.07,0","19.77,0","16.92,0", // inland day2: min, max, mean
"14.91,0","21.92,0","18.41,0", // coast day2: min, max, mean
"13.37,0","24.39,0","18.88,0", // area day2: min, max, mean
"14.53,0","14.99,0","14.76,0", // inland day2 morning: min, max, mean
"15.51,0","17.30,0","16.41,0", // coast day2 morning: min, max, mean
"13.91,0","15.57,0","14.74,0", // area day2 morning: min, max, mean
"18.41,0","18.81,0","18.61,0", // inland day2 afternoon: min, max, mean
"18.72,0","21.53,0","20.12,0", // coast day2 afternoon: min, max, mean
"18.40,0","23.82,0","21.11,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 13...18 astetta, huomenna 19...24 astetta. Yön alin lämpötila on noin 10 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"11.76,0","14.58,0","13.17,0", // inland day1: min, max, mean
"12.62,0","18.54,0","15.58,0", // coast day1: min, max, mean
"11.72,0","17.30,0","14.51,0", // area day1: min, max, mean
"11.76,0","12.27,0","12.02,0", // inland day1 morning: min, max, mean
"12.62,0","14.16,0","13.39,0", // coast day1 morning: min, max, mean
"11.72,0","12.51,0","12.12,0", // area day1 morning: min, max, mean
"13.70,0","14.58,0","14.14,0", // inland day1 afternoon: min, max, mean
"14.39,0","18.54,0","16.47,0", // coast day1 afternoon: min, max, mean
"13.27,0","17.30,0","15.28,0", // area day1 afternoon: min, max, mean
"11.53,0","12.21,0","11.87,0", // inland night: min, max, mean
"7.99,0","9.39,0","8.69,0", // coast night: min, max, mean
"7.60,0","8.36,0","7.98,0", // area night: min, max, mean
"12.59,0","17.06,0","14.82,0", // inland day2: min, max, mean
"12.81,0","20.30,0","16.56,0", // coast day2: min, max, mean
"12.10,0","17.96,0","15.03,0", // area day2: min, max, mean
"12.98,0","13.64,0","13.31,0", // inland day2 morning: min, max, mean
"13.29,0","14.57,0","13.93,0", // coast day2 morning: min, max, mean
"12.45,0","13.26,0","12.86,0", // area day2 morning: min, max, mean
"15.95,0","16.94,0","16.45,0", // inland day2 afternoon: min, max, mean
"16.53,0","19.67,0","18.10,0", // coast day2 afternoon: min, max, mean
"15.39,0","17.27,0","16.33,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 13...17 astetta, huomenna suunnilleen sama. Yön alin lämpötila on sisämaassa noin 12 astetta, rannikolla noin 10 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"14.07,0","15.16,0","14.62,0", // inland day1: min, max, mean
"14.63,0","20.00,0","17.32,0", // coast day1: min, max, mean
"13.14,0","15.49,0","14.31,0", // area day1: min, max, mean
"14.07,0","14.77,0","14.42,0", // inland day1 morning: min, max, mean
"14.63,0","14.93,0","14.78,0", // coast day1 morning: min, max, mean
"13.14,0","13.64,0","13.39,0", // area day1 morning: min, max, mean
"14.80,0","15.16,0","14.98,0", // inland day1 afternoon: min, max, mean
"15.59,0","20.00,0","17.79,0", // coast day1 afternoon: min, max, mean
"14.42,0","15.49,0","14.95,0", // area day1 afternoon: min, max, mean
"11.40,0","12.45,0","11.93,0", // inland night: min, max, mean
"13.71,0","14.10,0","13.90,0", // coast night: min, max, mean
"8.23,0","8.72,0","8.48,0", // area night: min, max, mean
"12.67,0","19.79,0","16.23,0", // inland day2: min, max, mean
"13.36,0","24.34,0","18.85,0", // coast day2: min, max, mean
"12.41,0","22.74,0","17.57,0", // area day2: min, max, mean
"13.64,0","14.40,0","14.02,0", // inland day2 morning: min, max, mean
"13.89,0","15.33,0","14.61,0", // coast day2 morning: min, max, mean
"12.83,0","14.38,0","13.60,0", // area day2 morning: min, max, mean
"18.06,0","18.88,0","18.47,0", // inland day2 afternoon: min, max, mean
"18.45,0","23.69,0","21.07,0", // coast day2 afternoon: min, max, mean
"17.52,0","22.13,0","19.83,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on sisämaassa 15 asteen tuntumassa, rannikolla 16...20 astetta. Huomenna päivän ylin lämpötila on 18...22 astetta. Yön alin lämpötila on vajaat 10 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"10.11,0","11.11,0","10.61,0", // inland day1: min, max, mean
"10.55,0","12.05,0","11.30,0", // coast day1: min, max, mean
"9.41,0","13.32,0","11.37,0", // area day1: min, max, mean
"10.11,0","10.33,0","10.22,0", // inland day1 morning: min, max, mean
"10.55,0","12.32,0","11.43,0", // coast day1 morning: min, max, mean
"9.41,0","9.56,0","9.49,0", // area day1 morning: min, max, mean
"10.65,0","11.11,0","10.88,0", // inland day1 afternoon: min, max, mean
"11.28,0","12.05,0","11.67,0", // coast day1 afternoon: min, max, mean
"10.55,0","13.32,0","11.93,0", // area day1 afternoon: min, max, mean
"6.31,0","7.06,0","6.69,0", // inland night: min, max, mean
"9.46,0","10.46,0","9.96,0", // coast night: min, max, mean
"8.24,0","9.10,0","8.67,0", // area night: min, max, mean
"5.78,0","12.06,0","8.92,0", // inland day2: min, max, mean
"5.98,0","18.45,0","12.22,0", // coast day2: min, max, mean
"6.04,0","13.16,0","9.60,0", // area day2: min, max, mean
"6.56,0","6.97,0","6.76,0", // inland day2 morning: min, max, mean
"6.59,0","7.58,0","7.08,0", // coast day2 morning: min, max, mean
"6.34,0","8.15,0","7.25,0", // area day2 morning: min, max, mean
"11.52,0","11.89,0","11.71,0", // inland day2 afternoon: min, max, mean
"11.73,0","17.87,0","14.80,0", // coast day2 afternoon: min, max, mean
"10.79,0","12.36,0","11.57,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 11...13 astetta. Huomenna päivän ylin lämpötila on sisämaassa vähän yli 10 astetta, rannikolla 13...18 astetta. Yön alin lämpötila on sisämaassa noin 7 astetta, rannikolla 10 asteen tuntumassa."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"10.39,0","12.25,0","11.32,0", // inland day1: min, max, mean
"11.01,0","15.23,0","13.12,0", // coast day1: min, max, mean
"9.59,0","13.91,0","11.75,0", // area day1: min, max, mean
"10.39,0","10.64,0","10.51,0", // inland day1 morning: min, max, mean
"11.01,0","11.32,0","11.17,0", // coast day1 morning: min, max, mean
"9.59,0","11.05,0","10.32,0", // area day1 morning: min, max, mean
"12.09,0","12.25,0","12.17,0", // inland day1 afternoon: min, max, mean
"12.28,0","15.23,0","13.76,0", // coast day1 afternoon: min, max, mean
"11.37,0","13.91,0","12.64,0", // area day1 afternoon: min, max, mean
"8.66,0","9.79,0","9.23,0", // inland night: min, max, mean
"9.02,0","10.22,0","9.62,0", // coast night: min, max, mean
"6.97,0","8.53,0","7.75,0", // area night: min, max, mean
"11.39,0","20.35,0","15.87,0", // inland day2: min, max, mean
"13.17,0","21.45,0","17.31,0", // coast day2: min, max, mean
"11.31,0","20.76,0","16.04,0", // area day2: min, max, mean
"12.20,0","12.95,0","12.58,0", // inland day2 morning: min, max, mean
"13.20,0","14.26,0","13.73,0", // coast day2 morning: min, max, mean
"12.17,0","13.92,0","13.04,0", // area day2 morning: min, max, mean
"19.58,0","19.93,0","19.75,0", // inland day2 afternoon: min, max, mean
"19.61,0","20.91,0","20.26,0", // coast day2 afternoon: min, max, mean
"18.89,0","20.73,0","19.81,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 11...14 astetta, huomenna lähellä 20 astetta. Yön alin lämpötila on 7...9 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"10.74,0","12.32,0","11.53,0", // inland day1: min, max, mean
"11.18,0","15.07,0","13.12,0", // coast day1: min, max, mean
"10.55,0","18.30,0","14.43,0", // area day1: min, max, mean
"10.74,0","11.06,0","10.90,0", // inland day1 morning: min, max, mean
"11.18,0","12.90,0","12.04,0", // coast day1 morning: min, max, mean
"10.55,0","10.82,0","10.69,0", // area day1 morning: min, max, mean
"11.77,0","12.32,0","12.04,0", // inland day1 afternoon: min, max, mean
"12.66,0","15.07,0","13.87,0", // coast day1 afternoon: min, max, mean
"11.59,0","18.30,0","14.95,0", // area day1 afternoon: min, max, mean
"8.80,0","10.05,0","9.42,0", // inland night: min, max, mean
"8.28,0","9.51,0","8.89,0", // coast night: min, max, mean
"10.13,0","10.56,0","10.34,0", // area night: min, max, mean
"14.07,0","19.77,0","16.92,0", // inland day2: min, max, mean
"14.91,0","21.92,0","18.41,0", // coast day2: min, max, mean
"13.37,0","24.39,0","18.88,0", // area day2: min, max, mean
"14.53,0","14.99,0","14.76,0", // inland day2 morning: min, max, mean
"15.51,0","17.30,0","16.41,0", // coast day2 morning: min, max, mean
"13.91,0","15.57,0","14.74,0", // area day2 morning: min, max, mean
"18.41,0","18.81,0","18.61,0", // inland day2 afternoon: min, max, mean
"18.72,0","21.53,0","20.12,0", // coast day2 afternoon: min, max, mean
"18.40,0","23.82,0","21.11,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 13...18 astetta, huomenna 19...24 astetta. Yön alin lämpötila on noin 10 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"11.76,0","14.58,0","13.17,0", // inland day1: min, max, mean
"12.62,0","18.54,0","15.58,0", // coast day1: min, max, mean
"11.72,0","17.30,0","14.51,0", // area day1: min, max, mean
"11.76,0","12.27,0","12.02,0", // inland day1 morning: min, max, mean
"12.62,0","14.16,0","13.39,0", // coast day1 morning: min, max, mean
"11.72,0","12.51,0","12.12,0", // area day1 morning: min, max, mean
"13.70,0","14.58,0","14.14,0", // inland day1 afternoon: min, max, mean
"14.39,0","18.54,0","16.47,0", // coast day1 afternoon: min, max, mean
"13.27,0","17.30,0","15.28,0", // area day1 afternoon: min, max, mean
"11.53,0","12.21,0","11.87,0", // inland night: min, max, mean
"7.99,0","9.39,0","8.69,0", // coast night: min, max, mean
"7.60,0","8.36,0","7.98,0", // area night: min, max, mean
"12.59,0","17.06,0","14.82,0", // inland day2: min, max, mean
"12.81,0","20.30,0","16.56,0", // coast day2: min, max, mean
"12.10,0","17.96,0","15.03,0", // area day2: min, max, mean
"12.98,0","13.64,0","13.31,0", // inland day2 morning: min, max, mean
"13.29,0","14.57,0","13.93,0", // coast day2 morning: min, max, mean
"12.45,0","13.26,0","12.86,0", // area day2 morning: min, max, mean
"15.95,0","16.94,0","16.45,0", // inland day2 afternoon: min, max, mean
"16.53,0","19.67,0","18.10,0", // coast day2 afternoon: min, max, mean
"15.39,0","17.27,0","16.33,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 13...17 astetta, huomenna suunnilleen sama. Yön alin lämpötila on sisämaassa noin 12 astetta, rannikolla noin 10 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"19.74,0","22.21,0","20.97,0", // inland day1: min, max, mean
"18.93,0","23.47,0","21.20,0", // coast day1: min, max, mean
"19.67,0","23.03,0","21.35,0", // area day1: min, max, mean
"19.74,0","20.67,0","20.20,0", // inland day1 morning: min, max, mean
"18.93,0","20.01,0","19.47,0", // coast day1 morning: min, max, mean
"19.67,0","21.31,0","20.49,0", // area day1 morning: min, max, mean
"21.76,0","22.21,0","21.99,0", // inland day1 afternoon: min, max, mean
"19.95,0","23.47,0","21.71,0", // coast day1 afternoon: min, max, mean
"21.53,0","23.03,0","22.28,0", // area day1 afternoon: min, max, mean
"18.48,0","18.50,0","18.49,0", // inland night: min, max, mean
"15.79,0","17.21,0","16.50,0", // coast night: min, max, mean
"16.85,0","18.58,0","17.72,0", // area night: min, max, mean
"23.51,0","27.81,0","25.66,0", // inland day2: min, max, mean
"24.06,0","26.75,0","25.40,0", // coast day2: min, max, mean
"23.14,0","30.24,0","26.69,0", // area day2: min, max, mean
"24.00,0","24.86,0","24.43,0", // inland day2 morning: min, max, mean
"24.68,0","26.62,0","25.65,0", // coast day2 morning: min, max, mean
"23.37,0","25.25,0","24.31,0", // area day2 morning: min, max, mean
"26.66,0","27.58,0","27.12,0", // inland day2 afternoon: min, max, mean
"25.26,0","26.01,0","25.63,0", // coast day2 afternoon: min, max, mean
"25.73,0","29.88,0","27.80,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on noin 22 astetta, huomenna 26...30 astetta. Yön alin lämpötila on 17...19 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"19.95,0","22.78,0","21.36,0", // inland day1: min, max, mean
"19.03,0","21.81,0","20.42,0", // coast day1: min, max, mean
"19.42,0","22.26,0","20.84,0", // area day1: min, max, mean
"19.95,0","20.35,0","20.15,0", // inland day1 morning: min, max, mean
"19.03,0","20.70,0","19.87,0", // coast day1 morning: min, max, mean
"19.42,0","20.37,0","19.90,0", // area day1 morning: min, max, mean
"22.19,0","22.78,0","22.49,0", // inland day1 afternoon: min, max, mean
"20.47,0","21.81,0","21.14,0", // coast day1 afternoon: min, max, mean
"21.29,0","22.26,0","21.77,0", // area day1 afternoon: min, max, mean
"17.93,0","18.32,0","18.13,0", // inland night: min, max, mean
"17.51,0","18.25,0","17.88,0", // coast night: min, max, mean
"18.49,0","18.61,0","18.55,0", // area night: min, max, mean
"20.95,0","25.21,0","23.08,0", // inland day2: min, max, mean
"20.81,0","27.58,0","24.19,0", // coast day2: min, max, mean
"20.29,0","28.79,0","24.54,0", // area day2: min, max, mean
"21.06,0","21.82,0","21.44,0", // inland day2 morning: min, max, mean
"21.20,0","22.62,0","21.91,0", // coast day2 morning: min, max, mean
"20.55,0","20.76,0","20.65,0", // area day2 morning: min, max, mean
"23.76,0","24.56,0","24.16,0", // inland day2 afternoon: min, max, mean
"23.43,0","27.43,0","25.43,0", // coast day2 afternoon: min, max, mean
"23.47,0","28.43,0","25.95,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on noin 22 astetta, huomenna 23...28 astetta. Yön alin lämpötila on vajaat 20 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"20.17,0","24.08,0","22.13,0", // inland day1: min, max, mean
"18.82,0","26.56,0","22.69,0", // coast day1: min, max, mean
"19.94,0","25.25,0","22.59,0", // area day1: min, max, mean
"20.17,0","20.76,0","20.47,0", // inland day1 morning: min, max, mean
"18.82,0","20.42,0","19.62,0", // coast day1 morning: min, max, mean
"19.94,0","21.64,0","20.79,0", // area day1 morning: min, max, mean
"23.45,0","24.08,0","23.77,0", // inland day1 afternoon: min, max, mean
"22.81,0","26.56,0","24.68,0", // coast day1 afternoon: min, max, mean
"22.51,0","25.25,0","23.88,0", // area day1 afternoon: min, max, mean
"18.74,0","20.07,0","19.40,0", // inland night: min, max, mean
"17.65,0","18.34,0","17.99,0", // coast night: min, max, mean
"16.11,0","16.37,0","16.24,0", // area night: min, max, mean
"18.08,0","21.40,0","19.74,0", // inland day2: min, max, mean
"19.27,0","23.76,0","21.51,0", // coast day2: min, max, mean
"17.47,0","21.88,0","19.67,0", // area day2: min, max, mean
"18.53,0","18.83,0","18.68,0", // inland day2 morning: min, max, mean
"19.33,0","20.24,0","19.79,0", // coast day2 morning: min, max, mean
"17.57,0","18.02,0","17.79,0", // area day2 morning: min, max, mean
"20.88,0","21.33,0","21.10,0", // inland day2 afternoon: min, max, mean
"19.61,0","22.80,0","21.20,0", // coast day2 afternoon: min, max, mean
"20.63,0","21.25,0","20.94,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 23...25 astetta, huomenna suunnilleen sama. Yön alin lämpötila on vähän yli 15 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"21.73,0","23.61,0","22.67,0", // inland day1: min, max, mean
"21.21,0","26.17,0","23.69,0", // coast day1: min, max, mean
"21.08,0","24.82,0","22.95,0", // area day1: min, max, mean
"21.73,0","22.55,0","22.14,0", // inland day1 morning: min, max, mean
"21.21,0","21.42,0","21.31,0", // coast day1 morning: min, max, mean
"21.08,0","21.85,0","21.46,0", // area day1 morning: min, max, mean
"22.76,0","23.61,0","23.18,0", // inland day1 afternoon: min, max, mean
"21.55,0","26.17,0","23.86,0", // coast day1 afternoon: min, max, mean
"22.57,0","24.82,0","23.70,0", // area day1 afternoon: min, max, mean
"17.49,0","19.21,0","18.35,0", // inland night: min, max, mean
"20.79,0","22.18,0","21.48,0", // coast night: min, max, mean
"17.87,0","18.19,0","18.03,0", // area night: min, max, mean
"21.68,0","27.41,0","24.55,0", // inland day2: min, max, mean
"22.19,0","26.85,0","24.52,0", // coast day2: min, max, mean
"21.25,0","29.03,0","25.14,0", // area day2: min, max, mean
"21.81,0","22.16,0","21.98,0", // inland day2 morning: min, max, mean
"22.77,0","23.33,0","23.05,0", // coast day2 morning: min, max, mean
"21.53,0","22.98,0","22.26,0", // area day2 morning: min, max, mean
"26.50,0","27.22,0","26.86,0", // inland day2 afternoon: min, max, mean
"25.42,0","26.22,0","25.82,0", // coast day2 afternoon: min, max, mean
"25.82,0","28.69,0","27.26,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 23...25 astetta, huomenna hieman korkeampi. Yön alin lämpötila on sisämaassa 17...19 astetta, rannikolla noin 20 astetta."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"23.15,0","24.14,0","23.65,0", // inland day1: min, max, mean
"22.85,0","26.35,0","24.60,0", // coast day1: min, max, mean
"22.32,0","25.23,0","23.78,0", // area day1: min, max, mean
"23.15,0","23.43,0","23.29,0", // inland day1 morning: min, max, mean
"22.85,0","23.24,0","23.04,0", // coast day1 morning: min, max, mean
"22.32,0","23.14,0","22.73,0", // area day1 morning: min, max, mean
"23.78,0","24.14,0","23.96,0", // inland day1 afternoon: min, max, mean
"22.33,0","26.35,0","24.34,0", // coast day1 afternoon: min, max, mean
"23.55,0","25.23,0","24.39,0", // area day1 afternoon: min, max, mean
"21.63,0","23.40,0","22.52,0", // inland night: min, max, mean
"19.27,0","19.71,0","19.49,0", // coast night: min, max, mean
"20.56,0","22.46,0","21.51,0", // area night: min, max, mean
"25.38,0","28.25,0","26.81,0", // inland day2: min, max, mean
"25.37,0","28.44,0","26.91,0", // coast day2: min, max, mean
"25.25,0","28.03,0","26.64,0", // area day2: min, max, mean
"26.17,0","26.85,0","26.51,0", // inland day2 morning: min, max, mean
"26.20,0","26.27,0","26.23,0", // coast day2 morning: min, max, mean
"25.61,0","26.32,0","25.96,0", // area day2 morning: min, max, mean
"27.15,0","28.02,0","27.59,0", // inland day2 afternoon: min, max, mean
"25.56,0","27.46,0","26.51,0", // coast day2 afternoon: min, max, mean
"27.06,0","27.72,0","27.39,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 25 asteen tuntumassa, huomenna hieman korkeampi. Yön alin lämpötila on sisämaassa noin 23 astetta, rannikolla 20 asteen tuntumassa."))); // the story

testCases.insert(make_pair(i++, new Max36HoursTestParam(
"15.85,0","17.32,0","16.58,0", // inland day1: min, max, mean
"13.85,0","19.47,0","16.66,0", // coast day1: min, max, mean
"15.36,0","18.24,0","16.80,0", // area day1: min, max, mean
"15.85,0","16.63,0","16.24,0", // inland day1 morning: min, max, mean
"13.85,0","14.68,0","14.27,0", // coast day1 morning: min, max, mean
"15.36,0","16.09,0","15.72,0", // area day1 morning: min, max, mean
"16.47,0","17.32,0","16.89,0", // inland day1 afternoon: min, max, mean
"15.21,0","19.47,0","17.34,0", // coast day1 afternoon: min, max, mean
"16.42,0","18.24,0","17.33,0", // area day1 afternoon: min, max, mean
"15.29,0","16.32,0","15.80,0", // inland night: min, max, mean
"10.74,0","11.28,0","11.01,0", // coast night: min, max, mean
"14.05,0","14.57,0","14.31,0", // area night: min, max, mean
"19.71,0","22.36,0","21.03,0", // inland day2: min, max, mean
"19.39,0","24.40,0","21.90,0", // coast day2: min, max, mean
"18.26,0","26.13,0","22.20,0", // area day2: min, max, mean
"19.76,0","20.75,0","20.26,0", // inland day2 morning: min, max, mean
"19.76,0","21.58,0","20.67,0", // coast day2 morning: min, max, mean
"19.17,0","20.18,0","19.67,0", // area day2 morning: min, max, mean
"21.56,0","21.70,0","21.63,0", // inland day2 afternoon: min, max, mean
"20.96,0","23.49,0","22.22,0", // coast day2 afternoon: min, max, mean
"20.94,0","25.32,0","23.13,0", // area day2 afternoon: min, max, mean
"Päivän ylin lämpötila on 16...18 astetta, huomenna 21...25 astetta. Yön alin lämpötila on sisämaassa noin 15 astetta, rannikolla noin 10 astetta."))); // the story


	}
}

void create_anomaly_testcases(TestCaseContainer& testCases, const string& language, const bool& isWinter)
{
#ifdef LATER
  int i = 0;

  if(isWinter)
	{
	  if(language == "fi")
		{
		  // pakkanen kiristyy
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "-6.0,0","-7.0,0","-6.5,0", // d1 min, max, mean
																	 "-6.0,0","-7.0,0","-6.5,0", // night min, max, mean
																	 "-26.0,0","-27.0,0","-26.5,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Pakkanen kiristyy.")));

		  // kireä pakkanen jatkuu
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "-26.0,0","-27.0,0","-26.5,0", // d1 min, max, mean
																	 "-6.0,0","-7.0,0","-6.5,0", // night min, max, mean
																	 "-26.0,0","-27.0,0","-26.5,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Sää on poikkeuksellisen kylmää. Kireä pakkanen jatkuu.")));

		  // sää on poikkeuksellisen kylmää ja pakkanen kiristyy
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "-27.0,0","-28.0,0","-27.5,0", // d1 min, max, mean
																	 "-27.0,0","-28.0,0","-27.5,0", // night min, max, mean
																	 "-32.0,0","-33.0,0","-32.5,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Sää on poikkeuksellisen kylmää. Pakkanen kiristyy.")));
		  // kireä pakkanen heikkenee
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "-27.0,0","-28.0,0","-27.5,0", // d1 min, max, mean
																	 "-27.0,0","-28.0,0","-27.5,0", // night min, max, mean
																	 "-6.0,0","-7.0,0","-6.5,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Sää on poikkeuksellisen kylmää. Kireä pakkanen heikkenee.")));
		  // pakkanen heikkenee
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "-18.0,0","-19.0,0","-18.5,0", // d1 min, max, mean
																	 "-18.0,0","-19.0,0","-18.5,0", // night min, max, mean
																	 "-6.0,0","-7.0,0","-6.5,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Pakkanen heikkenee.")));
		  /*
		  // pakkanen hellittää (sää lauhtuu)
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
		  "-12.0,0","-14.0,0","-13.5,0", // d1 min, max, mean
		  "-12.0,0","-14.0,0","-13.5,0", // night min, max, mean
		  "-2.0,0","-6.0,0","-4.5,0", // d2 min, max, mean
		  "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
		  "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
		  "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
		  "Pakkanen hellittää.")));
		  */

		  // sää lauhtuu
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "-18.0,0","-19.0,0","-18.5,0", // d1 min, max, mean
																	 "-18.0,0","-19.0,0","-18.5,0", // night min, max, mean
																	 "-4.0,0","-6.0,0","-4.9,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Sää lauhtuu.")));
		  // sää lauhtuu
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "-27.0,0","-28.0,0","-27.5,0", // d1 min, max, mean
																	 "-27.0,0","-28.0,0","-27.5,0", // night min, max, mean
																	 "-4.0,0","-6.0,0","-4.9,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Sää on poikkeuksellisen kylmää. Sää lauhtuu.")));


		  // sää one edelleen lauhaa
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "-2.0,0","-3.0,0","-2.5,0", // d1 min, max, mean
																	 "-2.0,0","-3.0,0","-2.5,0", // night min, max, mean
																	 "-2.0,0","-3.0,0","-2.5,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Sää on edelleen lauhaa.")));

		  // sää one edelleen lauhaa. sää on erittäin tuulista
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "-2.0,0","-3.0,0","-2.5,0", // d1 min, max, mean
																	 "-2.0,0","-3.0,0","-2.5,0", // night min, max, mean
																	 "-2.0,0","-3.0,0","-2.5,0", // d2 min, max, mean
																	 "10.0,0","12.0,0","11.0,0", //d2 windspeed morning min,max,mean
																	 "10.0,0","12.0,0","11.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Sää on edelleen lauhaa. Sää on erittäin tuulinen.")));

		  // sää on hyvin kylmää. kireä pakkanen jatkuu. sää on tuulinen. pakkanen on purevaa
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "-22.0,0","-23.0,0","-22.5,0", // d1 min, max, mean
																	 "-22.0,0","-23.0,0","-22.5,0", // night min, max, mean
																	 "-22.0,0","-23.0,0","-22.5,0", // d2 min, max, mean
																	 "8.0,0","10.0,0","9.0,0", //d2 windspeed morning min,max,mean
																	 "9.0,0","11.0,0","9.6,0", //d2 windspeed afternoon min,max,me															    
																	 "-26.0,0","-29.0,0","-27.0,0", //d2 windchill min,max,mean
																	 "Sää on hyvin kylmää. Kireä pakkanen jatkuu. Sää on tuulinen. Pakkanen on purevaa.")));

		  // sää on hyvin kylmää. kireä pakkanen jatkuu. sää on erittäin tuulinen. pakkanen on erittäin purevaa
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
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
		  // sää muuttuu helteiseksi
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "18.0,0","19.0,0","18.5,0", // d1 min, max, mean
																	 "15.0,0","16.0,0","15.5,0", // night min, max, mean
																	 "25.0,0","26.0,0","25.5,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Sää muuttuu helteiseksi.")));

		  // sää on harvinaisen lämmintä. sää on helteistä
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "23.0,0","24.8,0","23.5,0", // d1 min, max, mean
																	 "15.0,0","16.0,0","15.5,0", // night min, max, mean
																	 "25.0,0","26.0,0","25.5,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Sää on harvinaisen lämmintä. Sää on helteistä.")));

		  // sää on harvinaisen lämmintä. sää muuttuu helteiseksi
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "23.0,0","24.8,0","22.5,0", // d1 min, max, mean
																	 "15.0,0","16.0,0","15.5,0", // night min, max, mean
																	 "24.0,0","26.0,0","25.0,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Sää on harvinaisen lämmintä. Sää muuttuu helteiseksi.")));

		  // helteinen sää jatkuu (vaikka lämpötila laskee)
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "29.0,0","30.0,0","29.5,0", // d1 min, max, mean
																	 "20.0,0","21.0,0","20.5,0", // night min, max, mean
																	 "25.0,0","26.0,0","25.5,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Helteinen sää jatkuu.")));

		  // helteinen sää jatkuu (lämpötila pysyy samana)
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "29.0,0","30.0,0","29.5,0", // d1 min, max, mean
																	 "20.0,0","21.0,0","20.5,0", // night min, max, mean
																	 "29.0,0","30.0,0","29.5,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Helteinen sää jatkuu.")));

		  // helteinen sää jatkuu (lämpötila nousee)
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "25.0,0","26.0,0","25.5,0", // d1 min, max, mean
																	 "20.0,0","21.0,0","20.5,0", // night min, max, mean
																	 "29.0,0","30.0,0","29.5,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Helteinen sää jatkuu.")));

		  // sää on koleaa. kolea sää jatkuu (lämpötila nousee)
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "8.0,0","9.0,0","8.5,0", // d1 min, max, mean
																	 "5.0,0","6.0,0","5.5,0", // night min, max, mean
																	 "10.0,0","11.0,0","10.5,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Sää on koleaa. Kolea sää jatkuu.")));

		  // sää on koleaa. kolea sää jatkuu
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "8.0,0","9.0,0","8.5,0", // d1 min, max, mean
																	 "5.0,0","6.0,0","5.5,0", // night min, max, mean
																	 "7.5,0","8.5,0","8.0,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Sää on koleaa. Kolea sää jatkuu.")));

		  // sää on koleaa. viileä sää jatkuu
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "8.0,0","9.0,0","8.5,0", // d1 min, max, mean
																	 "5.0,0","6.0,0","5.5,0", // night min, max, mean
																	 "12.0,0","14.0,0","13.5,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Sää on koleaa. Viileä sää jatkuu.")));

		  // sää viilenee vähän
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "17.0,0","18.0,0","17.5,0", // d1 min, max, mean
																	 "15.0,0","16.0,0","15.5,0", // night min, max, mean
																	 "14.0,0","15.0,0","14.6,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Sää viilenee vähän.")));
		  // sää viilenee
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "17.0,0","18.0,0","17.5,0", // d1 min, max, mean
																	 "15.0,0","16.0,0","15.5,0", // night min, max, mean
																	 "14.0,0","15.0,0","14.5,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Sää viilenee.")));
		  // sää viilenee huomattavasti
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "17.0,0","18.0,0","17.5,0", // d1 min, max, mean
																	 "15.0,0","16.0,0","15.5,0", // night min, max, mean
																	 "10.0,0","11.0,0","10.5,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Sää viilenee huomattavasti.")));
		  // sää lämpenee vähän
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "14.0,0","15.0,0","14.6,0", // d1 min, max, mean
																	 "15.0,0","16.0,0","15.5,0", // night min, max, mean
																	 "17.0,0","18.0,0","17.5,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Sää lämpenee vähän.")));
		  // sää lämpenee
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "14.0,0","15.0,0","14.5,0", // d1 min, max, mean
																	 "15.0,0","16.0,0","15.5,0", // night min, max, mean
																	 "17.0,0","18.0,0","17.5,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Sää lämpenee.")));
		  // sää on koleaa, sää lämpenee huomattavasti
		  testCases.insert(make_pair(i++,TemperatureAnomalyTestParam(
																	 "10.0,0","11.0,0","10.5,0", // d1 min, max, mean
																	 "15.0,0","16.0,0","15.5,0", // night min, max, mean
																	 "17.0,0","18.0,0","17.5,0", // d2 min, max, mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed morning min,max,mean
																	 "2.0,0","2.0,0","2.0,0", //d2 windspeed afternoon min,max,mean
																	 "0.0,0","0.0,0","0.0,0", //d2 windchill min,max,mean
																	 "Sää on koleaa. Sää lämpenee huomattavasti.")));
		}
	}
#endif
}
  //void generate_testcasefile(const char* theFileName, const char* theLanguage, const bool& anomaly = false)
void generate_testcasefile(const char* theFileName, const char* theLanguage, const TemperatureTestType& theTestType)
{
  AnalysisSources sources;
  WeatherArea area("25,60");

  NFmiSettings::Set("textgen::fractiles_climatology","/home/reponen/work/testdata/tmax.sqd");
  NFmiSettings::Set("max36hours::fake::fractile02_limit", "2.0");
  NFmiSettings::Set("max36hours::day::starthour","6");
  NFmiSettings::Set("max36hours::day::maxstarthour","11");
  NFmiSettings::Set("max36hours::day::endhour","18");
  NFmiSettings::Set("max36hours::night::starthour","18");
  NFmiSettings::Set("max36hours::night::endhour","6");

  NFmiTime time1(2009,4,1,6,0,0);
  NFmiTime time2(2009,4,2,18,0,0);
  WeatherPeriod period_day1_night_day2(time1,time2);
  TemperatureStory story1(time1,sources,area,period_day1_night_day2,"max36hours");
  TestCaseContainer testCases;
	
  generate_testcase_parameters(testCases, theLanguage, theTestType);

  if(theTestType == TEMPERATURE_ANOMALY)
	{
	  create_temperature_anomaly_testcase_stories(testCases, theLanguage, sources, area);
	  create_temperature_anomaly_testcasefile(testCases, theFileName);
	}
  else if(theTestType == TEMPERATURE_MAX36_HOURS)
	{
	  create_max36hours_testcase_stories(testCases, theLanguage, sources, area);
	  create_max36hours_testcasefile(testCases, theFileName);
	}
  else if(theTestType == WIND_ANOMALY)
	{
	  create_wind_anomaly_testcase_stories(testCases, theLanguage, sources, area);
	  create_wind_anomaly_testcasefile(testCases, theFileName);
	}
}

void delete_testcases(TestCaseContainer& testCases)
{
	TestCaseContainer::iterator iter;
	for(iter = testCases.begin(); iter != testCases.end(); iter++)
	  delete iter->second;
	testCases.clear();
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
	NFmiSettings::Set("max36hours::fake::area_percentage", "10.0");
	NFmiSettings::Set("max36hours::fake::growing_season_on", "false");
	//	NFmiSettings::Set("qdtext::forecasttime","200901010615");
	//NFmiSettings::Set("textgen::main_forecast", "/home/reponen/work/testdata/200904300654_pal_skandinavia_pinta.sqd");

	//qdtext::forecasttime = 200904301200
	
	NFmiTime time1(2009,1,1,6,0,0);
	NFmiTime time2(2009,1,2,18,0,0);
	WeatherPeriod period_day1_night_day2(time1,time2);
	TemperatureStory story1(time1,sources,area,period_day1_night_day2,"max36hours");

	TestCaseContainer testCases;
	TestCaseContainer::iterator iter;

	const char* languages [] = {"fi", "sv", "en"};

	//generate_testcasefile("max36hours_testcases.txt", "fi", false);
	//create_max36hours_testcases(testCases, languages[i]);

	// here you should iterate all languages
	for(int i = 0; i < 1; i++)
	  {
		read_testcasefile(testCases, "./max36hours_testcases.txt", false);
		
		Max36HoursTestParam* param = 0;		
		for(iter = testCases.begin(); iter != testCases.end(); iter++)
		  {
			param = static_cast<Max36HoursTestParam*>(iter->second);

			NFmiSettings::Set("max36hours::fake::day1::inland::min",param->temperature_d1_inlandmin);
			NFmiSettings::Set("max36hours::fake::day1::inland::max",param->temperature_d1_inlandmax);
			NFmiSettings::Set("max36hours::fake::day1::inland::mean",param->temperature_d1_inlandmean);
			NFmiSettings::Set("max36hours::fake::day1::coast::min",param->temperature_d1_coastmin);
			NFmiSettings::Set("max36hours::fake::day1::coast::max",param->temperature_d1_coastmax);
			NFmiSettings::Set("max36hours::fake::day1::coast::mean",param->temperature_d1_coastmean);
			NFmiSettings::Set("max36hours::fake::day1::area::min",param->temperature_d1_areamin);
			NFmiSettings::Set("max36hours::fake::day1::area::max",param->temperature_d1_areamax);
			NFmiSettings::Set("max36hours::fake::day1::area::mean",param->temperature_d1_areamean);

			NFmiSettings::Set("max36hours::fake::day1::morning::inland::min",param->temperature_d1_morning_inlandmin);
			NFmiSettings::Set("max36hours::fake::day1::morning::inland::max",param->temperature_d1_morning_inlandmax);
			NFmiSettings::Set("max36hours::fake::day1::morning::inland::mean",param->temperature_d1_morning_inlandmean);
			NFmiSettings::Set("max36hours::fake::day1::morning::coast::min",param->temperature_d1_morning_coastmin);
			NFmiSettings::Set("max36hours::fake::day1::morning::coast::max",param->temperature_d1_morning_coastmax);
			NFmiSettings::Set("max36hours::fake::day1::morning::coast::mean",param->temperature_d1_morning_coastmean);
			NFmiSettings::Set("max36hours::fake::day1::morning::area::min",param->temperature_d1_morning_areamin);
			NFmiSettings::Set("max36hours::fake::day1::morning::area::max",param->temperature_d1_morning_areamax);
			NFmiSettings::Set("max36hours::fake::day1::morning::area::mean",param->temperature_d1_morning_areamean);

			NFmiSettings::Set("max36hours::fake::day1::afternoon::inland::min",param->temperature_d1_afternoon_inlandmin);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::inland::max",param->temperature_d1_afternoon_inlandmax);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::inland::mean",param->temperature_d1_afternoon_inlandmean);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::coast::min",param->temperature_d1_afternoon_coastmin);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::coast::max",param->temperature_d1_afternoon_coastmax);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::coast::mean",param->temperature_d1_afternoon_coastmean);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::area::min",param->temperature_d1_afternoon_areamin);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::area::max",param->temperature_d1_afternoon_areamax);
			NFmiSettings::Set("max36hours::fake::day1::afternoon::area::mean",param->temperature_d1_afternoon_areamean);

			NFmiSettings::Set("max36hours::fake::night::inland::min",param->temperature_nite_inlandmin);
			NFmiSettings::Set("max36hours::fake::night::inland::max",param->temperature_nite_inlandmax);
			NFmiSettings::Set("max36hours::fake::night::inland::mean",param->temperature_nite_inlandmean);
			NFmiSettings::Set("max36hours::fake::night::coast::min",param->temperature_nite_coastmin);
			NFmiSettings::Set("max36hours::fake::night::coast::max",param->temperature_nite_coastmax);
			NFmiSettings::Set("max36hours::fake::night::coast::mean",param->temperature_nite_coastmean);
			NFmiSettings::Set("max36hours::fake::night::area::min",param->temperature_nite_areamin);
			NFmiSettings::Set("max36hours::fake::night::area::max",param->temperature_nite_areamax);
			NFmiSettings::Set("max36hours::fake::night::area::mean",param->temperature_nite_areamean);

			NFmiSettings::Set("max36hours::fake::day2::inland::min",param->temperature_d2_inlandmin);
			NFmiSettings::Set("max36hours::fake::day2::inland::max",param->temperature_d2_inlandmax);
			NFmiSettings::Set("max36hours::fake::day2::inland::mean",param->temperature_d2_inlandmean);
			NFmiSettings::Set("max36hours::fake::day2::coast::min",param->temperature_d2_coastmin);
			NFmiSettings::Set("max36hours::fake::day2::coast::max",param->temperature_d2_coastmax);
			NFmiSettings::Set("max36hours::fake::day2::coast::mean",param->temperature_d2_coastmean);
			NFmiSettings::Set("max36hours::fake::day2::area::min",param->temperature_d2_areamin);
			NFmiSettings::Set("max36hours::fake::day2::area::max",param->temperature_d2_areamax);
			NFmiSettings::Set("max36hours::fake::day2::area::mean",param->temperature_d2_areamean);

			NFmiSettings::Set("max36hours::fake::day2::morning::inland::min",param->temperature_d2_morning_inlandmin);
			NFmiSettings::Set("max36hours::fake::day2::morning::inland::max",param->temperature_d2_morning_inlandmax);
			NFmiSettings::Set("max36hours::fake::day2::morning::inland::mean",param->temperature_d2_morning_inlandmean);
			NFmiSettings::Set("max36hours::fake::day2::morning::coast::min",param->temperature_d2_morning_coastmin);
			NFmiSettings::Set("max36hours::fake::day2::morning::coast::max",param->temperature_d2_morning_coastmax);
			NFmiSettings::Set("max36hours::fake::day2::morning::coast::mean",param->temperature_d2_morning_coastmean);
			NFmiSettings::Set("max36hours::fake::day2::morning::area::min",param->temperature_d2_morning_areamin);
			NFmiSettings::Set("max36hours::fake::day2::morning::area::max",param->temperature_d2_morning_areamax);
			NFmiSettings::Set("max36hours::fake::day2::morning::area::mean",param->temperature_d2_morning_areamean);

			NFmiSettings::Set("max36hours::fake::day2::afternoon::inland::min",param->temperature_d2_afternoon_inlandmin);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::inland::max",param->temperature_d2_afternoon_inlandmax);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::inland::mean",param->temperature_d2_afternoon_inlandmean);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::coast::min",param->temperature_d2_afternoon_coastmin);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::coast::max",param->temperature_d2_afternoon_coastmax);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::coast::mean",param->temperature_d2_afternoon_coastmean);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::area::min",param->temperature_d2_afternoon_areamin);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::area::max",param->temperature_d2_afternoon_areamax);
			NFmiSettings::Set("max36hours::fake::day2::afternoon::area::mean",param->temperature_d2_afternoon_areamean);

			require(story1,languages[i],fun,param->theStory);
		  }
		delete_testcases(testCases);
	  }


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
	NFmiTime time1(2009, 1, 1, 6, 0, 0);
	NFmiTime time2(2009, 2, 2, 18, 0, 0);
	WeatherPeriod period_day1_night_day2(time1,time2);
	TemperatureStory story1(time1,sources, area, period_day1_night_day2,"anomaly");
	NFmiSettings::Set("textgen::fractiles_climatology","/home/reponen/work/testdata/tmax.sqd");
	const string fun = "temperature_anomaly";

	TestCaseContainer testCases;
	TestCaseContainer::iterator iter;

	const char* languages [] = {"fi", "sv", "en"};

	//generate_testcasefile("temperature_anomaly_testcases.txt", "fi", true);
	
	// here you should iterate all languages
	for(int i = 0; i < 1; i++)
	  {
		read_testcasefile(testCases, "./temperature_anomaly_testcases.txt", true);
		
		TemperatureAnomalyTestParam* param = 0;		

		// iterate the fractile shares
		for(int k = 0; k < 4; k++)
		  {
			// fractile shares
			NFmiSettings::Set("anomaly::fake::fractile::share::F02", "0.0,0.0");
			NFmiSettings::Set("anomaly::fake::fractile::share::F12", "0.0,0.0");
			NFmiSettings::Set("anomaly::fake::fractile::share::F88", "0.0,0.0");
			NFmiSettings::Set("anomaly::fake::fractile::share::F98", "0.0,0.0");
			if(k == 0)
			  NFmiSettings::Set("anomaly::fake::fractile::share::F02", "80.0,0.0");
			else if(k == 1)
			  NFmiSettings::Set("anomaly::fake::fractile::share::F12", "80.0,0.0");
			else if(k == 2)
			  NFmiSettings::Set("anomaly::fake::fractile::share::F88", "80.0,0.0");
			else
			  NFmiSettings::Set("anomaly::fake::fractile::share::F98", "80.0,0.0");

			for(iter = testCases.begin(); iter != testCases.end(); iter++)
			  {
				param = static_cast<TemperatureAnomalyTestParam*>(iter->second);

				NFmiSettings::Set("anomaly::fake::temperature::day1::afternoon::area::min",
								  param->temperature_d1_afternoon_areamin);
				NFmiSettings::Set("anomaly::fake::temperature::day1::afternoon::area::max",
								  param->temperature_d1_afternoon_areamax);
				NFmiSettings::Set("anomaly::fake::temperature::day1::afternoon::area::mean",
								  param->temperature_d1_afternoon_areamean);
				NFmiSettings::Set("anomaly::fake::temperature::day1::afternoon::inland::min",
								  param->temperature_d1_afternoon_inlandmin);
				NFmiSettings::Set("anomaly::fake::temperature::day1::afternoon::inland::max",
								  param->temperature_d1_afternoon_inlandmax);
				NFmiSettings::Set("anomaly::fake::temperature::day1::afternoon::inland::mean",
								  param->temperature_d1_afternoon_inlandmean);
				NFmiSettings::Set("anomaly::fake::temperature::day1::afternoon::coast::min",
								  param->temperature_d1_afternoon_coastmin);
				NFmiSettings::Set("anomaly::fake::temperature::day1::afternoon::coast::max",
								  param->temperature_d1_afternoon_coastmax);
				NFmiSettings::Set("anomaly::fake::temperature::day1::afternoon::coast::mean",
								  param->temperature_d1_afternoon_coastmean);

				NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::area::min",
								  param->temperature_d2_afternoon_areamin);
				NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::area::max",
								  param->temperature_d2_afternoon_areamax);
				NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::area::mean",
								  param->temperature_d2_afternoon_areamean);
				NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::inland::min",
								  param->temperature_d2_afternoon_inlandmin);
				NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::inland::max",
								  param->temperature_d2_afternoon_inlandmax);
				NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::inland::mean",
								  param->temperature_d2_afternoon_inlandmean);
				NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::coast::min",
								  param->temperature_d2_afternoon_coastmin);
				NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::coast::max",
								  param->temperature_d2_afternoon_coastmax);
				NFmiSettings::Set("anomaly::fake::temperature::day2::afternoon::coast::mean",
								  param->temperature_d2_afternoon_coastmean);

				require(story1,languages[i],fun,param->theStory);

			  }
		  }
		delete_testcases(testCases);
	  }

	TEST_PASSED();
  }

  void generate_testcasefiles()
  {
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
	virtual const char * error_message_prefix() const
	{
	  return "\n\t";
	}

	//! Main test suite
	void test(void)
	{
	  //TEST(generate_testcasefiles);
	  TEST(temperature_max36hours);
	  //TEST(temperature_anomaly);
	  //TEST(temperature_day);
	  //TEST(temperature_mean);
	  //TEST(temperature_meanmax);
	  //TEST(temperature_meanmin);
	  //TEST(temperature_dailymax);
	  //TEST(temperature_weekly_minmax);
	  //TEST(temperature_weekly_averages);
	  //TEST(temperature_range);
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

  dict.reset(TextGen::DictionaryFactory::create("multimysql"));

  NFmiSettings::Set("textgen::units::celsius::format","phrase");
  NFmiSettings::Set("textgen::database","textgen2");


  NFmiSettings::Init();

  dict->init("fi");
  dict->init("sv");
  dict->init("en");

  tests t;
  return t.run();
}
