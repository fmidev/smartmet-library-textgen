#include "regression/tframe.h"
#include "MapSource.h"
#include "newbase/NFmiSvgPath.h"

using namespace std;

namespace MapSourceTest
{

  //! Test getMap()
  void getMap(void)
  {
	using namespace WeatherAnalysis;

	MapSource maps;

	// should succeed
	const NFmiSvgPath & uusimaa = maps.getMap("maatalous/uusimaa");
	if(uusimaa.empty())
	  TEST_FAILED("getMap returned empty SVG for uusimaa");

	// should succeed and have same address
	const NFmiSvgPath & kymenlaakso1 = maps.getMap("maatalous/kymenlaakso");
	const NFmiSvgPath & kymenlaakso2 = maps.getMap("maatalous/kymenlaakso");
	if(&kymenlaakso1 != &kymenlaakso2)
	  TEST_FAILED("getMap should return same map for save map name");

	// should throw
	try
	  {
		maps.getMap("foo/bar");
		TEST_FAILED("getMap should throw for map foo/bar");
	  }
	catch(...)
	  {
	  }

	TEST_PASSED();
  }

  //! Test copying
  void copying(void)
  {
	using namespace WeatherAnalysis;

	MapSource maps1;
	const NFmiSvgPath & uusimaa1 = maps1.getMap("maatalous/uusimaa");

	MapSource maps2(maps1);
	const NFmiSvgPath & uusimaa2 = maps2.getMap("maatalous/uusimaa");

	if(&uusimaa1 != &uusimaa2)
	  TEST_FAILED("Copy constructed MapSource returns different address maps");

	MapSource maps3;
	maps3 = maps1;
	const NFmiSvgPath & uusimaa3 = maps3.getMap("maatalous/uusimaa");
	if(&uusimaa1 != &uusimaa3)
	  TEST_FAILED("MapSource copied with = returns different address maps");

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
	  TEST(getMap);
	  TEST(copying);
	}

  }; // class tests

} // namespace MapSourceTest


int main(void)
{
  cout << endl
	   << "MapSource tester" << endl
	   << "================" << endl;
  MapSourceTest::tests t;
  return t.run();
}
