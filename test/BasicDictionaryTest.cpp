#include "tframe.h"
#include "BasicDictionary.h"

#include <iostream>
#include <string>

using namespace std;

namespace BasicDictionaryTest
{

  //! Test structors
  void structors(void)
  {
	using namespace textgen;

	{
	  // Should succeed
	  BasicDictionary dict;
	}

	{
	  // Should succeed
	  BasicDictionary * dict = new BasicDictionary();
	  // Should succeed
	  delete dict;
	}

	{
	  // Should succeed
	  BasicDictionary dict;
	  // Should succeed
	  BasicDictionary dict2(dict);
	  // Should succeed
	  BasicDictionary dict3 = dict;
	  // Should succeed
	  BasicDictionary dict4;
	  dict4 = dict;
	}

	TEST_PASSED();
  }

  //! Test interface functionality
  void interface(void)
  {
	using namespace textgen;

	// Should succeed
	BasicDictionary dict;
	dict.init("some language");

	// Should succeed
	if(!dict.empty())
	  TEST_FAILED("empty() returned false on empty dictionary");

	//! Should be zero
	if(dict.size()!=0)
	  TEST_FAILED("size()<>0 on empty directory");

	// Should fail
	if(dict.contains("keyword"))
	  TEST_FAILED("contains() succeeded on an empty dictionary");

	// Should throw
	try
	  {
		string word = dict.find("keyword");
		TEST_FAILED("find() should throw on empty dictionary");
	  }
	catch(...) { }

	// Should succeed
	dict.insert("keyword","phrase");

	if(dict.empty())
	  TEST_FAILED("empty() should return false after insert()");

	if(dict.size()!=1)
	  TEST_FAILED("size()!=1 after one insert()");

	// Should succeed
	if(!dict.contains("keyword"))
	  TEST_FAILED("contains(keyword) should succeed after insert(keyword,phrase)");

	// Should succeed
	if(dict.find("keyword") != "phrase")
	  TEST_FAILED("find(keyword) should succeed after insert(keyword,phrase)");

	// Should fail silently
	dict.insert("keyword","wrong phrase");
	if(dict.find("keyword") != "phrase")
	  TEST_FAILED("second insert(keyword,phrase) should have failed");

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
	  TEST(interface);
	}

  }; // class tests

} // namespace BasicDictionaryTest


int main(void)
{
  cout << endl
	   << "BasicDictionary tester" << endl
	   << "======================" << endl;
  BasicDictionaryTest::tests t;
  return t.run();
}
