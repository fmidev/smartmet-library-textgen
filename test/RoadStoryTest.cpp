#include "regression/tframe.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "PlainTextFormatter.h"
#include "RoadStory.h"
#include "Story.h"

#include "NFmiSettings.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;


namespace RoadStoryTest
{
  shared_ptr<TextGen::Dictionary> dict;
  TextGen::PlainTextFormatter formatter;
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Test RoadStory::daynightranges()
   */
  // ----------------------------------------------------------------------

  void roadtemperature_daynightranges()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test RoadStory::condition_overview()
   */
  // ----------------------------------------------------------------------

  void roadcondition_overview()
  {
	TEST_NOT_IMPLEMENTED();
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
	  TEST(roadtemperature_daynightranges);
	  TEST(roadcondition_overview);
	}

  }; // class tests

} // namespace RoadStoryTest


int main(void)
{
  using namespace RoadStoryTest;

  cout << endl
	   << "RoadStory tests" << endl
	   << "===============" << endl;

  dict = TextGen::DictionaryFactory::create("multimysql");

  dict->init("fi");
  dict->init("sv");
  dict->init("en");

  tests t;
  return t.run();
}