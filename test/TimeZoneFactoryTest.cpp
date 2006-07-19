#include "regression/tframe.h"
#include "TimeZoneFactory.h"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::local_time;

namespace TimeZoneFactoryTest
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Test the region list method
   */
  // ----------------------------------------------------------------------

  void region_list()
  {
	TimeZoneFactory & instance = TimeZoneFactory::instance();

	try
	{
	  vector<string> regions = instance.region_list();
	  TEST_FAILED("region_list should throw if region file is not set");
	}
	catch(...)
	  {
	  }

	instance.set_region_file("/data/conf/date_time_zonespec.csv");

	try
	{
	  vector<string> regions = instance.region_list();
	  if(regions.empty())
		TEST_FAILED("regions list size is zero");

#ifdef DEBUG
	  for(unsigned int i=0; i<regions.size(); i++)
		cout << regions[i] << endl;
#endif
		
	}
	catch(...)
	  {
		TEST_FAILED("Failed to initialize region list");
	  }

	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test timezone conversions to verify boost works
   */
  // ----------------------------------------------------------------------

  void conversions()
  {
	TimeZoneFactory & instance = TimeZoneFactory::instance();
	
	ptime summer(date(2006,Jul,19),hours(15));

	{
	  time_zone_ptr zone = instance.time_zone_from_region("Europe/Helsinki");

	  local_date_time datetime(summer,zone);

	  ptime hki = datetime.local_time();

	  time_duration timeofday = hki.time_of_day();

	  if(timeofday.hours() != 18)
		TEST_FAILED("Failed to get hour 18 for Helsinki");

	}

	{
	  time_zone_ptr zone = instance.time_zone_from_region("Europe/Stockholm");

	  local_date_time datetime(summer,zone);

	  ptime stock = datetime.local_time();

	  time_duration timeofday = stock.time_of_day();

	  if(timeofday.hours() != 17)
		TEST_FAILED("Failed to get hour 17 for Stockholm");

	}

	ptime winter(date(2006,Jan,19),hours(15));

	{
	  time_zone_ptr zone = instance.time_zone_from_region("Europe/Helsinki");

	  local_date_time datetime(winter,zone);

	  ptime hki = datetime.local_time();

	  time_duration timeofday = hki.time_of_day();

	  if(timeofday.hours() != 17)
		TEST_FAILED("Failed to get hour 17 for Helsinki");

	}

	{
	  time_zone_ptr zone = instance.time_zone_from_region("Europe/Stockholm");

	  local_date_time datetime(winter,zone);

	  ptime stock = datetime.local_time();

	  time_duration timeofday = stock.time_of_day();

	  if(timeofday.hours() != 16)
		TEST_FAILED("Failed to get hour 16 for Stockholm");

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
	  TEST(region_list);	// must be the first test
	  TEST(conversions);
	}

  }; // class tests

} // namespace TimeZoneFactoryTest


int main(void)
{
  cout << endl
	   << "TimeZoneFactory tester" << endl
	   << "======================" << endl;
  TimeZoneFactoryTest::tests t;
  return t.run();
}
