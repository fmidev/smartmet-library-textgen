#include "tframe.h"
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
	WeatherPeriod period(NFmiTime(2000,1,1), NFmiTime(2000,1,2));
	TemperatureStory story(sources,area,period,"test");

	const string fun = "temperature_mean";

	NFmiSettings::instance().set("test::fake::mean","0.1,1");
	require(story,"fi",fun,"Keskil�mp�tila 0 astetta.");
	require(story,"sv",fun,"Medeltemperaturen 0 grader.");
	require(story,"en",fun,"Mean temperature 0 degrees.");

	NFmiSettings::instance().set("test::fake::mean","0.5,1");
	require(story,"fi",fun,"Keskil�mp�tila 1 astetta.");
	require(story,"sv",fun,"Medeltemperaturen 1 grader.");
	require(story,"en",fun,"Mean temperature 1 degrees.");

	NFmiSettings::instance().set("test::fake::mean","10,1");
	require(story,"fi",fun,"Keskil�mp�tila 10 astetta.");
	require(story,"sv",fun,"Medeltemperaturen 10 grader.");
	require(story,"en",fun,"Mean temperature 10 degrees.");

	NFmiSettings::instance().set("test::fake::mean","-10.5,1");
	require(story,"fi",fun,"Keskil�mp�tila -11 astetta.");
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
	WeatherPeriod period(NFmiTime(2000,1,1), NFmiTime(2000,1,2));
	TemperatureStory story(sources,area,period,"test");

	const string fun = "temperature_meanmax";

	NFmiSettings::instance().set("test::fake::mean","0.1,1");
	require(story,"fi",fun,"Keskim��r�inen ylin l�mp�tila 0 astetta.");
	require(story,"sv",fun,"Den h�gsta temperaturen i medeltal 0 grader.");
	require(story,"en",fun,"Mean maximum temperature 0 degrees.");

	NFmiSettings::instance().set("test::fake::mean","0.5,1");
	require(story,"fi",fun,"Keskim��r�inen ylin l�mp�tila 1 astetta.");
	require(story,"sv",fun,"Den h�gsta temperaturen i medeltal 1 grader.");
	require(story,"en",fun,"Mean maximum temperature 1 degrees.");

	NFmiSettings::instance().set("test::fake::mean","10,1");
	require(story,"fi",fun,"Keskim��r�inen ylin l�mp�tila 10 astetta.");
	require(story,"sv",fun,"Den h�gsta temperaturen i medeltal 10 grader.");
	require(story,"en",fun,"Mean maximum temperature 10 degrees.");

	NFmiSettings::instance().set("test::fake::mean","-10.5,1");
	require(story,"fi",fun,"Keskim��r�inen ylin l�mp�tila -11 astetta.");
	require(story,"sv",fun,"Den h�gsta temperaturen i medeltal -11 grader.");
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
	WeatherPeriod period(NFmiTime(2000,1,1), NFmiTime(2000,1,2));
	TemperatureStory story(sources,area,period,"test");

	const string fun = "temperature_meanmin";

	NFmiSettings::instance().set("test::fake::mean","0.1,1");
	require(story,"fi",fun,"Keskim��r�inen alin l�mp�tila 0 astetta.");
	require(story,"sv",fun,"Den l�gsta temperaturen i medeltal 0 grader.");
	require(story,"en",fun,"Mean minimum temperature 0 degrees.");

	NFmiSettings::instance().set("test::fake::mean","0.5,1");
	require(story,"fi",fun,"Keskim��r�inen alin l�mp�tila 1 astetta.");
	require(story,"sv",fun,"Den l�gsta temperaturen i medeltal 1 grader.");
	require(story,"en",fun,"Mean minimum temperature 1 degrees.");

	NFmiSettings::instance().set("test::fake::mean","10,1");
	require(story,"fi",fun,"Keskim��r�inen alin l�mp�tila 10 astetta.");
	require(story,"sv",fun,"Den l�gsta temperaturen i medeltal 10 grader.");
	require(story,"en",fun,"Mean minimum temperature 10 degrees.");

	NFmiSettings::instance().set("test::fake::mean","-10.5,1");
	require(story,"fi",fun,"Keskim��r�inen alin l�mp�tila -11 astetta.");
	require(story,"sv",fun,"Den l�gsta temperaturen i medeltal -11 grader.");
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

	NFmiSettings::instance().set("test::significantly_higher","6");
	NFmiSettings::instance().set("test::higher","4");
	NFmiSettings::instance().set("test::somewhat_higher","2");
	NFmiSettings::instance().set("test::somewhat_lower","2");
	NFmiSettings::instance().set("test::lower","4");
	NFmiSettings::instance().set("test::significantly_lower","6");
	NFmiSettings::instance().set("test::mininterval","2");
	NFmiSettings::instance().set("test::always_interval_zero","true");
	NFmiSettings::instance().set("test::starthour","6");
	NFmiSettings::instance().set("test::endhour","18");
	NFmiSettings::instance().set("test::prefer_daynames","false");

	// Test the part concerning day 1
	{
	  WeatherPeriod period(NFmiTime(2003,6,1), NFmiTime(2003,6,2));
	  TemperatureStory story(sources,area,period,"test");
	
	  NFmiSettings::instance().set("test::fake::day1::minimum","5,1");
	  NFmiSettings::instance().set("test::fake::day1::mean","5,1");
	  NFmiSettings::instance().set("test::fake::day1::maximum","5,1");
	  require(story,"fi",fun,"P�iv�n ylin l�mp�tila on sunnuntaina noin 5 astetta.");
	  require(story,"sv",fun,"Dagens h�gsta temperatur �r p� s�ndag cirka 5 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday about 5 degrees.");

	  NFmiSettings::instance().set("test::fake::day1::minimum","5,1");
	  NFmiSettings::instance().set("test::fake::day1::mean","6,1");
	  NFmiSettings::instance().set("test::fake::day1::maximum","6,1");
	  require(story,"fi",fun,"P�iv�n ylin l�mp�tila on sunnuntaina noin 6 astetta.");
	  require(story,"sv",fun,"Dagens h�gsta temperatur �r p� s�ndag cirka 6 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday about 6 degrees.");

	  NFmiSettings::instance().set("test::fake::day1::minimum","5,1");
	  NFmiSettings::instance().set("test::fake::day1::mean","6,1");
	  NFmiSettings::instance().set("test::fake::day1::maximum","7,1");
	  require(story,"fi",fun,"P�iv�n ylin l�mp�tila on sunnuntaina 5...7 astetta.");
	  require(story,"sv",fun,"Dagens h�gsta temperatur �r p� s�ndag 5...7 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees.");

	  NFmiSettings::instance().set("test::fake::day1::minimum","0,1");
	  NFmiSettings::instance().set("test::fake::day1::mean","0,1");
	  NFmiSettings::instance().set("test::fake::day1::maximum","0,1");
	  require(story,"fi",fun,"P�iv�n ylin l�mp�tila on sunnuntaina noin 0 astetta.");
	  require(story,"sv",fun,"Dagens h�gsta temperatur �r p� s�ndag cirka 0 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday about 0 degrees.");

	  NFmiSettings::instance().set("test::fake::day1::minimum","0,1");
	  NFmiSettings::instance().set("test::fake::day1::mean","0,1");
	  NFmiSettings::instance().set("test::fake::day1::maximum","1,1");
	  require(story,"fi",fun,"P�iv�n ylin l�mp�tila on sunnuntaina 0...1 astetta.");
	  require(story,"sv",fun,"Dagens h�gsta temperatur �r p� s�ndag 0...1 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 0...1 degrees.");

	  NFmiSettings::instance().set("test::fake::day1::minimum","-1,1");
	  NFmiSettings::instance().set("test::fake::day1::mean","0,1");
	  NFmiSettings::instance().set("test::fake::day1::maximum","0,1");
	  require(story,"fi",fun,"P�iv�n ylin l�mp�tila on sunnuntaina -1...0 astetta.");
	  require(story,"sv",fun,"Dagens h�gsta temperatur �r p� s�ndag -1...0 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday -1...0 degrees.");

	  NFmiSettings::instance().set("test::fake::day1::minimum","-1,1");
	  NFmiSettings::instance().set("test::fake::day1::mean","0,1");
	  NFmiSettings::instance().set("test::fake::day1::maximum","1,1");
	  require(story,"fi",fun,"P�iv�n ylin l�mp�tila on sunnuntaina -1...1 astetta.");
	  require(story,"sv",fun,"Dagens h�gsta temperatur �r p� s�ndag -1...1 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday -1...1 degrees.");

	}

	// Test the part concerning day 2
	{
	  WeatherPeriod period(NFmiTime(2003,6,1), NFmiTime(2003,6,3));
	  TemperatureStory story(sources,area,period,"test");
	
	  NFmiSettings::instance().set("test::fake::day1::minimum","5,1");
	  NFmiSettings::instance().set("test::fake::day1::mean","6,1");
	  NFmiSettings::instance().set("test::fake::day1::maximum","7,1");

	  // change 0 degrees
	  NFmiSettings::instance().set("test::fake::day2::minimum","5,1");
	  NFmiSettings::instance().set("test::fake::day2::mean","6,1");
	  NFmiSettings::instance().set("test::fake::day2::maximum","7,1");
	  require(story,"fi",fun,"P�iv�n ylin l�mp�tila on sunnuntaina 5...7 astetta, seuraavana p�iv�n� suunnilleen sama.");
	  require(story,"sv",fun,"Dagens h�gsta temperatur �r p� s�ndag 5...7 grader, p� f�ljande dag ungef�r densamma.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day about the same.");

	  // change 1 degrees
	  NFmiSettings::instance().set("test::fake::day2::minimum","6,1");
	  NFmiSettings::instance().set("test::fake::day2::mean","7,1");
	  NFmiSettings::instance().set("test::fake::day2::maximum","8,1");
	  require(story,"fi",fun,"P�iv�n ylin l�mp�tila on sunnuntaina 5...7 astetta, seuraavana p�iv�n� suunnilleen sama.");
	  require(story,"sv",fun,"Dagens h�gsta temperatur �r p� s�ndag 5...7 grader, p� f�ljande dag ungef�r densamma.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day about the same.");

	  // change 2 degrees
	  NFmiSettings::instance().set("test::fake::day2::minimum","7,1");
	  NFmiSettings::instance().set("test::fake::day2::mean","8,1");
	  NFmiSettings::instance().set("test::fake::day2::maximum","9,1");
	  require(story,"fi",fun,"P�iv�n ylin l�mp�tila on sunnuntaina 5...7 astetta, seuraavana p�iv�n� hieman korkeampi.");
	  require(story,"sv",fun,"Dagens h�gsta temperatur �r p� s�ndag 5...7 grader, p� f�ljande dag n�got h�gre.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day somewhat higher.");

	  // change 4 degrees
	  NFmiSettings::instance().set("test::fake::day2::minimum","9,1");
	  NFmiSettings::instance().set("test::fake::day2::mean","10,1");
	  NFmiSettings::instance().set("test::fake::day2::maximum","11,1");
	  require(story,"fi",fun,"P�iv�n ylin l�mp�tila on sunnuntaina 5...7 astetta, seuraavana p�iv�n� korkeampi.");
	  require(story,"sv",fun,"Dagens h�gsta temperatur �r p� s�ndag 5...7 grader, p� f�ljande dag h�gre.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day higher.");

	  // change 6 degrees
	  NFmiSettings::instance().set("test::fake::day2::minimum","11,1");
	  NFmiSettings::instance().set("test::fake::day2::mean","12,1");
	  NFmiSettings::instance().set("test::fake::day2::maximum","13,1");
	  require(story,"fi",fun,"P�iv�n ylin l�mp�tila on sunnuntaina 5...7 astetta, seuraavana p�iv�n� huomattavasti korkeampi.");
	  require(story,"sv",fun,"Dagens h�gsta temperatur �r p� s�ndag 5...7 grader, p� f�ljande dag betydligt h�gre.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day significantly higher.");

	  // change - 2 degrees
	  NFmiSettings::instance().set("test::fake::day2::minimum","3,1");
	  NFmiSettings::instance().set("test::fake::day2::mean","4,1");
	  NFmiSettings::instance().set("test::fake::day2::maximum","5,1");
	  require(story,"fi",fun,"P�iv�n ylin l�mp�tila on sunnuntaina 5...7 astetta, seuraavana p�iv�n� hieman alempi.");
	  require(story,"sv",fun,"Dagens h�gsta temperatur �r p� s�ndag 5...7 grader, p� f�ljande dag n�got l�gre.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day somewhat lower.");

	  // change -4 degrees
	  NFmiSettings::instance().set("test::fake::day2::minimum","1,1");
	  NFmiSettings::instance().set("test::fake::day2::mean","2,1");
	  NFmiSettings::instance().set("test::fake::day2::maximum","3,1");
	  require(story,"fi",fun,"P�iv�n ylin l�mp�tila on sunnuntaina 5...7 astetta, seuraavana p�iv�n� alempi.");
	  require(story,"sv",fun,"Dagens h�gsta temperatur �r p� s�ndag 5...7 grader, p� f�ljande dag l�gre.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day lower.");

	  // change -6 degrees
	  NFmiSettings::instance().set("test::fake::day2::minimum","-1,1");
	  NFmiSettings::instance().set("test::fake::day2::mean","0,1");
	  NFmiSettings::instance().set("test::fake::day2::maximum","1,1");
	  require(story,"fi",fun,"P�iv�n ylin l�mp�tila on sunnuntaina 5...7 astetta, seuraavana p�iv�n� huomattavasti alempi.");
	  require(story,"sv",fun,"Dagens h�gsta temperatur �r p� s�ndag 5...7 grader, p� f�ljande dag betydligt l�gre.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day significantly lower.");

	}

	// Test the part concerning days 3 and 4
	{
	  WeatherPeriod period(NFmiTime(2003,6,1), NFmiTime(2003,6,5));
	  TemperatureStory story(sources,area,period,"test");
	
	  NFmiSettings::instance().set("test::fake::day1::minimum","5,1");
	  NFmiSettings::instance().set("test::fake::day1::mean","6,1");
	  NFmiSettings::instance().set("test::fake::day1::maximum","7,1");

	  NFmiSettings::instance().set("test::fake::day2::minimum","5,1");
	  NFmiSettings::instance().set("test::fake::day2::mean","6,1");
	  NFmiSettings::instance().set("test::fake::day2::maximum","7,1");

	  NFmiSettings::instance().set("test::fake::day3::minimum","8,1");
	  NFmiSettings::instance().set("test::fake::day3::mean","10,1");
	  NFmiSettings::instance().set("test::fake::day3::maximum","12,1");

	  NFmiSettings::instance().set("test::fake::day4::minimum","3,1");
	  NFmiSettings::instance().set("test::fake::day4::mean","4,1");
	  NFmiSettings::instance().set("test::fake::day4::maximum","4,1");

	  require(story,"fi",fun,"P�iv�n ylin l�mp�tila on sunnuntaina 5...7 astetta, seuraavana p�iv�n� suunnilleen sama, tiistaina 8...12 astetta, keskiviikkona noin 4 astetta.");
	  require(story,"sv",fun,"Dagens h�gsta temperatur �r p� s�ndag 5...7 grader, p� f�ljande dag ungef�r densamma, p� tisdag 8...12 grader, p� onsdag cirka 4 grader.");
	  require(story,"en",fun,"The maximum day temperature is on Sunday 5...7 degrees, the following day about the same, on Tuesday 8...12 degrees, on Wednesday about 4 degrees.");

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
	  TEST(temperature_mean);
	  TEST(temperature_meanmax);
	  TEST(temperature_meanmin);
	  TEST(temperature_dailymax);
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

  dict->init("fi");
  dict->init("sv");
  dict->init("en");

  tests t;
  return t.run();
}
