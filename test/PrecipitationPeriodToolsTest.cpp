#include "regression/tframe.h"
#include "PrecipitationPeriodTools.h"

using namespace std;

namespace PrecipitationPeriodToolsTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PrecipitationPeriodTools::findRainTimes
   */
  // ----------------------------------------------------------------------

  void findRainTimes()
  {
	TEST_NOT_IMPLEMENTED();
  }

  //! The actual test driver
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
	  TEST(findRainTimes);
	}

  }; // class tests

} // namespace PrecipitationPeriodToolsTest


int main(void)
{
  cout << endl
	   << "PrecipitationPeriodTools tester" << endl
	   << "===============================" << endl;
  PrecipitationPeriodToolsTest::tests t;
  return t.run();
}
