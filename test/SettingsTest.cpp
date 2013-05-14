#include <regression/tframe.h>

#include "Settings.h"
#include "Config.h"
#include "TextGenPosixTime.h"
#include "WeatherResult.h"

#include <boost/shared_ptr.hpp>

#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/locale.hpp>

using namespace std;
using namespace boost;

namespace SettingsTest
{
  void settings_set()
  {
	Settings::set("string_param", "string");
	Settings::set("bool_param1", "true");
	Settings::set("bool_param2", "false");
	Settings::set("bool_param3", "1");
	Settings::set("bool_param4", "0");
	Settings::set("int_param", "55");
	Settings::set("double_param", "23.56");
	Settings::set("hour_param1", "0");
	Settings::set("hour_param2", "15");
	Settings::set("hour_param3", "23");
	Settings::set("hour_param4", "24");
	Settings::set("days_param", "5");
	Settings::set("percentage_param", "18");
	Settings::set("time_param", "201305061358");
	Settings::set("result_param", "12.50,30.20");

	TEST_PASSED();
  }

  void settings_isset()
  {
	if(!Settings::isset("string_param"))
	  TEST_FAILED("Failed for string_param");
	if(!Settings::isset("bool_param1"))
	  TEST_FAILED("Failed for bool_param1");
	if(!Settings::isset("bool_param2"))
	  TEST_FAILED("Failed for bool_param2");
	if(!Settings::isset("bool_param3"))
	  TEST_FAILED("Failed for bool_param3");
	if(!Settings::isset("bool_param4"))
	  TEST_FAILED("Failed for bool_param5");
	if(!Settings::isset("int_param"))
	  TEST_FAILED("Failed for int_param");
	if(!Settings::isset("double_param"))
	  TEST_FAILED("Failed for double_param");
	if(!Settings::isset("hour_param1"))
	  TEST_FAILED("Failed for hour_param1");
	if(!Settings::isset("hour_param2"))
	  TEST_FAILED("Failed for hour_param2");
	if(!Settings::isset("hour_param3"))
	  TEST_FAILED("Failed for hour_param3");
	if(!Settings::isset("hour_param4"))
	  TEST_FAILED("Failed for hour_param4");
	if(!Settings::isset("days_param"))
	  TEST_FAILED("Failed for days_param");
	if(!Settings::isset("days_param"))
	  TEST_FAILED("Failed for days_param");
	if(!Settings::isset("percentage_param"))
	  TEST_FAILED("Failed for percentage_param");
	if(!Settings::isset("time_param"))
	  TEST_FAILED("Failed for time_param");
	if(!Settings::isset("result_param"))
	  TEST_FAILED("Failed for result_param");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test Settings require
   */
  // ----------------------------------------------------------------------

  void settings_require()
  {
	// string
	string string_param(Settings::require_string("string_param"));
	if(string_param != "string")
	  TEST_FAILED("Value of string_param sould be 'string' not " + string_param);

	string_param = Settings::require("bool_param1");
	if(string_param != "true")
	  TEST_FAILED("Value of string_param sould be 'true' not " + string_param);

	string_param = Settings::require("bool_param2");
	if(string_param != "false")
	  TEST_FAILED("Value of string_param sould be 'false' not " + string_param);

	string_param = Settings::require("bool_param3");
	if(string_param != "1")
	  TEST_FAILED("Value of string_param sould be '1' not " + string_param);

	string_param = Settings::require("bool_param4");
	if(string_param != "0")
	  TEST_FAILED("Value of string_param sould be '0' not " + string_param);

	string_param = Settings::require("int_param");
	if(string_param != "55")
	  TEST_FAILED("Value of string_param sould be '55' not " + string_param);

	string_param = Settings::require("double_param");
	if(string_param != "23.56")
	  TEST_FAILED("Value of string_param sould be '23.56' not " + string_param);

	// bool
	bool bool_param(Settings::require_bool("bool_param1"));
	if(bool_param != true)
	  TEST_FAILED("Value of bool_param sould be 'true' not false");

	bool_param = Settings::require_bool("bool_param2");
	if(bool_param != false)
	  TEST_FAILED("Value of bool_param sould be 'false' not true");

	bool_param = Settings::require_bool("bool_param3");
	if(bool_param != true)
	  TEST_FAILED("Value of bool_param sould be 'true' not false");

	bool_param = Settings::require_bool("bool_param4");
	if(bool_param != false)
	  TEST_FAILED("Value of bool_param sould be 'false' not true");

	// int
	int int_param(Settings::require_int("int_param"));
	if(int_param != 55)
	  {
		stringstream ss;
		ss << int_param;
		TEST_FAILED("Value of bool_param sould be '55' not " + ss.str());
	  }

	// double
	double double_param(Settings::require_double("double_param"));
	if(double_param != 23.56)
	  {
		stringstream ss;
		ss << double_param;
		TEST_FAILED("Value of bool_param sould be '23.56' not " + ss.str());
	  }

	// hour
	int hour_param(Settings::require_hour("hour_param1"));
	if(hour_param != 0)
	  {
		stringstream ss;
		ss << hour_param;
		TEST_FAILED("Value of hour_param sould be '0' not " + ss.str());
	  }

	hour_param = Settings::require_hour("hour_param2");
	if(hour_param != 15)
	  {
		stringstream ss;
		ss << hour_param;
		TEST_FAILED("Value of hour_param sould be '15' not " + ss.str());
	  }

	hour_param = Settings::require_hour("hour_param3");
	if(hour_param != 23)
	  {
		stringstream ss;
		ss << hour_param;
		TEST_FAILED("Value of hour_param sould be '23' not " + ss.str());
	  }

	// hour_param = Settings::require_hour("hour_param4");

	// days
	int days_param(Settings::require_hour("days_param"));
	if(days_param != 5)
	  {
		stringstream ss;
		ss << days_param;
		TEST_FAILED("Value of hour_param sould be '5' not " + ss.str());
	  }

	// percentage
	int percentage_param(Settings::require_percentage("percentage_param"));
	if(percentage_param != 18)
	  {
		stringstream ss;
		ss << percentage_param;
		TEST_FAILED("Value of hour_param sould be '18' not " + ss.str());
	  }

	// time
	TextGenPosixTime posixTime(Settings::require_time("time_param"));
	if(posixTime != TextGenPosixTime(2013, 5, 6, 13, 58))
	  TEST_FAILED("Value of time_param sould be '201305061358' not " + posixTime.ToStr(kYYYYMMDDHHMM));

	// result
	TextGen::WeatherResult result(Settings::require_result("result_param"));
	if(result != TextGen::WeatherResult(12.50,30.20))
	  TEST_FAILED("Value of result_param sould be '12.50,30.20' not " + posixTime.ToStr(kYYYYMMDDHHMM));

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test Settings optional
   */
  // ----------------------------------------------------------------------

  void settings_optional()
  {
	// string
	string string_param(Settings::optional_string("string_param", "default_string"));
	if(string_param != "string")
	  TEST_FAILED("Value of string_param sould be 'string' not " + string_param);

	string_param = Settings::optional_string("string_paramm", "default_string");
	if(string_param != "default_string")
	  TEST_FAILED("Value of string_param sould be 'default_string' not " + string_param);

	// bool
	bool bool_param(Settings::optional_bool("bool_param1", false));
	if(bool_param != true)
	  TEST_FAILED("Value of bool_param sould be 'true' not false");

	bool_param = Settings::optional_bool("bool_paramm1", true);
	if(bool_param != true)
	  TEST_FAILED("Value of bool_param sould be 'true' not false");

	bool_param = Settings::optional_bool("bool_paramm1", false);
	if(bool_param != false)
	  TEST_FAILED("Value of bool_param sould be 'false' not true");

	// int
	int int_param(Settings::optional_int("int_param", 60));
	if(int_param != 55)
	  {
		stringstream ss;
		ss << int_param;
		TEST_FAILED("Value of bool_param sould be '55' not " + ss.str());
	  }

	int_param = Settings::optional_int("int_paramm", 60);
	if(int_param != 60)
	  {
		stringstream ss;
		ss << int_param;
		TEST_FAILED("Value of bool_param sould be '60' not " + ss.str());
	  }

	// double
	double double_param(Settings::optional_double("double_param", 29.13));
	if(double_param != 23.56)
	  {
		stringstream ss;
		ss << double_param;
		TEST_FAILED("Value of bool_param sould be '23.56' not " + ss.str());
	  }

	double_param = Settings::optional_double("double_paramm", 29.13);
	if(double_param != 29.13)
	  {
		stringstream ss;
		ss << double_param;
		TEST_FAILED("Value of bool_param sould be '29.13' not " + ss.str());
	  }

	// hour
	int hour_param(Settings::optional_hour("hour_param1", 9));
	if(hour_param != 0)
	  {
		stringstream ss;
		ss << hour_param;
		TEST_FAILED("Value of hour_param sould be '0' not " + ss.str());
	  }

	hour_param = Settings::optional_hour("hour_paramm1", 9);
	if(hour_param != 9)
	  {
		stringstream ss;
		ss << hour_param;
		TEST_FAILED("Value of hour_param sould be '9' not " + ss.str());
	  }

	// days
	int days_param(Settings::optional_hour("days_param", 19));
	if(days_param != 5)
	  {
		stringstream ss;
		ss << days_param;
		TEST_FAILED("Value of hour_param sould be '5' not " + ss.str());
	  }

	days_param = Settings::optional_hour("days_paramm", 19);
	if(days_param != 19)
	  {
		stringstream ss;
		ss << days_param;
		TEST_FAILED("Value of hour_param sould be '19' not " + ss.str());
	  }

	// percentage
	int percentage_param(Settings::optional_percentage("percentage_param", 25));
	if(percentage_param != 18)
	  {
		stringstream ss;
		ss << percentage_param;
		TEST_FAILED("Value of hour_param sould be '18' not " + ss.str());
	  }

	percentage_param = Settings::optional_percentage("percentage_paramm", 25);
	if(percentage_param != 25)
	  {
		stringstream ss;
		ss << percentage_param;
		TEST_FAILED("Value of hour_param sould be '25' not " + ss.str());
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
	  TEST(settings_set);
	  TEST(settings_isset);
	  TEST(settings_require);
	  TEST(settings_optional);
	}

  }; // class tests

} // namespace SettingsTest


int main(void)
{
  boost::locale::generator generator;
  std::locale::global(generator(""));


  using namespace SettingsTest;

  cout << endl
	   << "Settings tests" << endl
	   << "====================" << endl;


  tests t;
  return t.run();
}
