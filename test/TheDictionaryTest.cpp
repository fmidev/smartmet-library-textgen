#include "tframe.h"
#include "DictionaryFactory.h"
#include "TextGenError.h"
#include "TheDictionary.h"
#include <iostream>
#include <string>

using namespace std;

namespace TheDictionaryTest
{
  //! Test instance() method
  void instance(void)
  {
	using namespace textgen;

	// instance() should succeed and return a NullDictionary
	if(!TheDictionary::instance().empty())
	  TEST_FAILED("empty() should return true before init");

	TEST_PASSED();
  }

  //! Test dictionary() method
  void dictionary(void)
  {
	using namespace textgen;

	TheDictionary::instance().dictionary(DictionaryFactory::create("null"));
	TheDictionary::instance().dictionary(DictionaryFactory::create("basic"));
	TheDictionary::instance().dictionary(DictionaryFactory::create("mysql"));
	
	TEST_PASSED();

  }

  //! Test init() method

  void init(void)
  {
	using namespace textgen;

	TheDictionary::instance().dictionary(DictionaryFactory::create("null"));

	// Should succeed
	TheDictionary::instance().dictionary(DictionaryFactory::create("mysql"));
	TheDictionary::instance().init("fi");
	TheDictionary::instance().init("en");

	// Should throw since Russian is not active
	try
	  {
		TheDictionary::instance().init("ru");
		TEST_FAILED("init('ru') should have failed");
	  }
	catch(const TextGenError & e) { }
	TEST_PASSED();
  }
  
  //! Test empty() method
  void empty(void)
  {
	using namespace textgen;

	TheDictionary::instance().dictionary(DictionaryFactory::create("null"));

	if(!TheDictionary::instance().empty())
	  TEST_FAILED("empty() should return true before dictionary()");

	TheDictionary::instance().dictionary(DictionaryFactory::create("mysql"));
	if(!TheDictionary::instance().empty())
	  TEST_FAILED("empty() should return true after dictionary()");

	TheDictionary::instance().init("fi");
	if(TheDictionary::instance().empty())
	  TEST_FAILED("empty() should return true after init()");

	TEST_PASSED();
  }

  //! Test size() method
  void size(void)
  {
	using namespace textgen;

	TheDictionary::instance().dictionary(DictionaryFactory::create("null"));

	if(TheDictionary::instance().size()!=0)
	  TEST_FAILED("size() should return 0 before dictionary()");

	TheDictionary::instance().dictionary(DictionaryFactory::create("mysql"));
	if(TheDictionary::instance().size()!=0)
	  TEST_FAILED("size() should return 0 before init()");

	TheDictionary::instance().init("fi");
	if(TheDictionary::instance().size()==0)
	  TEST_FAILED("size() should return > 0 after init(fi)");

	TEST_PASSED();
  }

  //! Test contains() method
  void contains(void)
  {
	using namespace textgen;

	TheDictionary::instance().dictionary(DictionaryFactory::create("null"));

	if(TheDictionary::instance().contains("foobar"))
	  TEST_FAILED("contains(foobar) should have failed");

	TheDictionary::instance().dictionary(DictionaryFactory::create("mysql"));
	if(TheDictionary::instance().contains("foobar"))
	  TEST_FAILED("contains(foobar) should have failed");

	TheDictionary::instance().init("fi");
	if(TheDictionary::instance().contains("foobar"))
	  TEST_FAILED("contains(foobar) should have failed");
	if(!TheDictionary::instance().contains("länsi"))
	  TEST_FAILED("contains(länsi) should have succeeded");

	TEST_PASSED();
  }
  
  //! Test find() method
  void find(void)
  {
	using namespace textgen;

	TheDictionary::instance().dictionary(DictionaryFactory::create("null"));

	try
	  {
		TheDictionary::instance().find("foobar");
		TEST_FAILED("find() should throw before dictionary()");
	  }
	catch(const TextGenError & e) { }

	TheDictionary::instance().dictionary(DictionaryFactory::create("mysql"));
	try
	  {
		TheDictionary::instance().find("foobar");
		TEST_FAILED("find() should throw before init()");
	  }
	catch(const TextGenError & e) { }

	TheDictionary::instance().init("fi");

	try
	  {
		TheDictionary::instance().find("foobar");
		TEST_FAILED("find(foobar) should have thrown");
	  }
	catch(const TextGenError & e) { }

	if(TheDictionary::instance().find("länsi") != "länsi")
	  TEST_FAILED("find(länsi) should have returned länsi");

	TheDictionary::instance().init("en");
	if(TheDictionary::instance().find("länsi") != "west")
	  TEST_FAILED("find(länsi) should have returned west");

	TEST_PASSED();
  }

  //! Test insert
  void insert(void)
  {
	using namespace textgen;

	TheDictionary::instance().dictionary(DictionaryFactory::create("null"));

	try
	  {
		TheDictionary::instance().insert("foo","bar");
		TEST_FAILED("insert(foo,bar) should throw even before dictionary()");
	  }
	catch(const TextGenError & e) { }

	TheDictionary::instance().dictionary(DictionaryFactory::create("mysql"));
	TheDictionary::instance().init("fi");

	try
	  {
		TheDictionary::instance().insert("foo","bar");
		TEST_FAILED("insert(foo,bar) should have thrown after init(fi)");
	  }
	catch(const TextGenError & e) { }

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
	  //	  TEST(instance);
	  TEST(dictionary);
	  TEST(init);
	  TEST(empty);
	  TEST(size);
	  TEST(contains);
	  TEST(find);
	  TEST(insert);
	}

  }; // class tests

} // namespace TheDictionaryTest


int main(void)
{
  cout << endl
	   << "TheDictionary tester" << endl
	   << "====================" << endl;
  TheDictionaryTest::tests t;
  return t.run();
}
