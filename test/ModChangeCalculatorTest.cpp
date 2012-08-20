#include <regression/tframe.h>
#include "ModChangeCalculator.h"
#include <newbase/NFmiGlobals.h>

#include <iostream>
#include <string>

using namespace std;
using namespace boost;


namespace ModChangeCalculatorTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test ModChangeCalculator
   */
  // ----------------------------------------------------------------------

  void modchange()
  {
	using namespace TextGen;

	ModChangeCalculator calc(10);

	if(calc() != kFloatMissing)
	  TEST_FAILED("Should return kFloatMissing after 0 inserts");

	calc(1);
	if(calc() != 0)
	  TEST_FAILED("Should return 0 after 1 insert");

	calc(2);
	if(calc() != 1)
	  TEST_FAILED("Failed to return 1 for [1,2]");

	calc(4);
	if(calc() != 3)
	  TEST_FAILED("Failed to return 3 for [1,2,4]");

	calc(8);
	if(calc() != 7)
	  TEST_FAILED("Failed to return 8 for [1,2,4,8]");

	calc(1);
	if(calc() != 10)
	  TEST_FAILED("Failed to return 10 for [1,2,4,8,1]");

	calc(2);
	if(calc() != 11)
	  TEST_FAILED("Failed to return 11 for [1,2,4,8,1,2]");

	calc(9);
	if(calc() != 8)
	  TEST_FAILED("Failed to return 11 for [1,2,4,8,1,2,9]");

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
	  TEST(modchange);
	}

  }; // class tests

} // namespace ModChangeCalculatorTest


int main(void)
{
  using namespace ModChangeCalculatorTest;

  cout << endl
	   << "ModChangeCalculator tests" << endl
	   << "=========================" << endl;

  tests t;
  return t.run();
}
