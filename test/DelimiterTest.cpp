#include "tframe.h"
#include "Delimiter.h"
#include "NullDictionary.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

namespace DelimiterTest
{

  //! Test structors
  void structors(void)
  {
	using namespace TextGen;

	// Should succeed
	Delimiter s(std::string(","));
	// Should succeed
	Delimiter s2(s);
	// Should succeed
	Delimiter s3 = s;
	// Should succeed
	Delimiter s4(",");
	s4 = s;

	TEST_PASSED();
  }

  //! Test separator()
  void separator(void)
  {
	using namespace TextGen;

	Delimiter s(",");

	if(s.prefix() != "")
	  TEST_FAILED("Word prefix for delimiters must be an empty string");
	if(s.suffix() != "")
	  TEST_FAILED("Word suffix for delimiters must be an empty string");

	TEST_PASSED();
  }


  //! Test realize()
  void realize(void)
  {
	using namespace TextGen;

	Delimiter s1(",");;
	if(s1.realize(NullDictionary()) != ",")
	  TEST_FAILED("realization of , failed");

	Delimiter s2("??");;
	if(s2.realize(NullDictionary()) != "??")
	  TEST_FAILED("realization of ?? failed");

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
	  TEST(realize);
	}

  }; // class tests

} // namespace DelimiterTest


int main(void)
{
  cout << endl
	   << "Delimiter tester" << endl
	   << "================" << endl;
  DelimiterTest::tests t;
  return t.run();
}