#include "tframe.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "PlainTextFormatter.h"
#include "RelativeHumidityStory.h"
#include "Story.h"

#include "NFmiSettings.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;


namespace RelativeHumidityStoryTest
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
   * \brief Test RelativeHumidityStory::lowest()
   */
  // ----------------------------------------------------------------------

  void relativehumidity_lowest()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("dummy");

	const string fun = "relativehumidity_lowest";

	NFmiSettings::instance().set("lowest::maxstarthour","6");
	NFmiSettings::instance().set("lowest::minendhour","18");
	NFmiSettings::instance().set("lowest::precision","10");
	NFmiSettings::instance().set("lowest::significantly_greater","50");
	NFmiSettings::instance().set("lowest::greater","30");
	NFmiSettings::instance().set("lowest::somewhat_greater","10");
	NFmiSettings::instance().set("lowest::somewhat_smaller","10");
	NFmiSettings::instance().set("lowest::smaller","30");
	NFmiSettings::instance().set("lowest::significantly_smaller","50");

	// 1-day forecasts
	{
	  NFmiTime time1(2003,6,3,6,0);
	  NFmiTime time2(2003,6,4,6,0);
	  WeatherPeriod period(time1,time2);
	  RelativeHumidityStory story(time1,sources,area,period,"lowest");
	  
	  NFmiSettings::instance().set("lowest::fake::day1::minimum","10,1");
	  require(story,"fi",fun,"Alin suhteellinen kosteus on tiistaina 10%.");
	  require(story,"sv",fun,"Den lägsta relativa fuktigheten är på tisdag 10%.");
	  require(story,"en",fun,"The smallest relative humidity is on Tuesday 10%.");
	  
	}

	// Another 1-day forecast, because 17 < 18 (minendhour)

	{
	  NFmiTime time1(2003,6,3,6,0);
	  NFmiTime time2(2003,6,4,17,0);
	  WeatherPeriod period(time1,time2);
	  RelativeHumidityStory story(time1,sources,area,period,"lowest");
	  
	  NFmiSettings::instance().set("lowest::fake::day1::minimum","20,1");
	  require(story,"fi",fun,"Alin suhteellinen kosteus on tiistaina 20%.");
	  require(story,"sv",fun,"Den lägsta relativa fuktigheten är på tisdag 20%.");
	  require(story,"en",fun,"The smallest relative humidity is on Tuesday 20%.");
	  
	}

	// 2-day forecasts

	{
	  NFmiTime time1(2003,6,3,6,0);
	  NFmiTime time2(2003,6,4,18,0);
	  WeatherPeriod period(time1,time2);
	  RelativeHumidityStory story(time1,sources,area,period,"lowest");
	  
	  NFmiSettings::instance().set("lowest::fake::day1::minimum","50,1");
	  NFmiSettings::instance().set("lowest::fake::day2::minimum","50,1");
	  require(story,"fi",fun,"Alin suhteellinen kosteus on tiistaina 50%, keskiviikkona sama.");
	  require(story,"sv",fun,"Den lägsta relativa fuktigheten är på tisdag 50%, på onsdag densamma.");
	  require(story,"en",fun,"The smallest relative humidity is on Tuesday 50%, on Wednesday the same.");

	  NFmiSettings::instance().set("lowest::fake::day1::minimum","50,1");
	  NFmiSettings::instance().set("lowest::fake::day2::minimum","0,1");
	  require(story,"fi",fun,"Alin suhteellinen kosteus on tiistaina 50%, keskiviikkona huomattavasti pienempi.");
	  require(story,"sv",fun,"Den lägsta relativa fuktigheten är på tisdag 50%, på onsdag betydligt mindre.");
	  require(story,"en",fun,"The smallest relative humidity is on Tuesday 50%, on Wednesday significantly smaller.");

	  NFmiSettings::instance().set("lowest::fake::day1::minimum","50,1");
	  NFmiSettings::instance().set("lowest::fake::day2::minimum","20,1");
	  require(story,"fi",fun,"Alin suhteellinen kosteus on tiistaina 50%, keskiviikkona pienempi.");
	  require(story,"sv",fun,"Den lägsta relativa fuktigheten är på tisdag 50%, på onsdag mindre.");
	  require(story,"en",fun,"The smallest relative humidity is on Tuesday 50%, on Wednesday smaller.");

	  NFmiSettings::instance().set("lowest::fake::day1::minimum","50,1");
	  NFmiSettings::instance().set("lowest::fake::day2::minimum","40,1");
	  require(story,"fi",fun,"Alin suhteellinen kosteus on tiistaina 50%, keskiviikkona hieman pienempi.");
	  require(story,"sv",fun,"Den lägsta relativa fuktigheten är på tisdag 50%, på onsdag något mindre.");
	  require(story,"en",fun,"The smallest relative humidity is on Tuesday 50%, on Wednesday somewhat smaller.");

	  NFmiSettings::instance().set("lowest::fake::day1::minimum","50,1");
	  NFmiSettings::instance().set("lowest::fake::day2::minimum","60,1");
	  require(story,"fi",fun,"Alin suhteellinen kosteus on tiistaina 50%, keskiviikkona hieman suurempi.");
	  require(story,"sv",fun,"Den lägsta relativa fuktigheten är på tisdag 50%, på onsdag något större.");
	  require(story,"en",fun,"The smallest relative humidity is on Tuesday 50%, on Wednesday somewhat greater.");

	  NFmiSettings::instance().set("lowest::fake::day1::minimum","50,1");
	  NFmiSettings::instance().set("lowest::fake::day2::minimum","80,1");
	  require(story,"fi",fun,"Alin suhteellinen kosteus on tiistaina 50%, keskiviikkona suurempi.");
	  require(story,"sv",fun,"Den lägsta relativa fuktigheten är på tisdag 50%, på onsdag större.");
	  require(story,"en",fun,"The smallest relative humidity is on Tuesday 50%, on Wednesday greater.");

	  NFmiSettings::instance().set("lowest::fake::day1::minimum","50,1");
	  NFmiSettings::instance().set("lowest::fake::day2::minimum","100,1");
	  require(story,"fi",fun,"Alin suhteellinen kosteus on tiistaina 50%, keskiviikkona huomattavasti suurempi.");
	  require(story,"sv",fun,"Den lägsta relativa fuktigheten är på tisdag 50%, på onsdag betydligt större.");
	  require(story,"en",fun,"The smallest relative humidity is on Tuesday 50%, on Wednesday significantly greater.");
	  
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
	  TEST(relativehumidity_lowest);
	}

  }; // class tests

} // namespace RelativeHumidityStoryTest


int main(void)
{
  using namespace RelativeHumidityStoryTest;

  cout << endl
	   << "RelativeHumidityStory tests" << endl
	   << "===========================" << endl;

  dict = TextGen::DictionaryFactory::create("multimysql");

  dict->init("fi");
  dict->init("sv");
  dict->init("en");

  tests t;
  return t.run();
}
