#include "tframe.h"
#include "DictionaryFactory.h"
#include "Phrase.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

namespace PhraseTest
{

  //! Test structors
  void structors(void)
  {
	using namespace TextGen;

	// Should succeed
	Phrase s(std::string("foobar"));
	// Should succeed
	Phrase s2(s);
	// Should succeed
	Phrase s3 = s;
	// Should succeed
	Phrase s4("foobar");
	s4 = s;

	TEST_PASSED();
  }

  //! Test separator()
  void separator(void)
  {
	using namespace TextGen;

	Phrase s(std::string("foobar"));

	if(s.prefix() != " ")
	  TEST_FAILED("Word prefix for regular words must be space");
	if(s.suffix() != "")
	  TEST_FAILED("Word suffix for regular words must be empty");

	TEST_PASSED();
  }


  //! Test realize()
  void realize(void)
  {
	using namespace TextGen;

	auto_ptr<Dictionary> finnish = DictionaryFactory::create("mysql");
	finnish->init("fi");

	auto_ptr<Dictionary> english = DictionaryFactory::create("mysql");
	english->init("en");
	
	Phrase s1("kaakko");;
	if(s1.realize(*english) != "south east")
	  TEST_FAILED("realization of kaakko in English failed");

	if(s1.realize(*finnish) != "kaakko")
	  TEST_FAILED("realization of kaakko in Finnish failed");

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

} // namespace PhraseTest


int main(void)
{
  cout << endl
	   << "Phrase tester" << endl
	   << "=============" << endl;
  PhraseTest::tests t;
  return t.run();
}