// ======================================================================
/*!
 * \file
 * \brief Interface of namespace WeatherAnalysis::WeatherResultTools
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERRESULTTOOLS_H
#define WEATHERANALYSIS_WEATHERRESULTTOOLS_H

#include "WeatherResult.h"
#include <string>

namespace WeatherAnalysis
{
  namespace WeatherResultTools
  {

	const WeatherResult
	min(const WeatherResult & theResult1, const WeatherResult & theResult2);

	const WeatherResult
	max(const WeatherResult & theResult1, const WeatherResult & theResult2);

	const WeatherResult
	mean(const WeatherResult & theResult1, const WeatherResult & theResult2);

	const WeatherResult
	min(const WeatherResult & theResult1, const WeatherResult & theResult2, const WeatherResult & theResult3);

	const WeatherResult
	max(const WeatherResult & theResult1, const WeatherResult & theResult2, const WeatherResult & theResult3);

	const WeatherResult
	mean(const WeatherResult & theResult1, const WeatherResult & theResult2, const WeatherResult & theResult3);


	bool isSimilarRange(const WeatherResult & theMinimum1,
						const WeatherResult & theMaximum1,
						const WeatherResult & theMinimum2,
						const WeatherResult & theMaximum2,
						const std::string & theVar);


  } // namespace WeatherResultTools
} // namespace TextGen

#endif // TEXTGEN_WEATHERRESULTTOOLS_H

// ======================================================================
