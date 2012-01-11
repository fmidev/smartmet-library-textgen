#include <regression/tframe.h>
#include "TrendCalculator.h"
#include <newbase/NFmiGlobals.h>
#include <boost/lexical_cast.hpp>
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
	  TEST_FAILED("Failed to return 100 for [1,2], got "+lexical_cast<string>(calc()));

	calc(3);
	if(calc() != 100)
	  TEST_FAILED("Failed to return 100 for [1,2,3], got "+lexical_cast<string>(calc()));

	calc(4);
	if(calc() != 100)
	  TEST_FAILED("Failed to return 100 for [1,2,3,4], got "+lexical_cast<string>(calc()));

	calc(3);
	if(calc() != 50)
	  TEST_FAILED("Failed to return 50 for [1,2,3,4,3], got "+lexical_cast<string>(calc()));

	calc(2);
	if(calc() != 20)
	  TEST_FAILED("Failed to return 20 for [1,2,3,4,3,2], got "+lexical_cast<string>(calc()));

	calc(1);
	if(calc() != 0)
	  TEST_FAILED("Failed to return 0 for [1,2,3,4,3,2,1], got "+lexical_cast<string>(calc()));

	calc(1);
	if(calc() != 0)
	  TEST_FAILED("Failed to return 0 for [1,2,3,4,3,2,1,1], got "+lexical_cast<string>(calc()));


	calc(0);
	if(calc() != -12.5)
	  TEST_FAILED("Failed to return -12.5 for [1,2,3,4,3,2,1,1,0], got "+lexical_cast<string>(calc()));


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
