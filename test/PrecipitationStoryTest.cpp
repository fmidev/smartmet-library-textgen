#include "regression/tframe.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "PlainTextFormatter.h"
#include "PrecipitationStory.h"
#include "Story.h"

#include "NFmiSettings.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;


namespace PrecipitationStoryTest
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

#define REQUIRE(story,lan,name,expected) \
result = require(story,lan,name,expected); \
if(!result.empty()) TEST_FAILED(result.c_str());

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PrecipitationStory::total()
   */
  // ----------------------------------------------------------------------

  void precipitation_total()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("dummy");
	NFmiTime time1(2000,1,1);
	NFmiTime time2(2000,1,2);
	WeatherPeriod period(time1,time2);
	PrecipitationStory story(time1,sources,area,period,"total");

	const string fun = "precipitation_total";
	string result;

	NFmiSettings::Set("total::fake::mean","0.1,0");
	REQUIRE(story,"fi",fun,"Sadesumma 0 millimetri�.");
	REQUIRE(story,"sv",fun,"Nederb�rdssumman 0 millimeter.");
	REQUIRE(story,"en",fun,"Total precipitation 0 millimeters.");

	NFmiSettings::Set("total::fake::mean","0.5,0");
	REQUIRE(story,"fi",fun,"Sadesumma 1 millimetri�.");
	REQUIRE(story,"sv",fun,"Nederb�rdssumman 1 millimeter.");
	REQUIRE(story,"en",fun,"Total precipitation 1 millimeters.");

	NFmiSettings::Set("total::fake::mean","10,0");
	REQUIRE(story,"fi",fun,"Sadesumma 10 millimetri�.");
	REQUIRE(story,"sv",fun,"Nederb�rdssumman 10 millimeter.");
	REQUIRE(story,"en",fun,"Total precipitation 10 millimeters.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PrecipitationStory::range()
   */
  // ----------------------------------------------------------------------

  void precipitation_range()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("dummy");
	NFmiTime time1(2000,1,1);
	NFmiTime time2(2000,1,2);
	WeatherPeriod period(time1,time2);
	PrecipitationStory story(time1,sources,area,period,"range");

	const string fun = "precipitation_range";
	string result;

	NFmiSettings::Set("range::maxrain","20");

	NFmiSettings::Set("range::fake::minimum","0,0");
	NFmiSettings::Set("range::fake::maximum","0,0");
	REQUIRE(story,"fi",fun,"Sadesumma on 0 millimetri�.");
	REQUIRE(story,"sv",fun,"Nederb�rdssumman �r 0 millimeter.");
	REQUIRE(story,"en",fun,"Total precipitation is 0 millimeters.");

	NFmiSettings::Set("range::fake::minimum","5,0");
	NFmiSettings::Set("range::fake::maximum","5,0");
	REQUIRE(story,"fi",fun,"Sadesumma on 5 millimetri�.");
	REQUIRE(story,"sv",fun,"Nederb�rdssumman �r 5 millimeter.");
	REQUIRE(story,"en",fun,"Total precipitation is 5 millimeters.");

	NFmiSettings::Set("range::fake::minimum","5,0");
	NFmiSettings::Set("range::fake::maximum","10,0");
	REQUIRE(story,"fi",fun,"Sadesumma on 5...10 millimetri�.");
	REQUIRE(story,"sv",fun,"Nederb�rdssumman �r 5...10 millimeter.");
	REQUIRE(story,"en",fun,"Total precipitation is 5...10 millimeters.");

	NFmiSettings::Set("range::fake::minimum","15,0");
	NFmiSettings::Set("range::fake::maximum","25,0");
	REQUIRE(story,"fi",fun,"Sadesumma on 15...25 millimetri�.");
	REQUIRE(story,"sv",fun,"Nederb�rdssumman �r 15...25 millimeter.");
	REQUIRE(story,"en",fun,"Total precipitation is 15...25 millimeters.");

	NFmiSettings::Set("range::fake::minimum","25,0");
	NFmiSettings::Set("range::fake::maximum","30,0");
	REQUIRE(story,"fi",fun,"Sadesumma on yli 20 millimetri�.");
	REQUIRE(story,"sv",fun,"Nederb�rdssumman �r �ver 20 millimeter.");
	REQUIRE(story,"en",fun,"Total precipitation is over 20 millimeters.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PrecipitationStory::sums()
   */
  // ----------------------------------------------------------------------

  void precipitation_sums()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("dummy");
	NFmiTime time1(2000,1,1);
	NFmiTime time2(2000,1,3);
	WeatherPeriod period(time1,time2);
	PrecipitationStory story(time1,sources,area,period,"sums");

	const string fun = "precipitation_sums";
	string result;

	NFmiSettings::Set("sums::mininterval","3");

	NFmiSettings::Set("sums::fake::period1::minimum","0,0");
	NFmiSettings::Set("sums::fake::period1::maximum","0,0");
	NFmiSettings::Set("sums::fake::period1::mean","0,0");
	NFmiSettings::Set("sums::fake::period2::minimum","0,0");
	NFmiSettings::Set("sums::fake::period2::maximum","0,0");
	NFmiSettings::Set("sums::fake::period2::mean","0,0");
	REQUIRE(story,"fi",fun,"Seuraavan 24 tunnin sadem��r� on 0 millimetri�.");
	REQUIRE(story,"sv",fun,"Nederb�rdssumman f�r de f�ljande 24 timmar �r 0 millimeter.");
	REQUIRE(story,"en",fun,"Total precipitation for the next 24 hours is 0 millimeters.");

	NFmiSettings::Set("sums::fake::period2::minimum","5,0");
	NFmiSettings::Set("sums::fake::period2::maximum","10,0");
	NFmiSettings::Set("sums::fake::period2::mean","8,0");
	REQUIRE(story,"fi",fun,"Ensimm�isen 12 tunnin sadem��r� on 0 millimetri�, seuraavan 12 tunnin 5...10 millimetri�.");
	REQUIRE(story,"sv",fun,"Nederb�rdssumman f�r de f�rsta 12 timmar �r 0 millimeter, f�r de f�ljande 12 timmar 5...10 millimeter.");
	REQUIRE(story,"en",fun,"Total precipitation for the first 12 hours is 0 millimeters, for the following 12 hours 5...10 millimeters.");

	NFmiSettings::Set("sums::fake::period1::minimum","0,0");
	NFmiSettings::Set("sums::fake::period1::maximum","2,0");
	NFmiSettings::Set("sums::fake::period1::mean","1,0");
	REQUIRE(story,"fi",fun,"Ensimm�isen 12 tunnin sadem��r� on noin 1 millimetri�, seuraavan 12 tunnin 5...10 millimetri�.");
	REQUIRE(story,"sv",fun,"Nederb�rdssumman f�r de f�rsta 12 timmar �r cirka 1 millimeter, f�r de f�ljande 12 timmar 5...10 millimeter.");
	REQUIRE(story,"en",fun,"Total precipitation for the first 12 hours is about 1 millimeters, for the following 12 hours 5...10 millimeters.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PrecipitationStory::classification()
   */
  // ----------------------------------------------------------------------

  void precipitation_classification()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("dummy");
	NFmiTime time1(2000,1,1);
	NFmiTime time2(2000,1,2);
	WeatherPeriod period(time1,time2);
	PrecipitationStory story(time1,sources,area,period,"classification");

	string result;

	const string fun = "precipitation_classification";
	NFmiSettings::Set("classification::classes","0...2,2...5,5...10,10...20");
	NFmiSettings::Set("classification::max_some_places","30");
	NFmiSettings::Set("classification::max_many_places","70");
	NFmiSettings::Set("classification::some_places","30");
	NFmiSettings::Set("classification::many_places","70");

	NFmiSettings::Set("classification::fake::minimum","0,0");
	NFmiSettings::Set("classification::fake::mean","0,0");
	NFmiSettings::Set("classification::fake::maximum","0,0");
	NFmiSettings::Set("classification::fake::percentage","0,0");
	REQUIRE(story,"fi",fun,"Sadesumma 0 millimetri�.");
	REQUIRE(story,"sv",fun,"Nederb�rdssumman 0 millimeter.");
	REQUIRE(story,"en",fun,"Total precipitation 0 millimeters.");

	NFmiSettings::Set("classification::fake::minimum","0,0");
	NFmiSettings::Set("classification::fake::mean","1,0");
	NFmiSettings::Set("classification::fake::maximum","2,0");
	NFmiSettings::Set("classification::fake::percentage","0,0");
	REQUIRE(story,"fi",fun,"Sadesumma 0...2 millimetri�.");
	REQUIRE(story,"sv",fun,"Nederb�rdssumman 0...2 millimeter.");
	REQUIRE(story,"en",fun,"Total precipitation 0...2 millimeters.");

	NFmiSettings::Set("classification::fake::minimum","0,0");
	NFmiSettings::Set("classification::fake::mean","4,0");
	NFmiSettings::Set("classification::fake::maximum","6,0");
	NFmiSettings::Set("classification::fake::percentage","0,0");
	REQUIRE(story,"fi",fun,"Sadesumma 2...5 millimetri�.");
	REQUIRE(story,"sv",fun,"Nederb�rdssumman 2...5 millimeter.");
	REQUIRE(story,"en",fun,"Total precipitation 2...5 millimeters.");

	NFmiSettings::Set("classification::fake::minimum","0,0");
	NFmiSettings::Set("classification::fake::mean","4,0");
	NFmiSettings::Set("classification::fake::maximum","6,0");
	NFmiSettings::Set("classification::fake::percentage","40,0");
	REQUIRE(story,"fi",fun,"Sadesumma 2...5 millimetri�, paikoin enemm�n.");
	REQUIRE(story,"sv",fun,"Nederb�rdssumman 2...5 millimeter, lokalt mera.");
	REQUIRE(story,"en",fun,"Total precipitation 2...5 millimeters, more in some places.");

	NFmiSettings::Set("classification::fake::minimum","0,0");
	NFmiSettings::Set("classification::fake::mean","4,0");
	NFmiSettings::Set("classification::fake::maximum","6,0");
	NFmiSettings::Set("classification::fake::percentage","80,0");
	REQUIRE(story,"fi",fun,"Sadesumma 2...5 millimetri�, monin paikoin enemm�n.");
	REQUIRE(story,"sv",fun,"Nederb�rdssumman 2...5 millimeter, flerst�des mera.");
	REQUIRE(story,"en",fun,"Total precipitation 2...5 millimeters, more in many places.");

	NFmiSettings::Set("classification::fake::minimum","0,0");
	NFmiSettings::Set("classification::fake::mean","15,0");
	NFmiSettings::Set("classification::fake::maximum","20,0");
	NFmiSettings::Set("classification::fake::percentage","0,0");
	REQUIRE(story,"fi",fun,"Sadesumma 10...20 millimetri�.");
	REQUIRE(story,"sv",fun,"Nederb�rdssumman 10...20 millimeter.");
	REQUIRE(story,"en",fun,"Total precipitation 10...20 millimeters.");

	NFmiSettings::Set("classification::fake::minimum","0,0");
	NFmiSettings::Set("classification::fake::mean","15,0");
	NFmiSettings::Set("classification::fake::maximum","25,0");
	NFmiSettings::Set("classification::fake::percentage","10,0");
	REQUIRE(story,"fi",fun,"Sadesumma 10...20 millimetri�.");
	REQUIRE(story,"sv",fun,"Nederb�rdssumman 10...20 millimeter.");
	REQUIRE(story,"en",fun,"Total precipitation 10...20 millimeters.");

	NFmiSettings::Set("classification::fake::minimum","0,0");
	NFmiSettings::Set("classification::fake::mean","15,0");
	NFmiSettings::Set("classification::fake::maximum","25,0");
	NFmiSettings::Set("classification::fake::percentage","40,0");
	REQUIRE(story,"fi",fun,"Sadesumma paikoin yli 20 millimetri�.");
	REQUIRE(story,"sv",fun,"Nederb�rdssumman lokalt �ver 20 millimeter.");
	REQUIRE(story,"en",fun,"Total precipitation in some places over 20 millimeters.");

	NFmiSettings::Set("classification::fake::minimum","0,0");
	NFmiSettings::Set("classification::fake::mean","15,0");
	NFmiSettings::Set("classification::fake::maximum","25,0");
	NFmiSettings::Set("classification::fake::percentage","80,0");
	REQUIRE(story,"fi",fun,"Sadesumma monin paikoin yli 20 millimetri�.");
	REQUIRE(story,"sv",fun,"Nederb�rdssumman flerst�des �ver 20 millimeter.");
	REQUIRE(story,"en",fun,"Total precipitation in many places over 20 millimeters.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PrecipitationStory::pop_twodays
   */
  // ----------------------------------------------------------------------

  void pop_twodays()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("dummy");

	const string fun = "pop_twodays";

	string result;

	NFmiSettings::Set("pop_twodays::day::starthour","6");
	NFmiSettings::Set("pop_twodays::day::endhour","18");
	NFmiSettings::Set("pop_twodays::precision","10");
	NFmiSettings::Set("pop_twodays::limit","10");
	NFmiSettings::Set("pop_twodays::comparison::significantly_greater","50");
	NFmiSettings::Set("pop_twodays::comparison::greater","30");
	NFmiSettings::Set("pop_twodays::comparison::somewhat_greater","10");
	NFmiSettings::Set("pop_twodays::comparison::somewhat_smaller","10");
	NFmiSettings::Set("pop_twodays::comparison::smaller","30");
	NFmiSettings::Set("pop_twodays::comparison::significantly_smaller","50");

	// 1-day forecasts
	{
	  NFmiTime time1(2003,6,3,6,0);
	  NFmiTime time2(2003,6,4,6,0);
	  WeatherPeriod period(time1,time2);
	  PrecipitationStory story(time1,sources,area,period,"pop_twodays");
	  
	  NFmiSettings::Set("pop_twodays::fake::day1::maximum","10,0");
	  REQUIRE(story,"fi",fun,"Sateen todenn�k�isyys on 10%.");
	  REQUIRE(story,"sv",fun,"Sannolikheten f�r nederb�rd �r 10%.");
	  REQUIRE(story,"en",fun,"Probability of precipitation is 10%.");
	  
	}

	// Another 1-day forecast, because 17 < 18 (minendhour)

	{
	  NFmiTime time1(2003,6,3,6,0);
	  NFmiTime time2(2003,6,4,17,0);
	  WeatherPeriod period(time1,time2);
	  PrecipitationStory story(time1,sources,area,period,"pop_twodays");
	  
	  NFmiSettings::Set("pop_twodays::fake::day1::maximum","20,0");
	  REQUIRE(story,"fi",fun,"Sateen todenn�k�isyys on 20%.");
	  REQUIRE(story,"sv",fun,"Sannolikheten f�r nederb�rd �r 20%.");
	  REQUIRE(story,"en",fun,"Probability of precipitation is 20%.");
	  
	}

	// Another 1-day forecast with small PoP

	{
	  NFmiTime time1(2003,6,3,6,0);
	  NFmiTime time2(2003,6,4,6,0);
	  WeatherPeriod period(time1,time2);
	  PrecipitationStory story(time1,sources,area,period,"pop_twodays");
	  
	  NFmiSettings::Set("pop_twodays::fake::day1::maximum","0,0");
	  REQUIRE(story,"fi",fun,"");
	  REQUIRE(story,"sv",fun,"");
	  REQUIRE(story,"en",fun,"");
	  
	}

	// 2-day forecasts

	{
	  NFmiTime time1(2003,6,3,6,0);
	  NFmiTime time2(2003,6,4,18,0);
	  WeatherPeriod period(time1,time2);
	  PrecipitationStory story(time1,sources,area,period,"pop_twodays");
	  
	  NFmiSettings::Set("pop_twodays::fake::day1::maximum","50,0");
	  NFmiSettings::Set("pop_twodays::fake::day2::maximum","50,0");
	  REQUIRE(story,"fi",fun,"Sateen todenn�k�isyys on 50%, huomenna sama.");
	  REQUIRE(story,"sv",fun,"Sannolikheten f�r nederb�rd �r 50%, i morgon densamma.");
	  REQUIRE(story,"en",fun,"Probability of precipitation is 50%, tomorrow the same.");

	  NFmiSettings::Set("pop_twodays::fake::day1::maximum","60,0");
	  NFmiSettings::Set("pop_twodays::fake::day2::maximum","10,0");
	  REQUIRE(story,"fi",fun,"Sateen todenn�k�isyys on 60%, huomenna huomattavasti pienempi.");
	  REQUIRE(story,"sv",fun,"Sannolikheten f�r nederb�rd �r 60%, i morgon betydligt mindre.");
	  REQUIRE(story,"en",fun,"Probability of precipitation is 60%, tomorrow significantly smaller.");

	  NFmiSettings::Set("pop_twodays::fake::day1::maximum","50,0");
	  NFmiSettings::Set("pop_twodays::fake::day2::maximum","20,0");
	  REQUIRE(story,"fi",fun,"Sateen todenn�k�isyys on 50%, huomenna pienempi.");
	  REQUIRE(story,"sv",fun,"Sannolikheten f�r nederb�rd �r 50%, i morgon mindre.");
	  REQUIRE(story,"en",fun,"Probability of precipitation is 50%, tomorrow smaller.");

	  NFmiSettings::Set("pop_twodays::fake::day1::maximum","50,0");
	  NFmiSettings::Set("pop_twodays::fake::day2::maximum","40,0");
	  REQUIRE(story,"fi",fun,"Sateen todenn�k�isyys on 50%, huomenna hieman pienempi.");
	  REQUIRE(story,"sv",fun,"Sannolikheten f�r nederb�rd �r 50%, i morgon n�got mindre.");
	  REQUIRE(story,"en",fun,"Probability of precipitation is 50%, tomorrow somewhat smaller.");

	  NFmiSettings::Set("pop_twodays::fake::day1::maximum","50,0");
	  NFmiSettings::Set("pop_twodays::fake::day2::maximum","60,0");
	  REQUIRE(story,"fi",fun,"Sateen todenn�k�isyys on 50%, huomenna hieman suurempi.");
	  REQUIRE(story,"sv",fun,"Sannolikheten f�r nederb�rd �r 50%, i morgon n�got st�rre.");
	  REQUIRE(story,"en",fun,"Probability of precipitation is 50%, tomorrow somewhat greater.");

	  NFmiSettings::Set("pop_twodays::fake::day1::maximum","50,0");
	  NFmiSettings::Set("pop_twodays::fake::day2::maximum","80,0");
	  REQUIRE(story,"fi",fun,"Sateen todenn�k�isyys on 50%, huomenna suurempi.");
	  REQUIRE(story,"sv",fun,"Sannolikheten f�r nederb�rd �r 50%, i morgon st�rre.");
	  REQUIRE(story,"en",fun,"Probability of precipitation is 50%, tomorrow greater.");

	  NFmiSettings::Set("pop_twodays::fake::day1::maximum","50,0");
	  NFmiSettings::Set("pop_twodays::fake::day2::maximum","100,0");
	  REQUIRE(story,"fi",fun,"Sateen todenn�k�isyys on 50%, huomenna huomattavasti suurempi.");
	  REQUIRE(story,"sv",fun,"Sannolikheten f�r nederb�rd �r 50%, i morgon betydligt st�rre.");
	  REQUIRE(story,"en",fun,"Probability of precipitation is 50%, tomorrow significantly greater.");


	  NFmiSettings::Set("pop_twodays::fake::day1::maximum","50,0");
	  NFmiSettings::Set("pop_twodays::fake::day2::maximum","0,0");
	  REQUIRE(story,"fi",fun,"Sateen todenn�k�isyys on 50%.");
	  REQUIRE(story,"sv",fun,"Sannolikheten f�r nederb�rd �r 50%.");
	  REQUIRE(story,"en",fun,"Probability of precipitation is 50%.");

	  NFmiSettings::Set("pop_twodays::fake::day1::maximum","0,0");
	  NFmiSettings::Set("pop_twodays::fake::day2::maximum","50,0");
	  REQUIRE(story,"fi",fun,"Sateen todenn�k�isyys on huomenna 50%.");
	  REQUIRE(story,"sv",fun,"Sannolikheten f�r nederb�rd �r i morgon 50%.");
	  REQUIRE(story,"en",fun,"Probability of precipitation is tomorrow 50%.");

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
	  TEST(precipitation_total);
	  TEST(precipitation_range);
	  TEST(precipitation_classification);
	  TEST(precipitation_sums);
	  TEST(pop_twodays);
	}

  }; // class tests

} // namespace PrecipitationStoryTest


int main(void)
{
  using namespace PrecipitationStoryTest;

  cout << endl
	   << "PrecipitationStory tests" << endl
	   << "========================" << endl;

  NFmiSettings::Init();
  NFmiSettings::Set("textgen::units::millimeters::format","phrase");

  dict = TextGen::DictionaryFactory::create("multimysql");

  dict->init("fi");
  dict->init("sv");
  dict->init("en");

  tests t;
  return t.run();
}
