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
