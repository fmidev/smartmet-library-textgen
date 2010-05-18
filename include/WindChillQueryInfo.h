#ifndef WINDCHILLQUERYINFO_H
#define WINDCHILLQUERYINFO_H

#include <newbase/NFmiFastQueryInfo.h>

class WindChillQueryInfo : public NFmiFastQueryInfo
{
public:

  WindChillQueryInfo(const NFmiFastQueryInfo& theInfo)
	: NFmiFastQueryInfo(theInfo)
  {}

  /*
  float FloatValue();
  float GetFloatValue(unsigned long theIndex);
  */
  using NFmiFastQueryInfo::FloatValue;

};

#endif
