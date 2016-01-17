#include <regression/tframe.h>
#include "WeatherResultTools.h"
#include "Settings.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

namespace WeatherResultToolsTest
{
// ----------------------------------------------------------------------
/*!
 * \brief Test WeatherResultTools::min
 */
// ----------------------------------------------------------------------

void min()
{
  using namespace TextGen;

  WeatherResult result1(1, 1);
  WeatherResult result2(2, 2);
  WeatherResult result3(3, 3);

  if (WeatherResultTools::min(result1, result2) != WeatherResult(1, 2))
    TEST_FAILED("min([1,1],[2,2]) should return [1,2]");

  if (WeatherResultTools::min(result1, result3) != WeatherResult(1, 3))
    TEST_FAILED("min([1,1],[3,3]) should return [1,3]");

  if (WeatherResultTools::min(result2, result3) != WeatherResult(2, 3))
    TEST_FAILED("min([2,2],[3,3]) should return [2,3]");

  if (WeatherResultTools::min(result1, result2, result3) != WeatherResult(1, 3))
    TEST_FAILED("min([1,1],[2,2],[3,3]) should return [1,3]");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test WeatherResultTools::max
 */
// ----------------------------------------------------------------------

void max()
{
  using namespace TextGen;

  WeatherResult result1(1, 1);
  WeatherResult result2(2, 2);
  WeatherResult result3(3, 3);

  if (WeatherResultTools::max(result1, result2) != WeatherResult(2, 2))
    TEST_FAILED("max([1,1],[2,2]) should return [2,2]");

  if (WeatherResultTools::max(result1, result3) != WeatherResult(3, 3))
    TEST_FAILED("max([1,1],[3,3]) should return [3,3]");

  if (WeatherResultTools::max(result2, result3) != WeatherResult(3, 3))
    TEST_FAILED("max([2,2],[3,3]) should return [3,3]");

  if (WeatherResultTools::max(result1, result2, result3) != WeatherResult(3, 3))
    TEST_FAILED("max([1,1],[2,2],[3,3]) should return [3,3]");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test WeatherResultTools::mean
 */
// ----------------------------------------------------------------------

void mean()
{
  using namespace TextGen;

  WeatherResult result1(1, 1);
  WeatherResult result2(2, 2);
  WeatherResult result3(3, 3);

  if (WeatherResultTools::mean(result1, result2) != WeatherResult(1.5, 2))
    TEST_FAILED("mean([1,1],[2,2]) should return [1.5,2]");

  if (WeatherResultTools::mean(result1, result3) != WeatherResult(2, 3))
    TEST_FAILED("mean([1,1],[3,3]) should return [2,3]");

  if (WeatherResultTools::mean(result2, result3) != WeatherResult(2.5, 3))
    TEST_FAILED("mean([2,2],[3,3]) should return [2.5,3]");

  if (WeatherResultTools::mean(result1, result2, result3) != WeatherResult(2, 3))
    TEST_FAILED("mean([1,1],[2,2],[3,3]) should return [2,3]");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test WeatherResultTools::isSimilarRange
 */
// ----------------------------------------------------------------------

void isSimilarRange()
{
  using namespace TextGen;

  const string var = "X";

  WeatherResult result1(1, 0);
  WeatherResult result2(2, 0);
  WeatherResult result3(3, 0);
  WeatherResult result4(4, 0);
  WeatherResult result5(5, 0);

  if (!WeatherResultTools::isSimilarRange(result1, result2, result1, result2, var))
    TEST_FAILED("1-2 is always similar to 1-2");

  if (WeatherResultTools::isSimilarRange(result1, result2, result1, result3, var))
    TEST_FAILED("1-2 is not similar to 1-3 by default");

  Settings::set(var + "::same::minimum", "1");
  Settings::set(var + "::same::maximum", "1");

  if (!WeatherResultTools::isSimilarRange(result1, result2, result1, result3, var))
    TEST_FAILED("1-2 is similar to 1-3 with accuracy 1");

  if (!WeatherResultTools::isSimilarRange(result2, result3, result1, result4, var))
    TEST_FAILED("2-3 is similar to 1-4 with accuracy 1");

  if (WeatherResultTools::isSimilarRange(result2, result3, result1, result5, var))
    TEST_FAILED("2-3 is not similar to 1-5 with accuracy 1");

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
  void test(void)
  {
    TEST(min);
    TEST(max);
    TEST(mean);
    TEST(isSimilarRange);
  }

};  // class tests

}  // namespace WeatherResultToolsTest

int main(void)
{
  using namespace WeatherResultToolsTest;

  cout << endl << "WeatherResultTools tests" << endl << "========================" << endl;

  tests t;
  return t.run();
}
