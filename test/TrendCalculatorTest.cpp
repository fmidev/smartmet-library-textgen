#include "regression/tframe.h"
#include "TrendCalculator.h"
#include "newbase/NFmiGlobals.h"

#include <iostream>
#include <string>

using namespace std;
using namespace boost;


namespace TrendCalculatorTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TrendCalculator
   */
  // ----------------------------------------------------------------------

  void trend()
  {
	using namespace WeatherAnalysis;

	TrendCalculator calc;

	if(calc() != kFloatMissing)
	  TEST_FAILED("Should return kFloatMissing after 0 inserts");

	calc(1);
	if(calc() != 0)
	  TEST_FAILED("Should return 0 after 1 insert");

	calc(2);
	if(calc() != 100)
	  TEST_FAILED("Failed to return 100 for [1,2]");

	calc(3);
	if(calc() != 100)
	  TEST_FAILED("Failed to return 100 for [1,2,3]");

	calc(4);
	if(calc() != 100)
	  TEST_FAILED("Failed to return 100 for [1,2,3,4]");

	calc(3);
	if(calc() != 75)
	  TEST_FAILED("Failed to return 75 for [1,2,3,4,3]");

	calc(2);
	if(calc() != 60)
	  TEST_FAILED("Failed to return 60 for [1,2,3,4,3,2]");

	calc(1);
	if(calc() != 50)
	  TEST_FAILED("Failed to return 50 for [1,2,3,4,3,2,1]");

	calc(1);
	if(calc() != 50)
	  TEST_FAILED("Failed to return 50 for [1,2,3,4,3,2,1,1]");

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
	  TEST(trend);
	}

  }; // class tests

} // namespace TrendCalculatorTest


int main(void)
{
  using namespace TrendCalculatorTest;

  cout << endl
	   << "TrendCalculator tests" << endl
	   << "=====================" << endl;

  tests t;
  return t.run();
}
