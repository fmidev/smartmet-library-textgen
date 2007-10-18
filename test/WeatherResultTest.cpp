#include <regression/tframe.h>
#include "WeatherResult.h"

using namespace std;

namespace WeatherResultTest
{

  //! Test structors
  void structors(void)
  {
	using namespace WeatherAnalysis;

	// Should succeed
	WeatherResult s(1,0.5);
	// Should succeed
	WeatherResult s2(s);
	// Should succeed
	s2 = s;
	
	TEST_PASSED();
  }

  //! Test accessors()
  void accessors(void)
  {
	using namespace WeatherAnalysis;

	WeatherResult result(1,0.5);
	if(result.value() != 1)
	  TEST_FAILED("value() accessor failed");
	if(result.error() != 0.5)
	  TEST_FAILED("error() accessor failed");

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

} // namespace WeatherResultTest


int main(void)
{
  cout << endl
	   << "WeatherResult tester" << endl
	   << "====================" << endl;
  WeatherResultTest::tests t;
  return t.run();
}
