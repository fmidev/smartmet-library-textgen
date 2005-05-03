#include "regression/tframe.h"
#include "NullDictionary.h"
#include "Float.h"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

namespace FloatTest
{

  //! Test structors
  void structors(void)
  {
	using namespace TextGen;

	// Should succeed
	Float n1(12.9);

	// Should succeed
	Float n2(n1);

	// Should succeed
	Float n3 = n1;

	Float n4(0.0);
	n4 = n1;

	TEST_PASSED();
  }

  //! Test realize() for floats
  void realize_float(void)
  {
	using namespace TextGen;

	Float n1(0.0);
	if(n1.realize(NullDictionary()) != "0.0")
	  TEST_FAILED("realization of float(0.0) failed");
	
	Float n2(123.4321);
	if(n2.realize(NullDictionary()) != "123.4")
	  TEST_FAILED("realization of float(123.4321) failed");
	
	Float n3(123.4321,2);
	if(n3.realize(NullDictionary()) != "123.43")
	  TEST_FAILED("realization of float(123.4321,2) failed");

	Float n4(123.4321,3,false);
	if(n4.realize(NullDictionary()) != "123,432")
	  TEST_FAILED("realization of float(123.4321,2.false) failed");

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
	  TEST(structors);
	  TEST(realize_float);
	}

  }; // class tests

} // namespace FloatTest


int main(void)
{
  cout << endl
	   << "Integer tester" << endl
	   << "==============" << endl;
  FloatTest::tests t;
  return t.run();
}
