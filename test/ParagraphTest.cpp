#include "tframe.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "Sentence.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;

namespace ParagraphTest
{

  //! Test structors
  void structors(void)
  {
	using namespace TextGen;

	{
	  // Should succeed
	  Paragraph p;
	}

	{
	  // Should succeed
	  Paragraph * p = new Paragraph();
	  // Should succeed
	  delete p;
	}

	{
	  // Should succeed
	  Paragraph p;
	  // Should succeed
	  Paragraph p2(p);
	  // Should succeed
	  Paragraph p3 = p;
	  // Should succeed
	  Paragraph p4;
	  p4 = p;
	}

	TEST_PASSED();
  }

  //! Test empty()
  void empty(void)
  {
	using namespace TextGen;

	Paragraph p1;
	if(!p1.empty())
	  TEST_FAILED("paragraph should be empty after void constructor");

	Sentence s;
	s << "foobar";
	Paragraph p2;
	p2 << s;
	if(p2.empty())
	  TEST_FAILED("paragraph should not be empty after explicit construction");

	Paragraph p3(p2);
	if(p3.empty())
	  TEST_FAILED("paragraph should not be empty after copy construction");

	TEST_PASSED();
  }

  //! Test size()
  void size(void)
  {
	using namespace TextGen;

	Paragraph p1;
	if(p1.size()!=0)
	  TEST_FAILED("paragraph size should be 0 after void constructor");

	Sentence s;
	s << "foobar";
	Paragraph p2;
	p2 << s;
	if(p2.size()!=1)
	  TEST_FAILED("paragraph size should be 1 after explicit construction");

	Paragraph p3(p2);
	if(p3.size()!=1)
	  TEST_FAILED("paragraph size should be 1 after copy constructor");

	TEST_PASSED();

  }

  //! Test operator<<
  void appending(void)
  {
	using namespace TextGen;

	Sentence s1;
	Sentence s2;
	Sentence s3;
	s1 << "a";
	s2 << "b";
	s3 << "c";

	Paragraph p1;
	Paragraph p2;
	p1 << s1;
	p2 << s2;
	
	p1 << p2;
	if(p1.size() != 2)
	  TEST_FAILED("size after a << b is not 2");
	
	p1 << s3;
	if(p1.size() != 3)
	  TEST_FAILED("size after ab << c is not 3");
	
	p1 << p1;
	if(p1.size() != 6)
	  {
		cout << p1.size() << endl;
		TEST_FAILED("size after abc << abc is not 6");
	  }

	Paragraph p;
	p << s1 << s2 << s3;
	if(p.size() != 3)
	  TEST_FAILED("size after << b << c << d is not 3");

	TEST_PASSED();

  }

  //! Test realize()
  void realize(void)
  {
	using namespace TextGen;

#if 0
	shared_ptr<Dictionary> finnish = DictionaryFactory::create("mysql");
	finnish->init("fi");

	shared_ptr<Dictionary> english = DictionaryFactory::create("mysql");
	english->init("en");
	
	Sentence s1;
	Sentence s2;
	s1 << "kaakko";
	s2 << "etelä";

	Paragraph p1;
	if(!p1.realize(*english).empty())
	  TEST_FAILED("realization of empty paragraph not empty");

	p1 << s1;
	if(p1.realize(*english) != "South east.")
	  TEST_FAILED("realization of kaakko in English failed");

	p1 << s2;
	if(p1.realize(*english) != "South east. South.")
	  TEST_FAILED("realization of kaakko etelä in English failed");

	if(p1.realize(*finnish) != "Kaakko. Etelä.")
	  TEST_FAILED("realization of kaakko etelä in Finnish failed");

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

} // namespace ParagraphTest


int main(void)
{
  cout << endl
	   << "Paragraph tester" << endl
	   << "================" << endl;
  ParagraphTest::tests t;
  return t.run();
}
