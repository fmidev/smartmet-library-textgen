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

	NFmiSettings::Set("total::fake::mean","0.1,0");
	require(story,"fi",fun,"Sadesumma 0 millimetriä.");
	require(story,"sv",fun,"Nederbördssumman 0 millimeter.");
	require(story,"en",fun,"Total precipitation 0 millimeters.");

	NFmiSettings::Set("total::fake::mean","0.5,0");
	require(story,"fi",fun,"Sadesumma 1 millimetriä.");
	require(story,"sv",fun,"Nederbördssumman 1 millimeter.");
	require(story,"en",fun,"Total precipitation 1 millimeters.");

	NFmiSettings::Set("total::fake::mean","10,0");
	require(story,"fi",fun,"Sadesumma 10 millimetriä.");
	require(story,"sv",fun,"Nederbördssumman 10 millimeter.");
	require(story,"en",fun,"Total precipitation 10 millimeters.");

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

	NFmiSettings::Set("range::maxrain","20");

	NFmiSettings::Set("range::fake::minimum","0,0");
	NFmiSettings::Set("range::fake::maximum","0,0");
	require(story,"fi",fun,"Sadesumma 0 millimetriä.");
	require(story,"sv",fun,"Nederbördssumman 0 millimeter.");
	require(story,"en",fun,"Total precipitation 0 millimeters.");

	NFmiSettings::Set("range::fake::minimum","5,0");
	NFmiSettings::Set("range::fake::maximum","5,0");
	require(story,"fi",fun,"Sadesumma 5 millimetriä.");
	require(story,"sv",fun,"Nederbördssumman 5 millimeter.");
	require(story,"en",fun,"Total precipitation 5 millimeters.");

	NFmiSettings::Set("range::fake::minimum","5,0");
	NFmiSettings::Set("range::fake::maximum","10,0");
	require(story,"fi",fun,"Sadesumma 5...10 millimetriä.");
	require(story,"sv",fun,"Nederbördssumman 5...10 millimeter.");
	require(story,"en",fun,"Total precipitation 5...10 millimeters.");

	NFmiSettings::Set("range::fake::minimum","15,0");
	NFmiSettings::Set("range::fake::maximum","25,0");
	require(story,"fi",fun,"Sadesumma 15...25 millimetriä.");
	require(story,"sv",fun,"Nederbördssumman 15...25 millimeter.");
	require(story,"en",fun,"Total precipitation 15...25 millimeters.");

	NFmiSettings::Set("range::fake::minimum","25,0");
	NFmiSettings::Set("range::fake::maximum","30,0");
	require(story,"fi",fun,"Sadesumma yli 20 millimetriä.");
	require(story,"sv",fun,"Nederbördssumman över 20 millimeter.");
	require(story,"en",fun,"Total precipitation over 20 millimeters.");

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
	require(story,"fi",fun,"Sadesumma 0 millimetriä.");
	require(story,"sv",fun,"Nederbördssumman 0 millimeter.");
	require(story,"en",fun,"Total precipitation 0 millimeters.");

	NFmiSettings::Set("classification::fake::minimum","0,0");
	NFmiSettings::Set("classification::fake::mean","1,0");
	NFmiSettings::Set("classification::fake::maximum","2,0");
	NFmiSettings::Set("classification::fake::percentage","0,0");
	require(story,"fi",fun,"Sadesumma 0...2 millimetriä.");
	require(story,"sv",fun,"Nederbördssumman 0...2 millimeter.");
	require(story,"en",fun,"Total precipitation 0...2 millimeters.");

	NFmiSettings::Set("classification::fake::minimum","0,0");
	NFmiSettings::Set("classification::fake::mean","4,0");
	NFmiSettings::Set("classification::fake::maximum","6,0");
	NFmiSettings::Set("classification::fake::percentage","0,0");
	require(story,"fi",fun,"Sadesumma 2...5 millimetriä.");
	require(story,"sv",fun,"Nederbördssumman 2...5 millimeter.");
	require(story,"en",fun,"Total precipitation 2...5 millimeters.");

	NFmiSettings::Set("classification::fake::minimum","0,0");
	NFmiSettings::Set("classification::fake::mean","4,0");
	NFmiSettings::Set("classification::fake::maximum","6,0");
	NFmiSettings::Set("classification::fake::percentage","40,0");
	require(story,"fi",fun,"Sadesumma 2...5 millimetriä, paikoin enemmän.");
	require(story,"sv",fun,"Nederbördssumman 2...5 millimeter, lokalt mera.");
	require(story,"en",fun,"Total precipitation 2...5 millimeters, more in some places.");

	NFmiSettings::Set("classification::fake::minimum","0,0");
	NFmiSettings::Set("classification::fake::mean","4,0");
	NFmiSettings::Set("classification::fake::maximum","6,0");
	NFmiSettings::Set("classification::fake::percentage","80,0");
	require(story,"fi",fun,"Sadesumma 2...5 millimetriä, monin paikoin enemmän.");
	require(story,"sv",fun,"Nederbördssumman 2...5 millimeter, flerstädes mera.");
	require(story,"en",fun,"Total precipitation 2...5 millimeters, more in many places.");

	NFmiSettings::Set("classification::fake::minimum","0,0");
	NFmiSettings::Set("classification::fake::mean","15,0");
	NFmiSettings::Set("classification::fake::maximum","20,0");
	NFmiSettings::Set("classification::fake::percentage","0,0");
	require(story,"fi",fun,"Sadesumma 10...20 millimetriä.");
	require(story,"sv",fun,"Nederbördssumman 10...20 millimeter.");
	require(story,"en",fun,"Total precipitation 10...20 millimeters.");

	NFmiSettings::Set("classification::fake::minimum","0,0");
	NFmiSettings::Set("classification::fake::mean","15,0");
	NFmiSettings::Set("classification::fake::maximum","25,0");
	NFmiSettings::Set("classification::fake::percentage","10,0");
	require(story,"fi",fun,"Sadesumma 10...20 millimetriä.");
	require(story,"sv",fun,"Nederbördssumman 10...20 millimeter.");
	require(story,"en",fun,"Total precipitation 10...20 millimeters.");

	NFmiSettings::Set("classification::fake::minimum","0,0");
	NFmiSettings::Set("classification::fake::mean","15,0");
	NFmiSettings::Set("classification::fake::maximum","25,0");
	NFmiSettings::Set("classification::fake::percentage","40,0");
	require(story,"fi",fun,"Sadesumma paikoin yli 20 millimetriä.");
	require(story,"sv",fun,"Nederbördssumman lokalt över 20 millimeter.");
	require(story,"en",fun,"Total precipitation in some places over 20 millimeters.");

	NFmiSettings::Set("classification::fake::minimum","0,0");
	NFmiSettings::Set("classification::fake::mean","15,0");
	NFmiSettings::Set("classification::fake::maximum","25,0");
	NFmiSettings::Set("classification::fake::percentage","80,0");
	require(story,"fi",fun,"Sadesumma monin paikoin yli 20 millimetriä.");
	require(story,"sv",fun,"Nederbördssumman flerstädes över 20 millimeter.");
	require(story,"en",fun,"Total precipitation in many places over 20 millimeters.");

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

	NFmiSettings::Set("pop_twodays::day::maxstarthour","6");
	NFmiSettings::Set("pop_twodays::day::minendhour","18");
	NFmiSettings::Set("pop_twodays::precision","10");
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
	  require(story,"fi",fun,"Sateen todennäköisyys on 10%.");
	  require(story,"sv",fun,"Sannolikheten för nederbörd är 10%.");
	  require(story,"en",fun,"Probability of precipitation is 10%.");
	  
	}

	// Another 1-day forecast, because 17 < 18 (minendhour)

	{
	  NFmiTime time1(2003,6,3,6,0);
	  NFmiTime time2(2003,6,4,17,0);
	  WeatherPeriod period(time1,time2);
	  PrecipitationStory story(time1,sources,area,period,"pop_twodays");
	  
	  NFmiSettings::Set("pop_twodays::fake::day1::maximum","20,0");
	  require(story,"fi",fun,"Sateen todennäköisyys on 20%.");
	  require(story,"sv",fun,"Sannolikheten för nederbörd är 20%.");
	  require(story,"en",fun,"Probability of precipitation is 20%.");
	  
	}

	// 2-day forecasts

	{
	  NFmiTime time1(2003,6,3,6,0);
	  NFmiTime time2(2003,6,4,18,0);
	  WeatherPeriod period(time1,time2);
	  PrecipitationStory story(time1,sources,area,period,"pop_twodays");
	  
	  NFmiSettings::Set("pop_twodays::fake::day1::maximum","50,0");
	  NFmiSettings::Set("pop_twodays::fake::day2::maximum","50,0");
	  require(story,"fi",fun,"Sateen todennäköisyys on 50%, huomenna sama.");
	  require(story,"sv",fun,"Sannolikheten för nederbörd är 50%, i morgon densamma.");
	  require(story,"en",fun,"Probability of precipitation is 50%, tomorrow the same.");

	  NFmiSettings::Set("pop_twodays::fake::day1::maximum","50,0");
	  NFmiSettings::Set("pop_twodays::fake::day2::maximum","0,0");
	  require(story,"fi",fun,"Sateen todennäköisyys on 50%, huomenna huomattavasti pienempi.");
	  require(story,"sv",fun,"Sannolikheten för nederbörd är 50%, i morgon betydligt mindre.");
	  require(story,"en",fun,"Probability of precipitation is 50%, tomorrow significantly smaller.");

	  NFmiSettings::Set("pop_twodays::fake::day1::maximum","50,0");
	  NFmiSettings::Set("pop_twodays::fake::day2::maximum","20,0");
	  require(story,"fi",fun,"Sateen todennäköisyys on 50%, huomenna pienempi.");
	  require(story,"sv",fun,"Sannolikheten för nederbörd är 50%, i morgon mindre.");
	  require(story,"en",fun,"Probability of precipitation is 50%, tomorrow smaller.");

	  NFmiSettings::Set("pop_twodays::fake::day1::maximum","50,0");
	  NFmiSettings::Set("pop_twodays::fake::day2::maximum","40,0");
	  require(story,"fi",fun,"Sateen todennäköisyys on 50%, huomenna hieman pienempi.");
	  require(story,"sv",fun,"Sannolikheten för nederbörd är 50%, i morgon något mindre.");
	  require(story,"en",fun,"Probability of precipitation is 50%, tomorrow somewhat smaller.");

	  NFmiSettings::Set("pop_twodays::fake::day1::maximum","50,0");
	  NFmiSettings::Set("pop_twodays::fake::day2::maximum","60,0");
	  require(story,"fi",fun,"Sateen todennäköisyys on 50%, huomenna hieman suurempi.");
	  require(story,"sv",fun,"Sannolikheten för nederbörd är 50%, i morgon något större.");
	  require(story,"en",fun,"Probability of precipitation is 50%, tomorrow somewhat greater.");

	  NFmiSettings::Set("pop_twodays::fake::day1::maximum","50,0");
	  NFmiSettings::Set("pop_twodays::fake::day2::maximum","80,0");
	  require(story,"fi",fun,"Sateen todennäköisyys on 50%, huomenna suurempi.");
	  require(story,"sv",fun,"Sannolikheten för nederbörd är 50%, i morgon större.");
	  require(story,"en",fun,"Probability of precipitation is 50%, tomorrow greater.");

	  NFmiSettings::Set("pop_twodays::fake::day1::maximum","50,0");
	  NFmiSettings::Set("pop_twodays::fake::day2::maximum","100,0");
	  require(story,"fi",fun,"Sateen todennäköisyys on 50%, huomenna huomattavasti suurempi.");
	  require(story,"sv",fun,"Sannolikheten för nederbörd är 50%, i morgon betydligt större.");
	  require(story,"en",fun,"Probability of precipitation is 50%, tomorrow significantly greater.");
	  
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
