#include "regression/tframe.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "WindStoryTools.h"

#include "NFmiSettings.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;

namespace WindStoryToolsTest
{
  shared_ptr<TextGen::Dictionary> dict;

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WindStoryTools::minspeed
   */
  // ----------------------------------------------------------------------

  void minspeed()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WindStoryTools::maxspeed
   */
  // ----------------------------------------------------------------------

  void maxspeed()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WindStoryTools::meanspeed
   */
  // ----------------------------------------------------------------------

  void meanspeed()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WindStoryTools::similar_speed_range
   */
  // ----------------------------------------------------------------------

  void similar_speed_range()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WindStoryTools::direction_accuracy
   */
  // ----------------------------------------------------------------------

  void direction_accuracy()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WindStoryTools::direction8th
   */
  // ----------------------------------------------------------------------

  void direction8th()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WindStoryTools::direction_sentence
   */
  // ----------------------------------------------------------------------

  void direction_sentence()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WindStoryTools::speed_range_sentence
   */
  // ----------------------------------------------------------------------

  void speed_range_sentence()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WindStoryTools::directed_speed_sentence
   */
  // ----------------------------------------------------------------------

  void directed_speed_sentence()
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
	  TEST(minspeed);
	  TEST(maxspeed);
	  TEST(meanspeed);
	  TEST(similar_speed_range);
	  TEST(direction_accuracy);
	  TEST(direction8th);
	  TEST(direction_sentence);
	  TEST(speed_range_sentence);
	  TEST(directed_speed_sentence);
		
	}

  }; // class tests

} // namespace WindStoryToolsTest


int main(void)
{
  using namespace WindStoryToolsTest;

  cout << endl
	   << "WindStoryTools tests" << endl
	   << "====================" << endl;

  dict = TextGen::DictionaryFactory::create("null");

  NFmiSettings::Set("textgen::units::celsius::format","phrase");

  tests t;
  return t.run();
}
