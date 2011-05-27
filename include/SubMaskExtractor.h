#ifndef SUBMASKEXTRACTOR_H
#define SUBMASKEXTRACTOR_H

#include "AnalysisSources.h"
#include "WeatherParameter.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"
#include "Acceptor.h"
#include "AreaTools.h"

#include <vector>

#include <newbase/NFmiIndexMask.h>
#include <newbase/NFmiPoint.h>
#include <newbase/NFmiGrid.h>
#include <newbase/NFmiSvgPath.h>

using namespace std;
using namespace TextGen::AreaTools;

namespace WeatherAnalysis
{
  double GetLocationCoordinates(const AnalysisSources& theSources,
								const WeatherParameter& theParameter,
								const WeatherArea& theArea,
								const WeatherPeriod& thePeriod,
								const Acceptor& theAcceptor,
								vector<NFmiPoint*>& theResultData);

  double ExtractMask(const AnalysisSources& theSources,
					 const WeatherParameter& theParameter,
					 const WeatherArea& theArea,
					 const WeatherPeriod& thePeriod,
					 const Acceptor& theAcceptor,
					 NFmiIndexMask& theResultIndexMask);

  const NFmiIndexMask MaskDirection(const NFmiGrid & theGrid,
									const NFmiSvgPath & thePath,
									const direction_id& theDirectionId);

 void PrintLatLon(const AnalysisSources& theSources,
				   const WeatherParameter& theParameter,
				   const NFmiIndexMask& theIndexMask);
}

#endif
