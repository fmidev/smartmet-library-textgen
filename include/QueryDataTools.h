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
	bool findIndices(NFmiFastQueryInfo & theQI,
					 const NFmiTime & theStartTime,
					 const NFmiTime & theEndTime,
					 unsigned long & theStartIndex,
					 unsigned long & theEndIndex);

	bool firstTime(NFmiFastQueryInfo & theQI,
				   const NFmiTime & theTime);

	bool lastTime(NFmiFastQueryInfo & theQI,
				  const NFmiTime & theTime);
  }
}

#endif // WEATHERANALYSIS_QUERYDATATOOLS_H

// ======================================================================
