#include "tframe.h"
#include "WeekdayTools.h"
#include "NFmiTime.h"

using namespace std;

namespace WeekdayToolsTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WeekdayTools::on_weekday()
   */
  // ----------------------------------------------------------------------

  void on_weekday()
  {
	using namespace TextGen;

	if(WeekdayTools::on_weekday(NFmiTime(2003,6,1)) != "7-na")
	  TEST_FAILED("June 1 2003 should return '7-na'");
	if(WeekdayTools::on_weekday(NFmiTime(2003,6,2)) != "1-na")
	  TEST_FAILED("June 2 2003 should return '1-na'");
	if(WeekdayTools::on_weekday(NFmiTime(2003,6,3)) != "2-na")
	  TEST_FAILED("June 3 2003 should return '1-na'");
	if(WeekdayTools::on_weekday(NFmiTime(2003,6,4)) != "3-na")
	  TEST_FAILED("June 4 2003 should return '1-na'");
	if(WeekdayTools::on_weekday(NFmiTime(2003,6,5)) != "4-na")
	  TEST_FAILED("June 5 2003 should return '1-na'");
	if(WeekdayTools::on_weekday(NFmiTime(2003,6,6)) != "5-na")
	  TEST_FAILED("June 6 2003 should return '1-na'");
	if(WeekdayTools::on_weekday(NFmiTime(2003,6,7)) != "6-na")
	  TEST_FAILED("June 7 2003 should return '1-na'");
	if(WeekdayTools::on_weekday(NFmiTime(2003,6,8)) != "7-na")
	  TEST_FAILED("June 8 2003 should return '1-na'");
	if(WeekdayTools::on_weekday(NFmiTime(2003,6,9)) != "1-na")
	  TEST_FAILED("June 9 2003 should return '1-na'");

	TEST_PASSED();
  }


  // ----------------------------------------------------------------------
  /*!
   * \brief Test WeekdayTools::night_against_weekday()
   */
  // ----------------------------------------------------------------------

  void night_against_weekday()
  {
	using namespace TextGen;

	if(WeekdayTools::night_against_weekday(NFmiTime(2003,6,1)) != "7-vastaisena yönä")
	  TEST_FAILED("June 1 2003 should return '7-vastaisena yönä'");
	if(WeekdayTools::night_against_weekday(NFmiTime(2003,6,2)) != "1-vastaisena yönä")
	  TEST_FAILED("June 2 2003 should return '1-vastaisena yönä'");
	if(WeekdayTools::night_against_weekday(NFmiTime(2003,6,3)) != "2-vastaisena yönä")
	  TEST_FAILED("June 3 2003 should return '1-vastaisena yönä'");
	if(WeekdayTools::night_against_weekday(NFmiTime(2003,6,4)) != "3-vastaisena yönä")
	  TEST_FAILED("June 4 2003 should return '1-vastaisena yönä'");
	if(WeekdayTools::night_against_weekday(NFmiTime(2003,6,5)) != "4-vastaisena yönä")
	  TEST_FAILED("June 5 2003 should return '1-vastaisena yönä'");
	if(WeekdayTools::night_against_weekday(NFmiTime(2003,6,6)) != "5-vastaisena yönä")
	  TEST_FAILED("June 6 2003 should return '1-vastaisena yönä'");
	if(WeekdayTools::night_against_weekday(NFmiTime(2003,6,7)) != "6-vastaisena yönä")
	  TEST_FAILED("June 7 2003 should return '1-vastaisena yönä'");
	if(WeekdayTools::night_against_weekday(NFmiTime(2003,6,8)) != "7-vastaisena yönä")
	  TEST_FAILED("June 8 2003 should return '1-vastaisena yönä'");
	if(WeekdayTools::night_against_weekday(NFmiTime(2003,6,9)) != "1-vastaisena yönä")
	  TEST_FAILED("June 9 2003 should return '1-vastaisena yönä'");

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
	  TEST(on_weekday);
	  TEST(night_against_weekday);
	}

  }; // class tests

} // namespace WeekdayToolsTest


int main(void)
{
  cout << endl
	   << "WeekdayTools tester" << endl
	   << "===================" << endl;
  WeekdayToolsTest::tests t;
  return t.run();
}
