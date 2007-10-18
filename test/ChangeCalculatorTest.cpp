#include <regression/tframe.h>
#include "ChangeCalculator.h"
#include <newbase/NFmiGlobals.h>

#include <iostream>
#include <string>

using namespace std;
using namespace boost;


namespace ChangeCalculatorTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test ChangeCalculator
   */
  // ----------------------------------------------------------------------

  void change()
  {
	using namespace WeatherAnalysis;

	ChangeCalculator calc;

	if(calc() != kFloatMissing)
	  TEST_FAILED("Should return kFloatMissing after 0 inserts");

	calc(1);
	if(calc() != 0)
	  TEST_FAILED("Should return 0 after 1 insert");

	calc(2);
	if(calc() != 2-1)
	  TEST_FAILED("Failed to return 2-1");

	calc(3);
	if(calc() != 3-1)
	  TEST_FAILED("Failed to return 3-1");

	calc(4);
	if(calc() != 4-1)
	  TEST_FAILED("Failed to return 4-1");

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
	  TEST(change);
	}

  }; // class tests

} // namespace ChangeCalculatorTest


int main(void)
{
  using namespace ChangeCalculatorTest;

  cout << endl
	   << "ChangeCalculator tests" << endl
	   << "======================" << endl;

  tests t;
  return t.run();
}
