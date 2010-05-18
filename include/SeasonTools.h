
// ======================================================================
/*!
 * \file
 * \brief Interface of namespace WeatherAnalysis::SeasonTools
 */
// ======================================================================

#ifndef WEATHERANALYSIS_SEASONTOOLS_H
#define WEATHERANALYSIS_SEASONTOOLS_H

#include <ctime>

using namespace std;
#include <string>

class NFmiTime;

namespace WeatherAnalysis
{
  namespace SeasonTools
  {
	enum forecast_season_id{SUMMER_SEASON, 
							WINTER_SEASON,
							UNDEFINED_SEASON};
	

	bool isWinter(const NFmiTime& theDate, const std::string& theVar);
	bool isSpring(const NFmiTime& theDate, const std::string& theVar);
	bool isSummer(const NFmiTime& theDate, const std::string& theVar);
	bool isAutumn(const NFmiTime& theDate, const std::string& theVar);

	bool isWinterHalf(const NFmiTime& theDate, const std::string& theVar);
	bool isSummerHalf(const NFmiTime& theDate, const std::string& theVar);
  } // namespace SeasonTools
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_SEASONTOOLS_H

// ======================================================================
