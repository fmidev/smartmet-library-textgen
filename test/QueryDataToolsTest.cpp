#include "regression/tframe.h"
#include "QueryDataTools.h"

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
	TEST_NOT_IMPLEMENTED();
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
