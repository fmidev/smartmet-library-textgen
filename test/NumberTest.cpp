#include "tframe.h"
#include "NullDictionary.h"
#include "Number.h"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

namespace NumberTest
{

  template <typename Value>
  struct MyTraits
  {
	static std::string tostring(const Value & theValue)
	{
	  std::ostringstream os;
#ifdef OLDGCC
	  os << std::ios::fixed << std::setprecision(2) << theValue;
#else
	  os << std::fixed << std::setprecision(2) << theValue;
#endif
	  return os.str();
	}
  };


  // Round to degree multiples
  template <typename Value, int Mult>
  struct MyAdvancedTraits
  {
	static std::string tostring(const Value & theValue)
	{
	  std::ostringstream os;
	  int value = static_cast<int>(std::floor(theValue/Mult+0.5)*Mult);
	  value %= 360;
	  os << value;
	  return os.str();
	}
  };


  //! Test structors
  void structors(void)
  {
	using namespace TextGen;

	// Should succeed
	Number<int> n1(12);
	// Should succeed
	Number<float> n2(1.1f);
	// Should succeed
	Number<double> n3(1.2);

	// Should succeed
	Number<int> n4(n1);

	// Should succeed
	Number<int> n5 = n1;

	Number<int> n6(0);
	n6 = n1;

	TEST_PASSED();
  }

  //! Test separator()
  void separator(void)
  {
	using namespace TextGen;

	Number<int> s(0);

	if(s.prefix() != " ")
	  TEST_FAILED("Word prefix for numbers must be space");
	if(s.suffix() != "")
	  TEST_FAILED("Word prefix for numbers must be empty");

	TEST_PASSED();
  }


  //! Test realize() for ints
  void realize_int(void)
  {
	using namespace TextGen;

	Number<int> n1(0);
	if(n1.realize(NullDictionary()) != "0")
	  TEST_FAILED("realization of int(0) failed");
	
	Number<int> n2(123);
	if(n2.realize(NullDictionary()) != "123")
	  TEST_FAILED("realization of int(123) failed");
	
	TEST_PASSED();
  }

  //! Test realize() for floats
  void realize_float(void)
  {
	using namespace TextGen;

	Number<float> n1(0);
	if(n1.realize(NullDictionary()) != "0")
	  TEST_FAILED("realization of float(0) failed");
	
	Number<float> n2(123);
	if(n2.realize(NullDictionary()) != "123")
	  TEST_FAILED("realization of float(123) failed");
	
	Number<float> n3(123.123);
	if(n3.realize(NullDictionary()) != "123.1")
	  TEST_FAILED("realization of float(123.123) failed");

	Number<float> n4(123.199);
	if(n4.realize(NullDictionary()) != "123.2")
	  TEST_FAILED("realization of float(123.199) failed");

	Number<float> n5(123.999);
	if(n5.realize(NullDictionary()) != "124")
	  TEST_FAILED("realization of float(123.999) failed");

	Number<float> n6(-123.999);
	if(n6.realize(NullDictionary()) != "-124")
	  TEST_FAILED("realization of float(-123.999) failed");

	TEST_PASSED();
  }

  //! Test realize() for own traits
  void realize_traits(void)
  {
	using namespace TextGen;

	typedef Number<float,MyTraits<float> > MyNumber;
	
	MyNumber n1(0);
	if(n1.realize(NullDictionary()) != "0.00")
	  TEST_FAILED("realization of float(0) failed");
	
	MyNumber n2(123);
	if(n2.realize(NullDictionary()) != "123.00")
	  TEST_FAILED("realization of float(123) failed");
	
	MyNumber n3(123.123);
	if(n3.realize(NullDictionary()) != "123.12")
	  TEST_FAILED("realization of float(123.123) failed");
	
	MyNumber n4(123.199);
	if(n4.realize(NullDictionary()) != "123.20")
	  TEST_FAILED("realization of float(123.199) failed");
	
	MyNumber n5(123.999);
	if(n5.realize(NullDictionary()) != "124.00")
	  TEST_FAILED("realization of float(123.999) failed");
	
	MyNumber n6(-123.999);
	if(n6.realize(NullDictionary()) != "-124.00")
	  TEST_FAILED("realization of float(-123.999) failed");
	
	TEST_PASSED();
  }

  //! Test realize() for advanced traits
  void realize_advanced_traits(void)
  {
	using namespace TextGen;

	typedef Number<float,MyAdvancedTraits<float,45> > MyNumber;
	
	MyNumber n1(10);
	if(n1.realize(NullDictionary()) != "0")
	  TEST_FAILED("realization of float(10) failed");
	
	MyNumber n2(22.4);
	if(n2.realize(NullDictionary()) != "0")
	  TEST_FAILED("realization of float(22.4) failed");

	MyNumber n3(22.5);
	if(n3.realize(NullDictionary()) != "45")
	  TEST_FAILED("realization of float(22.5) failed");
	
	MyNumber n4(181);
	if(n4.realize(NullDictionary()) != "180")
	  TEST_FAILED("realization of float(181) failed");
	
	MyNumber n5(320);
	if(n5.realize(NullDictionary()) != "315")
	  TEST_FAILED("realization of float(320) failed");
	
	MyNumber n6(350);
	if(n6.realize(NullDictionary()) != "0")
	  TEST_FAILED("realization of float(350) failed");
	
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
	  TEST(separator);
	  TEST(realize_int);
	  TEST(realize_float);
	  TEST(realize_traits);
	  TEST(realize_advanced_traits);
	}

  }; // class tests

} // namespace NumberTest


int main(void)
{
  cout << endl
	   << "Number tester" << endl
	   << "=============" << endl;
  NumberTest::tests t;
  return t.run();
}