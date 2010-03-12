// ======================================================================
/*!
 * \file
 * \brief Interface of namespace WeatherAnalysis::SeasonTools
 */
// ======================================================================

#ifndef WEATHERANALYSIS_SEASONTOOLS_H
#define WEATHERANALYSIS_SEASONTOOLS_H

#include <ctime>

class NFmiTime;

namespace WeatherAnalysis
{
  namespace SeasonTools
  {
	
	bool isWinter(const NFmiTime& theDate); // dec-feb
	bool isSpring(const NFmiTime& theDate); // mar-may
	bool isSummer(const NFmiTime& theDate); // jun-aug
	bool isAutumn(const NFmiTime& theDate); // sep-nov

	bool isWinterHalf(const NFmiTime& theDate); // nov-apr
	bool isSummerHalf(const NFmiTime& theDate); // may-oct
  } // namespace SeasonTools
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_SEASONTOOLS_H

// ======================================================================
