#include "tframe.h"
#include "WeatherPeriod.h"

using namespace std;

namespace WeatherPeriodTest
{

  //! Test structors
  void structors(void)
  {
	using namespace WeatherAnalysis;

	NFmiTime time1;
	NFmiTime time2;

	// Should succeed
	WeatherPeriod s(time1,time2);
	// Should succeed
	WeatherPeriod s2(s);
	// Should succeed
	s2 = s;

	TEST_PASSED();
  }

  //! Test accessors()
  void accessors(void)
  {
	using namespace WeatherAnalysis;

	NFmiTime time1;
	NFmiTime time2;

	WeatherPeriod period(time1,time2);
	if(!period.startTime().IsEqual(time1))
	  TEST_FAILED("startTime() accessor failed");
	if(!period.endTime().IsEqual(time2))
	  TEST_FAILED("endTime() accessor failed");

	TEST_PASSED();
  }

  //! The actual test driver
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
	  TEST(structors);
	  TEST(accessors);
	}

  }; // class tests

} // namespace WeatherPeriodTest


int main(void)
{
  cout << endl
	   << "WeatherPeriod tester" << endl
	   << "====================" << endl;
  WeatherPeriodTest::tests t;
  return t.run();
}
