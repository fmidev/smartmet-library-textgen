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
	  TEST_FAILED("June 3 2003 should return '2-na'");
	if(WeekdayTools::on_weekday(NFmiTime(2003,6,4)) != "3-na")
	  TEST_FAILED("June 4 2003 should return '3-na'");
	if(WeekdayTools::on_weekday(NFmiTime(2003,6,5)) != "4-na")
	  TEST_FAILED("June 5 2003 should return '4-na'");
	if(WeekdayTools::on_weekday(NFmiTime(2003,6,6)) != "5-na")
	  TEST_FAILED("June 6 2003 should return '5-na'");
	if(WeekdayTools::on_weekday(NFmiTime(2003,6,7)) != "6-na")
	  TEST_FAILED("June 7 2003 should return '6-na'");
	if(WeekdayTools::on_weekday(NFmiTime(2003,6,8)) != "7-na")
	  TEST_FAILED("June 8 2003 should return '7-na'");
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

	if(WeekdayTools::night_against_weekday(NFmiTime(2003,6,1)) != "7-vastaisena y�n�")
	  TEST_FAILED("June 1 2003 should return '7-vastaisena y�n�'");
	if(WeekdayTools::night_against_weekday(NFmiTime(2003,6,2)) != "1-vastaisena y�n�")
	  TEST_FAILED("June 2 2003 should return '1-vastaisena y�n�'");
	if(WeekdayTools::night_against_weekday(NFmiTime(2003,6,3)) != "2-vastaisena y�n�")
	  TEST_FAILED("June 3 2003 should return '2-vastaisena y�n�'");
	if(WeekdayTools::night_against_weekday(NFmiTime(2003,6,4)) != "3-vastaisena y�n�")
	  TEST_FAILED("June 4 2003 should return '3-vastaisena y�n�'");
	if(WeekdayTools::night_against_weekday(NFmiTime(2003,6,5)) != "4-vastaisena y�n�")
	  TEST_FAILED("June 5 2003 should return '4-vastaisena y�n�'");
	if(WeekdayTools::night_against_weekday(NFmiTime(2003,6,6)) != "5-vastaisena y�n�")
	  TEST_FAILED("June 6 2003 should return '5-vastaisena y�n�'");
	if(WeekdayTools::night_against_weekday(NFmiTime(2003,6,7)) != "6-vastaisena y�n�")
	  TEST_FAILED("June 7 2003 should return '6-vastaisena y�n�'");
	if(WeekdayTools::night_against_weekday(NFmiTime(2003,6,8)) != "7-vastaisena y�n�")
	  TEST_FAILED("June 8 2003 should return '7-vastaisena y�n�'");
	if(WeekdayTools::night_against_weekday(NFmiTime(2003,6,9)) != "1-vastaisena y�n�")
	  TEST_FAILED("June 9 2003 should return '1-vastaisena y�n�'");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WeekdayTools::until_weekday_morning()
   */
  // ----------------------------------------------------------------------

  void until_weekday_morning()
  {
	using namespace TextGen;

	if(WeekdayTools::until_weekday_morning(NFmiTime(2003,6,1)) != "7-aamuun")
	  TEST_FAILED("June 1 2003 should return '7-aamuun'");
	if(WeekdayTools::until_weekday_morning(NFmiTime(2003,6,2)) != "1-aamuun")
	  TEST_FAILED("June 2 2003 should return '1-aamuun'");
	if(WeekdayTools::until_weekday_morning(NFmiTime(2003,6,3)) != "2-aamuun")
	  TEST_FAILED("June 3 2003 should return '2-aamuun'");
	if(WeekdayTools::until_weekday_morning(NFmiTime(2003,6,4)) != "3-aamuun")
	  TEST_FAILED("June 4 2003 should return '3-aamuun'");
	if(WeekdayTools::until_weekday_morning(NFmiTime(2003,6,5)) != "4-aamuun")
	  TEST_FAILED("June 5 2003 should return '4-aamuun'");
	if(WeekdayTools::until_weekday_morning(NFmiTime(2003,6,6)) != "5-aamuun")
	  TEST_FAILED("June 6 2003 should return '5-aamuun'");
	if(WeekdayTools::until_weekday_morning(NFmiTime(2003,6,7)) != "6-aamuun")
	  TEST_FAILED("June 7 2003 should return '6-aamuun'");
	if(WeekdayTools::until_weekday_morning(NFmiTime(2003,6,8)) != "7-aamuun")
	  TEST_FAILED("June 8 2003 should return '7-aamuun'");
	if(WeekdayTools::until_weekday_morning(NFmiTime(2003,6,9)) != "1-aamuun")
	  TEST_FAILED("June 9 2003 should return '1-aamuun'");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WeekdayTools::until_weekday_evening()
   */
  // ----------------------------------------------------------------------

  void until_weekday_evening()
  {
	using namespace TextGen;

	if(WeekdayTools::until_weekday_evening(NFmiTime(2003,6,1)) != "7-iltaan")
	  TEST_FAILED("June 1 2003 should return '7-iltaan'");
	if(WeekdayTools::until_weekday_evening(NFmiTime(2003,6,2)) != "1-iltaan")
	  TEST_FAILED("June 2 2003 should return '1-iltaan'");
	if(WeekdayTools::until_weekday_evening(NFmiTime(2003,6,3)) != "2-iltaan")
	  TEST_FAILED("June 3 2003 should return '2-iltaan'");
	if(WeekdayTools::until_weekday_evening(NFmiTime(2003,6,4)) != "3-iltaan")
	  TEST_FAILED("June 4 2003 should return '3-iltaan'");
	if(WeekdayTools::until_weekday_evening(NFmiTime(2003,6,5)) != "4-iltaan")
	  TEST_FAILED("June 5 2003 should return '4-iltaan'");
	if(WeekdayTools::until_weekday_evening(NFmiTime(2003,6,6)) != "5-iltaan")
	  TEST_FAILED("June 6 2003 should return '5-iltaan'");
	if(WeekdayTools::until_weekday_evening(NFmiTime(2003,6,7)) != "6-iltaan")
	  TEST_FAILED("June 7 2003 should return '6-iltaan'");
	if(WeekdayTools::until_weekday_evening(NFmiTime(2003,6,8)) != "7-iltaan")
	  TEST_FAILED("June 8 2003 should return '7-iltaan'");
	if(WeekdayTools::until_weekday_evening(NFmiTime(2003,6,9)) != "1-iltaan")
	  TEST_FAILED("June 9 2003 should return '1-iltaan'");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WeekdayTools::from_weekday_morning()
   */
  // ----------------------------------------------------------------------

  void from_weekday_morning()
  {
	using namespace TextGen;

	if(WeekdayTools::from_weekday_morning(NFmiTime(2003,6,1)) != "7-aamusta")
	  TEST_FAILED("June 1 2003 should return '7-aamusta'");
	if(WeekdayTools::from_weekday_morning(NFmiTime(2003,6,2)) != "1-aamusta")
	  TEST_FAILED("June 2 2003 should return '1-aamusta'");
	if(WeekdayTools::from_weekday_morning(NFmiTime(2003,6,3)) != "2-aamusta")
	  TEST_FAILED("June 3 2003 should return '2-aamusta'");
	if(WeekdayTools::from_weekday_morning(NFmiTime(2003,6,4)) != "3-aamusta")
	  TEST_FAILED("June 4 2003 should return '3-aamusta'");
	if(WeekdayTools::from_weekday_morning(NFmiTime(2003,6,5)) != "4-aamusta")
	  TEST_FAILED("June 5 2003 should return '4-aamusta'");
	if(WeekdayTools::from_weekday_morning(NFmiTime(2003,6,6)) != "5-aamusta")
	  TEST_FAILED("June 6 2003 should return '5-aamusta'");
	if(WeekdayTools::from_weekday_morning(NFmiTime(2003,6,7)) != "6-aamusta")
	  TEST_FAILED("June 7 2003 should return '6-aamusta'");
	if(WeekdayTools::from_weekday_morning(NFmiTime(2003,6,8)) != "7-aamusta")
	  TEST_FAILED("June 8 2003 should return '7-aamusta'");
	if(WeekdayTools::from_weekday_morning(NFmiTime(2003,6,9)) != "1-aamusta")
	  TEST_FAILED("June 9 2003 should return '1-aamusta'");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WeekdayTools::from_weekday_evening()
   */
  // ----------------------------------------------------------------------

  void from_weekday_evening()
  {
	using namespace TextGen;

	if(WeekdayTools::from_weekday_evening(NFmiTime(2003,6,1)) != "7-illasta")
	  TEST_FAILED("June 1 2003 should return '7-illasta'");
	if(WeekdayTools::from_weekday_evening(NFmiTime(2003,6,2)) != "1-illasta")
	  TEST_FAILED("June 2 2003 should return '1-illasta'");
	if(WeekdayTools::from_weekday_evening(NFmiTime(2003,6,3)) != "2-illasta")
	  TEST_FAILED("June 3 2003 should return '2-illasta'");
	if(WeekdayTools::from_weekday_evening(NFmiTime(2003,6,4)) != "3-illasta")
	  TEST_FAILED("June 4 2003 should return '3-illasta'");
	if(WeekdayTools::from_weekday_evening(NFmiTime(2003,6,5)) != "4-illasta")
	  TEST_FAILED("June 5 2003 should return '4-illasta'");
	if(WeekdayTools::from_weekday_evening(NFmiTime(2003,6,6)) != "5-illasta")
	  TEST_FAILED("June 6 2003 should return '5-illasta'");
	if(WeekdayTools::from_weekday_evening(NFmiTime(2003,6,7)) != "6-illasta")
	  TEST_FAILED("June 7 2003 should return '6-illasta'");
	if(WeekdayTools::from_weekday_evening(NFmiTime(2003,6,8)) != "7-illasta")
	  TEST_FAILED("June 8 2003 should return '7-illasta'");
	if(WeekdayTools::from_weekday_evening(NFmiTime(2003,6,9)) != "1-illasta")
	  TEST_FAILED("June 9 2003 should return '1-illasta'");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WeekdayTools::until_weekday_time()
   */
  // ----------------------------------------------------------------------

  void until_weekday_time()
  {
	using namespace TextGen;

	if(WeekdayTools::until_weekday_time(NFmiTime(2003,6,1,6)) != "7-aamuun")
	  TEST_FAILED("June 1 2003 06:00 should return '7-aamuun'");

	if(WeekdayTools::until_weekday_time(NFmiTime(2003,6,1,18)) != "7-iltaan")
	  TEST_FAILED("June 1 2003 18:00 should return '7-iltaan'");

	try
	  {
		WeekdayTools::until_weekday_time(NFmiTime(2003,6,1));
		TEST_FAILED("June 1 2003 00:00 should throw");
	  }
	catch(...)

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test WeekdayTools::from_weekday_time()
   */
  // ----------------------------------------------------------------------

  void from_weekday_time()
  {
	using namespace TextGen;

	if(WeekdayTools::from_weekday_time(NFmiTime(2003,6,1,6)) != "7-aamusta")
	  TEST_FAILED("June 1 2003 06:00 should return '7-aamusta'");

	if(WeekdayTools::from_weekday_time(NFmiTime(2003,6,1,18)) != "7-illasta")
	  TEST_FAILED("June 1 2003 18:00 should return '7-illasta'");

	try
	  {
		WeekdayTools::from_weekday_time(NFmiTime(2003,6,1));
		TEST_FAILED("June 1 2003 00:00 should throw");
	  }
	catch(...)

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
	  TEST(until_weekday_morning);
	  TEST(until_weekday_evening);
	  TEST(from_weekday_morning);
	  TEST(from_weekday_evening);
	  TEST(until_weekday_time);
	  TEST(from_weekday_time);
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