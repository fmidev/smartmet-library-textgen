#include "regression/tframe.h"
#include "WeatherPeriodTools.h"
#include "WeatherPeriod.h"
#include "NFmiTime.h"

using namespace std;

namespace WeatherPeriodToolsTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WeatherPeriodTools::hours
   */
  // ----------------------------------------------------------------------

  void hours(void)
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WeatherPeriodTools::countPeriods
   */
  // ----------------------------------------------------------------------

  void countperiods(void)
  {
	using namespace WeatherAnalysis;

	// A simple period from 12 to 17
	{
	  WeatherPeriod period(NFmiTime(2000,1,1,12,0),
						   NFmiTime(2000,1,1,17,0));

	  if(WeatherPeriodTools::countPeriods(period,06,18,06,18) != 0)
		TEST_FAILED("Failed to get 0 subperiod 06-18");

	  if(WeatherPeriodTools::countPeriods(period,06,18,12,18) != 0)
		TEST_FAILED("Failed to get 0 subperiod 12-18");

	  if(WeatherPeriodTools::countPeriods(period,06,18,12,17) != 1)
		TEST_FAILED("Failed to get 1 subperiod 12-17");
	}

	// A two-day period from 12 to 17
	{
	  WeatherPeriod period(NFmiTime(2000,1,1,12,0),
						   NFmiTime(2000,1,2,17,0));

	  if(WeatherPeriodTools::countPeriods(period,06,18,06,18) != 0)
		TEST_FAILED("Failed to get 0 subperiod 06-18");

	  if(WeatherPeriodTools::countPeriods(period,06,18,12,18) != 1)
		TEST_FAILED("Failed to get 1 subperiod 12-18");

	  if(WeatherPeriodTools::countPeriods(period,06,18,12,17) != 2)
		TEST_FAILED("Failed to get 2 subperiod 12-17");

	  if(WeatherPeriodTools::countPeriods(period,18,06,18,06) != 1)
		TEST_FAILED("Failed to get 1 subperiod 18-06");
	}

	// A three-day period from 12 to 17
	{
	  WeatherPeriod period(NFmiTime(2000,1,1,12,0),
						   NFmiTime(2000,1,3,17,0));

	  if(WeatherPeriodTools::countPeriods(period,06,18,06,18) != 1)
		TEST_FAILED("Failed to get 1 subperiod 06-18");

	  if(WeatherPeriodTools::countPeriods(period,06,18,12,18) != 2)
		TEST_FAILED("Failed to get 2 subperiod 12-18");

	  if(WeatherPeriodTools::countPeriods(period,06,18,12,17) != 3)
		TEST_FAILED("Failed to get 3 subperiod 12-17");

	  if(WeatherPeriodTools::countPeriods(period,18,06,18,06) != 2)
		TEST_FAILED("Failed to get 2 subperiod 18-06");

	}

	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WeatherPeriodTools::getPeriod
   */
  // ----------------------------------------------------------------------

  void getperiod(void)
  {
	using namespace WeatherAnalysis;
	using namespace WeatherAnalysis::WeatherPeriodTools;

	// A simple period from 12 to 17
	{
	  WeatherPeriod period(NFmiTime(2000,1,1,12,0),
						   NFmiTime(2000,1,1,17,0));

	  try
		{
		  getPeriod(period,0,06,18,06,18);
		  TEST_FAILED("getPeriod(0) should throw");
		}
	  catch(...) { }
		
	  try
		{
		  getPeriod(period,1,06,18,12,18);
		  TEST_FAILED("getPeriod(1) should fail for subperiod 12-18");
		}
	  catch(...) { }
	  
	  {
		WeatherPeriod expected = WeatherPeriod(NFmiTime(2000,1,1,12,0),
											   NFmiTime(2000,1,1,17,0));
		WeatherPeriod result = getPeriod(period,1,06,18,12,17);
		if(expected != result)
		  TEST_FAILED("Failed to get 1st subperiod 12-17");
	  }
	}

	// A two-day period from 12 to 17
	{
	  WeatherPeriod period(NFmiTime(2000,1,1,12,0),
						   NFmiTime(2000,1,2,17,0));

	  {
		WeatherPeriod expected = WeatherPeriod(NFmiTime(2000,1,1,12,0),
											   NFmiTime(2000,1,1,18,0));
		WeatherPeriod result = getPeriod(period,1,06,18,12,18);
		if(result != expected)
		  TEST_FAILED("Failed to get 1/1 subperiod 12-18");
	  }

	  {
		WeatherPeriod expected = WeatherPeriod(NFmiTime(2000,1,1,12,0),
											   NFmiTime(2000,1,1,18,0));
		WeatherPeriod result = getPeriod(period,1,06,18,12,17);
		if(result != expected)
		  TEST_FAILED("Failed to get 1/2 subperiod 12-18");
	  }

	  {
		WeatherPeriod expected = WeatherPeriod(NFmiTime(2000,1,2,06,0),
											   NFmiTime(2000,1,2,17,0));
		WeatherPeriod result = getPeriod(period,2,06,18,12,17);
		if(result != expected)
		  TEST_FAILED("Failed to get 2/2 subperiod 06-17");
	  }

	  {
		WeatherPeriod expected = WeatherPeriod(NFmiTime(2000,1,1,18,0),
											   NFmiTime(2000,1,2,06,0));
		WeatherPeriod result = getPeriod(period,1,18,06,18,06);
		
		if(result != expected)
		  TEST_FAILED("Failed to get 1/1 subperiod 18-06");
		
	  }
	  
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
	  TEST(hours);
	  TEST(countperiods);
	  TEST(getperiod);
	}

  }; // class tests

} // namespace WeatherPeriodToolsTest


int main(void)
{
  cout << endl
	   << "WeatherPeriodTools tester" << endl
	   << "====================" << endl;
  WeatherPeriodToolsTest::tests t;
  return t.run();
}
