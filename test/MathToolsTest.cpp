#include "regression/tframe.h"
#include "MathTools.h"

using namespace std;

namespace MathToolsTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test MathTools::to_precision()
   */
  // ----------------------------------------------------------------------

  void to_precision()
  {
	if(MathTools::to_precision(10,1) != 10)
	  TEST_FAILED("to_precision(10,1) failed");

	if(MathTools::to_precision(15,5) != 15)
	  TEST_FAILED("to_precision(15,5) failed");

	if(MathTools::to_precision(15,2) != 16)
	  TEST_FAILED("to_precision(15,2) failed");

	if(MathTools::to_precision(15,10) != 20)
	  TEST_FAILED("to_precision(15,10) failed");

	if(MathTools::to_precision(15,20) != 20)
	  TEST_FAILED("to_precision(15,20) failed");

	if(MathTools::to_precision(15,50) != 0)
	  TEST_FAILED("to_precision(15,50) failed");

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
	  TEST(to_precision);
	}

  }; // class tests

} // namespace MathToolsTest


int main(void)
{
  cout << endl
	   << "MathTools tester" << endl
	   << "================" << endl;
  MathToolsTest::tests t;
  return t.run();
}
