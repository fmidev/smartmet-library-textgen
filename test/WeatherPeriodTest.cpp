#include <regression/tframe.h>
#include "WeatherPeriod.h"

using namespace std;

namespace WeatherPeriodTest
{
//! Test structors
void structors(void)
{
  using namespace TextGen;

  TextGenPosixTime time1;
  TextGenPosixTime time2;

  // Should succeed
  WeatherPeriod s(time1, time2);
  // Should succeed
  WeatherPeriod s2(s);
  // Should succeed
  s2 = s;

  TEST_PASSED();
}

//! Test accessors
void accessors(void)
{
  using namespace TextGen;

  TextGenPosixTime time1(1999, 1, 1);
  TextGenPosixTime time2(2003, 12, 31);

  WeatherPeriod period(time1, time2);
  if (!period.localStartTime().IsEqual(time1)) TEST_FAILED("startTime() accessor failed");
  if (!period.localEndTime().IsEqual(time2)) TEST_FAILED("endTime() accessor failed");

  TEST_PASSED();
}

//! Test comparisons
void comparisons(void)
{
  using namespace TextGen;

  TextGenPosixTime time1(1999, 1, 1);
  TextGenPosixTime time2(2003, 12, 31);
  TextGenPosixTime time3(2004, 1, 1);

  WeatherPeriod period1(time1, time2);
  WeatherPeriod period2(time1, time3);
  WeatherPeriod period3(time2, time3);

  if (!(period1 == period1)) TEST_FAILED("period1==period1 test failed");
  if (period1 != period1) TEST_FAILED("period1!=period1 test failed");

  if (period1 == period2) TEST_FAILED("period1==period2 test failed");
  if (period1 == period3) TEST_FAILED("period1==period3 test failed");
  if (period2 == period3) TEST_FAILED("period2==period3 test failed");

  if (!(period1 != period2)) TEST_FAILED("period1!=period2 test failed");
  if (!(period1 != period3)) TEST_FAILED("period1!=period3 test failed");
  if (!(period2 != period3)) TEST_FAILED("period2!=period3 test failed");

  TEST_PASSED();
}

//! The actual test driver
class tests : public tframe::tests
{
  //! Overridden message separator
  virtual const char* error_message_prefix() const { return "\n\t"; }
  //! Main test suite
  void test(void)
  {
    TEST(structors);
    TEST(accessors);
    TEST(comparisons);
  }

};  // class tests

}  // namespace WeatherPeriodTest

int main(void)
{
  cout << endl << "WeatherPeriod tester" << endl << "====================" << endl;
  WeatherPeriodTest::tests t;
  return t.run();
}
