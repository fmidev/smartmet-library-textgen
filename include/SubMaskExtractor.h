#ifndef SUBMASKEXTRACTOR_H
#define SUBMASKEXTRACTOR_H

#include <newbase/NFmiIndexMask.h>
#include <newbase/NFmiPoint.h>

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

  void PrintLatLon(const AnalysisSources& theSources,
				   const WeatherParameter& theParameter,
				   const NFmiIndexMask& theIndexMask);
}

#endif
