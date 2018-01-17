#include "SeasonTools.h"
#include <calculator/Settings.h>
#include <calculator/TextGenPosixTime.h>
#include <regression/tframe.h>

#include <newbase/NFmiSettings.h>

using namespace std;

namespace SeasonToolsTest
{
void season_tools_test()
{
  Settings::set("a::fake::winter::startdate", "1201");
  Settings::set("a::fake::winter::enddate", "0315");

  Settings::set("a::fake::summer::startdate", "0401");
  Settings::set("a::fake::summer::enddate", "0930");

  if (TextGen::SeasonTools::isWinter(TextGenPosixTime(2017, 11, 30), "a::fake") == true)
    TEST_FAILED("30.11.2017 should NOT be wintertime!");
  if (TextGen::SeasonTools::isWinter(TextGenPosixTime(2017, 12, 1), "a::fake") == false)
    TEST_FAILED("1.12.2017 should be wintertime!");
  if (TextGen::SeasonTools::isWinter(TextGenPosixTime(2017, 12, 10), "a::fake") == false)
    TEST_FAILED("12.10.2017 should be wintertime!");
  if (TextGen::SeasonTools::isWinter(TextGenPosixTime(2017, 12, 31), "a::fake") == false)
    TEST_FAILED("31.12.2017 should be wintertime!");
  if (TextGen::SeasonTools::isWinter(TextGenPosixTime(2018, 1, 1), "a::fake") == false)
    TEST_FAILED("1.1.2018 should be wintertime!");
  if (TextGen::SeasonTools::isWinter(TextGenPosixTime(2018, 3, 15), "a::fake") == false)
    TEST_FAILED("15.3.2018 should be wintertime!");
  if (TextGen::SeasonTools::isWinter(TextGenPosixTime(2018, 3, 16), "a::fake") == true)
    TEST_FAILED("16.3.2018 should NOT be wintertime!");

  if (TextGen::SeasonTools::isSummer(TextGenPosixTime(2017, 3, 31), "a::fake") == true)
    TEST_FAILED("31.3.2017 should NOT be summertime!");
  if (TextGen::SeasonTools::isSummer(TextGenPosixTime(2017, 4, 1), "a::fake") == false)
    TEST_FAILED("1.4.2017 should be summertime!");
  if (TextGen::SeasonTools::isSummer(TextGenPosixTime(2017, 5, 31), "a::fake") == false)
    TEST_FAILED("12.5.2017 should be summertime!");
  if (TextGen::SeasonTools::isSummer(TextGenPosixTime(2017, 9, 30), "a::fake") == false)
    TEST_FAILED("30.9.2017 should be summertime!");
  if (TextGen::SeasonTools::isSummer(TextGenPosixTime(2017, 10, 1), "a::fake") == true)
    TEST_FAILED("1.10.2017 should NOT be summertime!");

  TEST_PASSED();
}
// ----------------------------------------------------------------------
/*!
 * \brief The actual test driver
 */
// ----------------------------------------------------------------------

class tests : public tframe::tests
{
  //! Main test suite
  void test(void) { TEST(season_tools_test); }

};  // class tests

}  // namespace SeasonToolsTest

int main(void)
{
  using namespace SeasonToolsTest;

  cout << endl << "SeasonToolsTools tests" << endl << "===========================" << endl;

  tests t;
  return t.run();
}
