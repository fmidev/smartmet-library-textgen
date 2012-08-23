#include <regression/tframe.h>
#include "TimeTools.h"
#include <newbase/NFmiTime.h>

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
	using TextGen::TimeTools::isSameDay;

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
	using TextGen::TimeTools::isNextDay;

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
	using TextGen::TimeTools::isSeveralDays;

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
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Test TimeTools::dayStart()
   */
  // ----------------------------------------------------------------------

  void day_start()
  {
	using TextGen::TimeTools::dayStart;
	if(!dayStart(NFmiTime(2003,9,29)).IsEqual(NFmiTime(2003,9,29)))
	  TEST_FAILED("Failed to round down 29.09.2003 00:00");
	if(!dayStart(NFmiTime(2003,9,29,1)).IsEqual(NFmiTime(2003,9,29)))
	  TEST_FAILED("Failed to round down 29.09.2003 01:00");
	if(!dayStart(NFmiTime(2003,9,29,0,1)).IsEqual(NFmiTime(2003,9,29)))
	  TEST_FAILED("Failed to round down 29.09.2003 00:01");
	if(!dayStart(NFmiTime(2003,9,29,0,0,1)).IsEqual(NFmiTime(2003,9,29)))
	  TEST_FAILED("Failed to round down 29.09.2003 00:00:01");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TimeTools::dayEnd()
   */
  // ----------------------------------------------------------------------

  void day_end()
  {
	using TextGen::TimeTools::dayEnd;

	if(!dayEnd(NFmiTime(2003,9,29)).IsEqual(NFmiTime(2003,9,29)))
	  TEST_FAILED("Failed to round up 29.09.2003 00:00");
	if(!dayEnd(NFmiTime(2003,9,29,1)).IsEqual(NFmiTime(2003,9,30)))
	  TEST_FAILED("Failed to round up 29.09.2003 01:00");
	if(!dayEnd(NFmiTime(2003,9,29,0,1)).IsEqual(NFmiTime(2003,9,30)))
	  TEST_FAILED("Failed to round up 29.09.2003 00:01");
	if(!dayEnd(NFmiTime(2003,9,29,0,0,1)).IsEqual(NFmiTime(2003,9,30)))
	  TEST_FAILED("Failed to round up 29.09.2003 00:00:01");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TimeTools::toUtcTime()
   */
  // ----------------------------------------------------------------------

  void toutctime()
  {
	using TextGen::TimeTools::toUtcTime;

	const char * hki = "TZ=Europe/Helsinki";
	putenv(const_cast<char *>(hki));
	tzset();
	if(!toUtcTime(NFmiTime(2003,11,7,12)).IsEqual(NFmiTime(2003,11,7,10)))
	  TEST_FAILED("Failed to convert 7.11.2003 12:00 FIN to 10:00 UTC");
	if(!toUtcTime(NFmiTime(2003,9,7,12)).IsEqual(NFmiTime(2003,9,7,9)))
	  TEST_FAILED("Failed to convert 7.9.2003 12:00 FIN to 9:00 UTC");

	const char * sto = "TZ=Europe/Stockholm";
	putenv(const_cast<char *>(sto));
	tzset();
	if(!toUtcTime(NFmiTime(2003,11,7,12)).IsEqual(NFmiTime(2003,11,7,11)))
	  TEST_FAILED("Failed to convert 7.11.2003 12:00 SWE to 11:00 UTC");
	if(!toUtcTime(NFmiTime(2003,9,7,12)).IsEqual(NFmiTime(2003,9,7,10)))
	  TEST_FAILED("Failed to convert 7.9.2003 12:00 SWE to 10:00 UTC");

	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TimeTools::toLocalTime()
   */
  // ----------------------------------------------------------------------

  void tolocaltime()
  {
	using TextGen::TimeTools::toLocalTime;

	const char * hki = "TZ=Europe/Helsinki";
	putenv(const_cast<char *>(hki));
	tzset();
	if(!toLocalTime(NFmiTime(2003,11,7,12)).IsEqual(NFmiTime(2003,11,7,14)))
	  TEST_FAILED("Failed to convert 7.11.2003 12:00 UTC to 14:00 UTC");
	if(!toLocalTime(NFmiTime(2003,9,7,12)).IsEqual(NFmiTime(2003,9,7,15)))
	  TEST_FAILED("Failed to convert 7.9.2003 12:00 UTC to 15:00 UTC");

	const char * sto = "TZ=Europe/Stockholm";
	putenv(const_cast<char *>(sto));
	tzset();
	if(!toLocalTime(NFmiTime(2003,11,7,12)).IsEqual(NFmiTime(2003,11,7,13)))
	  TEST_FAILED("Failed to convert 7.11.2003 12:00 UTC to 13:00 SWE");
	if(!toLocalTime(NFmiTime(2003,9,7,12)).IsEqual(NFmiTime(2003,9,7,14)))
	  TEST_FAILED("Failed to convert 7.9.2003 12:00 UTC to 14:00 UTC");

	TEST_PASSED();

  }

  //! The actual test driver
  class tests : public tframe::tests
  {
	//! Overridden message separator
	virtual const char * error_message_prefix() const
	{
	  return "\n\t";
	}

	//! Main test suite
	void test(void)
	{
	  TEST(is_same_day);
	  TEST(is_next_day);
	  TEST(is_several_days);
	  TEST(day_start);
	  TEST(day_end);

	  TEST(toutctime);
	  TEST(tolocaltime);

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
