#include "tframe.h"
#include "DictionaryFactory.h"
#include "Number.h"
#include "Delimiter.h"
#include "Sentence.h"
#include "TheDictionary.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

namespace SentenceTest
{

  //! Test structors
  void structors(void)
  {
	using namespace TextGen;

	{
	  // Should succeed
	  Sentence s;
	}

	{
	  // Should succeed
	  Sentence * s = new Sentence();
	  // Should succeed
	  delete s;
	}

	{
	  // Should succeed
	  Sentence s;
	  // Should succeed
	  Sentence s2(s);
	  // Should succeed
	  Sentence s3 = s;
	  // Should succeed
	  Sentence s4;
	  s4 = s;
	}

	TEST_PASSED();
  }

  //! Test empty()
  void empty(void)
  {
	using namespace TextGen;

	Sentence s1;
	if(!s1.empty())
	  TEST_FAILED("sentence should be empty after void constructor");
	
	Sentence s2;
	s2 << "foobar";
	if(s2.empty())
	  TEST_FAILED("sentence should not be empty after adding phrase");

	Sentence s3(s2);
	if(s3.empty())
	  TEST_FAILED("sentence should not be empty after copy construction");

	TEST_PASSED();
  }

  //! Test size()
  void size(void)
  {
	using namespace TextGen;

	Sentence s1;
	if(s1.size()!=0)
	  TEST_FAILED("sentence size should be 0 after void constructor");

	Sentence s2;
	s2 << "foobar";
	if(s2.size()!=1)
	  TEST_FAILED("sentence size should be 1 after adding a phrase");

	Sentence s3(s2);
	if(s3.size()!=1)
	  TEST_FAILED("sentence size should be 1 after copy constructor");

	TEST_PASSED();

  }

  //! Test operator<<
  void appending(void)
  {
	using namespace TextGen;

	{
	  Sentence s1;
	  Sentence s2;
	  s1 << "a";
	  s2 << "b";

	  s1 << s2;
	  if(s1.size() != 2)
		TEST_FAILED("size after a << b is not 2");
	  
	  s1 << string("c");
	  if(s1.size() != 3)
		TEST_FAILED("size after ab << c is not 3");
	  
	  s1 << "d";
	  if(s1.size() != 4)
		TEST_FAILED("size after abc << d is not 4");
	  
	  s1 << s1;
	  if(s1.size() != 8)
		TEST_FAILED("size after abcd << abcd is not 8");
	  
	  Sentence s;
	  s << "a";
	  s << "b" << "c" << "d" << "e";
	  if(s.size() != 5)
		TEST_FAILED("size after a << b << c << d << e is not 5");
	}

	{
	  Number<int> num(1);
	  Sentence s1;
	  Sentence s2;
	  s1 << num;
	  s2 << 2;

	  s1 << s2;
	  if(s1.size() != 2)
		TEST_FAILED("size after 1 << 2 is not 2");
	  
	  s1 << 3;
	  if(s1.size() != 3)
		TEST_FAILED("size after 12 << 3 is not 3");
	  
	  s1 << s1;
	  if(s1.size() != 6)
		TEST_FAILED("size after 123 << 123 is not 6");
	  
	  Sentence s;
	  s << 1 << 2 << 3 << 4 << 5;
	  if(s.size() != 5)
		TEST_FAILED("size after << 1 << 2 << 3 << 4 << 5 is not 5");
	}

	TEST_PASSED();

  }

  //! Test realize()
  void realize(void)
  {
	using namespace TextGen;

#if 0
	auto_ptr<Dictionary> finnish = DictionaryFactory::create("mysql");
	finnish->init("fi");

	auto_ptr<Dictionary> english = DictionaryFactory::create("mysql");
	english->init("en");
	
	Sentence s1;
	if(!s1.realize(*english).empty())
	  TEST_FAILED("realization of empty sentence not empty");

	s1 << "kaakko";
	if(s1.realize(*english) != "South east.")
	  TEST_FAILED("realization of kaakko in English failed");

	s1 << "etel�";
	if(s1.realize(*english) != "South east south.")
	  TEST_FAILED("realization of kaakko etel� in English failed");

	if(s1.realize(*finnish) != "Kaakko etel�.")
	  TEST_FAILED("realization of kaakko etel� in Finnish failed");

	Sentence s2;
	s2 << "kaakko" << 12;
	if(s2.realize(*english) != "South east 12.")
	  TEST_FAILED("realization of kaakko 12 in English failed");

	if(s2.realize(*finnish) != "Kaakko 12.")
	  TEST_FAILED("realization of kaakko 12 in Finnish failed");

	TEST_PASSED();

	Sentence s3;
	s3 << "kaakko" << 12 << Delimiter(",") << "etel�" << 13;
	if(s3.realize(*english) != "South east 12, south 13")
	  TEST_FAILED("realization of kaakko 12, etel� 13 in English failed");

	Sentence s4;
	s4 << "kaakko" << 12 << "," << "etel�" << 13;
	if(s4.realize(*finnish) != "Kaakko 12, etel� 13")
	  TEST_FAILED("realization of kaakko 12, etel� 13 in Finnish failed");

	TEST_PASSED();
#endif

	TEST_NOT_IMPLEMENTED();

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
	  TEST(empty);
	  TEST(size);
	  TEST(appending);
	  TEST(realize);
	}

  }; // class tests

} // namespace SentenceTest


int main(void)
{
  cout << endl
	   << "Sentence tester" << endl
	   << "===============" << endl;
  SentenceTest::tests t;
  return t.run();
}
