#include "tframe.h"
#include "DictionaryFactory.h"
#include "TheDictionary.h"
#include "PhraseWord.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

namespace PhraseWordTest
{

  //! Test structors
  void structors(void)
  {
	using namespace TextGen;

	// Should succeed
	PhraseWord s(std::string("foobar"));
	// Should succeed
	PhraseWord s2(s);
	// Should succeed
	PhraseWord s3 = s;
	// Should succeed
	PhraseWord s4("foobar");
	s4 = s;

	TEST_PASSED();
  }


  //! Test realize()
  void realize(void)
  {
	using namespace TextGen;

	auto_ptr<Dictionary> dict = DictionaryFactory::create("mysql");
	dict->init("fi");
	
	TheDictionary::instance().dictionary(DictionaryFactory::create("mysql"));
	TheDictionary::instance().init("en");
	
	PhraseWord s1("kaakko");;
	if(s1.realize() != "south east")
	  TEST_FAILED("realization of kaakko in English failed");

	if(s1.realize(*dict) != "kaakko")
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
	  TEST(realize);
	}

  }; // class tests

} // namespace PhraseWordTest


int main(void)
{
  cout << endl
	   << "PhraseWord tester" << endl
	   << "=================" << endl;
  PhraseWordTest::tests t;
  return t.run();
}
