#include "tframe.h"
#include "WeatherHourPeriodGenerator.h"
#include "WeatherPeriod.h"
#include "NFmiTime.h"

using namespace std;

namespace WeatherPeriodToolsTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WeatherHourPeriodGenerator::size()
   */
  // ----------------------------------------------------------------------

  void size(void)
  {
	using namespace WeatherAnalysis;

	// A simple period from 12 to 17
	{
	  WeatherPeriod period(NFmiTime(2000,1,1,12,0),
						   NFmiTime(2000,1,1,17,0));

	  {
		WeatherHourPeriodGenerator generator(period,12,18,12,18);
		if(generator.size() != 0)
		  TEST_FAILED("Size should be zero - 12-18 does not fit into 12-17");
	  }

	  {
		WeatherHourPeriodGenerator generator(period,12,18,12,17);
		if(generator.size() != 1)
		  TEST_FAILED("Size should be zero - 12-17 does not fit into 12-18(17)");
	  }
	}

	// A two-day period from 12 to 17
	{
	  WeatherPeriod period(NFmiTime(2000,1,1,12,0),
						   NFmiTime(2000,1,2,17,0));


	  {
		WeatherHourPeriodGenerator generator(period,12,18,12,18);
		if(generator.size() != 1)
		  TEST_FAILED("Size should be 1, 12-18 fits into first day");
	  }

	  {
		WeatherHourPeriodGenerator generator(period,12,18,12,17);
		if(generator.size() != 2)
		  TEST_FAILED("Size should be 2, 12-18 fits into first day, 12-17 into second");
	  }

	}

	// A three-day period from 12 to 17
	{
	  WeatherPeriod period(NFmiTime(2000,1,1,12,0),
						   NFmiTime(2000,1,3,17,0));

	  {
		WeatherHourPeriodGenerator generator(period,12,18,12,18);
		if(generator.size() != 2)
		  TEST_FAILED("Size should be 2, 12-18 fits into first and second days");
	  }

	  {
		WeatherHourPeriodGenerator generator(period,12,18,12,17);
		if(generator.size() != 3)
		  TEST_FAILED("Size should be 3, 12-18 fits into first and second days, 12-17 into third");
	  }

	}

	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WeatherHourPeriodGenerator::period()
   */
  // ----------------------------------------------------------------------

  void period(void)
  {
	using namespace WeatherAnalysis;
	
	// A simple period from 12 to 17
	{
	  WeatherPeriod period(NFmiTime(2000,1,1,12,0),
						   NFmiTime(2000,1,1,17,0));
	  
	  {
		WeatherHourPeriodGenerator generator(period,12,18,12,18);
		try
		  {
			generator.period(1);
			TEST_FAILED("period(1) should have failed - 12-18 does not fit into 12-17");
		  }
		catch(...) { }
	  }

	  {
		WeatherHourPeriodGenerator generator(period,12,18,12,17);
		
		WeatherPeriod p = generator.period(1);
		if(p != WeatherPeriod(NFmiTime(2000,1,1,12),NFmiTime(2000,1,1,17)))
		  TEST_FAILED("Failed to generate correct first 12-17 period");
	  }
	}

	// A two-day period from 12 to 17
	{
	  WeatherPeriod period(NFmiTime(2000,1,1,12,0),
						   NFmiTime(2000,1,2,17,0));


	  {
		WeatherHourPeriodGenerator generator(period,12,18,12,18);
		WeatherPeriod p = generator.period(1);
		if(p != WeatherPeriod(NFmiTime(2000,1,1,12),NFmiTime(2000,1,1,18)))
		  TEST_FAILED("Failed to generate correct first 12-18 period");
	  }

	  {
		WeatherHourPeriodGenerator generator(period,12,18,12,17);
		WeatherPeriod p = generator.period(1);
		if(p != WeatherPeriod(NFmiTime(2000,1,1,12),NFmiTime(2000,1,1,18)))
		  TEST_FAILED("Failed to generate correct first/2 12-18 period");
		p = generator.period(2);
		if(p != WeatherPeriod(NFmiTime(2000,1,2,12),NFmiTime(2000,1,2,17)))
		  TEST_FAILED("Failed to generate correct second/2 12-17 period");
	  }

	}

	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * The actual test driver
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
	  TEST(size);
	  TEST(period);
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