#include "tframe.h"
#include "MeanCalculator.h"
#include "NFmiGlobals.h"

#include <iostream>
#include <string>

using namespace std;
using namespace boost;


namespace MeanCalculatorTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test MeanCalculator
   */
  // ----------------------------------------------------------------------

  void mean()
  {
	using namespace WeatherAnalysis;

	MeanCalculator calc;

	if(calc() != kFloatMissing)
	  TEST_FAILED("Should return kFloatMissing after 0 inserts");

	calc(1);
	if(calc() != 1)
	  TEST_FAILED("Failed to return 1");

	calc(2);
	if(calc() != (1+2)/2.0)
	  TEST_FAILED("Failed to return (1+2)/2");

	calc(3);
	if(calc() != (1+2+3)/3.0)
	  TEST_FAILED("Failed to return (1+2+3)/3");

	calc(4);
	if(calc() != (1+2+3+4)/4.0)
	  TEST_FAILED("Failed to return (1+2+3+4)/4");

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
	  TEST(mean);
	}

  }; // class tests

} // namespace MeanCalculatorTest


int main(void)
{
  using namespace MeanCalculatorTest;

  cout << endl
	   << "MeanCalculator tests" << endl
	   << "====================" << endl;

  tests t;
  return t.run();
}