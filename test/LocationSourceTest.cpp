#include "regression/tframe.h"
#include "LocationSource.h"
#include "NFmiPoint.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;

namespace LocationSourceTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test hasCoordinates
   */
  // ----------------------------------------------------------------------

  void hascoordinates()
  {
	using WeatherAnalysis::LocationSource;

	if(LocationSource::instance().hasCoordinates("foobar"))
	  TEST_FAILED("Location foobar should not have coordinates");

	if(!LocationSource::instance().hasCoordinates("Helsinki"))
	  TEST_FAILED("Location Helsinki should have coordinates");

	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test coordinates
   */
  // ----------------------------------------------------------------------

  void coordinates()
  {
	using WeatherAnalysis::LocationSource;

	try
	  {
		NFmiPoint p = LocationSource::instance().coordinates("foobar");
		TEST_FAILED("Location foobar should not have coordinates");
	  }
	catch(...) { }

	try
	  {
		NFmiPoint p = LocationSource::instance().coordinates("Helsinki");
	  }
	catch(...)
	  { TEST_FAILED("Location Helsinki should have coordinates"); }

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
	  TEST(hascoordinates);
	  TEST(coordinates);
	}

  }; // class tests

} // namespace LocationSourceTest


int main(void)
{
  cout << endl
	   << "LocationSource tester" << endl
	   << "=====================" << endl;
  LocationSourceTest::tests t;
  return t.run();
}