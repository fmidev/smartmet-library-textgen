#include "regression/tframe.h"
#include "QueryDataTools.h"

#include "NFmiStreamQueryData.h"
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
	using WeatherAnalysis::QueryDataTools::firstTime;

	NFmiStreamQueryData qd;
	if(!qd.ReadData("data/skandinavia_pinta.sqd"))
	  throw runtime_error("Reading data/skandinavia_pinta.sqd failed");

	NFmiFastQueryInfo * qi = qd.QueryInfoIter();

	if(!firstTime(*qi,NFmiTime(1999,1,1)))
	  TEST_FAILED("Should succeed for 1.1.1999");

	if(!firstTime(*qi,NFmiTime(2003,8,15)))
	  TEST_FAILED("Should succeed for 15.08.2003");

	if(firstTime(*qi,NFmiTime(2030,1,1)))
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
	using WeatherAnalysis::QueryDataTools::findIndices;

	// The data has timesteps from 200308140600 to 200308220900
	NFmiStreamQueryData qd;
	if(!qd.ReadData("data/skandinavia_pinta.sqd"))
	  throw runtime_error("Reading data/skandinavia_pinta.sqd failed");

	NFmiFastQueryInfo * qi = qd.QueryInfoIter();

	unsigned long startindex, endindex;

	if(!findIndices(*qi,
					NFmiTime(2003,8,15),
					NFmiTime(2003,8,16),
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
	virtual const char * const error_message_prefix() const
	{
	  return "\n\t";
	}

	//! Main test suite
	void test(void)
	{
	  TEST(firstTime);
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
