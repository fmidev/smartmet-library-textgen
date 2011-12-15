#include <regression/tframe.h>
#include "SumCalculator.h"
#include <newbase/NFmiGlobals.h>

#include <iostream>
#include <string>

using namespace std;
using namespace boost;


namespace SumCalculatorTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test SumCalculator
   */
  // ----------------------------------------------------------------------

  void sum()
  {
	using namespace WeatherAnalysis;

	SumCalculator calc;

	if(calc() != kFloatMissing)
	  TEST_FAILED("Should return kFloatMissing after 0 inserts");

	calc(1);
	if(calc() != 1)
	  TEST_FAILED("Failed to return 1");

	calc(2);
	if(calc() != 1+2)
	  TEST_FAILED("Failed to return 1+2");

	calc(3);
	if(calc() != 1+2+3)
	  TEST_FAILED("Failed to return 1+2+3");

	calc(4);
	if(calc() != 1+2+3+4)
	  TEST_FAILED("Failed to return 1+2+3+4");

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
	  TEST(sum);
	}

  }; // class tests

} // namespace SumCalculatorTest


int main(void)
{
  using namespace SumCalculatorTest;

  cout << endl
	   << "SumCalculator tests" << endl
	   << "===================" << endl;

  tests t;
  return t.run();
}
