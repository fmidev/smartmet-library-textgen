#include "tframe.h"
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
	PrecipitationStory story(time1,sources,area,period,"test");

	const string fun = "precipitation_total";

	NFmiSettings::instance().set("test::fake::mean","0.1,1");
	require(story,"fi",fun,"Sadesumma 0 millimetriä.");
	require(story,"sv",fun,"Nederbördsumman 0 millimeter.");
	require(story,"en",fun,"Total precipitation 0 millimeters.");

	NFmiSettings::instance().set("test::fake::mean","0.5,1");
	require(story,"fi",fun,"Sadesumma 1 millimetriä.");
	require(story,"sv",fun,"Nederbördsumman 1 millimeter.");
	require(story,"en",fun,"Total precipitation 1 millimeters.");

	NFmiSettings::instance().set("test::fake::mean","10,1");
	require(story,"fi",fun,"Sadesumma 10 millimetriä.");
	require(story,"sv",fun,"Nederbördsumman 10 millimeter.");
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
	PrecipitationStory story(time1,sources,area,period,"test");

	const string fun = "precipitation_range";

	NFmiSettings::instance().set("test::maxrain","20");

	NFmiSettings::instance().set("test::fake::minimum","0,1");
	NFmiSettings::instance().set("test::fake::maximum","0,1");
	require(story,"fi",fun,"Sadesumma 0 millimetriä.");
	require(story,"sv",fun,"Nederbördsumman 0 millimeter.");
	require(story,"en",fun,"Total precipitation 0 millimeters.");

	NFmiSettings::instance().set("test::fake::minimum","5,1");
	NFmiSettings::instance().set("test::fake::maximum","5,1");
	require(story,"fi",fun,"Sadesumma 5 millimetriä.");
	require(story,"sv",fun,"Nederbördsumman 5 millimeter.");
	require(story,"en",fun,"Total precipitation 5 millimeters.");

	NFmiSettings::instance().set("test::fake::minimum","5,1");
	NFmiSettings::instance().set("test::fake::maximum","10,1");
	require(story,"fi",fun,"Sadesumma 5...10 millimetriä.");
	require(story,"sv",fun,"Nederbördsumman 5...10 millimeter.");
	require(story,"en",fun,"Total precipitation 5...10 millimeters.");

	NFmiSettings::instance().set("test::fake::minimum","15,1");
	NFmiSettings::instance().set("test::fake::maximum","25,1");
	require(story,"fi",fun,"Sadesumma 15...25 millimetriä.");
	require(story,"sv",fun,"Nederbördsumman 15...25 millimeter.");
	require(story,"en",fun,"Total precipitation 15...25 millimeters.");

	NFmiSettings::instance().set("test::fake::minimum","25,1");
	NFmiSettings::instance().set("test::fake::maximum","30,1");
	require(story,"fi",fun,"Sadesumma yli 20 millimetriä.");
	require(story,"sv",fun,"Nederbördsumman över 20 millimeter.");
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
	PrecipitationStory story(time1,sources,area,period,"test");

	const string fun = "precipitation_classification";
	NFmiSettings::instance().set("test::classes","0...2,2...5,5...10,10...20");
	NFmiSettings::instance().set("test::maxlimit1","30");
	NFmiSettings::instance().set("test::maxlimit2","70");
	NFmiSettings::instance().set("test::limit1","30");
	NFmiSettings::instance().set("test::limit2","70");

	NFmiSettings::instance().set("test::fake::minimum","0,1");
	NFmiSettings::instance().set("test::fake::mean","0,1");
	NFmiSettings::instance().set("test::fake::maximum","0,1");
	NFmiSettings::instance().set("test::fake::percentage","0,1");
	require(story,"fi",fun,"Sadesumma 0 millimetriä.");
	require(story,"sv",fun,"Nederbördsumman 0 millimeter.");
	require(story,"en",fun,"Total precipitation 0 millimeters.");

	NFmiSettings::instance().set("test::fake::minimum","0,1");
	NFmiSettings::instance().set("test::fake::mean","1,1");
	NFmiSettings::instance().set("test::fake::maximum","2,1");
	NFmiSettings::instance().set("test::fake::percentage","0,1");
	require(story,"fi",fun,"Sadesumma 0...2 millimetriä.");
	require(story,"sv",fun,"Nederbördsumman 0...2 millimeter.");
	require(story,"en",fun,"Total precipitation 0...2 millimeters.");

	NFmiSettings::instance().set("test::fake::minimum","0,1");
	NFmiSettings::instance().set("test::fake::mean","4,1");
	NFmiSettings::instance().set("test::fake::maximum","6,1");
	NFmiSettings::instance().set("test::fake::percentage","0,1");
	require(story,"fi",fun,"Sadesumma 2...5 millimetriä.");
	require(story,"sv",fun,"Nederbördsumman 2...5 millimeter.");
	require(story,"en",fun,"Total precipitation 2...5 millimeters.");

	NFmiSettings::instance().set("test::fake::minimum","0,1");
	NFmiSettings::instance().set("test::fake::mean","4,1");
	NFmiSettings::instance().set("test::fake::maximum","6,1");
	NFmiSettings::instance().set("test::fake::percentage","40,1");
	require(story,"fi",fun,"Sadesumma 2...5 millimetriä, paikoin enemmän.");
	require(story,"sv",fun,"Nederbördsumman 2...5 millimeter, lokalt mera.");
	require(story,"en",fun,"Total precipitation 2...5 millimeters, more in some places.");

	NFmiSettings::instance().set("test::fake::minimum","0,1");
	NFmiSettings::instance().set("test::fake::mean","4,1");
	NFmiSettings::instance().set("test::fake::maximum","6,1");
	NFmiSettings::instance().set("test::fake::percentage","80,1");
	require(story,"fi",fun,"Sadesumma 2...5 millimetriä, monin paikoin enemmän.");
	require(story,"sv",fun,"Nederbördsumman 2...5 millimeter, på många håll mera.");
	require(story,"en",fun,"Total precipitation 2...5 millimeters, more in many places.");

	NFmiSettings::instance().set("test::fake::minimum","0,1");
	NFmiSettings::instance().set("test::fake::mean","15,1");
	NFmiSettings::instance().set("test::fake::maximum","20,1");
	NFmiSettings::instance().set("test::fake::percentage","0,1");
	require(story,"fi",fun,"Sadesumma 10...20 millimetriä.");
	require(story,"sv",fun,"Nederbördsumman 10...20 millimeter.");
	require(story,"en",fun,"Total precipitation 10...20 millimeters.");

	NFmiSettings::instance().set("test::fake::minimum","0,1");
	NFmiSettings::instance().set("test::fake::mean","15,1");
	NFmiSettings::instance().set("test::fake::maximum","25,1");
	NFmiSettings::instance().set("test::fake::percentage","10,1");
	require(story,"fi",fun,"Sadesumma 10...20 millimetriä.");
	require(story,"sv",fun,"Nederbördsumman 10...20 millimeter.");
	require(story,"en",fun,"Total precipitation 10...20 millimeters.");

	NFmiSettings::instance().set("test::fake::minimum","0,1");
	NFmiSettings::instance().set("test::fake::mean","15,1");
	NFmiSettings::instance().set("test::fake::maximum","25,1");
	NFmiSettings::instance().set("test::fake::percentage","40,1");
	require(story,"fi",fun,"Sadesumma paikoin yli 20 millimetriä.");
	require(story,"sv",fun,"Nederbördsumman lokalt över 20 millimeter.");
	require(story,"en",fun,"Total precipitation in some places over 20 millimeters.");

	NFmiSettings::instance().set("test::fake::minimum","0,1");
	NFmiSettings::instance().set("test::fake::mean","15,1");
	NFmiSettings::instance().set("test::fake::maximum","25,1");
	NFmiSettings::instance().set("test::fake::percentage","80,1");
	require(story,"fi",fun,"Sadesumma monin paikoin yli 20 millimetriä.");
	require(story,"sv",fun,"Nederbördsumman på många håll över 20 millimeter.");
	require(story,"en",fun,"Total precipitation in many places over 20 millimeters.");

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
	}

  }; // class tests

} // namespace PrecipitationStoryTest


int main(void)
{
  using namespace PrecipitationStoryTest;

  cout << endl
	   << "PrecipitationStory tests" << endl
	   << "========================" << endl;

  dict = TextGen::DictionaryFactory::create("multimysql");

  dict->init("fi");
  dict->init("sv");
  dict->init("en");

  tests t;
  return t.run();
}
