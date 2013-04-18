// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::QueryDataTools
 */
// ======================================================================

#ifndef TEXTGEN_QUERYDATATOOLS_H
#define TEXTGEN_QUERYDATATOOLS_H

class NFmiFastQueryInfo;
class TextGenTime;

namespace TextGen
{
  namespace QueryDataTools
  {
	bool findIndices(NFmiFastQueryInfo & theQI,
					 const TextGenTime & theStartTime,
					 const TextGenTime & theEndTime,
					 unsigned long & theStartIndex,
					 unsigned long & theEndIndex);

	bool firstTime(NFmiFastQueryInfo & theQI,
				   const TextGenTime & theTime,
				   const TextGenTime & theEndTime);

	bool lastTime(NFmiFastQueryInfo & theQI,
				  const TextGenTime & theTime);
  }
}

#endif // TEXTGEN_QUERYDATATOOLS_H

// ======================================================================
