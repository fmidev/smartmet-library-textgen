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
	
	if(w0.realize() != "Sun") TEST_FAILED("0 should be Sun in English");
	if(w1.realize() != "Mon") TEST_FAILED("1 should be Mon in English");
	if(w2.realize() != "Tue") TEST_FAILED("2 should be Tue in English");
	if(w3.realize() != "Wed") TEST_FAILED("3 should be Wed in English");
	if(w4.realize() != "Thu") TEST_FAILED("4 should be Thu in English");
	if(w5.realize() != "Fri") TEST_FAILED("5 should be Fri in English");
	if(w6.realize() != "Sat") TEST_FAILED("6 should be Sat in English");
	if(w7.realize() != "Sun") TEST_FAILED("7 should be Sun in English");
	if(w8.realize() != "Mon") TEST_FAILED("8 should be Mon in English");

	if(w0.realize(*dict) != "Su") TEST_FAILED("0 should be Su in Finnish");
	if(w1.realize(*dict) != "Ma") TEST_FAILED("1 should be Ma in Finnish");
	if(w2.realize(*dict) != "Ti") TEST_FAILED("2 should be Ti in Finnish");
	if(w3.realize(*dict) != "Ke") TEST_FAILED("3 should be Ke in Finnish");
	if(w4.realize(*dict) != "To") TEST_FAILED("4 should be To in Finnish");
	if(w5.realize(*dict) != "Pe") TEST_FAILED("5 should be Pe in Finnish");
	if(w6.realize(*dict) != "La") TEST_FAILED("6 should be La in Finnish");
	if(w7.realize(*dict) != "Su") TEST_FAILED("7 should be Su in Finnish");
	if(w8.realize(*dict) != "Ma") TEST_FAILED("8 should be Ma in Finnish");

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

} // namespace PhraseShortWeekdayTest


int main(void)
{
  cout << endl
	   << "PhraseShortWeekday tester" << endl
	   << "=========================" << endl;
  PhraseShortWeekdayTest::tests t;
  return t.run();
}
