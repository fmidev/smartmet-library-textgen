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
	using namespace TextGen;
	using namespace TextGen::CloudinessStoryTools;

	// these tests hardcode the table in the documentation
	// of the cloudiness_overview story

	if(!similartype(Cloudy,Cloudy).second)
	  TEST_FAILED("Cloudy is similar to Cloudy");
	if(!similartype(Cloudy,PartlyCloudy).second)
	  TEST_FAILED("Cloudy is similar to PartlyCloudy");
	if(similartype(Cloudy,Clear).second)
	  TEST_FAILED("Cloudy is not similar to Clear");
	if(!similartype(Cloudy,MostlyCloudy).second)
	  TEST_FAILED("Cloudy is similar to MostlyCloudy");
	if(!similartype(Cloudy,MostlyPartlyCloudy).second)
	  TEST_FAILED("Cloudy is similar to MostlyPartlyCloudy");
	if(similartype(Cloudy,MostlyClear).second)
	  TEST_FAILED("Cloudy is not similar to MostlyClear");
	if(!similartype(Cloudy,CloudyOrPartlyCloudy).second)
	  TEST_FAILED("Cloudy is similar to CloudyOrPartlyCloudy");
	if(similartype(Cloudy,ClearOrPartlyCloudy).second)
	  TEST_FAILED("Cloudy is not similar to ClearOrPartlyCloudy");
	if(similartype(Cloudy,DecreasingCloudiness).second)
	  TEST_FAILED("Cloudy is not similar to DecreasingCloudiness");
	if(similartype(Cloudy,IncreasingCloudiness).second)
	  TEST_FAILED("Cloudy is not similar to IncreasingCloudiness");
	if(similartype(Cloudy,VariableCloudiness).second)
	  TEST_FAILED("Cloudy is not similar to VariableCloudiness");

	if(!similartype(PartlyCloudy,Cloudy).second)
	  TEST_FAILED("PartlyCloudy is similar to Cloudy");
	if(!similartype(PartlyCloudy,PartlyCloudy).second)
	  TEST_FAILED("PartlyCloudy is similar to PartlyCloudy");
	if(!similartype(PartlyCloudy,Clear).second)
	  TEST_FAILED("PartlyCloudy is similar to Clear");
	if(!similartype(PartlyCloudy,MostlyCloudy).second)
	  TEST_FAILED("PartlyCloudy is similar to MostlyCloudy");
	if(!similartype(PartlyCloudy,MostlyPartlyCloudy).second)
	  TEST_FAILED("PartlyCloudy is similar to MostlyPartlyCloudy");
	if(!similartype(PartlyCloudy,MostlyClear).second)
	  TEST_FAILED("PartlyCloudy is similar to MostlyClear");
	if(!similartype(PartlyCloudy,CloudyOrPartlyCloudy).second)
	  TEST_FAILED("PartlyCloudy is similar to CloudyOrPartlyCloudy");
	if(!similartype(PartlyCloudy,ClearOrPartlyCloudy).second)
	  TEST_FAILED("PartlyCloudy is similar to ClearOrPartlyCloudy");
	if(similartype(PartlyCloudy,DecreasingCloudiness).second)
	  TEST_FAILED("PartlyCloudy is not similar to DecreasingCloudiness");
	if(similartype(PartlyCloudy,IncreasingCloudiness).second)
	  TEST_FAILED("PartlyCloudy is not similar to IncreasingCloudiness");
	if(!similartype(PartlyCloudy,VariableCloudiness).second)
	  TEST_FAILED("PartlyCloudy is similar to VariableCloudiness");

	if(similartype(Clear,Cloudy).second)
	  TEST_FAILED("Clear is not similar to Cloudy");
	if(!similartype(Clear,PartlyCloudy).second)
	  TEST_FAILED("Clear is similar to PartlyCloudy");
	if(!similartype(Clear,Clear).second)
	  TEST_FAILED("Clear is similar to Clear");
	if(similartype(Clear,MostlyCloudy).second)
	  TEST_FAILED("Clear is not similar to MostlyCloudy");
	if(!similartype(Clear,MostlyPartlyCloudy).second)
	  TEST_FAILED("Clear is similar to MostlyPartlyCloudy");
	if(!similartype(Clear,MostlyClear).second)
	  TEST_FAILED("Clear is similar to MostlyClear");
	if(similartype(Clear,CloudyOrPartlyCloudy).second)
	  TEST_FAILED("Clear is not similar to CloudyOrPartlyCloudy");
	if(!similartype(Clear,ClearOrPartlyCloudy).second)
	  TEST_FAILED("Clear is similar to ClearOrPartlyCloudy");
	if(similartype(Clear,DecreasingCloudiness).second)
	  TEST_FAILED("Clear is not similar to DecreasingCloudiness");
	if(similartype(Clear,IncreasingCloudiness).second)
	  TEST_FAILED("Clear is not similar to IncreasingCloudiness");
	if(similartype(Clear,VariableCloudiness).second)
	  TEST_FAILED("Clear is not similar to VariableCloudiness");

	if(!similartype(MostlyCloudy,Cloudy).second)
	  TEST_FAILED("MostlyCloudy is similar to Cloudy");
	if(!similartype(MostlyCloudy,PartlyCloudy).second)
	  TEST_FAILED("MostlyCloudy is similar to PartlyCloudy");
	if(similartype(MostlyCloudy,Clear).second)
	  TEST_FAILED("MostlyCloudy is not similar to Clear");
	if(!similartype(MostlyCloudy,MostlyCloudy).second)
	  TEST_FAILED("MostlyCloudy is similar to MostlyCloudy");
	if(!similartype(MostlyCloudy,MostlyPartlyCloudy).second)
	  TEST_FAILED("MostlyCloudy is similar to MostlyPartlyCloudy");
	if(similartype(MostlyCloudy,MostlyClear).second)
	  TEST_FAILED("MostlyCloudy is not similar to MostlyClear");
	if(!similartype(MostlyCloudy,CloudyOrPartlyCloudy).second)
	  TEST_FAILED("MostlyCloudy is similar to CloudyOrPartlyCloudy");
	if(similartype(MostlyCloudy,ClearOrPartlyCloudy).second)
	  TEST_FAILED("MostlyCloudy is not similar to ClearOrPartlyCloudy");
	if(similartype(MostlyCloudy,DecreasingCloudiness).second)
	  TEST_FAILED("MostlyCloudy is not similar to DecreasingCloudiness");
	if(similartype(MostlyCloudy,IncreasingCloudiness).second)
	  TEST_FAILED("MostlyCloudy is not similar to IncreasingCloudiness");
	if(similartype(MostlyCloudy,VariableCloudiness).second)
	  TEST_FAILED("MostlyCloudy is not similar to VariableCloudiness");

	if(!similartype(MostlyPartlyCloudy,Cloudy).second)
	  TEST_FAILED("MostlyPartlyCloudy is similar to Cloudy");
	if(!similartype(MostlyPartlyCloudy,PartlyCloudy).second)
	  TEST_FAILED("MostlyPartlyCloudy is similar to PartlyCloudy");
	if(!similartype(MostlyPartlyCloudy,Clear).second)
	  TEST_FAILED("MostlyPartlyCloudy is similar to Clear");
	if(!similartype(MostlyPartlyCloudy,MostlyCloudy).second)
	  TEST_FAILED("MostlyPartlyCloudy is similar to MostlyCloudy");
	if(!similartype(MostlyPartlyCloudy,MostlyPartlyCloudy).second)
	  TEST_FAILED("MostlyPartlyCloudy is similar to MostlyPartlyCloudy");
	if(!similartype(MostlyPartlyCloudy,MostlyClear).second)
	  TEST_FAILED("MostlyPartlyCloudy is similar to MostlyClear");
	if(!similartype(MostlyPartlyCloudy,CloudyOrPartlyCloudy).second)
	  TEST_FAILED("MostlyPartlyCloudy is similar to CloudyOrPartlyCloudy");
	if(!similartype(MostlyPartlyCloudy,ClearOrPartlyCloudy).second)
	  TEST_FAILED("MostlyPartlyCloudy is similar to ClearOrPartlyCloudy");
	if(similartype(MostlyPartlyCloudy,DecreasingCloudiness).second)
	  TEST_FAILED("MostlyPartlyCloudy is not similar to DecreasingCloudiness");
	if(similartype(MostlyPartlyCloudy,IncreasingCloudiness).second)
	  TEST_FAILED("MostlyPartlyCloudy is not similar to IncreasingCloudiness");
	if(!similartype(MostlyPartlyCloudy,VariableCloudiness).second)
	  TEST_FAILED("MostlyPartlyCloudy is similar to VariableCloudiness");

	if(similartype(MostlyClear,Cloudy).second)
	  TEST_FAILED("MostlyClear is not similar to Cloudy");
	if(!similartype(MostlyClear,PartlyCloudy).second)
	  TEST_FAILED("MostlyClear is similar to PartlyCloudy");
	if(!similartype(MostlyClear,Clear).second)
	  TEST_FAILED("MostlyClear is similar to Clear");
	if(similartype(MostlyClear,MostlyCloudy).second)
	  TEST_FAILED("MostlyClear is not similar to MostlyCloudy");
	if(!similartype(MostlyClear,MostlyPartlyCloudy).second)
	  TEST_FAILED("MostlyClear is similar to MostlyPartlyCloudy");
	if(!similartype(MostlyClear,MostlyClear).second)
	  TEST_FAILED("MostlyClear is similar to MostlyClear");
	if(similartype(MostlyClear,CloudyOrPartlyCloudy).second)
	  TEST_FAILED("MostlyClear is not similar to CloudyOrPartlyCloudy");
	if(!similartype(MostlyClear,ClearOrPartlyCloudy).second)
	  TEST_FAILED("MostlyClear is similar to ClearOrPartlyCloudy");
	if(similartype(MostlyClear,DecreasingCloudiness).second)
	  TEST_FAILED("MostlyClear is not similar to DecreasingCloudiness");
	if(similartype(MostlyClear,IncreasingCloudiness).second)
	  TEST_FAILED("MostlyClear is not similar to IncreasingCloudiness");
	if(similartype(MostlyClear,VariableCloudiness).second)
	  TEST_FAILED("MostlyClear is not similar to VariableCloudiness");

	if(!similartype(CloudyOrPartlyCloudy,Cloudy).second)
	  TEST_FAILED("CloudyOrPartlyCloudy is similar to Cloudy");
	if(!similartype(CloudyOrPartlyCloudy,PartlyCloudy).second)
	  TEST_FAILED("CloudyOrPartlyCloudy is similar to PartlyCloudy");
	if(similartype(CloudyOrPartlyCloudy,Clear).second)
	  TEST_FAILED("CloudyOrPartlyCloudy is not similar to Clear");
	if(!similartype(CloudyOrPartlyCloudy,MostlyCloudy).second)
	  TEST_FAILED("CloudyOrPartlyCloudy is similar to MostlyCloudy");
	if(!similartype(CloudyOrPartlyCloudy,MostlyPartlyCloudy).second)
	  TEST_FAILED("CloudyOrPartlyCloudy is similar to MostlyPartlyCloudy");
	if(similartype(CloudyOrPartlyCloudy,MostlyClear).second)
	  TEST_FAILED("CloudyOrPartlyCloudy is not similar to MostlyClear");
	if(!similartype(CloudyOrPartlyCloudy,CloudyOrPartlyCloudy).second)
	  TEST_FAILED("CloudyOrPartlyCloudy is similar to CloudyOrPartlyCloudy");
	if(!similartype(CloudyOrPartlyCloudy,ClearOrPartlyCloudy).second)
	  TEST_FAILED("CloudyOrPartlyCloudy is similar to ClearOrPartlyCloudy");
	if(similartype(CloudyOrPartlyCloudy,DecreasingCloudiness).second)
	  TEST_FAILED("CloudyOrPartlyCloudy is not similar to DecreasingCloudiness");
	if(similartype(CloudyOrPartlyCloudy,IncreasingCloudiness).second)
	  TEST_FAILED("CloudyOrPartlyCloudy is not similar to IncreasingCloudiness");
	if(!similartype(CloudyOrPartlyCloudy,VariableCloudiness).second)
	  TEST_FAILED("CloudyOrPartlyCloudy is similar to VariableCloudiness");

	if(similartype(ClearOrPartlyCloudy,Cloudy).second)
	  TEST_FAILED("ClearOrPartlyCloudy is not similar to Cloudy");
	if(!similartype(ClearOrPartlyCloudy,PartlyCloudy).second)
	  TEST_FAILED("ClearOrPartlyCloudy is similar to PartlyCloudy");
	if(!similartype(ClearOrPartlyCloudy,Clear).second)
	  TEST_FAILED("ClearOrPartlyCloudy is similar to Clear");
	if(!similartype(ClearOrPartlyCloudy,MostlyCloudy).second)
	  TEST_FAILED("ClearOrPartlyCloudy is similar to MostlyCloudy");
	if(!similartype(ClearOrPartlyCloudy,MostlyPartlyCloudy).second)
	  TEST_FAILED("ClearOrPartlyCloudy is similar to MostlyPartlyCloudy");
	if(!similartype(ClearOrPartlyCloudy,MostlyClear).second)
	  TEST_FAILED("ClearOrPartlyCloudy is similar to MostlyClear");
	if(!similartype(ClearOrPartlyCloudy,CloudyOrPartlyCloudy).second)
	  TEST_FAILED("ClearOrPartlyCloudy is similar to CloudyOrPartlyCloudy");
	if(!similartype(ClearOrPartlyCloudy,ClearOrPartlyCloudy).second)
	  TEST_FAILED("ClearOrPartlyCloudy is similar to ClearOrPartlyCloudy");
	if(similartype(ClearOrPartlyCloudy,DecreasingCloudiness).second)
	  TEST_FAILED("ClearOrPartlyCloudy is not similar to DecreasingCloudiness");
	if(similartype(ClearOrPartlyCloudy,IncreasingCloudiness).second)
	  TEST_FAILED("ClearOrPartlyCloudy is not similar to IncreasingCloudiness");
	if(!similartype(ClearOrPartlyCloudy,VariableCloudiness).second)
	  TEST_FAILED("ClearOrPartlyCloudy is similar to VariableCloudiness");

	if(similartype(DecreasingCloudiness,Cloudy).second)
	  TEST_FAILED("DecreasingCloudiness is not similar to Cloudy");
	if(similartype(DecreasingCloudiness,PartlyCloudy).second)
	  TEST_FAILED("DecreasingCloudiness is not similar to PartlyCloudy");
	if(!similartype(DecreasingCloudiness,Clear).second)
	  TEST_FAILED("DecreasingCloudiness is similar to Clear");
	if(similartype(DecreasingCloudiness,MostlyCloudy).second)
	  TEST_FAILED("DecreasingCloudiness is not similar to MostlyCloudy");
	if(similartype(DecreasingCloudiness,MostlyPartlyCloudy).second)
	  TEST_FAILED("DecreasingCloudiness is not similar to MostlyPartlyCloudy");
	if(!similartype(DecreasingCloudiness,MostlyClear).second)
	  TEST_FAILED("DecreasingCloudiness is similar to MostlyClear");
	if(similartype(DecreasingCloudiness,CloudyOrPartlyCloudy).second)
	  TEST_FAILED("DecreasingCloudiness is not similar to CloudyOrPartlyCloudy");
	if(similartype(DecreasingCloudiness,ClearOrPartlyCloudy).second)
	  TEST_FAILED("DecreasingCloudiness is not similar to ClearOrPartlyCloudy");
	if(!similartype(DecreasingCloudiness,DecreasingCloudiness).second)
	  TEST_FAILED("DecreasingCloudiness is similar to DecreasingCloudiness");
	if(similartype(DecreasingCloudiness,IncreasingCloudiness).second)
	  TEST_FAILED("DecreasingCloudiness is not similar to IncreasingCloudiness");
	if(similartype(DecreasingCloudiness,VariableCloudiness).second)
	  TEST_FAILED("DecreasingCloudiness is not similar to VariableCloudiness");

	if(!similartype(IncreasingCloudiness,Cloudy).second)
	  TEST_FAILED("IncreasingCloudiness is similar to Cloudy");
	if(similartype(IncreasingCloudiness,PartlyCloudy).second)
	  TEST_FAILED("IncreasingCloudiness is not similar to PartlyCloudy");
	if(similartype(IncreasingCloudiness,Clear).second)
	  TEST_FAILED("IncreasingCloudiness is not similar to Clear");
	if(!similartype(IncreasingCloudiness,MostlyCloudy).second)
	  TEST_FAILED("IncreasingCloudiness is similar to MostlyCloudy");
	if(similartype(IncreasingCloudiness,MostlyPartlyCloudy).second)
	  TEST_FAILED("IncreasingCloudiness is not similar to MostlyPartlyCloudy");
	if(similartype(IncreasingCloudiness,MostlyClear).second)
	  TEST_FAILED("IncreasingCloudiness is not similar to MostlyClear");
	if(similartype(IncreasingCloudiness,CloudyOrPartlyCloudy).second)
	  TEST_FAILED("IncreasingCloudiness is not similar to CloudyOrPartlyCloudy");
	if(similartype(IncreasingCloudiness,ClearOrPartlyCloudy).second)
	  TEST_FAILED("IncreasingCloudiness is not similar to ClearOrPartlyCloudy");
	if(similartype(IncreasingCloudiness,DecreasingCloudiness).second)
	  TEST_FAILED("IncreasingCloudiness is not similar to DecreasingCloudiness");
	if(!similartype(IncreasingCloudiness,IncreasingCloudiness).second)
	  TEST_FAILED("IncreasingCloudiness is similar to IncreasingCloudiness");
	if(!similartype(IncreasingCloudiness,VariableCloudiness).second)
	  TEST_FAILED("IncreasingCloudiness is similar to VariableCloudiness");

	if(similartype(VariableCloudiness,Cloudy).second)
	  TEST_FAILED("VariableCloudiness is not similar to Cloudy");
	if(!similartype(VariableCloudiness,PartlyCloudy).second)
	  TEST_FAILED("VariableCloudiness is similar to PartlyCloudy");
	if(similartype(VariableCloudiness,Clear).second)
	  TEST_FAILED("VariableCloudiness is not similar to Clear");
	if(!similartype(VariableCloudiness,MostlyCloudy).second)
	  TEST_FAILED("VariableCloudiness is similar to MostlyCloudy");
	if(!similartype(VariableCloudiness,MostlyPartlyCloudy).second)
	  TEST_FAILED("VariableCloudiness is similar to MostlyPartlyCloudy");
	if(!similartype(VariableCloudiness,MostlyClear).second)
	  TEST_FAILED("VariableCloudiness is similar to MostlyClear");
	if(!similartype(VariableCloudiness,CloudyOrPartlyCloudy).second)
	  TEST_FAILED("VariableCloudiness is similar to CloudyOrPartlyCloudy");
	if(!similartype(VariableCloudiness,ClearOrPartlyCloudy).second)
	  TEST_FAILED("VariableCloudiness is similar to ClearOrPartlyCloudy");
	if(similartype(VariableCloudiness,DecreasingCloudiness).second)
	  TEST_FAILED("VariableCloudiness is not similar to DecreasingCloudiness");
	if(!similartype(VariableCloudiness,IncreasingCloudiness).second)
	  TEST_FAILED("VariableCloudiness is similar to IncreasingCloudiness");
	if(!similartype(VariableCloudiness,VariableCloudiness).second)
	  TEST_FAILED("VariableCloudiness is similar to VariableCloudiness");

	// test that the combination is always correct

	if(similartype(Cloudy,Cloudy).first != Cloudy)
	  TEST_FAILED("Cloudy+Cloudy is similar to Cloudy");
	if(similartype(Cloudy,PartlyCloudy).first != CloudyOrPartlyCloudy)
	  TEST_FAILED("Cloudy+PartlyCloudy is similar to CloudyOrPartlyCloudy");
	if(similartype(Cloudy,MostlyCloudy).first != MostlyCloudy)
	  TEST_FAILED("Cloudy+MostlyCloudy is similar to MostlyCloudy");
	if(similartype(Cloudy,MostlyPartlyCloudy).first != CloudyOrPartlyCloudy)
	  TEST_FAILED("Cloudy+MostlyPartlyCloudy is similar to CloudyOrPartlyCloudy");
	if(similartype(Cloudy,CloudyOrPartlyCloudy).first != CloudyOrPartlyCloudy)
	  TEST_FAILED("Cloudy+CloudyOrPartlyCloudy is similar to CloudyOrPartlyCloudy");

	if(similartype(PartlyCloudy,Cloudy).first != CloudyOrPartlyCloudy)
	  TEST_FAILED("PartlyCloudy+Cloudy is similar to CloudyOrPartlyCloudy");
	if(similartype(PartlyCloudy,PartlyCloudy).first != PartlyCloudy)
	  TEST_FAILED("PartlyCloudy+PartlyCloudy is similar to PartlyCloudy");
	if(similartype(PartlyCloudy,Clear).first != ClearOrPartlyCloudy)
	  TEST_FAILED("PartlyCloudy+Clear is similar to ClearOrPartlyCloudy");
	if(similartype(PartlyCloudy,MostlyCloudy).first != CloudyOrPartlyCloudy)
	  TEST_FAILED("PartlyCloudy+MostlyCloudy is similar to CloudyOrPartlyCloudy");
	if(similartype(PartlyCloudy,MostlyPartlyCloudy).first != MostlyPartlyCloudy)
	  TEST_FAILED("PartlyCloudy+MostlyPartlyCloudy is similar to MostlyPartlyCloudy");
	if(similartype(PartlyCloudy,MostlyClear).first != ClearOrPartlyCloudy)
	  TEST_FAILED("PartlyCloudy+MostlyClear is similar to ClearOrPartlyCloudy");
	if(similartype(PartlyCloudy,CloudyOrPartlyCloudy).first != CloudyOrPartlyCloudy)
	  TEST_FAILED("PartlyCloudy+CloudyOrPartlyCloudy is similar to CloudyOrPartlyCloudy");
	if(similartype(PartlyCloudy,ClearOrPartlyCloudy).first != ClearOrPartlyCloudy)
	  TEST_FAILED("PartlyCloudy+ClearOrPartlyCloudy is similar to ClearOrPartlyCloudy");
	if(similartype(PartlyCloudy,VariableCloudiness).first != VariableCloudiness)
	  TEST_FAILED("PartlyCloudy+VariableCloudiness is similar to VariableCloudiness");

	if(similartype(Clear,PartlyCloudy).first != ClearOrPartlyCloudy)
	  TEST_FAILED("Clear+PartlyCloudy is similar to ClearOrPartlyCloudy");
	if(similartype(Clear,Clear).first != Clear)
	  TEST_FAILED("Clear+Clear is similar to Clear");
	if(similartype(Clear,MostlyPartlyCloudy).first != ClearOrPartlyCloudy)
	  TEST_FAILED("Clear+MostlyPartlyCloudy is similar to ClearOrPartlyCloudy");
	if(similartype(Clear,MostlyClear).first != MostlyClear)
	  TEST_FAILED("Clear+MostlyClear is similar to MostlyClear");
	if(similartype(Clear,ClearOrPartlyCloudy).first != ClearOrPartlyCloudy)
	  TEST_FAILED("Clear+ClearOrPartlyCloudy is similar to ClearOrPartlyCloudy");

	if(similartype(MostlyCloudy,Cloudy).first != MostlyCloudy)
	  TEST_FAILED("MostlyCloudy+Cloudy is similar to MostlyCloudy");
	if(similartype(MostlyCloudy,PartlyCloudy).first != CloudyOrPartlyCloudy)
	  TEST_FAILED("MostlyCloudy+PartlyCloudy is similar to CloudyOrPartlyCloudy");
	if(similartype(MostlyCloudy,MostlyCloudy).first != MostlyCloudy)
	  TEST_FAILED("MostlyCloudy+MostlyCloudy is similar to MostlyCloudy");
	if(similartype(MostlyCloudy,MostlyPartlyCloudy).first != CloudyOrPartlyCloudy)
	  TEST_FAILED("MostlyCloudy+MostlyPartlyCloudy is similar to CloudyOrPartlyCloudy");
	if(similartype(MostlyCloudy,CloudyOrPartlyCloudy).first != CloudyOrPartlyCloudy)
	  TEST_FAILED("MostlyCloudy+CloudyOrPartlyCloudy is similar to CloudyOrPartlyCloudy");

	if(similartype(MostlyPartlyCloudy,Cloudy).first != CloudyOrPartlyCloudy)
	  TEST_FAILED("MostlyPartlyCloudy+Cloudy is similar to CloudyOrPartlyCloudy");
	if(similartype(MostlyPartlyCloudy,PartlyCloudy).first != MostlyPartlyCloudy)
	  TEST_FAILED("MostlyPartlyCloudy+PartlyCloudy is similar to MostlyPartlyCloudy");
	if(similartype(MostlyPartlyCloudy,Clear).first != ClearOrPartlyCloudy)
	  TEST_FAILED("MostlyPartlyCloudy+Clear is similar to ClearOrPartlyCloudy");
	if(similartype(MostlyPartlyCloudy,MostlyCloudy).first != CloudyOrPartlyCloudy)
	  TEST_FAILED("MostlyPartlyCloudy+MostlyCloudy is similar to CloudyOrPartlyCloudy");
	if(similartype(MostlyPartlyCloudy,MostlyPartlyCloudy).first != MostlyPartlyCloudy)
	  TEST_FAILED("MostlyPartlyCloudy+MostlyPartlyCloudy is similar to MostlyPartlyCloudy");
	if(similartype(MostlyPartlyCloudy,MostlyClear).first != ClearOrPartlyCloudy)
	  TEST_FAILED("MostlyPartlyCloudy+MostlyClear is similar to ClearOrPartlyCloudy");
	if(similartype(MostlyPartlyCloudy,CloudyOrPartlyCloudy).first != VariableCloudiness)
	  TEST_FAILED("MostlyPartlyCloudy+CloudyOrPartlyCloudy is similar to VariableCloudiness");
	if(similartype(MostlyPartlyCloudy,ClearOrPartlyCloudy).first != ClearOrPartlyCloudy)
	  TEST_FAILED("MostlyPartlyCloudy+ClearOrPartlyCloudy is similar to ClearOrPartlyCloudy");
	if(similartype(MostlyPartlyCloudy,VariableCloudiness).first != VariableCloudiness)
	  TEST_FAILED("MostlyPartlyCloudy+VariableCloudiness is similar to VariableCloudiness");

	if(similartype(MostlyClear,PartlyCloudy).first != ClearOrPartlyCloudy)
	  TEST_FAILED("MostlyClear+PartlyCloudy is similar to ClearOrPartlyCloudy");
	if(similartype(MostlyClear,Clear).first != MostlyClear)
	  TEST_FAILED("MostlyClear+Clear is similar to MostlyClear");
	if(similartype(MostlyClear,MostlyPartlyCloudy).first != ClearOrPartlyCloudy)
	  TEST_FAILED("MostlyClear+MostlyPartlyCloudy is similar to ClearOrPartlyCloudy");
	if(similartype(MostlyClear,MostlyClear).first != MostlyClear)
	  TEST_FAILED("MostlyClear+MostlyClear is similar to MostlyClear");
	if(similartype(MostlyClear,ClearOrPartlyCloudy).first != ClearOrPartlyCloudy)
	  TEST_FAILED("MostlyClear+ClearOrPartlyCloudy is similar to ClearOrPartlyCloudy");

	if(similartype(CloudyOrPartlyCloudy,Cloudy).first != CloudyOrPartlyCloudy)
	  TEST_FAILED("CloudyOrPartlyCloudy+Cloudy is similar to CloudyOrPartlyCloudy");
	if(similartype(CloudyOrPartlyCloudy,PartlyCloudy).first != CloudyOrPartlyCloudy)
	  TEST_FAILED("CloudyOrPartlyCloudy+PartlyCloudy is similar to CloudyOrPartlyCloudy");
	if(similartype(CloudyOrPartlyCloudy,MostlyCloudy).first != CloudyOrPartlyCloudy)
	  TEST_FAILED("CloudyOrPartlyCloudy+MostlyCloudy is similar to CloudyOrPartlyCloudy");
	if(similartype(CloudyOrPartlyCloudy,MostlyPartlyCloudy).first != CloudyOrPartlyCloudy)
	  TEST_FAILED("CloudyOrPartlyCloudy+MostlyPartlyCloudy is similar to CloudyOrPartlyCloudy");
	if(similartype(CloudyOrPartlyCloudy,CloudyOrPartlyCloudy).first != CloudyOrPartlyCloudy)
	  TEST_FAILED("CloudyOrPartlyCloudy+CloudyOrPartlyCloudy is similar to CloudyOrPartlyCloudy");
	if(similartype(CloudyOrPartlyCloudy,ClearOrPartlyCloudy).first != VariableCloudiness)
	  TEST_FAILED("CloudyOrPartlyCloudy+ClearOrPartlyCloudy is similar to VariableCloudiness");
	if(similartype(CloudyOrPartlyCloudy,VariableCloudiness).first != VariableCloudiness)
	  TEST_FAILED("CloudyOrPartlyCloudy+VariableCloudiness is similar to VariableCloudiness");

	if(similartype(ClearOrPartlyCloudy,PartlyCloudy).first != ClearOrPartlyCloudy)
	  TEST_FAILED("ClearOrPartlyCloudy+PartlyCloudy is similar to ClearOrPartlyCloudy");
	if(similartype(ClearOrPartlyCloudy,Clear).first != ClearOrPartlyCloudy)
	  TEST_FAILED("ClearOrPartlyCloudy+Clear is similar to ClearOrPartlyCloudy");
	if(similartype(ClearOrPartlyCloudy,MostlyCloudy).first != VariableCloudiness)
	  TEST_FAILED("ClearOrPartlyCloudy+MostlyCloudy is similar to VariableCloudiness");
	if(similartype(ClearOrPartlyCloudy,MostlyPartlyCloudy).first != ClearOrPartlyCloudy)
	  TEST_FAILED("ClearOrPartlyCloudy+MostlyPartlyCloudy is similar to ClearOrPartlyCloudy");
	if(similartype(ClearOrPartlyCloudy,MostlyClear).first != ClearOrPartlyCloudy)
	  TEST_FAILED("ClearOrPartlyCloudy+MostlyClear is similar to ClearOrPartlyCloudy");
	if(similartype(ClearOrPartlyCloudy,CloudyOrPartlyCloudy).first != VariableCloudiness)
	  TEST_FAILED("ClearOrPartlyCloudy+CloudyOrPartlyCloudy is similar to VariableCloudiness");
	if(similartype(ClearOrPartlyCloudy,ClearOrPartlyCloudy).first != ClearOrPartlyCloudy)
	  TEST_FAILED("ClearOrPartlyCloudy+ClearOrPartlyCloudy is similar to ClearOrPartlyCloudy");
	if(similartype(ClearOrPartlyCloudy,VariableCloudiness).first != VariableCloudiness)
	  TEST_FAILED("ClearOrPartlyCloudy+VariableCloudiness is similar to VariableCloudiness");

	if(similartype(DecreasingCloudiness,Clear).first != DecreasingCloudiness)
	  TEST_FAILED("DecreasingCloudiness+Clear is similar to DecreasingCloudiness");
	if(similartype(DecreasingCloudiness,MostlyClear).first != DecreasingCloudiness)
	  TEST_FAILED("DecreasingCloudiness+MostlyClear is similar to DecreasingCloudiness");
	if(similartype(DecreasingCloudiness,DecreasingCloudiness).first != DecreasingCloudiness)
	  TEST_FAILED("DecreasingCloudiness+DecreasingCloudiness is similar to DecreasingCloudiness");

	if(similartype(IncreasingCloudiness,Cloudy).first != IncreasingCloudiness)
	  TEST_FAILED("IncreasingCloudiness+Cloudy is similar to IncreasingCloudiness");
	if(similartype(IncreasingCloudiness,MostlyCloudy).first != IncreasingCloudiness)
	  TEST_FAILED("IncreasingCloudiness+MostlyCloudy is similar to IncreasingCloudiness");
	if(similartype(IncreasingCloudiness,IncreasingCloudiness).first != IncreasingCloudiness)
	  TEST_FAILED("IncreasingCloudiness+IncreasingCloudiness is similar to IncreasingCloudiness");
	if(similartype(IncreasingCloudiness,VariableCloudiness).first != VariableCloudiness)
	  TEST_FAILED("IncreasingCloudiness+VariableCloudiness is similar to VariableCloudiness");

	if(similartype(VariableCloudiness,PartlyCloudy).first != VariableCloudiness)
	  TEST_FAILED("VariableCloudiness+PartlyCloudy is similar to VariableCloudiness");
	if(similartype(VariableCloudiness,MostlyCloudy).first != VariableCloudiness)
	  TEST_FAILED("VariableCloudiness+MostlyCloudy is similar to VariableCloudiness");
	if(similartype(VariableCloudiness,MostlyPartlyCloudy).first != VariableCloudiness)
	  TEST_FAILED("VariableCloudiness+MostlyPartlyCloudy is similar to VariableCloudiness");
	if(similartype(VariableCloudiness,MostlyClear).first != VariableCloudiness)
	  TEST_FAILED("VariableCloudiness+MostlyClear is similar to VariableCloudiness");
	if(similartype(VariableCloudiness,CloudyOrPartlyCloudy).first != VariableCloudiness)
	  TEST_FAILED("VariableCloudiness+CloudyOrPartlyCloudy is similar to VariableCloudiness");
	if(similartype(VariableCloudiness,ClearOrPartlyCloudy).first != VariableCloudiness)
	  TEST_FAILED("VariableCloudiness+ClearOrPartlyCloudy is similar to VariableCloudiness");
	if(similartype(VariableCloudiness,IncreasingCloudiness).first != VariableCloudiness)
	  TEST_FAILED("VariableCloudiness+IncreasingCloudiness is similar to VariableCloudiness");
	if(similartype(VariableCloudiness,VariableCloudiness).first != VariableCloudiness)
	  TEST_FAILED("VariableCloudiness+VariableCloudiness is similar to VariableCloudiness");

	// test combinations

	std::vector<CloudinessType> vec;
	vec.push_back(PartlyCloudy);
	vec.push_back(MostlyPartlyCloudy);
	if(similartype(vec).first != MostlyPartlyCloudy)
	  TEST_FAILED("PartlyCloudy+MostlyPartlyCloudy = MostlyPartlyCloudy");

	vec.push_back(MostlyClear);
	if(similartype(vec).first != ClearOrPartlyCloudy)
	  TEST_FAILED("PartlyCloudy+MostlyPartlyCloudy+MostlyClear = ClearOrPartlyCloudy");
	
	vec.push_back(MostlyCloudy);
	if(similartype(vec).first != VariableCloudiness)
	  TEST_FAILED("PartlyCloudy+MostlyPartlyCloudy+MostlyClear+MostlyCloudy = VariableCloudiness");


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
