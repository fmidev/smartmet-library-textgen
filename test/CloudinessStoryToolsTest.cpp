#include "regression/tframe.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "CloudinessStoryTools.h"

#include "NFmiSettings.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;

namespace CloudinessStoryToolsTest
{
  shared_ptr<TextGen::Dictionary> dict;

  // ----------------------------------------------------------------------
  /*!
   * \brief Test CloudinessStoryTools::cloudinesstype
   */
  // ----------------------------------------------------------------------

  void cloudinesstype()
  {
	// Default values based on documentation:
	//
	// single_class_limit = 90
	// mostly_class_limit = 80
	// no_class_limit     = 20
	// trend_limit        = 80

	using namespace TextGen;
	using namespace TextGen::CloudinessStoryTools;

	// running cloudiness
	if(CloudinessStoryTools::cloudinesstype("",95,0,0) != Cloudy)
	  TEST_FAILED("Failed to return Cloudy for 95% cloudy, 0% clear");
	if(CloudinessStoryTools::cloudinesstype("",90,0,0) != Cloudy)
	  TEST_FAILED("Failed to return Cloudy for 90% cloudy, 0% clear");
	if(CloudinessStoryTools::cloudinesstype("",85,0,0) != MostlyCloudy)
	  TEST_FAILED("Failed to return MostlyCloudy for 85% cloudy, 0% clear");
	if(CloudinessStoryTools::cloudinesstype("",80,0,0) != MostlyCloudy)
	  TEST_FAILED("Failed to return MostlyCloudy for 80% cloudy, 0% clear");
	if(CloudinessStoryTools::cloudinesstype("",75,0,0) != CloudyOrPartlyCloudy)
	  TEST_FAILED("Failed to return CloudyOrPartlyCloudy for 75% cloudy, 0% clear");
	if(CloudinessStoryTools::cloudinesstype("",60,0,0) != CloudyOrPartlyCloudy)
	  TEST_FAILED("Failed to return CloudyOrPartlyCloudy for 60% cloudy, 0% clear");
	if(CloudinessStoryTools::cloudinesstype("",50,0,0) != CloudyOrPartlyCloudy)
	  TEST_FAILED("Failed to return CloudyOrPartlyCloudy for 50% cloudy, 0% clear");
	if(CloudinessStoryTools::cloudinesstype("",40,0,0) != CloudyOrPartlyCloudy)
	  TEST_FAILED("Failed to return CloudyOrPartlyCloudy for 40% cloudy, 0% clear");
	if(CloudinessStoryTools::cloudinesstype("",30,0,0) != CloudyOrPartlyCloudy)
	  TEST_FAILED("Failed to return CloudyOrPartlyCloudy for 30% cloudy, 0% clear");
	if(CloudinessStoryTools::cloudinesstype("",20,0,0) != MostlyPartlyCloudy)
	  TEST_FAILED("Failed to return MostlyPartlyCloudy for 20% cloudy, 0% clear");
	if(CloudinessStoryTools::cloudinesstype("",10,0,0) != PartlyCloudy)
	  TEST_FAILED("Failed to return PartlyCloudy for 10% cloudy, 0% clear");

	// running clear
	if(CloudinessStoryTools::cloudinesstype("",0,95,0) != Clear)
	  TEST_FAILED("Failed to return Clear for 95% clear, 0% cloudy");
	if(CloudinessStoryTools::cloudinesstype("",0,90,0) != Clear)
	  TEST_FAILED("Failed to return Clear for 90% clear, 0% cloudy");
	if(CloudinessStoryTools::cloudinesstype("",0,85,0) != MostlyClear)
	  TEST_FAILED("Failed to return MostlyClear for 85% clear, 0% cloudy");
	if(CloudinessStoryTools::cloudinesstype("",0,80,0) != MostlyClear)
	  TEST_FAILED("Failed to return MostlyClear for 80% clear, 0% cloudy");
	if(CloudinessStoryTools::cloudinesstype("",0,75,0) != ClearOrPartlyCloudy)
	  TEST_FAILED("Failed to return ClearOrPartlyCloudy for 75% clear, 0% cloudy");
	if(CloudinessStoryTools::cloudinesstype("",0,60,0) != ClearOrPartlyCloudy)
	  TEST_FAILED("Failed to return ClearOrPartlyCloudy for 60% clear, 0% cloudy");
	if(CloudinessStoryTools::cloudinesstype("",0,50,0) != ClearOrPartlyCloudy)
	  TEST_FAILED("Failed to return ClearOrPartlyCloudy for 50% clear, 0% cloudy");
	if(CloudinessStoryTools::cloudinesstype("",0,40,0) != ClearOrPartlyCloudy)
	  TEST_FAILED("Failed to return ClearOrPartlyCloudy for 40% clear, 0% cloudy");
	if(CloudinessStoryTools::cloudinesstype("",0,30,0) != ClearOrPartlyCloudy)
	  TEST_FAILED("Failed to return ClearOrPartlyCloudy for 30% clear, 0% cloudy");
	if(CloudinessStoryTools::cloudinesstype("",0,20,0) != MostlyPartlyCloudy)
	  TEST_FAILED("Failed to return MostlyPartlyCloudy for 20% clear, 0% cloudy");
	if(CloudinessStoryTools::cloudinesstype("",0,10,0) != PartlyCloudy)
	  TEST_FAILED("Failed to return PartlyCloudy for 10% clear, 0% cloudy");

	// fixed clear, running cloudy

	if(CloudinessStoryTools::cloudinesstype("",80,20,0) != MostlyCloudy)
	  TEST_FAILED("Failed to return MostlyCloudy for 80% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",75,20,0) != VariableCloudiness)
	  TEST_FAILED("Failed to return VariableCloudiness for 75% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",60,20,0) != VariableCloudiness)
	  TEST_FAILED("Failed to return VariableCloudiness for 60% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",50,20,0) != VariableCloudiness)
	  TEST_FAILED("Failed to return VariableCloudiness for 50% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",40,20,0) != VariableCloudiness)
	  TEST_FAILED("Failed to return VariableCloudiness for 40% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",30,20,0) != VariableCloudiness)
	  TEST_FAILED("Failed to return VariableCloudiness for 30% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",20,20,0) != VariableCloudiness)
	  TEST_FAILED("Failed to return VariableCloudiness for 20% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",10,20,0) != ClearOrPartlyCloudy)
	  TEST_FAILED("Failed to return ClearOrPartlyCloudy for 10% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",0,20,0) != MostlyPartlyCloudy)
	  TEST_FAILED("Failed to return MostlyPartlyCloudy for 0% cloudy, 20% clear");

	// fixed clear, running cloudy, with big trend

	if(CloudinessStoryTools::cloudinesstype("",80,20,90) != MostlyCloudy)
	  TEST_FAILED("Failed to return MostlyCloudy for 80% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",70,20,90) != IncreasingCloudiness)
	  TEST_FAILED("Failed to return IncreasingCloudiness for 70% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",60,20,90) != IncreasingCloudiness)
	  TEST_FAILED("Failed to return IncreasingCloudiness for 60% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",50,20,90) != IncreasingCloudiness)
	  TEST_FAILED("Failed to return IncreasingCloudiness for 50% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",40,20,90) != IncreasingCloudiness)
	  TEST_FAILED("Failed to return IncreasingCloudiness for 40% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",30,20,90) != IncreasingCloudiness)
	  TEST_FAILED("Failed to return IncreasingCloudiness for 30% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",20,20,90) != IncreasingCloudiness)
	  TEST_FAILED("Failed to return IncreasingCloudiness for 20% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",20,20,90) != IncreasingCloudiness)
	  TEST_FAILED("Failed to return IncreasingCloudiness for 10% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",0,20,90) != MostlyPartlyCloudy)
	  TEST_FAILED("Failed to return MostlyPartlyCloudy for 0% cloudy, 20% clear");

	// fixed clear, running cloudy, with big decreasing trend

	if(CloudinessStoryTools::cloudinesstype("",80,20,-90) != MostlyCloudy)
	  TEST_FAILED("Failed to return MostlyCloudy for 80% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",70,20,-90) != DecreasingCloudiness)
	  TEST_FAILED("Failed to return DecreasingCloudiness for 70% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",60,20,-90) != DecreasingCloudiness)
	  TEST_FAILED("Failed to return DecreasingCloudiness for 60% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",50,20,-90) != DecreasingCloudiness)
	  TEST_FAILED("Failed to return DecreasingCloudiness for 50% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",40,20,-90) != DecreasingCloudiness)
	  TEST_FAILED("Failed to return DecreasingCloudiness for 40% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",30,20,-90) != DecreasingCloudiness)
	  TEST_FAILED("Failed to return DecreasingCloudiness for 30% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",20,20,-90) != DecreasingCloudiness)
	  TEST_FAILED("Failed to return DecreasingCloudiness for 20% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",20,20,-90) != DecreasingCloudiness)
	  TEST_FAILED("Failed to return DecreasingCloudiness for 10% cloudy, 20% clear");
	if(CloudinessStoryTools::cloudinesstype("",0,20,-90) != MostlyPartlyCloudy)
	  TEST_FAILED("Failed to return MostlyPartlyCloudy for 0% cloudy, 20% clear");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test CloudinessStoryTools::similartype
   */
  // ----------------------------------------------------------------------

  void similartype()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief The actual test driver
   */
  // ----------------------------------------------------------------------

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
	  TEST(cloudinesstype);
	  TEST(similartype);
	}

  }; // class tests

} // namespace CloudinessStoryToolsTest


int main(void)
{
  using namespace CloudinessStoryToolsTest;

  cout << endl
	   << "CloudinessStoryTools tests" << endl
	   << "==========================" << endl;

  dict = TextGen::DictionaryFactory::create("null");

  tests t;
  return t.run();
}
