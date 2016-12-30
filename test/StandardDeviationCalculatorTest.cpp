#include <regression/tframe.h>
#include "StandardDeviationCalculator.h"
#include <newbase/NFmiGlobals.h>

#include <cmath>
#include <iostream>
#include <string>

using namespace std;
using namespace boost;

namespace StandardDeviationCalculatorTest
{
// ----------------------------------------------------------------------
/*!
 * \brief Test StandardDeviationCalculator
 */
// ----------------------------------------------------------------------

void standarddeviation()
{
  using namespace TextGen;

  StandardDeviationCalculator calc;

  if (calc() != kFloatMissing) TEST_FAILED("Should return kFloatMissing after 0 inserts");

  calc(1);
  if (calc() != kFloatMissing) TEST_FAILED("Should return kFloatMissing after 1 inserts");

  float expected = sqrt((1 * 1 + 2 * 2 - 3 * 3 / 2.0) / 1);
  calc(2);
  if (calc() != expected) TEST_FAILED("Failed for [1,2]");

  expected = sqrt((1 * 1 + 2 * 2 + 3 * 3 - 6 * 6 / 3.0) / 2.0);
  calc(3);
  if (calc() != expected) TEST_FAILED("Failed for [1,2,3]");

  expected = sqrt((1 * 1 + 2 * 2 + 3 * 3 + 4 * 4 - 10 * 10 / 4.0) / 3.0);
  calc(4);
  if (calc() != expected) TEST_FAILED("Failed for [1,2,3,4]");

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
  virtual const char* error_message_prefix() const { return "\n\t"; }
  //! Main test suite
  void test(void) { TEST(standarddeviation); }
};  // class tests

}  // namespace StandardDeviationCalculatorTest

int main(void)
{
  using namespace StandardDeviationCalculatorTest;

  cout << endl
       << "StandardDeviationCalculator tests" << endl
       << "=================================" << endl;

  tests t;
  return t.run();
}
