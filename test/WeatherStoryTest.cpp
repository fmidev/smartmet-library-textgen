#include "regression/tframe.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "PlainTextFormatter.h"
#include "WeatherStory.h"
#include "Story.h"

#include "newbase/NFmiSettings.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;


namespace WeatherStoryTest
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
   * \brief Test WeatherStory::short_overview()
   */
  // ----------------------------------------------------------------------

  void short_overview()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("dummy");

	const string fun = "weather_shortoverview";

	NFmiTime time1(2003,6,1);
	NFmiTime time2(2003,6,4);
	WeatherPeriod period(time1,time2);
	WeatherStory story(time1,sources,area,period,"a");

	
	NFmiSettings::instance().set("a::cloudiness::clear","40");
	NFmiSettings::instance().set("a::cloudiness::cloudy","70");
	NFmiSettings::instance().set("a::cloudiness::single_limit","60");
	NFmiSettings::instance().set("a::cloudiness::double_limit","20");
	NFmiSettings::instance().set("a::precipitation::rainy","1");
	NFmiSettings::instance().set("a::precipitation::partly_rainy","0.1");
	NFmiSettings::instance().set("a::precipitation::unstable","50");

	// Test cloudiness phrases
	{
	  NFmiSettings::instance().set("a::fake::day1::precipitation","0,1");
	  NFmiSettings::instance().set("a::fake::day2::precipitation","0,1");
	  NFmiSettings::instance().set("a::fake::day3::precipitation","0,1");
	  
	  NFmiSettings::instance().set("a::fake::clear_percentage","70,1");
	  NFmiSettings::instance().set("a::fake::cloudy_percentage","20,1");
	  require(story,"fi",fun,"Enimm�kseen selke��, poutaa.");
	  require(story,"sv",fun,"Mestadels klart, uppeh�ll.");
	  require(story,"en",fun,"Mostly sunny, clear.");
	  
	  NFmiSettings::instance().set("a::fake::clear_percentage","20,1");
	  NFmiSettings::instance().set("a::fake::cloudy_percentage","70,1");
	  require(story,"fi",fun,"Enimm�kseen pilvist�, poutaa.");
	  require(story,"sv",fun,"Mestadels mulet, uppeh�ll.");
	  require(story,"en",fun,"Mostly cloudy, clear.");

	  NFmiSettings::instance().set("a::fake::clear_percentage","20,1");
	  NFmiSettings::instance().set("a::fake::cloudy_percentage","20,1");
	  require(story,"fi",fun,"Enimm�kseen puolipilvist�, poutaa.");
	  require(story,"sv",fun,"Mestadels halvmulet, uppeh�ll.");
	  require(story,"en",fun,"Mostly partly cloudy, clear.");

	  NFmiSettings::instance().set("a::fake::clear_percentage","10,1");
	  NFmiSettings::instance().set("a::fake::cloudy_percentage","40,1");
	  require(story,"fi",fun,"Enimm�kseen pilvist� tai puolipilvist�, poutaa.");
	  require(story,"sv",fun,"Mestadels mulet eller halvmulet, uppeh�ll.");
	  require(story,"en",fun,"Mostly cloudy or partly cloudy, clear.");

	  NFmiSettings::instance().set("a::fake::clear_percentage","40,1");
	  NFmiSettings::instance().set("a::fake::cloudy_percentage","10,1");
	  require(story,"fi",fun,"Enimm�kseen selke�� tai puolipilvist�, poutaa.");
	  require(story,"sv",fun,"Mestadels klart eller halvmulet, uppeh�ll.");
	  require(story,"en",fun,"Mostly sunny or partly cloudy, clear.");

	  NFmiSettings::instance().set("a::fake::clear_percentage","30,1");
	  NFmiSettings::instance().set("a::fake::cloudy_percentage","30,1");
	  require(story,"fi",fun,"Vaihtelevaa pilvisyytt�, poutaa.");
	  require(story,"sv",fun,"V�xlande molnighet, uppeh�ll.");
	  require(story,"en",fun,"Variable cloudiness, clear.");

	}

	// Test rain phrases
	{
	  NFmiSettings::instance().set("a::fake::clear_percentage","30,1");
	  NFmiSettings::instance().set("a::fake::cloudy_percentage","30,1");

	  NFmiSettings::instance().set("a::fake::day1::precipitation","0,1");
	  NFmiSettings::instance().set("a::fake::day2::precipitation","0,1");
	  NFmiSettings::instance().set("a::fake::day3::precipitation","0,1");
	  require(story,"fi",fun,"Vaihtelevaa pilvisyytt�, poutaa.");
	  require(story,"sv",fun,"V�xlande molnighet, uppeh�ll.");
	  require(story,"en",fun,"Variable cloudiness, clear.");

	  NFmiSettings::instance().set("a::fake::day1::precipitation","0.2,1");
	  NFmiSettings::instance().set("a::fake::day2::precipitation","0,1");
	  NFmiSettings::instance().set("a::fake::day3::precipitation","0,1");
	  require(story,"fi",fun,"Vaihtelevaa pilvisyytt�, sunnuntaina paikoin sadetta.");
	  require(story,"sv",fun,"V�xlande molnighet, p� s�ndagen lokalt regn.");
	  require(story,"en",fun,"Variable cloudiness, on Sunday in some places rain.");

	  NFmiSettings::instance().set("a::fake::day1::precipitation","2,1");
	  NFmiSettings::instance().set("a::fake::day2::precipitation","0,1");
	  NFmiSettings::instance().set("a::fake::day3::precipitation","0,1");
	  require(story,"fi",fun,"Vaihtelevaa pilvisyytt�, sunnuntaina sadetta.");
	  require(story,"sv",fun,"V�xlande molnighet, p� s�ndagen regn.");
	  require(story,"en",fun,"Variable cloudiness, on Sunday rain.");

	  NFmiSettings::instance().set("a::fake::day1::precipitation","0,1");
	  NFmiSettings::instance().set("a::fake::day2::precipitation","2,1");
	  NFmiSettings::instance().set("a::fake::day3::precipitation","0,1");
	  require(story,"fi",fun,"Vaihtelevaa pilvisyytt�, maanantaina sadetta.");
	  require(story,"sv",fun,"V�xlande molnighet, p� m�ndagen regn.");
	  require(story,"en",fun,"Variable cloudiness, on Monday rain.");

	  NFmiSettings::instance().set("a::fake::day1::precipitation","0.1,1");
	  NFmiSettings::instance().set("a::fake::day2::precipitation","2,1");
	  NFmiSettings::instance().set("a::fake::day3::precipitation","0,1");
	  require(story,"fi",fun,"Vaihtelevaa pilvisyytt�, ajoittain sateista.");
	  require(story,"sv",fun,"V�xlande molnighet, tidvis regnigt.");
	  require(story,"en",fun,"Variable cloudiness, intermittent rain.");

	  NFmiSettings::instance().set("a::fake::day1::precipitation","2,1");
	  NFmiSettings::instance().set("a::fake::day2::precipitation","2,1");
	  NFmiSettings::instance().set("a::fake::day3::precipitation","0,1");
	  require(story,"fi",fun,"S�� on ep�vakaista.");
	  require(story,"sv",fun,"Ostadigt v�der.");
	  require(story,"en",fun,"The weather is unstable.");

	  NFmiSettings::instance().set("a::fake::day1::precipitation","2,1");
	  NFmiSettings::instance().set("a::fake::day2::precipitation","2,1");
	  NFmiSettings::instance().set("a::fake::day3::precipitation","2,1");
	  require(story,"fi",fun,"S�� on ep�vakaista.");
	  require(story,"sv",fun,"Ostadigt v�der.");
	  require(story,"en",fun,"The weather is unstable.");

	}


	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WeatherStory::thunderprobability()
   */
  // ----------------------------------------------------------------------

  void thunderprobability()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("dummy");

	const string fun = "weather_thunderprobability";

	NFmiTime time1(2003,6,1);
	NFmiTime time2(2003,6,4);
	WeatherPeriod period(time1,time2);
	WeatherStory story(time1,sources,area,period,"b");

	
	NFmiSettings::instance().set("b::precision","10");
	NFmiSettings::instance().set("b::limit","10");

	NFmiSettings::instance().set("b::fake::probability","0,1");
	require(story,"fi",fun,"");
	require(story,"sv",fun,"");
	require(story,"en",fun,"");

	NFmiSettings::instance().set("b::fake::probability","5,1");
	require(story,"fi",fun,"Ukkosen todenn�k�isyys on 10%.");
	require(story,"sv",fun,"Sannolikheten f�r �ska �r 10%.");
	require(story,"en",fun,"Probability of thunder is 10%.");

	NFmiSettings::instance().set("b::fake::probability","64,1");
	require(story,"fi",fun,"Ukkosen todenn�k�isyys on 60%.");
	require(story,"sv",fun,"Sannolikheten f�r �ska �r 60%.");
	require(story,"en",fun,"Probability of thunder is 60%.");

	NFmiSettings::instance().set("b::fake::probability","99,1");
	require(story,"fi",fun,"Ukkosen todenn�k�isyys on 100%.");
	require(story,"sv",fun,"Sannolikheten f�r �ska �r 100%.");
	require(story,"en",fun,"Probability of thunder is 100%.");
	
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
	  TEST(short_overview);
	  TEST(thunderprobability);
	}

  }; // class tests

} // namespace WeatherStoryTest


int main(void)
{
  using namespace WeatherStoryTest;

  cout << endl
	   << "WeatherStory tests" << endl
	   << "==================" << endl;

  dict = TextGen::DictionaryFactory::create("multimysql");

  dict->init("fi");
  dict->init("sv");
  dict->init("en");

  tests t;
  return t.run();
}
