#include "tframe.h"
#include "NumberFactory.h"
#include "DictionaryFactory.h"
#include "PlainTextFormatter.h"
#include "NFmiSettings.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;

namespace NumberFactoryTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test NumberFactory::create(int)
   */
  // ----------------------------------------------------------------------

  void default_numbers()
  {
	using namespace TextGen;
	using namespace NumberFactory;

	shared_ptr<Dictionary> finnish = DictionaryFactory::create("mysql");
	finnish->init("fi");

	PlainTextFormatter formatter;
	formatter.dictionary(finnish);

	NFmiSettings::instance().set("textgen::numberformatter","default");

	shared_ptr<Sentence> num;

	num = create(1);
	if(num->realize(formatter) != "1.")
	  TEST_FAILED("create(1) failed to realize '1.'");

	num = create(0);
	if(num->realize(formatter) != "0.")
	  TEST_FAILED("create(0) failed to realize '0'");

	num = create(87);
	if(num->realize(formatter) != "87.")
	  TEST_FAILED("create(87) failed to realize '87'");

	num = create(-87);
	if(num->realize(formatter) != "-87.")
	  TEST_FAILED("create(-87) failed to realize '-87'");
	
	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test NumberFactory::create(int,int)
   */
  // ----------------------------------------------------------------------

  void default_ranges()
  {
	using namespace TextGen;
	using namespace NumberFactory;

	shared_ptr<Dictionary> finnish = DictionaryFactory::create("mysql");
	finnish->init("fi");

	PlainTextFormatter formatter;
	formatter.dictionary(finnish);

	NFmiSettings::instance().set("textgen::numberformatter","default");

	shared_ptr<Glyph> range;

	range = create(1,1);
	if(range->realize(formatter) != "1.")
	  TEST_FAILED("create(1,1) failed to realize '1'");

	range = create(10,20);
	if(range->realize(formatter) != "10...20.")
	  TEST_FAILED("create(10,20) failed to realize '10...20'");

	range = create(-10,-5);
	if(range->realize(formatter) != "-10...-5.")
	  TEST_FAILED("create(-10,-5) failed to realize '-10...-5'");
	
	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test NumberFactory::create(int)
   */
  // ----------------------------------------------------------------------

  void sonera_numbers()
  {
	using namespace TextGen;
	using namespace NumberFactory;

	shared_ptr<Dictionary> finnish = DictionaryFactory::create("mysql");
	finnish->init("fi");

	PlainTextFormatter formatter;
	formatter.dictionary(finnish);

	NFmiSettings::instance().set("textgen::numberformatter","sonera");

	shared_ptr<Glyph> num;

	num = create(1);
	if(num->realize(formatter) != "1.")
	  TEST_FAILED("create(1) failed to realize '1.'");

	num = create(0);
	if(num->realize(formatter) != "0.")
	  TEST_FAILED("create(0) failed to realize '0.'");

	num = create(87);
	if(num->realize(formatter) != "87.")
	  TEST_FAILED("create(87) failed to realize '87.'");

	num = create(-87);
	if(num->realize(formatter) != "Miinus 87.")
	  TEST_FAILED("create(-87) failed to realize 'Miinus 87.'");
	
	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test NumberFactory::create(int,int)
   */
  // ----------------------------------------------------------------------

  void sonera_ranges()
  {
	using namespace TextGen;
	using namespace NumberFactory;

	shared_ptr<Dictionary> finnish = DictionaryFactory::create("mysql");
	finnish->init("fi");

	PlainTextFormatter formatter;
	formatter.dictionary(finnish);

	NFmiSettings::instance().set("textgen::numberformatter","sonera");

	shared_ptr<Glyph> range;

	range = create(1,1);
	if(range->realize(formatter) != "1.")
	  TEST_FAILED("create(1,1) failed to realize '1.'");

	range = create(10,20);
	if(range->realize(formatter) != "10 viiva 20.")
	  {
		cout << "'" << range->realize(formatter) << "'" << endl;
		TEST_FAILED("create(10,20) failed to realize '10 viiva 20.'");
	  }

	range = create(-10,-5);
	if(range->realize(formatter) != "Miinus 10 viiva miinus 5.")
	  TEST_FAILED("create(-10,-5) failed to realize 'Miinus 10 viiva miinus 5.'");

	
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
	  TEST(default_numbers);
	  TEST(default_ranges);
	  TEST(sonera_numbers);
	  TEST(sonera_ranges);
	}

  }; // class tests

} // namespace NumberFactoryTest


int main(void)
{
  cout << endl
	   << "NumberFactory tester" << endl
	   << "====================" << endl;
  NumberFactoryTest::tests t;
  return t.run();
}
