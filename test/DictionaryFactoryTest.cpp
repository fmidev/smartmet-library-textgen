#include "tframe.h"
#include "DictionaryFactory.h"

#include <iostream>
#include <string>

using namespace std;

namespace DictionaryFactoryTest
{

  //! Test create
  void create(void)
  {
	using namespace TextGen;

	// Should succeed
	std::auto_ptr<Dictionary> dict1 = DictionaryFactory::create("null");
	std::auto_ptr<Dictionary> dict2 = DictionaryFactory::create("basic");
	std::auto_ptr<Dictionary> dict3 = DictionaryFactory::create("mysql");

	// Should throw
	try
	  {
		std::auto_ptr<Dictionary> dict4 = DictionaryFactory::create("foobar");
		TEST_FAILED("create(foobar) should have thrown");
	  }
	catch(...) { }

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
	  TEST(create);
	}

  }; // class tests

} // namespace DictionaryFactoryTest


int main(void)
{
  cout << endl
	   << "DictionaryFactory tester" << endl
	   << "========================" << endl;
  DictionaryFactoryTest::tests t;
  return t.run();
}
