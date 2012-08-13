#include <regression/tframe.h>
#include "CssTextFormatter.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "SpecialStory.h"
#include "Story.h"
#include "StoryTag.h"
#include "MessageLogger.h"

#include <newbase/NFmiSettings.h>

#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/locale.hpp>

using namespace std;
using namespace boost;

namespace SpecialStoryTest
{
  shared_ptr<TextGen::Dictionary> dict;
  TextGen::CssTextFormatter formatter;

  void require(const TextGen::Story & theStory,
			   const string & theLanguage,
			   const string & theName,
			   const string & theExpected,
  			   const string & theStoryVar = "")
  {
	dict->init(theLanguage);
	formatter.dictionary(dict);

	TextGen::Paragraph para;
	para << TextGen::StoryTag(theStoryVar, true);
	para << theStory.makeStory(theName);
	para << TextGen::StoryTag(theStoryVar, false);
	const string value = para.realize(formatter);

	if(value != theExpected)
	  {
		const string msg = "'" + value + "' <> '" + theExpected + "'";
		TEST_FAILED(msg.c_str());
	  }
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test SpecialStory::text()
   */
  // ----------------------------------------------------------------------

  void css_text()
  {
	using namespace std;
	using namespace TextGen;

	AnalysisSources sources;
	WeatherArea area("25,60");
	NFmiTime time1(2000,1,1);
	NFmiTime time2(2000,1,2);
	WeatherPeriod period(time1,time2);
	SpecialStory story(time1,sources,area,period,"text");

	const string fun = "text";

	NFmiSettings::Set("text::value","This is the value of the string.");

	require(story,"fi",fun,"<span>\nThis is the value of the string.\n</span>\n");
	require(story,"sv",fun,"<span>\nThis is the value of the string.\n</span>\n");
	require(story,"en",fun,"<span>\nThis is the value of the string.\n</span>\n");

	NFmiSettings::Set("text::value","@data/special_text.fi");
	require(story,"fi",fun,"<span>\nSuomenkielinen teksti.\n</span>\n");

	NFmiSettings::Set("text::value","@data/special_text.en");
	require(story,"fi",fun,"<span>\nEnglish text.\n</span>\n");

	NFmiSettings::Set("text::value","@data/special_text.php");
	require(story,"fi",fun,"<span>\nSuomenkieltä PHP-ohjelmasta\n</span>\n");

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
	virtual const char * error_message_prefix() const
	{
	  return "\n\t";
	}

	//! Main test suite
	void test(void)
	{
	  TEST(css_text);
	}

  }; // class tests

} // namespace SpecialStoryTest


int main(void)
{
  boost::locale::generator generator;
  std::locale::global(generator(""));

  NFmiSettings::Init();
  NFmiSettings::Set("textgen::database","textgen2");

  using namespace SpecialStoryTest;


  MessageLogger::open("my.log");


  cout << endl
	   << "CssTextFormatter tests" << endl
	   << "=================" << endl;

  dict.reset(TextGen::DictionaryFactory::create("multimysql"));

  dict->init("fi");
  dict->init("sv");
  dict->init("en");

  NFmiSettings::Set("textgen::specialseason","true");

  tests t;
  return t.run();
}
