#include "tframe.h"
#include "DictionaryFactory.h"
#include "PhraseNumber.h"
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

	{
	  // Should succeed
	  Sentence s1(std::string("foobar"));
	  // Should succeed via automatic casting
	  Sentence s2("foobar");
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

	Sentence s2("foobar");
	if(s2.empty())
	  TEST_FAILED("sentence should not be empty after explicit construction");

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

	Sentence s2("foobar");
	if(s2.size()!=1)
	  TEST_FAILED("sentence size should be 1 after explicit construction");

	Sentence s3(s2);
	if(s3.size()!=1)
	  TEST_FAILED("sentence size should be 1 after copy constructor");

	TEST_PASSED();

  }

  //! Test operator+= and operator+
  void adding(void)
  {
	using namespace TextGen;

	{
	  Sentence s1("a");

	  Sentence s2("b");
	  s1 += s2;
	  if(s1.size() != 2)
		TEST_FAILED("size after a += b is not 2");

	  s1 += string("c");
	  if(s1.size() != 3)
		TEST_FAILED("size after ab += c is not 3");

	  s1 += "d";
	  if(s1.size() != 4)
		TEST_FAILED("size after abc += d is not 4");

	  s1 += s1;
	  if(s1.size() != 8)
		TEST_FAILED("size after abcd += abcd is not 8");
	}

	{
	  Sentence s1(1);

	  Sentence s2(2);
	  s1 += s2;
	  if(s1.size() != 2)
		TEST_FAILED("size after 1 += 2 is not 2");

	  s1 += PhraseNumber<float>(3.0);
	  if(s1.size() != 3)
		TEST_FAILED("size after 12 += 3 is not 3");

	  s1 += 4;
	  if(s1.size() != 4)
		TEST_FAILED("size after 123 += 4 is not 4");

	  s1 += s1;
	  if(s1.size() != 8)
		TEST_FAILED("size after 1234 += 1234 is not 8");
	}

	{
	  Sentence s1("a");
	  Sentence s2("b");

	  Sentence s3 = s1+s2;
	  if(s3.size()!=2)
		TEST_FAILED("size after a+b is not 2");

	  s3 = s3 + string("c");
	  if(s3.size()!=3)
		TEST_FAILED("size after ab+c is not 3");
	  
	  s3 = s3 + "d";
	  if(s3.size()!=4)
		TEST_FAILED("size after abc+d is not 4");

	  s3 = s3 + s3;
	  if(s3.size()!=8)
		TEST_FAILED("size after abcd+abcd is not 8");

	}

	{
	  Sentence s1(1);
	  Sentence s2(2);

	  Sentence s3 = s1+s2;
	  if(s3.size()!=2)
		TEST_FAILED("size after 1+2 is not 2");

	  s3 = s3 + PhraseNumber<int>(3);
	  if(s3.size()!=3)
		TEST_FAILED("size after 12+3 is not 3");
	  
	  s3 = s3 + 4;
	  if(s3.size()!=4)
		TEST_FAILED("size after 123+4 is not 4");

	  s3 = s3 + s3;
	  if(s3.size()!=8)
		TEST_FAILED("size after 1234+1234 is not 8");

	}

	TEST_PASSED();

  }

  //! Test operator<<
  void appending(void)
  {
	using namespace TextGen;

	{
	  Sentence s1("a");
	  
	  Sentence s2("b");
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
	  
	  Sentence s("a");
	  s << "b" << "c" << "d" << "e";
	  if(s.size() != 5)
		TEST_FAILED("size after a << b << c << d << e is not 5");
	}

	{
	  Sentence s1(PhraseNumber<int>(1));
	  
	  Sentence s2(2);
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

	auto_ptr<Dictionary> dict = DictionaryFactory::create("mysql");
	dict->init("fi");
	
	TheDictionary::instance().dictionary(DictionaryFactory::create("mysql"));
	TheDictionary::instance().init("en");
	
	Sentence s1;
	if(!s1.realize().empty())
	  TEST_FAILED("realization of empty sentence not empty");

	s1 += "kaakko";
	if(s1.realize() != "South east.")
	  TEST_FAILED("realization of kaakko in English failed");

	s1 += "etelä";
	if(s1.realize() != "South east south.")
	  TEST_FAILED("realization of kaakko etelä in English failed");

	if(s1.realize(*dict) != "Kaakko etelä.")
	  TEST_FAILED("realization of kaakko etelä in Finnish failed");

	Sentence s2;
	s2 << "kaakko" << 12;
	if(s2.realize() != "South east 12.")
	  {
		cout << "'" << s1.realize() << "'" << endl;
		TEST_FAILED("realization of kaakko 12 in English failed");
	  }

	if(s2.realize(*dict) != "Kaakko 12.")
	  TEST_FAILED("realization of kaakko 12 in Finnish failed");

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
	  TEST(empty);
	  TEST(size);
	  TEST(adding);
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
