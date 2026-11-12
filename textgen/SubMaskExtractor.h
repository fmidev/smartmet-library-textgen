#pragma once

#include "AreaTools.h"
#include <calculator/WeatherArea.h>
#include <calculator/WeatherParameter.h>
#include <newbase/NFmiIndexMask.h>
#include <vector>

class NFmiGrid;
class NFmiPoint;

namespace TextGen
{
class Acceptor;
class AnalysisSources;
class WeatherPeriod;

double GetLocationCoordinates(const AnalysisSources& theSources,
                              const WeatherParameter& theParameter,
                              const WeatherArea& theArea,
                              const WeatherPeriod& thePeriod,
                              const Acceptor& theAcceptor,
                              std::vector<NFmiPoint*>& theResultData);

double ExtractMask(const AnalysisSources& theSources,
                   const WeatherParameter& theParameter,
                   const WeatherArea& theArea,
                   const WeatherPeriod& thePeriod,
                   const Acceptor& theAcceptor,
                   NFmiIndexMask& theResultIndexMask);

NFmiIndexMask MaskDirection(const NFmiGrid& theGrid,
                            const WeatherArea& theArea,
                            const AreaTools::direction_id& theDirectionId);

void PrintLatLon(const AnalysisSources& theSources,
                 const WeatherParameter& theParameter,
                 const NFmiIndexMask& theIndexMask);
}  // namespace TextGen
