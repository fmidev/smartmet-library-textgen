#include "tframe.h"
#include "PhraseSeparator.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

namespace PhraseSeparatorTest
{

  //! Test structors
  void structors(void)
  {
	using namespace TextGen;

	// Should succeed
	PhraseSeparator s(std::string(","));
	// Should succeed
	PhraseSeparator s2(s);
	// Should succeed
	PhraseSeparator s3 = s;
	// Should succeed
	PhraseSeparator s4(",");
	s4 = s;

	TEST_PASSED();
  }

  //! Test wordseparator()
  void wordseparator(void)
  {
	using namespace TextGen;

	PhraseSeparator s(",");

	if(s.wordseparator() != "")
	  TEST_FAILED("Word separator for separators must be an empty string");

	TEST_PASSED();
  }


  //! Test realize()
  void realize(void)
  {
	using namespace TextGen;

	PhraseSeparator s1(",");;
	if(s1.realize() != ",")
	  TEST_FAILED("realization of , failed");

	PhraseSeparator s2("??");;
	if(s2.realize() != "??")
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
	  TEST(wordseparator);
	  TEST(realize);
	}

  }; // class tests

} // namespace PhraseSeparatorTest


int main(void)
{
  cout << endl
	   << "PhraseSeparator tester" << endl
	   << "======================" << endl;
  PhraseSeparatorTest::tests t;
  return t.run();
}
