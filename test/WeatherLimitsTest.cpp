#include "tframe.h"
#include "WeatherLimits.h"

using namespace std;

namespace WeatherLimitsTest
{

  //! Test structors
  void structors(void)
  {
	using namespace WeatherAnalysis;

	// Should succeed
	WeatherLimits s;
	// Should succeed
	WeatherLimits s2(s);
	// Should succeed
	s2 = s;
	
	TEST_PASSED();
  }

  //! Test setting limits
  void setting(void)
  {
	using namespace WeatherAnalysis;

	WeatherLimits limits;

	if(limits.hasLowerLimit())
	  TEST_FAILED("lower limit cannot be on after construction");
	if(limits.hasUpperLimit())
	  TEST_FAILED("upper limit cannot be on after construction");
	
	limits.lowerLimit(1);
	if(!limits.hasLowerLimit())
	  TEST_FAILED("setting lower limit failed");
	if(limits.lowerLimit() != 1)
	  TEST_FAILED("setting lower limit value failed");

	limits.upperLimit(2);
	if(!limits.hasUpperLimit())
	  TEST_FAILED("setting upper limit failed");
	if(limits.upperLimit() != 2)
	  TEST_FAILED("setting upper limit value failed");

	TEST_PASSED();
  }

  //! Test testing limits
  void testing(void)
  {
	using namespace WeatherAnalysis;

	{
	  WeatherLimits limits;
	  limits.lowerLimit(1);
	  
	  if(limits.inLimits(0))
		TEST_FAILED("0 is not in range [1,inf)");
	  if(!limits.inLimits(2))
		TEST_FAILED("2 is in range [1,inf)");
	  if(!limits.inLimits(1))
		TEST_FAILED("1 is in range [1,inf)");
	}

	{
	  WeatherLimits limits;
	  limits.lowerLimit(1,false);

	  if(limits.inLimits(0))
		TEST_FAILED("0 is not in range (1,inf)");
	  if(!limits.inLimits(2))
		TEST_FAILED("2 is in range (1,inf)");
	  if(limits.inLimits(1))
		TEST_FAILED("1 is not in range (1,inf)");
	}

	{
	  WeatherLimits limits;
	  limits.upperLimit(2);
	  
	  if(!limits.inLimits(0))
		TEST_FAILED("0 is in range (-inf,2]");
	  if(limits.inLimits(3))
		TEST_FAILED("3 is not in range (-inf,2]");
	  if(!limits.inLimits(2))
		TEST_FAILED("2 is in range (-inf,2]");
	}

	{
	  WeatherLimits limits;
	  limits.upperLimit(2,false);
	  
	  if(!limits.inLimits(0))
		TEST_FAILED("0 is in range (-inf,2)");
	  if(limits.inLimits(3))
		TEST_FAILED("3 is not in range (-inf,2)");
	  if(limits.inLimits(2))
		TEST_FAILED("2 is not in range (-inf,2)");
	}

	{
	  WeatherLimits limits;
	  limits.lowerLimit(1);
	  limits.upperLimit(3);
	  
	  if(limits.inLimits(0))
		TEST_FAILED("0 is not in range [1,3]");
	  if(!limits.inLimits(2))
		TEST_FAILED("2 is in range [1,3]");
	  if(limits.inLimits(4))
		TEST_FAILED("4 is not in range [1,3]");
	}

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
	  TEST(setting);
	  TEST(testing);
	}

  }; // class tests

} // namespace WeatherLimitsTest


int main(void)
{
  cout << endl
	   << "WeatherLimits tester" << endl
	   << "====================" << endl;
  WeatherLimitsTest::tests t;
  return t.run();
}
