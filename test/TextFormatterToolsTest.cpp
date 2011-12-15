#include <regression/tframe.h>
#include "DebugDictionary.h"
#include "Sentence.h"
#include "Integer.h"
#include "PlainTextFormatter.h"
#include "TextFormatterTools.h"
#include "UnitFactory.h"

#include <newbase/NFmiSettings.h>

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;
using namespace TextGen;

namespace TextFormatterToolsTest
{
  shared_ptr<TextGen::Dictionary> dict;

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TextFormatterTools::capitalize
   */
  // ----------------------------------------------------------------------

  void capitalize()
  {
	string tmp = "testi 1";
	TextFormatterTools::capitalize(tmp);
	if(tmp != "Testi 1")
	  TEST_FAILED("Failed to capitalize 'testi 1'");

	tmp = "testi 2";
	TextFormatterTools::capitalize(tmp);
	if(tmp != "Testi 2")
	  TEST_FAILED("Failed to handle 'Testi 2'");

	tmp = "ähtäri";
	TextFormatterTools::capitalize(tmp);
	if(tmp != "Ähtäri")
	  TEST_FAILED("Failed to capitalize 'ähtäri'");

	TEST_PASSED();


  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TextFormatterTools::punctuate
   */
  // ----------------------------------------------------------------------

  void punctuate()
  {
	string tmp = "testi 1";
	TextFormatterTools::punctuate(tmp);
	if(tmp != "testi 1.")
	  TEST_FAILED("Failed to punctuate 'testi 1'");

	tmp = "";
	TextFormatterTools::punctuate(tmp);
	if(tmp != "")
	  TEST_FAILED("Failed to punctuate ''");

	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TextFormatterTools::realize
   */
  // ----------------------------------------------------------------------

  void realize()
  {
	string tmp;

	PlainTextFormatter formatter;
	formatter.dictionary(dict);

	// Test 1: normal case
	{
	  Sentence s;
	  s << "lämpötila" << "on" << "[1] asteen paikkeilla" << TextGen::Integer(10);

	  tmp = TextFormatterTools::realize(s.begin(),s.end(),formatter," ","");
	  if(tmp != "lämpötila on 10 asteen paikkeilla")
		TEST_FAILED("Test 1 failed: " + tmp);
	}

	// Test 2: normal case with 2 values
	{
	  Sentence s;
	  s << "lämpötila" << "on" << "[1] viiva [2] astetta"
		<< TextGen::Integer(10) << TextGen::Integer(15);	  

	  tmp = TextFormatterTools::realize(s.begin(),s.end(),formatter," ","");
	  if(tmp != "lämpötila on 10 viiva 15 astetta")
		TEST_FAILED("Test 2 failed: " + tmp);
	}


	// Test 3: degrees
	{
	  NFmiSettings::Set("textgen::units::celsius::format","phrase");

	  Sentence s;
	  s << "lämpötila" << "on noin" << "[1] [2]"
		<< TextGen::Integer(10) << *UnitFactory::create(DegreesCelsius);

	  tmp = TextFormatterTools::realize(s.begin(),s.end(),formatter," ","");
	  if(tmp != "lämpötila on noin 10 astetta")
		TEST_FAILED("Test 3 failed: " + tmp);

	}

	// Test 4: SI units
	{
	  NFmiSettings::Set("textgen::units::celsius::format","SI");

	  Sentence s;
	  s << "lämpötila" << "on noin" << "[1] [2]"
		<< TextGen::Integer(10) << *UnitFactory::create(DegreesCelsius);

	  tmp = TextFormatterTools::realize(s.begin(),s.end(),formatter," ","");
	  if(tmp != "lämpötila on noin 10°C")
		TEST_FAILED("Test 4 failed: " + tmp);

	}

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief The actual test driver
   */
  // ----------------------------------------------------------------------

  class tests : public tframe::tests
  {
	//! Overridden message separator
	virtual const char * error_message_prefix() const
	{
	  return "\n\t";
	}

	//! Main test suite
	void test(void)
	{
	  TEST(capitalize);
	  TEST(punctuate);
	  TEST(realize);
	}

  }; // class tests

} // namespace TextFormatterToolsTest


int main(void)
{
  using namespace TextFormatterToolsTest;

  cout << endl
	   << "TextFormatterTools tests" << endl
	   << "========================" << endl;

  dict.reset(new TextGen::DebugDictionary());

  tests t;
  return t.run();
}
