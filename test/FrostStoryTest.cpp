#include "tframe.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "PlainTextFormatter.h"
#include "FrostStory.h"
#include "Story.h"

#include "NFmiSettings.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;


namespace FrostStoryTest
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
   * \brief Test FrostStory::mean()
   */
  // ----------------------------------------------------------------------

  void frost_mean()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("dummy");
	WeatherPeriod period(NFmiTime(2000,1,1), NFmiTime(2000,1,2));
	FrostStory story(sources,area,period,"test");

	const string fun = "frost_mean";

	NFmiSettings::instance().set("test::precision","10");
	NFmiSettings::instance().set("test::frost_limit","20");
	NFmiSettings::instance().set("test::severe_frost_limit","10");

	NFmiSettings::instance().set("test::fake::mean","0,1");
	NFmiSettings::instance().set("test::fake::severe_mean","0,1");
	require(story,"fi",fun,"");
	require(story,"sv",fun,"");
	require(story,"en",fun,"");

	NFmiSettings::instance().set("test::fake::mean","10,1");
	NFmiSettings::instance().set("test::fake::severe_mean","0,1");
	require(story,"fi",fun,"");
	require(story,"sv",fun,"");
	require(story,"en",fun,"");

	NFmiSettings::instance().set("test::fake::mean","20,1");
	NFmiSettings::instance().set("test::fake::severe_mean","0,1");
	require(story,"fi",fun,"Hallan todennäköisyys on 20%.");
	require(story,"sv",fun,"Sannolikheten för nattfrost är 20%.");
	require(story,"en",fun,"Probability of frost is 20%.");

	NFmiSettings::instance().set("test::fake::mean","20,1");
	NFmiSettings::instance().set("test::fake::severe_mean","10,1");
	require(story,"fi",fun,"Ankaran hallan todennäköisyys on 10%.");
	require(story,"sv",fun,"Sannolikheten för stark nattfrost är 10%.");
	require(story,"en",fun,"Probability of severe frost is 10%.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test FrostStory::maximum()
   */
  // ----------------------------------------------------------------------

  void frost_maximum()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("dummy");
	WeatherPeriod period(NFmiTime(2000,1,1), NFmiTime(2000,1,2));
	FrostStory story(sources,area,period,"test");

	const string fun = "frost_maximum";

	NFmiSettings::instance().set("test::precision","10");
	NFmiSettings::instance().set("test::frost_limit","20");
	NFmiSettings::instance().set("test::severe_frost_limit","10");

	NFmiSettings::instance().set("test::fake::maximum","0,1");
	NFmiSettings::instance().set("test::fake::severe_maximum","0,1");
	require(story,"fi",fun,"");
	require(story,"sv",fun,"");
	require(story,"en",fun,"");

	NFmiSettings::instance().set("test::fake::maximum","10,1");
	NFmiSettings::instance().set("test::fake::severe_maximum","0,1");
	require(story,"fi",fun,"");
	require(story,"sv",fun,"");
	require(story,"en",fun,"");

	NFmiSettings::instance().set("test::fake::maximum","20,1");
	NFmiSettings::instance().set("test::fake::severe_maximum","0,1");
	require(story,"fi",fun,"Hallan todennäköisyys on 20%.");
	require(story,"sv",fun,"Sannolikheten för nattfrost är 20%.");
	require(story,"en",fun,"Probability of frost is 20%.");

	NFmiSettings::instance().set("test::fake::maximum","20,1");
	NFmiSettings::instance().set("test::fake::severe_maximum","10,1");
	require(story,"fi",fun,"Ankaran hallan todennäköisyys on 10%.");
	require(story,"sv",fun,"Sannolikheten för stark nattfrost är 10%.");
	require(story,"en",fun,"Probability of severe frost is 10%.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test FrostStory::range()
   */
  // ----------------------------------------------------------------------

  void frost_range()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("dummy");
	WeatherPeriod period(NFmiTime(2000,1,1), NFmiTime(2000,1,2));
	FrostStory story(sources,area,period,"test");

	const string fun = "frost_range";

	NFmiSettings::instance().set("test::precision","10");
	NFmiSettings::instance().set("test::frost_limit","50");
	NFmiSettings::instance().set("test::severe_frost_limit","30");

	NFmiSettings::instance().set("test::fake::minimum","0,1");
	NFmiSettings::instance().set("test::fake::maximum","0,1");
	NFmiSettings::instance().set("test::fake::severe_minimum","0,1");
	NFmiSettings::instance().set("test::fake::severe_maximum","0,1");
	require(story,"fi",fun,"");
	require(story,"sv",fun,"");
	require(story,"en",fun,"");

	NFmiSettings::instance().set("test::fake::minimum","50,1");
	NFmiSettings::instance().set("test::fake::maximum","50,1");
	NFmiSettings::instance().set("test::fake::severe_minimum","0,1");
	NFmiSettings::instance().set("test::fake::severe_maximum","0,1");
	require(story,"fi",fun,"Hallan todennäköisyys on 50%.");
	require(story,"sv",fun,"Sannolikheten för nattfrost är 50%.");
	require(story,"en",fun,"Probability of frost is 50%.");

	NFmiSettings::instance().set("test::fake::minimum","40,1");
	NFmiSettings::instance().set("test::fake::maximum","60,1");
	NFmiSettings::instance().set("test::fake::severe_minimum","0,1");
	NFmiSettings::instance().set("test::fake::severe_maximum","0,1");
	require(story,"fi",fun,"Hallan todennäköisyys on 40...60%.");
	require(story,"sv",fun,"Sannolikheten för nattfrost är 40...60%.");
	require(story,"en",fun,"Probability of frost is 40...60%.");

	NFmiSettings::instance().set("test::fake::minimum","50,1");
	NFmiSettings::instance().set("test::fake::maximum","70,1");
	NFmiSettings::instance().set("test::fake::severe_minimum","10,1");
	NFmiSettings::instance().set("test::fake::severe_maximum","20,1");
	require(story,"fi",fun,"Hallan todennäköisyys on 50...70%.");
	require(story,"sv",fun,"Sannolikheten för nattfrost är 50...70%.");
	require(story,"en",fun,"Probability of frost is 50...70%.");

	NFmiSettings::instance().set("test::fake::minimum","40,1");
	NFmiSettings::instance().set("test::fake::maximum","60,1");
	NFmiSettings::instance().set("test::fake::severe_minimum","30,1");
	NFmiSettings::instance().set("test::fake::severe_maximum","40,1");
	require(story,"fi",fun,"Ankaran hallan todennäköisyys on 30...40%.");
	require(story,"sv",fun,"Sannolikheten för stark nattfrost är 30...40%.");
	require(story,"en",fun,"Probability of severe frost is 30...40%.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test FrostStory::twonights()
   */
  // ----------------------------------------------------------------------

  void frost_twonights()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("dummy");

	const string fun = "frost_twonights";

	NFmiSettings::instance().set("test::precision","10");
	NFmiSettings::instance().set("test::frost_limit","20");
	NFmiSettings::instance().set("test::severe_frost_limit","10");

	NFmiSettings::instance().set("test::starthour","21");
	NFmiSettings::instance().set("test::endhour","09");

	// One night
	{
	  WeatherPeriod period(NFmiTime(2003,6,3,12,0), NFmiTime(2003,6,4,12,0));
	  FrostStory story(sources,area,period,"test");

	  NFmiSettings::instance().set("test::fake::day1::mean","10,1");
	  NFmiSettings::instance().set("test::fake::day1::severe_mean","0,1");
	  require(story,"fi",fun,"");
	  require(story,"sv",fun,"");
	  require(story,"en",fun,"");

	  NFmiSettings::instance().set("test::fake::day1::mean","20,1");
	  NFmiSettings::instance().set("test::fake::day1::severe_mean","0,1");
	  require(story,"fi",fun,"Hallan todennäköisyys on keskiviikon vastaisena yönä 20%.");
	  require(story,"sv",fun,"Sannolikheten för nattfrost är natten mot onsdag 20%.");
	  require(story,"en",fun,"Probability of frost is on Wednesday night 20%.");

	  NFmiSettings::instance().set("test::fake::day1::mean","80,1");
	  NFmiSettings::instance().set("test::fake::day1::severe_mean","20,1");
	  require(story,"fi",fun,"Ankaran hallan todennäköisyys on keskiviikon vastaisena yönä 20%.");
	  require(story,"sv",fun,"Sannolikheten för stark nattfrost är natten mot onsdag 20%.");
	  require(story,"en",fun,"Probability of severe frost is on Wednesday night 20%.");

	}

	// Two nights, nothing on second day
	{
	  WeatherPeriod period(NFmiTime(2003,6,3,12,0), NFmiTime(2003,6,5,12,0));
	  FrostStory story(sources,area,period,"test");

	  NFmiSettings::instance().set("test::fake::day1::mean","10,1");
	  NFmiSettings::instance().set("test::fake::day1::severe_mean","0,1");
	  NFmiSettings::instance().set("test::fake::day2::mean","0,1");
	  NFmiSettings::instance().set("test::fake::day2::severe_mean","0,1");
	  require(story,"fi",fun,"");
	  require(story,"sv",fun,"");
	  require(story,"en",fun,"");

	  NFmiSettings::instance().set("test::fake::day1::mean","20,1");
	  NFmiSettings::instance().set("test::fake::day1::severe_mean","0,1");
	  NFmiSettings::instance().set("test::fake::day2::mean","0,1");
	  NFmiSettings::instance().set("test::fake::day2::severe_mean","0,1");
	  require(story,"fi",fun,"Hallan todennäköisyys on keskiviikon vastaisena yönä 20%, seuraava yö on lämpimämpi.");
	  require(story,"sv",fun,"Sannolikheten för nattfrost är natten mot onsdag 20%, följande natt är varmare.");
	  require(story,"en",fun,"Probability of frost is on Wednesday night 20%, the following night is warmer.");

	  NFmiSettings::instance().set("test::fake::day1::mean","80,1");
	  NFmiSettings::instance().set("test::fake::day1::severe_mean","20,1");
	  NFmiSettings::instance().set("test::fake::day2::mean","0,1");
	  NFmiSettings::instance().set("test::fake::day2::severe_mean","0,1");
	  require(story,"fi",fun,"Ankaran hallan todennäköisyys on keskiviikon vastaisena yönä 20%, seuraava yö on huomattavasti lämpimämpi.");
	  require(story,"sv",fun,"Sannolikheten för stark nattfrost är natten mot onsdag 20%, följande natt är betydligt varmare.");
	  require(story,"en",fun,"Probability of severe frost is on Wednesday night 20%, the following night is significantly warmer.");

	}

	// First day frost
	{
	  WeatherPeriod period(NFmiTime(2003,6,3,12,0), NFmiTime(2003,6,5,12,0));
	  FrostStory story(sources,area,period,"test");

	  NFmiSettings::instance().set("test::fake::day1::mean","20,1");
	  NFmiSettings::instance().set("test::fake::day1::severe_mean","0,1");

	  NFmiSettings::instance().set("test::fake::day2::mean","0,1");
	  NFmiSettings::instance().set("test::fake::day2::severe_mean","0,1");
	  require(story,"fi",fun,"Hallan todennäköisyys on keskiviikon vastaisena yönä 20%, seuraava yö on lämpimämpi.");
	  require(story,"sv",fun,"Sannolikheten för nattfrost är natten mot onsdag 20%, följande natt är varmare.");
	  require(story,"en",fun,"Probability of frost is on Wednesday night 20%, the following night is warmer.");

	  NFmiSettings::instance().set("test::fake::day2::mean","30,1");
	  NFmiSettings::instance().set("test::fake::day2::severe_mean","00,1");
	  require(story,"fi",fun,"Hallan todennäköisyys on keskiviikon vastaisena yönä 20%, seuraavana yönä 30%.");
	  require(story,"sv",fun,"Sannolikheten för nattfrost är natten mot onsdag 20%, följande natt 30%.");
	  require(story,"en",fun,"Probability of frost is on Wednesday night 20%, the following night 30%.");

	  NFmiSettings::instance().set("test::fake::day2::mean","80,1");
	  NFmiSettings::instance().set("test::fake::day2::severe_mean","20,1");
	  require(story,"fi",fun,"Hallan todennäköisyys on keskiviikon vastaisena yönä 20%, seuraavana yönä ankaran hallan todennäköisyys on 20%.");
	  require(story,"sv",fun,"Sannolikheten för nattfrost är natten mot onsdag 20%, följande natt sannolikheten för stark nattfrost är 20%.");
	  require(story,"en",fun,"Probability of frost is on Wednesday night 20%, the following night probability of severe frost is 20%.");

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
	  TEST(frost_mean);
	  TEST(frost_maximum);
	  TEST(frost_range);
	  TEST(frost_twonights);
	}

  }; // class tests

} // namespace FrostStoryTest


int main(void)
{
  using namespace FrostStoryTest;

  cout << endl
	   << "FrostStory tests" << endl
	   << "================" << endl;

  dict = TextGen::DictionaryFactory::create("multimysql");

  dict->init("fi");
  dict->init("sv");
  dict->init("en");

  tests t;
  return t.run();
}
