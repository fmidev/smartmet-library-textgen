#include "regression/tframe.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "CloudinessStoryTools.h"

#include "NFmiSettings.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;

namespace CloudinessStoryToolsTest
{
  shared_ptr<TextGen::Dictionary> dict;

  // ----------------------------------------------------------------------
  /*!
   * \brief Test CloudinessStoryTools::cloudinesstype
   */
  // ----------------------------------------------------------------------

  void cloudinesstype()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test CloudinessStoryTools::similartype
   */
  // ----------------------------------------------------------------------

  void similartype()
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
	  TEST(cloudinesstype);
	  TEST(similartype);
	}

  }; // class tests

} // namespace CloudinessStoryToolsTest


int main(void)
{
  using namespace CloudinessStoryToolsTest;

  cout << endl
	   << "CloudinessStoryTools tests" << endl
	   << "==========================" << endl;

  dict = TextGen::DictionaryFactory::create("null");

  tests t;
  return t.run();
}
