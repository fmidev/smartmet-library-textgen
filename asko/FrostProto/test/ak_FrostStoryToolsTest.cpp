#include <regression/tframe.h>
#include "Dictionary.h"
#include "DictionaryFactory.h"

#include "ak_FrostStoryTools.h"

#include <newbase/NFmiSettings.h>

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;

namespace AK_FrostStoryToolsTest
{
  shared_ptr<TextGen::Dictionary> dict;

  // ----------------------------------------------------------------------
  /*!
   * \brief Test AK_FrostStoryTools::frost_sentence
   */
  // ----------------------------------------------------------------------

  void frost_sentence()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test AK_FrostStoryTools::severe_frost_sentence
   */
  // ----------------------------------------------------------------------

  void severe_frost_sentence()
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
	  TEST(frost_sentence);
	  TEST(severe_frost_sentence);
	}

  }; // class tests

} // namespace AK_FrostStoryToolsTest


int main(void)
{
  using namespace AK_FrostStoryToolsTest;

  cout << endl
	   << "AK_FrostStoryTools tests" << endl
	   << "========================" << endl;

  dict = TextGen::DictionaryFactory::create("null");

  NFmiSettings::Set("textgen::units::celsius::format","phrase");

  tests t;
  return t.run();
}
