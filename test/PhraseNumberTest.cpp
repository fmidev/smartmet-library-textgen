#include "tframe.h"
#include "DictionaryFactory.h"
#include "TheDictionary.h"
#include "PhraseNumber.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

namespace PhraseNumberTest
{

  template <typename Value>
  struct MyTraits
  {
	static std::string tostring(const Value & theValue)
	{
	  std::ostringstream os;
	  os << std::ios::fixed << std::setprecision(2) << theValue;
	  return os.str();
	}
  };

  //! Test structors
  void structors(void)
  {
	using namespace TextGen;

	// Should succeed
	PhraseNumber<int> n1(12);
	// Should succeed
	PhraseNumber<float> n2(1.1f);
	// Should succeed
	PhraseNumber<double> n3(1.2);

	// Should succeed
	PhraseNumber<int> n4(n1);

	// Should succeed
	PhraseNumber<int> n5 = n1;

	PhraseNumber<int> n6(0);
	n6 = n1;

	TEST_PASSED();
  }


  //! Test realize() for ints
  void realize_int(void)
  {
	using namespace TextGen;

	PhraseNumber<int> n1(0);
	if(n1.realize() != "0")
	  TEST_FAILED("realization of int(0) failed");
	
	PhraseNumber<int> n2(123);
	if(n2.realize() != "123")
	  TEST_FAILED("realization of int(123) failed");
	
	TEST_PASSED();
  }

  //! Test realize() for floats
  void realize_float(void)
  {
	using namespace TextGen;

	PhraseNumber<float> n1(0);
	if(n1.realize() != "0")
	  TEST_FAILED("realization of float(0) failed");
	
	PhraseNumber<float> n2(123);
	if(n2.realize() != "123")
	  TEST_FAILED("realization of float(123) failed");
	
	PhraseNumber<float> n3(123.123);
	if(n3.realize() != "123.1")
	  TEST_FAILED("realization of float(123.123) failed");

	PhraseNumber<float> n4(123.199);
	if(n4.realize() != "123.2")
	  TEST_FAILED("realization of float(123.199) failed");

	PhraseNumber<float> n5(123.999);
	if(n5.realize() != "124")
	  TEST_FAILED("realization of float(123.999) failed");

	PhraseNumber<float> n6(-123.999);
	if(n6.realize() != "-124")
	  TEST_FAILED("realization of float(-123.999) failed");

	TEST_PASSED();
  }

  //! Test realize() for own traits
  void realize_traits(void)
  {
	using namespace TextGen;

	typedef PhraseNumber<float,MyTraits<float> > MyNumber;
	
	MyNumber n1(0);
	if(n1.realize() != "0.00")
	  TEST_FAILED("realization of float(0) failed");
	
	MyNumber n2(123);
	if(n2.realize() != "123.00")
	  TEST_FAILED("realization of float(123) failed");
	
	MyNumber n3(123.123);
	if(n3.realize() != "123.12")
	  TEST_FAILED("realization of float(123.123) failed");
	
	MyNumber n4(123.199);
	if(n4.realize() != "123.20")
	  TEST_FAILED("realization of float(123.199) failed");
	
	MyNumber n5(123.999);
	if(n5.realize() != "124.00")
	  TEST_FAILED("realization of float(123.999) failed");
	
	MyNumber n6(-123.999);
	if(n6.realize() != "-124.00")
	  TEST_FAILED("realization of float(-123.999) failed");
	
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
	  TEST(realize_int);
	  TEST(realize_float);
	  TEST(realize_traits);
	}

  }; // class tests

} // namespace PhraseNumberTest


int main(void)
{
  cout << endl
	   << "PhraseNumber tester" << endl
	   << "===================" << endl;
  PhraseNumberTest::tests t;
  return t.run();
}
