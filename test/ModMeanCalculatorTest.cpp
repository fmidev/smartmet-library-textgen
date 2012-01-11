#include <regression/tframe.h>
#include "ModMeanCalculator.h"
#include <newbase/NFmiGlobals.h>

#include <iostream>
#include <string>

using namespace std;
using namespace boost;


namespace ModMeanCalculatorTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test ModMeanCalculator
   */
  // ----------------------------------------------------------------------

  void modmean()
  {
	using namespace WeatherAnalysis;

	ModMeanCalculator calc(10);

	if(calc() != kFloatMissing)
	  TEST_FAILED("Should return kFloatMissing after 0 inserts");

	calc(1);
	if(calc() != 1)
	  TEST_FAILED("Failed mean(1) = 1");

	calc(9);
	if(calc() != (1-1)/2.0)
	  TEST_FAILED("Failed mean(1,9) = 0");

	calc(7);
	if(calc() != (11+9+7)/3.0)
	  TEST_FAILED("Failed mean(1,9,7) = 9");

	calc(3);
	if(calc() != (11+9+7+3)/4.0)
	  TEST_FAILED("Failed mean(1,9,7,3) = 7.5");

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
	  TEST(modmean);
	}

  }; // class tests

} // namespace ModMeanCalculatorTest


int main(void)
{
  using namespace ModMeanCalculatorTest;

  cout << endl
	   << "ModMeanCalculator tests" << endl
	   << "=======================" << endl;

  tests t;
  return t.run();
}
