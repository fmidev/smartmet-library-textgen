#include "regression/tframe.h"
#include "TimeTools.h"
#include "NFmiTime.h"

using namespace std;

namespace TimeToolsTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TimeTools::isSameDay()
   */
  // ----------------------------------------------------------------------

  void is_same_day()
  {
	using WeatherAnalysis::TimeTools::isSameDay;

	NFmiTime date(2003,07,11);

	if(!isSameDay(date,NFmiTime(2003,07,11)))
	  TEST_FAILED("Failed for 11.07.2003");
	if(isSameDay(date,NFmiTime(2003,07,10)))
	  TEST_FAILED("Failed for 10.07.2003");
	if(isSameDay(date,NFmiTime(2003,06,11)))
	  TEST_FAILED("Failed for 11.06.2003");
	if(isSameDay(date,NFmiTime(2002,07,11)))
	  TEST_FAILED("Failed for 11.07.2002");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TimeTools::isNextDay()
   */
  // ----------------------------------------------------------------------

  void is_next_day()
  {
	using WeatherAnalysis::TimeTools::isNextDay;

	NFmiTime date(2003,07,11);
	
	if(isNextDay(date,NFmiTime(2003,07,11)))
	  TEST_FAILED("Failed for 11.07.2003");
	if(isNextDay(date,NFmiTime(2003,07,10)))
	  TEST_FAILED("Failed for 10.07.2003");
	if(!isNextDay(date,NFmiTime(2003,07,12)))
	  TEST_FAILED("Failed for 12.07.2003");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TimeTools::isSeveralDays()
   */
  // ----------------------------------------------------------------------

  void is_several_days()
  {
	using WeatherAnalysis::TimeTools::isSeveralDays;

	NFmiTime date(2003,07,11);
	
	if(isSeveralDays(date,NFmiTime(2003,07,11)))
	  TEST_FAILED("Failed for 11.07.2003");
	if(isSeveralDays(date,NFmiTime(2003,07,12)))
	  TEST_FAILED("Failed for 12.07.2003");
	if(!isSeveralDays(date,NFmiTime(2003,07,12,1)))
	  TEST_FAILED("Failed for 12.07.2003 01:00");
	if(!isSeveralDays(date,NFmiTime(2003,07,13)))
	  TEST_FAILED("Failed for 13.07.2003");

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
	  TEST(is_same_day);
	  TEST(is_next_day);
	  TEST(is_several_days);
	}

  }; // class tests

} // namespace TimeToolsTest


int main(void)
{
  cout << endl
	   << "TimeTools tester" << endl
	   << "================" << endl;
  TimeToolsTest::tests t;
  return t.run();
}
