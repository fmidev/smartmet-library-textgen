#include "tframe.h"
#include "TextGenError.h"

#include <iostream>
#include <string>

using namespace std;

namespace TextGenErrorTest
{

  //! Test throwing exceptions
  void throwing(void)
  {
	using namespace textgen;
	try
	  {
		throw TextGenError("This must be catched");
		TEST_FAILED("throw TextGenError failed");
	  }
	catch(const TextGenError & e) { }

	TEST_PASSED();
  }

  //! Test the message content comes through
  void messages(void)
  {
	using namespace textgen;

	string message = "The message";
	try
	  {
		throw TextGenError(message);
	  }
	catch(const TextGenError & e)
	  {
		if(e.what() != message)
		  TEST_FAILED("what() does not return what was thrown");
	  }

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
	  TEST(throwing);
	  TEST(messages);
	}


  }; // class tests

} // namespace TextGenErrorTest


int main(void)
{
  cout << endl
	   << "TextGenErrorTest tester" << endl
	   << "=======================" << endl;
  TextGenErrorTest::tests t;
  return t.run();
}
