#include "tframe.h"
#include "DictionaryFactory.h"
#include "TheDictionary.h"
#include "PhraseShortWeekday.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

namespace PhraseShortWeekdayTest
{

  //! Test structors
  void structors(void)
  {
	using namespace TextGen;

	// Should succeed
	PhraseShortWeekday s(1);
	// Should succeed
	PhraseShortWeekday s2(s);
	// Should succeed
	PhraseShortWeekday s3 = s;
	// Should succeed
	PhraseShortWeekday s4(2);
	s4 = s;

	TEST_PASSED();
  }

  //! Test wordseparator()
  void wordseparator(void)
  {
	using namespace TextGen;

	PhraseShortWeekday s(0);

	if(s.wordseparator() != " ")
	  TEST_FAILED("Word separator for weekdays must be space");

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

	PhraseShortWeekday w0(0);
	PhraseShortWeekday w1(1);
	PhraseShortWeekday w2(2);
	PhraseShortWeekday w3(3);
	PhraseShortWeekday w4(4);
	PhraseShortWeekday w5(5);
	PhraseShortWeekday w6(6);
	PhraseShortWeekday w7(7);
	PhraseShortWeekday w8(8);
	
	if(w0.realize() != "sun") TEST_FAILED("0 should be sun in English");
	if(w1.realize() != "mon") TEST_FAILED("1 should be mon in English");
	if(w2.realize() != "tue") TEST_FAILED("2 should be tue in English");
	if(w3.realize() != "wed") TEST_FAILED("3 should be wed in English");
	if(w4.realize() != "thu") TEST_FAILED("4 should be thu in English");
	if(w5.realize() != "fri") TEST_FAILED("5 should be fri in English");
	if(w6.realize() != "sat") TEST_FAILED("6 should be sat in English");
	if(w7.realize() != "sun") TEST_FAILED("7 should be sun in English");
	if(w8.realize() != "mon") TEST_FAILED("8 should be mon in English");

	if(w0.realize(*dict) != "su") TEST_FAILED("0 should be su in Finnish");
	if(w1.realize(*dict) != "ma") TEST_FAILED("1 should be ma in Finnish");
	if(w2.realize(*dict) != "ti") TEST_FAILED("2 should be ti in Finnish");
	if(w3.realize(*dict) != "ke") TEST_FAILED("3 should be ke in Finnish");
	if(w4.realize(*dict) != "to") TEST_FAILED("4 should be to in Finnish");
	if(w5.realize(*dict) != "pe") TEST_FAILED("5 should be pe in Finnish");
	if(w6.realize(*dict) != "la") TEST_FAILED("6 should be la in Finnish");
	if(w7.realize(*dict) != "su") TEST_FAILED("7 should be su in Finnish");
	if(w8.realize(*dict) != "ma") TEST_FAILED("8 should be ma in Finnish");

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

} // namespace PhraseShortWeekdayTest


int main(void)
{
  cout << endl
	   << "PhraseShortWeekday tester" << endl
	   << "=========================" << endl;
  PhraseShortWeekdayTest::tests t;
  return t.run();
}
