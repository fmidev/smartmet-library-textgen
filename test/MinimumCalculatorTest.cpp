#include "tframe.h"
#include "MinimumCalculator.h"
#include "NFmiGlobals.h"

#include <iostream>
#include <string>

using namespace std;
using namespace boost;


namespace MinimumCalculatorTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test MinimumCalculator
   */
  // ----------------------------------------------------------------------

  void minimum()
  {
	using namespace WeatherAnalysis;

	MinimumCalculator calc;

	if(calc() != kFloatMissing)
	  TEST_FAILED("Should return kFloatMissing after 0 inserts");

	calc(1);
	if(calc() != 1)
	  TEST_FAILED("Failed to return 1");

	calc(2);
	if(calc() != 1)
	  TEST_FAILED("Failed to return min(1,2)");

	calc(3);
	if(calc() != 1)
	  TEST_FAILED("Failed to return min(1,2,3)");

	calc(4);
	if(calc() != 1)
	  TEST_FAILED("Failed to return min(1,2,3,4)");

	calc(-1);
	if(calc() != -1)
	  TEST_FAILED("Failed to return min(1,2,3,4,-1)");

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
	  TEST(minimum);
	}

  }; // class tests

} // namespace MinimumCalculatorTest


int main(void)
{
  using namespace MinimumCalculatorTest;

  cout << endl
	   << "MinimumCalculator tests" << endl
	   << "=======================" << endl;

  tests t;
  return t.run();
}
