#include "regression/tframe.h"
#include "WeatherArea.h"

using namespace std;

namespace WeatherAreaTest
{

  //! Test structors
  void structors(void)
  {
	using namespace WeatherAnalysis;

	WeatherArea s1(NFmiPoint(25,60),"Helsinki");
	WeatherArea s2(NFmiPoint(25,60),"Helsingin seutu",30);
	WeatherArea s3("25,60");
	WeatherArea s4("25,60:30");
	WeatherArea s5("Helsinki");
	WeatherArea s6("Helsinki:30");

	WeatherArea s7(s1);
	s7 = s1;

	TEST_PASSED();
  }

  //! Test accessors
  void accessors(void)
  {
	using namespace WeatherAnalysis;

	WeatherArea area1("25,60");
	WeatherArea area2("25,60:30","Helsinki");
	WeatherArea area3("Helsinki");
	WeatherArea area4("Helsinki:30","Helsinki");

	if(area1.isNamed())
	  TEST_FAILED("isNamed failed for unnamed (25,60)");
	if(!area2.isNamed())
	  TEST_FAILED("isNamed failed for named (25,60)");
	if(area3.isNamed())
	  TEST_FAILED("isNamed failed for unnamed Helsinki");
	if(!area4.isNamed())
	  TEST_FAILED("isNamed failed for named Helsinki");

	if(!(area1.isPoint()))
	  TEST_FAILED("isPoint failed for 25,60");
	if(area2.isPoint())
	  TEST_FAILED("isPoint failed for 25,60:30");
	if(!(area3.isPoint()))
	  TEST_FAILED("isPoint failed for Helsinki");
	if(area4.isPoint())
	  TEST_FAILED("isPoint failed for Helsinki:30");

	if(area2.name() != "Helsinki")
	  TEST_FAILED("Failed to return name Helsinki for 25,60:30");

	if(area4.name() != "Helsinki")
	  TEST_FAILED("Failed to return name Helsinki for Helsinki:30");

	if(area1.point() != NFmiPoint(25,60))
	  TEST_FAILED("Failed to return point 25,60");

	if(area1.radius() != 0)
	  TEST_FAILED("Failed to return radius 0 for 25,60");
	if(area2.radius() != 30)
	  TEST_FAILED("Failed to return radius 30 for 25,60:30");
	if(area3.radius() != 0)
	  TEST_FAILED("Failed to return radius 0 for Helsinki");
	if(area4.radius() != 30)
	  TEST_FAILED("Failed to return radius 30 for Helsinki:30");

	TEST_PASSED();
  }

  //! Test comparisons
  void comparisons(void)
  {
	using namespace WeatherAnalysis;

	WeatherArea area1(NFmiPoint(25,60),"Helsinki");
	WeatherArea area2(NFmiPoint(25,60),"Helsingin seutu",30);
	WeatherArea area3("25,60","Helsinki");
	WeatherArea area4("25,60:30","Helsinki");
	WeatherArea area5("Helsinki","Helsinki");
	WeatherArea area6("Helsinki:30","Helsingin seutu");

	if(!(area1==area1))
	  TEST_FAILED("area1==area1 test failed");
	if(area1!=area1)
	  TEST_FAILED("area1!=area1 test failed");

	if(area1==area2)
	  TEST_FAILED("area1==area2 test failed");
	if(!(area1==area3))
	  TEST_FAILED("area1!=area3 test failed");
	if(!(area1==area4))
	  TEST_FAILED("area1!=area4 test failed");
	if(!(area1==area5))
	  TEST_FAILED("area1!=area5 test failed");
	if(area1==area6)
	  TEST_FAILED("area1==area6 test failed");
	if(!(area2==area6))
	  TEST_FAILED("area2!=area6 test failed");

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
