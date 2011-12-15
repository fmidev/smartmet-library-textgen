#include <regression/tframe.h>
#include "DefaultAcceptor.h"
#include <newbase/NFmiGlobals.h>

#include <iostream>
#include <string>

using namespace std;

namespace DefaultAcceptorTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test DefaultAcceptor
   */
  // ----------------------------------------------------------------------

  void accept()
  {
	using namespace WeatherAnalysis;

	DefaultAcceptor acc;

	if(!acc.accept(0))
	  TEST_FAILED("Should accept 0");

	if(acc.accept(kFloatMissing))
	  TEST_FAILED("Should not accept kFloatMissing");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief The actual test driver
   */
  // ----------------------------------------------------------------------

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
	  TEST(accept);
	}

  }; // class tests

} // namespace DefaultAcceptorTest


int main(void)
{
  using namespace DefaultAcceptorTest;

  cout << endl
	   << "DefaultAcceptor tests" << endl
	   << "=====================" << endl;

  tests t;
  return t.run();
}
