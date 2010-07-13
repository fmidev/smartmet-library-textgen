#include <regression/tframe.h>
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "TemperatureStoryTools.h"

#include <newbase/NFmiSettings.h>

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;

namespace TemperatureStoryToolsTest
{
  shared_ptr<TextGen::Dictionary> dict;

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TemperatureStoryTools::temperature_comparison_phrase
   */
  // ----------------------------------------------------------------------

  void temperature_comparison_phrase()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TemperatureStoryTools::temperature_sentence
   */
  // ----------------------------------------------------------------------

  void temperature_sentence()
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
	  TEST(temperature_comparison_phrase);
	  TEST(temperature_sentence);
	}

  }; // class tests

} // namespace TemperatureStoryToolsTest


int main(void)
{
  using namespace TemperatureStoryToolsTest;

  cout << endl
	   << "TemperatureStoryTools tests" << endl
	   << "===========================" << endl;

  dict.reset(TextGen::DictionaryFactory::create("null"));

  NFmiSettings::Set("textgen::units::celsius::format","phrase");

  tests t;
  return t.run();
}
