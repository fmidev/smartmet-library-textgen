#include "regression/tframe.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "PrecipitationStoryTools.h"

#include "NFmiSettings.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;

namespace PrecipitationStoryToolsTest
{
  shared_ptr<TextGen::Dictionary> dict;

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PrecipitationStoryTools::rain_phrase
   */
  // ----------------------------------------------------------------------

  void rain_phrase()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PrecipitationStoryTools::places_phrase
   */
  // ----------------------------------------------------------------------

  void places_phrase()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PrecipitationStoryTools::type_phrase
   */
  // ----------------------------------------------------------------------

  void type_phrase()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PrecipitationStoryTools::strength_phrase
   */
  // ----------------------------------------------------------------------

  void strength_phrase()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PrecipitationStoryTools::sum_phrase
   */
  // ----------------------------------------------------------------------

  void sum_phrase()
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
	  TEST(rain_phrase);
	  TEST(places_phrase);
	  TEST(type_phrase);
	  TEST(strength_phrase);
	  TEST(sum_phrase);
	}

  }; // class tests

} // namespace PrecipitationStoryToolsTest


int main(void)
{
  using namespace PrecipitationStoryToolsTest;

  cout << endl
	   << "PrecipitationStoryTools tests" << endl
	   << "=============================" << endl;

  dict = TextGen::DictionaryFactory::create("null");

  NFmiSettings::Set("textgen::units::celsius::format","phrase");

  tests t;
  return t.run();
}
