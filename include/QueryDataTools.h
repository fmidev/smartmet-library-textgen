// ======================================================================
/*!
 * \file
 * \brief Interface of namespace WeatherAnalysis::QueryDataTools
 */
// ======================================================================

#ifndef WEATHERANALYSIS_QUERYDATATOOLS_H
#define WEATHERANALYSIS_QUERYDATATOOLS_H

class NFmiFastQueryInfo;
class NFmiTime;

namespace WeatherAnalysis
{
  namespace QueryDataTools
  {
	bool firstTime(NFmiFastQueryInfo & theQI,const NFmiTime & theTime);
  }
}

#endif // WEATHERANALYSIS_QUERYDATATOOLS_H

// ======================================================================
