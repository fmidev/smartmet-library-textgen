#include "regression/tframe.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "PlainTextFormatter.h"
#include "DewPointStory.h"
#include "Story.h"

#include "NFmiSettings.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;


namespace DewPointStoryTest
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
   * \brief Test DewPointStory::range()
   */
  // ----------------------------------------------------------------------

  void dewpoint_range()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("25,60");
	const string fun = "dewpoint_range";

	NFmiSettings::Set("range::mininterval","2");
	NFmiSettings::Set("range::always_interval_zero","true");

	NFmiTime time1(2003,6,1);
	NFmiTime time2(2003,6,2);
	WeatherPeriod period(time1,time2);
	DewPointStory story(time1,sources,area,period,"range");
	
	NFmiSettings::Set("range::fake::minimum","5,0");
	NFmiSettings::Set("range::fake::mean","5,0");
	NFmiSettings::Set("range::fake::maximum","5,0");
	require(story,"fi",fun,"Kastepiste on noin 5 astetta.");
	require(story,"sv",fun,"Daggpunkt �r cirka 5 grader.");
	require(story,"en",fun,"Dewpoint is about 5 degrees.");
	
	NFmiSettings::Set("range::fake::minimum","5,0");
	NFmiSettings::Set("range::fake::mean","6,0");
	NFmiSettings::Set("range::fake::maximum","6,0");
	require(story,"fi",fun,"Kastepiste on noin 6 astetta.");
	require(story,"sv",fun,"Daggpunkt �r cirka 6 grader.");
	require(story,"en",fun,"Dewpoint is about 6 degrees.");
	
	NFmiSettings::Set("range::fake::minimum","5,0");
	NFmiSettings::Set("range::fake::mean","6,0");
	NFmiSettings::Set("range::fake::maximum","7,0");
	require(story,"fi",fun,"Kastepiste on 5...7 astetta.");
	require(story,"sv",fun,"Daggpunkt �r 5...7 grader.");
	require(story,"en",fun,"Dewpoint is 5...7 degrees.");
	
	NFmiSettings::Set("range::fake::minimum","0,0");
	NFmiSettings::Set("range::fake::mean","0,0");
	NFmiSettings::Set("range::fake::maximum","1,0");
	require(story,"fi",fun,"Kastepiste on 0...1 astetta.");
	require(story,"sv",fun,"Daggpunkt �r 0...1 grader.");
	require(story,"en",fun,"Dewpoint is 0...1 degrees.");
	
	NFmiSettings::Set("range::fake::minimum","-1,0");
	NFmiSettings::Set("range::fake::mean","0,0");
	NFmiSettings::Set("range::fake::maximum","0,0");
	require(story,"fi",fun,"Kastepiste on -1...0 astetta.");
	require(story,"sv",fun,"Daggpunkt �r -1...0 grader.");
	require(story,"en",fun,"Dewpoint is -1...0 degrees.");
	
	NFmiSettings::Set("range::fake::minimum","-1,0");
	NFmiSettings::Set("range::fake::mean","0,0");
	NFmiSettings::Set("range::fake::maximum","1,0");
	require(story,"fi",fun,"Kastepiste on -1...1 astetta.");
	require(story,"sv",fun,"Daggpunkt �r -1...1 grader.");
	require(story,"en",fun,"Dewpoint is -1...1 degrees.");

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
	  TEST(dewpoint_range);
	}

  }; // class tests

} // namespace DewPointStoryTest


int main(void)
{
  using namespace DewPointStoryTest;

  cout << endl
	   << "DewPointStory tests" << endl
	   << "===================" << endl;

  dict = TextGen::DictionaryFactory::create("multimysql");

  NFmiSettings::Init();
  NFmiSettings::Set("textgen::units::celsius::format","phrase");

  dict->init("fi");
  dict->init("sv");
  dict->init("en");

  tests t;
  return t.run();
}