#include <regression/tframe.h>
#include "ModTrendCalculator.h"
#include <newbase/NFmiGlobals.h>

#include <iostream>
#include <string>

using namespace std;
using namespace boost;


namespace ModTrendCalculatorTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test ModTrendCalculator
   */
  // ----------------------------------------------------------------------

  void modtrend()
  {
	using namespace TextGen;

	ModTrendCalculator calc(10);

	if(calc() != kFloatMissing)
	  TEST_FAILED("Should return kFloatMissing after 0 inserts");

	calc(1);
	if(calc() != 0)
	  TEST_FAILED("Should return 0 after 1 insert");

	calc(2);
	if(calc() != 100)
	  TEST_FAILED("Failed to return 100 for [1,2]");

	calc(4);
	if(calc() != 100)
	  TEST_FAILED("Failed to return 100 for [1,2,4]");

	calc(8);
	if(calc() != 100)
	  TEST_FAILED("Failed to return 100 for [1,2,4,8]");

	calc(1);
	if(calc() != 100)
	  TEST_FAILED("Failed to return 100 for [1,2,4,8,1]");

	calc(9);
	if(calc() != 80)
	  TEST_FAILED("Failed to return 80 for [1,2,4,8,1,9]");

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
	  TEST(modtrend);
	}

  }; // class tests

} // namespace ModTrendCalculatorTest


int main(void)
{
  using namespace ModTrendCalculatorTest;

  cout << endl
	   << "ModTrendCalculator tests" << endl
	   << "========================" << endl;

  tests t;
  return t.run();
}
