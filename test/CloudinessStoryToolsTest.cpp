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
