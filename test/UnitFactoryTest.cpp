#include "regression/tframe.h"
#include "UnitFactory.h"
#include "DictionaryFactory.h"
#include "PlainTextFormatter.h"
#include "Sentence.h"
#include "newbase/NFmiSettings.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;

namespace UnitFactoryTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test UnitFactory::create(DegreesCelsius)
   */
  // ----------------------------------------------------------------------

  void celsius()
  {
	using namespace TextGen;
	using namespace UnitFactory;

	shared_ptr<Dictionary> finnish = DictionaryFactory::create("mysql");
	finnish->init("fi");

	PlainTextFormatter formatter;
	formatter.dictionary(finnish);

	shared_ptr<Sentence> unit;

	NFmiSettings::instance().set("textgen::units::celsius::format","SI");
	unit = create(DegreesCelsius);
	if(unit->realize(formatter) != "\260C.")
	  TEST_FAILED("create() failed to realize \260C.");

	NFmiSettings::instance().set("textgen::units::celsius::format","phrase");
	unit = create(DegreesCelsius);
	if(unit->realize(formatter) != "Astetta.")
	  TEST_FAILED("create() failed to realize Astetta.");
	
	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test UnitFactory::create(MetersPerSecond)
   */
  // ----------------------------------------------------------------------

  void meterspersecond()
  {
	using namespace TextGen;
	using namespace UnitFactory;

	shared_ptr<Dictionary> finnish = DictionaryFactory::create("mysql");
	finnish->init("fi");

	PlainTextFormatter formatter;
	formatter.dictionary(finnish);

	shared_ptr<Sentence> unit;

	NFmiSettings::instance().set("textgen::units::meterspersecond::format","SI");
	unit = create(MetersPerSecond);
	if(unit->realize(formatter) != "M/s.")
	  TEST_FAILED("create() failed to realize M/s.");

	NFmiSettings::instance().set("textgen::units::meterspersecond::format","phrase");
	unit = create(MetersPerSecond);
	if(unit->realize(formatter) != "Metriä sekunnissa.")
	  TEST_FAILED("create() failed to realize Metriä sekunnissa.");
	
	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test UnitFactory::create(Millimeters)
   */
  // ----------------------------------------------------------------------

  void millimeters()
  {
	using namespace TextGen;
	using namespace UnitFactory;

	shared_ptr<Dictionary> finnish = DictionaryFactory::create("mysql");
	finnish->init("fi");

	PlainTextFormatter formatter;
	formatter.dictionary(finnish);

	shared_ptr<Sentence> unit;

	NFmiSettings::instance().set("textgen::units::millimeters::format","SI");
	unit = create(Millimeters);
	if(unit->realize(formatter) != "Mm.")
	  TEST_FAILED("create() failed to realize Mm.");

	NFmiSettings::instance().set("textgen::units::millimeters::format","phrase");
	unit = create(Millimeters);
	if(unit->realize(formatter) != "Millimetriä.")
	  TEST_FAILED("create() failed to realize Millimetriä.");
	
	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test UnitFactory::create(Percent)
   */
  // ----------------------------------------------------------------------

  void percent()
  {
	using namespace TextGen;
	using namespace UnitFactory;

	shared_ptr<Dictionary> finnish = DictionaryFactory::create("mysql");
	finnish->init("fi");

	PlainTextFormatter formatter;
	formatter.dictionary(finnish);

	shared_ptr<Sentence> unit;

	NFmiSettings::instance().set("textgen::units::percent::format","SI");
	unit = create(Percent);
	if(unit->realize(formatter) != "%.")
	  TEST_FAILED("create() failed to realize %.");

	NFmiSettings::instance().set("textgen::units::percent::format","phrase");
	unit = create(Percent);
	if(unit->realize(formatter) != "Prosenttia.")
	  TEST_FAILED("create() failed to realize Prosenttia.");
	
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
	  TEST(celsius);
	  TEST(meterspersecond);
	  TEST(millimeters);
	  TEST(percent);
	}

  }; // class tests

} // namespace UnitFactoryTest


int main(void)
{
  cout << endl
	   << "UnitFactory tester" << endl
	   << "==================" << endl;
  UnitFactoryTest::tests t;
  return t.run();
}
