#include "regression/tframe.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "PlainTextFormatter.h"
#include "TemperatureStory.h"
#include "Story.h"

#include "NFmiSettings.h"

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
   * \brief Test TemperatureStory::mean()
   */
  // ----------------------------------------------------------------------

  void temperature_mean()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("dummy");
	NFmiTime time1(2000,1,1);
	NFmiTime time2(2000,1,2);
	WeatherPeriod period(time1,time2);
	TemperatureStory story(time1,sources,area,period,"mean");

	const string fun = "temperature_mean";

	NFmiSettings::instance().set("mean::fake::mean","0.1,0");
	require(story,"fi",fun,"Keskilämpötila 0\260C.");
	require(story,"sv",fun,"Medeltemperaturen 0 grader.");
	require(story,"en",fun,"Mean temperature 0 degrees.");

	NFmiSettings::instance().set("mean::fake::mean","0.5,0");
	require(story,"fi",fun,"Keskilämpötila 1\260C.");
	require(story,"sv",fun,"Medeltemperaturen 1 grader.");
	require(story,"en",fun,"Mean temperature 1 degrees.");

	NFmiSettings::instance().set("mean::fake::mean","10,0");
	require(story,"fi",fun,"Keskilämpötila 10\260C.");
	require(story,"sv",fun,"Medeltemperaturen 10 grader.");
	require(story,"en",fun,"Mean temperature 10 degrees.");

	NFmiSettings::instance().set("mean::fake::mean","-10.5,0");
	require(story,"fi",fun,"Keskilämpötila -11\260C.");
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
	WeatherArea area("dummy");
	NFmiTime time1(2000,1,1);
	NFmiTime time2(2000,1,2);
	WeatherPeriod period(time1,time2);
	TemperatureStory story(time1,sources,area,period,"meanmax");

	const string fun = "temperature_meanmax";

	NFmiSettings::instance().set("meanmax::fake::mean","0.1,0");
	require(story,"fi",fun,"Keskimääräinen ylin lämpötila 0 astetta.");
	require(story,"sv",fun,"Den maximi temperaturen i medeltal 0 grader.");
	require(story,"en",fun,"Mean maximum temperature 0 degrees.");

	NFmiSettings::instance().set("meanmax::fake::mean","0.5,0");
	require(story,"fi",fun,"Keskimääräinen ylin lämpötila 1 astetta.");
	require(story,"sv",fun,"Den maximi temperaturen i medeltal 1 grader.");
	require(story,"en",fun,"Mean maximum temperature 1 degrees.");

	NFmiSettings::instance().set("meanmax::fake::mean","10,0");
	require(story,"fi",fun,"Keskimääräinen ylin lämpötila 10 astetta.");
	require(story,"sv",fun,"Den maximi temperaturen i medeltal 10 grader.");
	require(story,"en",fun,"Mean maximum temperature 10 degrees.");

	NFmiSettings::instance().set("meanmax::fake::mean","-10.5,0");
	require(story,"fi",fun,"Keskimääräinen ylin lämpötila -11 astetta.");
	require(story,"sv",fun,"Den maximi temperaturen i medeltal -11 grader.");
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
	WeatherArea area("dummy");
	NFmiTime time1(2000,1,1);
	NFmiTime time2(2000,1,2);
	WeatherPeriod period(time1,time2);
	TemperatureStory story(time1,sources,area,period,"meanmin");

	const string fun = "temperature_meanmin";

	NFmiSettings::instance().set("meanmin::fake::mean","0.1,0");
	require(story,"fi",fun,"Keskimääräinen alin lämpötila 0 astetta.");
	require(story,"sv",fun,"Den minimi temperaturen i medeltal 0 grader.");
	require(story,"en",fun,"Mean minimum temperature 0 degrees.");

	NFmiSettings::instance().set("meanmin::fake::mean","0.5,0");
	require(story,"fi",fun,"Keskimääräinen alin lämpötila 1 astetta.");
	require(story,"sv",fun,"Den minimi temperaturen i medeltal 1 grader.");
	require(story,"en",fun,"Mean minimum temperature 1 degrees.");

	NFmiSettings::instance().set("meanmin::fake::mean","10,0");
	require(story,"fi",fun,"Keskimääräinen alin lämpötila 10 astetta.");
	require(story,"sv",fun,"Den minimi temperaturen i medeltal 10 grader.");
	require(story,"en",fun,"Mean minimum temperature 10 degrees.");

	NFmiSettings::instance().set("meanmin::fake::mean","-10.5,0");
	require(story,"fi",fun,"Keskimääräinen alin lämpötila -11 astetta.");
	require(story,"sv",fun,"Den minimi temperaturen i medeltal -11 grader.");
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
	WeatherArea area("dummy");
	const string fun = "temperature_dailymax";

	NFmiSettings::instance().set("dailymax::significantly_higher","6");
	NFmiSettings::instance().set("dailymax::higher","4");
	NFmiSettings::instance().set("dailymax::somewhat_higher","2");
	NFmiSettings::instance().set("dailymax::somewhat_lower","2");
	NFmiSettings::instance().set("dailymax::lower","4");
	NFmiSettings::instance().set("dailymax::significantly_lower","6");
	NFmiSettings::instance().set("dailymax::mininterval","2");
	NFmiSettings::instance().set("dailymax::always_interval_zero","true");
	NFmiSettings::instance().set("dailymax::starthour","6");
	NFmiSettings::instance().set("dailymax::endhour","18");
	NFmiSettings::instance().set("dailymax::prefer_daynames","false");

	// Test the part concerning day 1
	{
	  NFmiTime time1(2003,6,1);
	  NFmiTime time2(2003,6,2);
	  WeatherPeriod period(time1,time2);
	  TemperatureStory story(time1,sources,area,period,"dailymax");
	
	  NFmiSettings::instance().set("dailymax::fake::day1::minimum","5,0");
	  NFmiSettings::instance().set("dailymax::fake::day1::mean","5,0");
	  NFmiSettings::instance().set("dailymax::fake::day1::maximum","5,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina noin 5 astetta.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen cirka 5 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday about 5 degrees.");

	  NFmiSettings::instance().set("dailymax::fake::day1::minimum","5,0");
	  NFmiSettings::instance().set("dailymax::fake::day1::mean","6,0");
	  NFmiSettings::instance().set("dailymax::fake::day1::maximum","6,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina noin 6 astetta.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen cirka 6 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday about 6 degrees.");

	  NFmiSettings::instance().set("dailymax::fake::day1::minimum","5,0");
	  NFmiSettings::instance().set("dailymax::fake::day1::mean","6,0");
	  NFmiSettings::instance().set("dailymax::fake::day1::maximum","7,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina 5...7 astetta.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen 5...7 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees.");

	  NFmiSettings::instance().set("dailymax::fake::day1::minimum","0,0");
	  NFmiSettings::instance().set("dailymax::fake::day1::mean","0,0");
	  NFmiSettings::instance().set("dailymax::fake::day1::maximum","0,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina noin 0 astetta.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen cirka 0 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday about 0 degrees.");

	  NFmiSettings::instance().set("dailymax::fake::day1::minimum","0,0");
	  NFmiSettings::instance().set("dailymax::fake::day1::mean","0,0");
	  NFmiSettings::instance().set("dailymax::fake::day1::maximum","1,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina 0...1 astetta.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen 0...1 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 0...1 degrees.");

	  NFmiSettings::instance().set("dailymax::fake::day1::minimum","-1,0");
	  NFmiSettings::instance().set("dailymax::fake::day1::mean","0,0");
	  NFmiSettings::instance().set("dailymax::fake::day1::maximum","0,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina -1...0 astetta.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen -1...0 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday -1...0 degrees.");

	  NFmiSettings::instance().set("dailymax::fake::day1::minimum","-1,0");
	  NFmiSettings::instance().set("dailymax::fake::day1::mean","0,0");
	  NFmiSettings::instance().set("dailymax::fake::day1::maximum","1,0");
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
	
	  NFmiSettings::instance().set("dailymax::fake::day1::minimum","5,0");
	  NFmiSettings::instance().set("dailymax::fake::day1::mean","6,0");
	  NFmiSettings::instance().set("dailymax::fake::day1::maximum","7,0");

	  // change 0 degrees
	  NFmiSettings::instance().set("dailymax::fake::day2::minimum","5,0");
	  NFmiSettings::instance().set("dailymax::fake::day2::mean","6,0");
	  NFmiSettings::instance().set("dailymax::fake::day2::maximum","7,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina 5...7 astetta, seuraavana päivänä suunnilleen sama.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen 5...7 grader, följande dag ungefär densamma.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day about the same.");

	  // change 1 degrees
	  NFmiSettings::instance().set("dailymax::fake::day2::minimum","6,0");
	  NFmiSettings::instance().set("dailymax::fake::day2::mean","7,0");
	  NFmiSettings::instance().set("dailymax::fake::day2::maximum","8,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina 5...7 astetta, seuraavana päivänä suunnilleen sama.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen 5...7 grader, följande dag ungefär densamma.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day about the same.");

	  // change 2 degrees
	  NFmiSettings::instance().set("dailymax::fake::day2::minimum","7,0");
	  NFmiSettings::instance().set("dailymax::fake::day2::mean","8,0");
	  NFmiSettings::instance().set("dailymax::fake::day2::maximum","9,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina 5...7 astetta, seuraavana päivänä hieman korkeampi.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen 5...7 grader, följande dag något högre.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day somewhat higher.");

	  // change 4 degrees
	  NFmiSettings::instance().set("dailymax::fake::day2::minimum","9,0");
	  NFmiSettings::instance().set("dailymax::fake::day2::mean","10,0");
	  NFmiSettings::instance().set("dailymax::fake::day2::maximum","11,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina 5...7 astetta, seuraavana päivänä korkeampi.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen 5...7 grader, följande dag högre.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day higher.");

	  // change 6 degrees
	  NFmiSettings::instance().set("dailymax::fake::day2::minimum","11,0");
	  NFmiSettings::instance().set("dailymax::fake::day2::mean","12,0");
	  NFmiSettings::instance().set("dailymax::fake::day2::maximum","13,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina 5...7 astetta, seuraavana päivänä huomattavasti korkeampi.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen 5...7 grader, följande dag betydligt högre.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day significantly higher.");

	  // change - 2 degrees
	  NFmiSettings::instance().set("dailymax::fake::day2::minimum","3,0");
	  NFmiSettings::instance().set("dailymax::fake::day2::mean","4,0");
	  NFmiSettings::instance().set("dailymax::fake::day2::maximum","5,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina 5...7 astetta, seuraavana päivänä hieman alempi.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen 5...7 grader, följande dag något lägre.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day somewhat lower.");

	  // change -4 degrees
	  NFmiSettings::instance().set("dailymax::fake::day2::minimum","1,0");
	  NFmiSettings::instance().set("dailymax::fake::day2::mean","2,0");
	  NFmiSettings::instance().set("dailymax::fake::day2::maximum","3,0");
	  require(story,"fi",fun,"Päivän ylin lämpötila on sunnuntaina 5...7 astetta, seuraavana päivänä alempi.");
	  require(story,"sv",fun,"Dagens högsta temperatur är på söndagen 5...7 grader, följande dag lägre.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day lower.");

	  // change -6 degrees
	  NFmiSettings::instance().set("dailymax::fake::day2::minimum","-1,0");
	  NFmiSettings::instance().set("dailymax::fake::day2::mean","0,0");
	  NFmiSettings::instance().set("dailymax::fake::day2::maximum","1,0");
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
	
	  NFmiSettings::instance().set("dailymax::fake::day1::minimum","5,0");
	  NFmiSettings::instance().set("dailymax::fake::day1::mean","6,0");
	  NFmiSettings::instance().set("dailymax::fake::day1::maximum","7,0");

	  NFmiSettings::instance().set("dailymax::fake::day2::minimum","5,0");
	  NFmiSettings::instance().set("dailymax::fake::day2::mean","6,0");
	  NFmiSettings::instance().set("dailymax::fake::day2::maximum","7,0");

	  NFmiSettings::instance().set("dailymax::fake::day3::minimum","8,0");
	  NFmiSettings::instance().set("dailymax::fake::day3::mean","10,0");
	  NFmiSettings::instance().set("dailymax::fake::day3::maximum","12,0");

	  NFmiSettings::instance().set("dailymax::fake::day4::minimum","3,0");
	  NFmiSettings::instance().set("dailymax::fake::day4::mean","4,0");
	  NFmiSettings::instance().set("dailymax::fake::day4::maximum","4,0");

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
	WeatherArea area("dummy");
	NFmiTime time1(2000,1,1);
	NFmiTime time2(2000,1,5);
	WeatherPeriod period(time1,time2);
	TemperatureStory story(time1,sources,area,period,"weekly_minmax");

	const string fun = "temperature_weekly_minmax";

	NFmiSettings::instance().set("weekly_minmax::day::starthour","6");
	NFmiSettings::instance().set("weekly_minmax::day::endhour","18");
	NFmiSettings::instance().set("weekly_minmax::night::starthour","18");
	NFmiSettings::instance().set("weekly_minmax::night::endhour","6");

	NFmiSettings::instance().set("weekly_minmax::fake::day::minimum","10,0");
	NFmiSettings::instance().set("weekly_minmax::fake::day::mean","12,0");
	NFmiSettings::instance().set("weekly_minmax::fake::day::maximum","15,0");
	NFmiSettings::instance().set("weekly_minmax::fake::night::minimum","2,0");
	NFmiSettings::instance().set("weekly_minmax::fake::night::mean","4,0");
	NFmiSettings::instance().set("weekly_minmax::fake::night::maximum","5,0");
	require(story,"fi",fun,"Päivien ylin lämpötila on 10...15 astetta, öiden alin lämpötila 2...5 astetta.");
	require(story,"sv",fun,"Dagens maximi temperaturer är 10...15 grader, nattens minimi temperaturer 2...5 grader.");
	require(story,"en",fun,"Daily maximum temperature is 10...15 degrees, nightly minimum temperature 2...5 degrees.");

	NFmiSettings::instance().set("weekly_minmax::night::mininterval","4");
	require(story,"fi",fun,"Päivien ylin lämpötila on 10...15 astetta, öiden alin lämpötila noin 4 astetta.");
	require(story,"sv",fun,"Dagens maximi temperaturer är 10...15 grader, nattens minimi temperaturer cirka 4 grader.");
	require(story,"en",fun,"Daily maximum temperature is 10...15 degrees, nightly minimum temperature about 4 degrees.");

	NFmiSettings::instance().set("weekly_minmax::day::mininterval","6");
	NFmiSettings::instance().set("weekly_minmax::night::mininterval","2");
	require(story,"fi",fun,"Päivien ylin lämpötila on noin 12 astetta, öiden alin lämpötila 2...5 astetta.");
	require(story,"sv",fun,"Dagens maximi temperaturer är cirka 12 grader, nattens minimi temperaturer 2...5 grader.");
	require(story,"en",fun,"Daily maximum temperature is about 12 degrees, nightly minimum temperature 2...5 degrees.");

	NFmiSettings::instance().set("weekly_minmax::emphasize_night_minimum","true");
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
	WeatherArea area("dummy");
	NFmiTime time1(2000,1,1);
	NFmiTime time2(2000,1,5);
	WeatherPeriod period(time1,time2);
	TemperatureStory story(time1,sources,area,period,"weekly_averages");

	const string fun = "temperature_weekly_averages";

	NFmiSettings::instance().set("weekly_averages::day::starthour","6");
	NFmiSettings::instance().set("weekly_averages::day::endhour","18");
	NFmiSettings::instance().set("weekly_averages::night::starthour","18");
	NFmiSettings::instance().set("weekly_averages::night::endhour","6");

	NFmiSettings::instance().set("weekly_averages::fake::day::minimum","10,0");
	NFmiSettings::instance().set("weekly_averages::fake::day::mean","12,0");
	NFmiSettings::instance().set("weekly_averages::fake::day::maximum","15,0");
	NFmiSettings::instance().set("weekly_averages::fake::night::minimum","2,0");
	NFmiSettings::instance().set("weekly_averages::fake::night::mean","4,0");
	NFmiSettings::instance().set("weekly_averages::fake::night::maximum","5,0");
	require(story,"fi",fun,"Päivälämpötila on 10...15 astetta, yölämpötila 2...5 astetta.");
	require(story,"sv",fun,"Dagstemperaturen är 10...15 grader, nattemperaturen 2...5 grader.");
	require(story,"en",fun,"Daily temperature is 10...15 degrees, nightly temperature 2...5 degrees.");

	NFmiSettings::instance().set("weekly_averages::night::mininterval","4");
	require(story,"fi",fun,"Päivälämpötila on 10...15 astetta, yölämpötila noin 4 astetta.");
	require(story,"sv",fun,"Dagstemperaturen är 10...15 grader, nattemperaturen cirka 4 grader.");
	require(story,"en",fun,"Daily temperature is 10...15 degrees, nightly temperature about 4 degrees.");

	NFmiSettings::instance().set("weekly_averages::day::mininterval","6");
	NFmiSettings::instance().set("weekly_averages::night::mininterval","2");
	require(story,"fi",fun,"Päivälämpötila on noin 12 astetta, yölämpötila 2...5 astetta.");
	require(story,"sv",fun,"Dagstemperaturen är cirka 12 grader, nattemperaturen 2...5 grader.");
	require(story,"en",fun,"Daily temperature is about 12 degrees, nightly temperature 2...5 degrees.");

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
	  TEST(temperature_mean);
	  TEST(temperature_meanmax);
	  TEST(temperature_meanmin);
	  TEST(temperature_dailymax);
	  TEST(temperature_weekly_minmax);
	  TEST(temperature_weekly_averages);
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

  NFmiSettings::instance().set("textgen::units::celsius::format","phrase");

  dict->init("fi");
  dict->init("sv");
  dict->init("en");

  tests t;
  return t.run();
}
