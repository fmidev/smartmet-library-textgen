#include "tframe.h"
#include "WeatherArea.h"

using namespace std;

namespace WeatherAreaTest
{

  //! Test structors
  void structors(void)
  {
	using namespace WeatherAnalysis;

	// Should succeed
	WeatherArea s(std::string("Suomi"));
	// Should succeed
	WeatherArea s2("Suomi");
	// Should succeed
	WeatherArea s3(s);
	// Should succeed
	s2 = s3;

	TEST_PASSED();
  }

  //! Test accessors
  void accessors(void)
  {
	using namespace WeatherAnalysis;

	WeatherArea area("Suomi");
	if(area.name() != "Suomi")
	  TEST_FAILED("name() accessor failed");

	TEST_PASSED();
  }

  //! Test comparisons
  void comparisons(void)
  {
	using namespace WeatherAnalysis;

	WeatherArea area1("Suomi");
	WeatherArea area2("Etelä-Suomi");

	if(!(area1==area1))
	  TEST_FAILED("area1==area1 test failed");
	if(area1!=area1)
	  TEST_FAILED("area1!=area1 test failed");

	if(area1==area2)
	  TEST_FAILED("area1==area2 test failed");
	if(!(area1!=area2))
	  TEST_FAILED("area1!=area2 test failed");

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
	  TEST(structors);
	  TEST(accessors);
	  TEST(comparisons);
	}

  }; // class tests

} // namespace WeatherAreaTest


int main(void)
{
  cout << endl
	   << "WeatherArea tester" << endl
	   << "==================" << endl;
  WeatherAreaTest::tests t;
  return t.run();
}
