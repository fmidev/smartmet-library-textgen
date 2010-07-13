#include <regression/tframe.h>
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "PlainTextFormatter.h"
#include "WeatherStory.h"
#include "Story.h"

#include <newbase/NFmiSettings.h>

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
	WeatherArea area("25,60");

	const string fun = "weather_shortoverview";

	NFmiTime time1(2003,6,1);
	NFmiTime time2(2003,6,4);
	WeatherPeriod period(time1,time2);
	WeatherStory story(time1,sources,area,period,"a");

	
	NFmiSettings::Set("a::cloudiness::clear","40");
	NFmiSettings::Set("a::cloudiness::cloudy","70");
	NFmiSettings::Set("a::cloudiness::single_limit","60");
	NFmiSettings::Set("a::cloudiness::double_limit","20");
	NFmiSettings::Set("a::precipitation::rainy","1");
	NFmiSettings::Set("a::precipitation::partly_rainy","0.1");
	NFmiSettings::Set("a::precipitation::unstable","50");

	// Test cloudiness phrases
	{
	  NFmiSettings::Set("a::fake::day1::precipitation","0,0");
	  NFmiSettings::Set("a::fake::day2::precipitation","0,0");
	  NFmiSettings::Set("a::fake::day3::precipitation","0,0");
	  
	  NFmiSettings::Set("a::fake::clear_percentage","70,0");
	  NFmiSettings::Set("a::fake::cloudy_percentage","20,0");
	  require(story,"fi",fun,"Enimm�kseen selke��, poutaa.");
	  require(story,"sv",fun,"Mestadels klart, uppeh�ll.");
	  require(story,"en",fun,"Mostly sunny, fair weather.");
	  
	  NFmiSettings::Set("a::fake::clear_percentage","20,0");
	  NFmiSettings::Set("a::fake::cloudy_percentage","70,0");
	  require(story,"fi",fun,"Enimm�kseen pilvist�, poutaa.");
	  require(story,"sv",fun,"Mestadels mulet, uppeh�ll.");
	  require(story,"en",fun,"Mostly cloudy, fair weather.");

	  NFmiSettings::Set("a::fake::clear_percentage","20,0");
	  NFmiSettings::Set("a::fake::cloudy_percentage","20,0");
	  require(story,"fi",fun,"Enimm�kseen puolipilvist�, poutaa.");
	  require(story,"sv",fun,"Mestadels halvmulet, uppeh�ll.");
	  require(story,"en",fun,"Mostly partly cloudy, fair weather.");

	  NFmiSettings::Set("a::fake::clear_percentage","10,0");
	  NFmiSettings::Set("a::fake::cloudy_percentage","40,0");
	  require(story,"fi",fun,"Enimm�kseen pilvist� tai puolipilvist�, poutaa.");
	  require(story,"sv",fun,"Mestadels mulet eller halvmulet, uppeh�ll.");
	  require(story,"en",fun,"Mostly cloudy or partly cloudy, fair weather.");

	  NFmiSettings::Set("a::fake::clear_percentage","40,0");
	  NFmiSettings::Set("a::fake::cloudy_percentage","10,0");
	  require(story,"fi",fun,"Enimm�kseen selke�� tai puolipilvist�, poutaa.");
	  require(story,"sv",fun,"Mestadels klart eller halvmulet, uppeh�ll.");
	  require(story,"en",fun,"Mostly sunny or partly cloudy, fair weather.");

	  NFmiSettings::Set("a::fake::clear_percentage","30,0");
	  NFmiSettings::Set("a::fake::cloudy_percentage","30,0");
	  require(story,"fi",fun,"Vaihtelevaa pilvisyytt�, poutaa.");
	  require(story,"sv",fun,"V�xlande molnighet, uppeh�ll.");
	  require(story,"en",fun,"Variable cloudiness, fair weather.");

	}

	// Test rain phrases
	{
	  NFmiSettings::Set("a::fake::clear_percentage","30,0");
	  NFmiSettings::Set("a::fake::cloudy_percentage","30,0");

	  NFmiSettings::Set("a::fake::day1::precipitation","0,0");
	  NFmiSettings::Set("a::fake::day2::precipitation","0,0");
	  NFmiSettings::Set("a::fake::day3::precipitation","0,0");
	  require(story,"fi",fun,"Vaihtelevaa pilvisyytt�, poutaa.");
	  require(story,"sv",fun,"V�xlande molnighet, uppeh�ll.");
	  require(story,"en",fun,"Variable cloudiness, fair weather.");

	  NFmiSettings::Set("a::fake::day1::precipitation","0.2,0");
	  NFmiSettings::Set("a::fake::day2::precipitation","0,0");
	  NFmiSettings::Set("a::fake::day3::precipitation","0,0");
	  require(story,"fi",fun,"Vaihtelevaa pilvisyytt�, sunnuntaina paikoin sadetta.");
	  require(story,"sv",fun,"V�xlande molnighet, p� s�ndagen lokalt regn.");
	  require(story,"en",fun,"Variable cloudiness, on Sunday in some places rain.");

	  NFmiSettings::Set("a::fake::day1::precipitation","2,0");
	  NFmiSettings::Set("a::fake::day2::precipitation","0,0");
	  NFmiSettings::Set("a::fake::day3::precipitation","0,0");
	  require(story,"fi",fun,"Vaihtelevaa pilvisyytt�, sunnuntaina sadetta.");
	  require(story,"sv",fun,"V�xlande molnighet, p� s�ndagen regn.");
	  require(story,"en",fun,"Variable cloudiness, on Sunday rain.");

	  NFmiSettings::Set("a::fake::day1::precipitation","0,0");
	  NFmiSettings::Set("a::fake::day2::precipitation","2,0");
	  NFmiSettings::Set("a::fake::day3::precipitation","0,0");
	  require(story,"fi",fun,"Vaihtelevaa pilvisyytt�, maanantaina sadetta.");
	  require(story,"sv",fun,"V�xlande molnighet, p� m�ndagen regn.");
	  require(story,"en",fun,"Variable cloudiness, on Monday rain.");

	  NFmiSettings::Set("a::fake::day1::precipitation","0.1,0");
	  NFmiSettings::Set("a::fake::day2::precipitation","2,0");
	  NFmiSettings::Set("a::fake::day3::precipitation","0,0");
	  require(story,"fi",fun,"Vaihtelevaa pilvisyytt�, ajoittain sateista.");
	  require(story,"sv",fun,"V�xlande molnighet, tidvis regnigt.");
	  require(story,"en",fun,"Variable cloudiness, intermittent rain.");

	  NFmiSettings::Set("a::fake::day1::precipitation","2,0");
	  NFmiSettings::Set("a::fake::day2::precipitation","2,0");
	  NFmiSettings::Set("a::fake::day3::precipitation","0,0");
	  require(story,"fi",fun,"S�� on ep�vakaista.");
	  require(story,"sv",fun,"Ostadigt v�der.");
	  require(story,"en",fun,"The weather is unstable.");

	  NFmiSettings::Set("a::fake::day1::precipitation","2,0");
	  NFmiSettings::Set("a::fake::day2::precipitation","2,0");
	  NFmiSettings::Set("a::fake::day3::precipitation","2,0");
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
	WeatherArea area("25,60");

	const string fun = "weather_thunderprobability";

	NFmiTime time1(2003,6,1);
	NFmiTime time2(2003,6,4);
	WeatherPeriod period(time1,time2);
	WeatherStory story(time1,sources,area,period,"b");

	
	NFmiSettings::Set("b::precision","10");
	NFmiSettings::Set("b::limit","10");

	NFmiSettings::Set("b::fake::probability","0,0");
	require(story,"fi",fun,"");
	require(story,"sv",fun,"");
	require(story,"en",fun,"");

	NFmiSettings::Set("b::fake::probability","5,0");
	require(story,"fi",fun,"Ukkosen todenn�k�isyys on 10%.");
	require(story,"sv",fun,"Sannolikheten f�r �ska �r 10%.");
	require(story,"en",fun,"Probability of thunder is 10%.");

	NFmiSettings::Set("b::fake::probability","64,0");
	require(story,"fi",fun,"Ukkosen todenn�k�isyys on 60%.");
	require(story,"sv",fun,"Sannolikheten f�r �ska �r 60%.");
	require(story,"en",fun,"Probability of thunder is 60%.");

	NFmiSettings::Set("b::fake::probability","99,0");
	require(story,"fi",fun,"Ukkosen todenn�k�isyys on 100%.");
	require(story,"sv",fun,"Sannolikheten f�r �ska �r 100%.");
	require(story,"en",fun,"Probability of thunder is 100%.");
	
	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WeatherStory::thunderprobability_simplified()
   */
  // ----------------------------------------------------------------------

  void thunderprobability_simplified()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("25,60");

	const string fun = "weather_thunderprobability_simplified";

	NFmiTime time1(2003,6,1);
	NFmiTime time2(2003,6,4);
	WeatherPeriod period(time1,time2);
	WeatherStory story(time1,sources,area,period,"b");
	
	NFmiSettings::Set("b::precision","10");
	NFmiSettings::Set("b::limit","30");

	NFmiSettings::Set("b::fake::probability","15,0");
	require(story,"fi",fun,"");
	require(story,"sv",fun,"");
	require(story,"en",fun,"");

	NFmiSettings::Set("b::fake::probability","50,0");
	require(story,"fi",fun,"Ukkoskuurot paikoin mahdollisia.");
	require(story,"sv",fun,"Lokala �skskurar m�jliga.");
	require(story,"en",fun,"Possible local thunder showers.");

	NFmiSettings::Set("b::fake::probability","90,0");
	require(story,"fi",fun,"Ukkoskuurot todenn�k�isi�.");
	require(story,"sv",fun,"�skskurar sannolika.");
	require(story,"en",fun,"Probable thunder showers.");
	
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
      TEST(thunderprobability_simplified);
	}

  }; // class tests

} // namespace WeatherStoryTest


int main(void)
{
  using namespace WeatherStoryTest;

  cout << endl
	   << "WeatherStory tests" << endl
	   << "==================" << endl;

  dict.reset(TextGen::DictionaryFactory::create("multimysql"));

  dict->init("fi");
  dict->init("sv");
  dict->init("en");

  tests t;
  return t.run();
}
