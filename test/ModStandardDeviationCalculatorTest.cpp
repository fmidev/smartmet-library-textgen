#include <regression/tframe.h>
#include "ModStandardDeviationCalculator.h"
#include <newbase/NFmiGlobals.h>

#include <cmath>
#include <iostream>
#include <string>

using namespace std;
using namespace boost;

namespace ModStandardDeviationCalculatorTest
{
// ----------------------------------------------------------------------
/*!
 * \brief Test ModStandardDeviationCalculator
 */
// ----------------------------------------------------------------------

void modmean()
{
  using namespace TextGen;

  ModStandardDeviationCalculator calc(10);

  if (calc() != kFloatMissing) TEST_FAILED("Should return kFloatMissing after 0 inserts");

  calc(1);
  if (calc() != 0) TEST_FAILED("Failed std(1) = 0");

  float expected = sqrt((1 * 1 + 1 * 1 - (1 - 1) * (1 - 1) / 2.0) / 2.0);
  calc(9);
  if (calc() != expected)
  {
    cout << calc() << endl;
    TEST_FAILED("Failed std(1,9) = 1.63299...");
  }

  expected = sqrt((1 * 1 + 1 * 1 + 3 * 3 - (1 - 1 - 3) * (1 - 1 - 3) / 3.0) / 3.0);
  calc(7);
  if (calc() != expected)
  {
    cout << calc() << endl;
    TEST_FAILED("Failed std(1,9,7) = 1.63299...");
  }

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
  void test(void) { TEST(modmean); }
};  // class tests

}  // namespace ModStandardDeviationCalculatorTest

int main(void)
{
  using namespace ModStandardDeviationCalculatorTest;

  cout << endl
       << "ModStandardDeviationCalculator tests" << endl
       << "====================================" << endl;

  tests t;
  return t.run();
}
