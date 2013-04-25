#include <regression/tframe.h>
#include "QueryDataTools.h"
#include "TextGenPosixTime.h"

#include <newbase/NFmiQueryData.h>
#include <newbase/NFmiFastQueryInfo.h>
#include <stdexcept>

using namespace std;

namespace QueryDataToolsTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test QueryDataTools::firstTime
   */
  // ----------------------------------------------------------------------

  void firstTime()
  {
	using TextGen::QueryDataTools::firstTime;

	NFmiQueryData qd("data/skandinavia_pinta.sqd");

	NFmiFastQueryInfo qi(*qd.Info());

	if(firstTime(qi,TextGenPosixTime(1999,1,1),TextGenPosixTime(1999,1,2)))
	  TEST_FAILED("Should fail for 1.1.1999");

	if(!firstTime(qi,TextGenPosixTime(2003,8,15),TextGenPosixTime(2003,8,16)))
	  TEST_FAILED("Should succeed for 15.08.2003");

	if(firstTime(qi,TextGenPosixTime(2030,1,1),TextGenPosixTime(2030,1,2)))
	  TEST_FAILED("Should fail for 1.1.2030");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test QueryDataTools::lastTime
   */
  // ----------------------------------------------------------------------

  void lastTime()
  {
	using TextGen::QueryDataTools::lastTime;

	NFmiQueryData qd("data/skandinavia_pinta.sqd");

	NFmiFastQueryInfo qi = NFmiFastQueryInfo(*qd.Info());

	if(!lastTime(qi,TextGenPosixTime(1999,1,1)))
	  TEST_FAILED("Should succeed for 1.1.1999");

	if(!lastTime(qi,TextGenPosixTime(2003,8,15)))
	  TEST_FAILED("Should succeed for 15.08.2003");

	if(lastTime(qi,TextGenPosixTime(2030,1,1)))
	  TEST_FAILED("Should fail for 1.1.2030");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test QueryDataTools::findIndices
   */
  // ----------------------------------------------------------------------

  void findIndices()
  {
	using TextGen::QueryDataTools::findIndices;

	// The data has timesteps from 200308140600 to 200308220900
	NFmiQueryData qd("data/skandinavia_pinta.sqd");

	NFmiFastQueryInfo qi = NFmiFastQueryInfo(*qd.Info());

	unsigned long startindex, endindex;

	if(!findIndices(qi,
					TextGenPosixTime(2003,8,15),
					TextGenPosixTime(2003,8,16),
					startindex,
					endindex))
	  TEST_FAILED("Should succeed for 15.08.2003-16.08.2003");

	if(startindex != 21)
	  TEST_FAILED("Failed to get start index 21 for 15.08.2003 00:00");
	if(endindex != 21 + 25)
	  TEST_FAILED("Failed to get end index 46 for 16.08.2003 00:00");

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
	  TEST(firstTime);
	  TEST(lastTime);
	  TEST(findIndices);
	}

  }; // class tests

} // namespace QueryDataToolsTest


int main(void)
{
  cout << endl
	   << "QueryDataTools tester" << endl
	   << "=====================" << endl;
  QueryDataToolsTest::tests t;
  return t.run();
}
