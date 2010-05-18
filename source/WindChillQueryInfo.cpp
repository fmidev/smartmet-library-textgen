#include "WindChillQueryInfo.h"
#include <newbase/NFmiMetMath.h>

/*
float WindChillQueryInfo::FloatValue()
{
  if(!Param(kFmiTemperature))
	return kFloatMissing;

  float t2m = NFmiFastQueryInfo::FloatValue();

  if(!Param(kFmiWindSpeedMS))
	return kFloatMissing;

  float wspd = NFmiFastQueryInfo::FloatValue();

  return FmiWindChill(wspd, t2m);
}

float WindChillQueryInfo::GetFloatValue(unsigned long theIndex)
{
  if(!Param(kFmiTemperature))
	return kFloatMissing;

  float t2m = NFmiFastQueryInfo::GetFloatValue(theIndex);

  if(!Param(kFmiWindSpeedMS))
	return kFloatMissing;

  float wspd = NFmiFastQueryInfo::GetFloatValue(theIndex);

  return FmiWindChill(wspd, t2m);
}
*/
