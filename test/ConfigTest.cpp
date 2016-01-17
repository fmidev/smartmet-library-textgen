#include <regression/tframe.h>
#include "Config.h"
#include "TextGenPosixTime.h"

using namespace std;

namespace ConfigTest
{
// Fmi::Config settings;
// ----------------------------------------------------------------------
/*!
 * \brief Test Config::set()
 */
// ----------------------------------------------------------------------

void set_function()
{
  Fmi::Config::set("string_param", "string");
  Fmi::Config::set("bool_param1", "true");
  Fmi::Config::set("bool_param2", "false");
  Fmi::Config::set("bool_param3", "1");
  Fmi::Config::set("bool_param4", "0");
  Fmi::Config::set("int_param", "55");
  Fmi::Config::set("double_param", "23.56");
  Fmi::Config::set("hour_param1", "0");
  Fmi::Config::set("hour_param2", "15");
  Fmi::Config::set("hour_param3", "23");
  Fmi::Config::set("hour_param4", "24");
  Fmi::Config::set("days_param", "5");
  Fmi::Config::set("percentage_param", "18");
  Fmi::Config::set("time_param", "201305061358");
  Fmi::Config::set("result_param", "12.50,30.20");

  /*
  Fmi::Config::set("key1", "value1");
  Fmi::Config::set("key2", "value2");
  Fmi::Config::set("key3", "value3");
  Fmi::Config::set("key4", "value4");
  Fmi::Config::set("key5", "value5");
  */

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test Config::isNextDay()
 */
// ----------------------------------------------------------------------

void isset_function()
{
  if (!Fmi::Config::isset("string_param")) TEST_FAILED("Failed for string_param");
  if (!Fmi::Config::isset("bool_param1")) TEST_FAILED("Failed for bool_param1");
  if (!Fmi::Config::isset("bool_param2")) TEST_FAILED("Failed for bool_param2");
  if (!Fmi::Config::isset("bool_param3")) TEST_FAILED("Failed for bool_param3");
  if (!Fmi::Config::isset("bool_param4")) TEST_FAILED("Failed for bool_param5");
  if (!Fmi::Config::isset("int_param")) TEST_FAILED("Failed for int_param");
  if (!Fmi::Config::isset("double_param")) TEST_FAILED("Failed for double_param");
  if (!Fmi::Config::isset("hour_param1")) TEST_FAILED("Failed for hour_param1");
  if (!Fmi::Config::isset("hour_param2")) TEST_FAILED("Failed for hour_param2");
  if (!Fmi::Config::isset("hour_param3")) TEST_FAILED("Failed for hour_param3");
  if (!Fmi::Config::isset("hour_param4")) TEST_FAILED("Failed for hour_param4");
  if (!Fmi::Config::isset("days_param")) TEST_FAILED("Failed for days_param");
  if (!Fmi::Config::isset("days_param")) TEST_FAILED("Failed for days_param");
  if (!Fmi::Config::isset("percentage_param")) TEST_FAILED("Failed for percentage_param");
  if (!Fmi::Config::isset("time_param")) TEST_FAILED("Failed for time_param");
  if (!Fmi::Config::isset("result_param")) TEST_FAILED("Failed for result_param");

  TEST_PASSED();
}

void require_function()
{
  string string_param(Fmi::Config().require<string>("string_param"));
  if (string_param != "string")
    TEST_FAILED("Value of string_param sould be 'string' not " + string_param);
  bool bool_param1(Fmi::Config().require<bool>("bool_param1"));
  if (bool_param1 != true) TEST_FAILED("Value of bool_param sould be 'true' not false");

  bool bool_param2(Fmi::Config().require<bool>("bool_param2"));
  if (bool_param2 != false) TEST_FAILED("Value of bool_param sould be 'false' not true");

  bool bool_param3(Fmi::Config().require<bool>("bool_param3"));
  if (bool_param3 != true) TEST_FAILED("Value of bool_param sould be 'true' not false");

  bool bool_param4(Fmi::Config().require<bool>("bool_param4"));
  if (bool_param4 != false) TEST_FAILED("Value of bool_param sould be 'false' not true");

  int int_param(Fmi::Config().require<int>("int_param"));
  if (int_param != 55)
  {
    stringstream ss;
    ss << int_param;
    TEST_FAILED("Value of bool_param sould be '55' not " + ss.str());
  }

  double double_param(Fmi::Config().require<double>("double_param"));
  if (double_param != 23.56)
  {
    stringstream ss;
    ss << double_param;
    TEST_FAILED("Value of bool_param sould be '23.56' not " + ss.str());
  }
  int hour_param1(Fmi::Config().require("hour_param1", 0, 23));
  if (hour_param1 != 0)
  {
    stringstream ss;
    ss << hour_param1;
    TEST_FAILED("Value of hour_param sould be '0' not " + ss.str());
  }

  int hour_param2(Fmi::Config().require("hour_param2", 0, 23));
  if (hour_param2 != 15)
  {
    stringstream ss;
    ss << hour_param2;
    TEST_FAILED("Value of hour_param sould be '15' not " + ss.str());
  }

  int hour_param3(Fmi::Config().require("hour_param3", 0, 23));
  if (hour_param3 != 23)
  {
    stringstream ss;
    ss << hour_param3;
    TEST_FAILED("Value of hour_param sould be '23' not " + ss.str());
  }

  //	int hour_param4(Fmi::Config().require("hour_param4", 0, 23));

  int days_param(Fmi::Config().require("days_param", 0, 31));
  if (days_param != 5)
  {
    stringstream ss;
    ss << days_param;
    TEST_FAILED("Value of hour_param sould be '5' not " + ss.str());
  }

  int percentage_param(Fmi::Config().require("percentage_param", 0, 100));
  if (percentage_param != 18)
  {
    stringstream ss;
    ss << percentage_param;
    TEST_FAILED("Value of hour_param sould be '18' not " + ss.str());
  }

  TEST_PASSED();
}

//! The actual test driver
class tests : public tframe::tests
{
  //! Overridden message separator
  virtual const char* error_message_prefix() const { return "\n\t"; }
  //! Main test suite
  void test(void)
  {
    TEST(set_function);
    TEST(isset_function);
    TEST(require_function);
  }

};  // class tests

}  // namespace ConfigTest

int main(void)
{
  cout << endl << "Config tester" << endl << "================" << endl;
  ConfigTest::tests t;
  return t.run();
}
