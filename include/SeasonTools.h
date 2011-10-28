
// ======================================================================
/*!
 * \file
 * \brief Interface of namespace WeatherAnalysis::SeasonTools
 */
// ======================================================================

#ifndef WEATHERANALYSIS_SEASONTOOLS_H
#define WEATHERANALYSIS_SEASONTOOLS_H

#include <ctime>

//using namespace NFmiLocation;
using namespace std;
#include <string>

class NFmiTime;

namespace WeatherAnalysis
{
  class WeatherPeriod;
  class WeatherResult;
  class AnalysisSources;
  class WeatherArea;
  class WeatherPeriodGenerator;

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

	float growing_season_percentage(const WeatherArea& theArea, 
									const AnalysisSources& theSources,
									const WeatherPeriod& thePeriod,
									const std::string& theVariable);

	bool growing_season_going_on(const WeatherArea& theArea,
								 const AnalysisSources& theSources,
								 const WeatherPeriod& thePeriod,
								 const std::string theVariable);

	forecast_season_id get_forecast_season(const WeatherArea& theArea,
										   const AnalysisSources& theSources,
										   const WeatherPeriod& thePeriod,
										   const std::string theVariable);

  } // namespace SeasonTools
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_SEASONTOOLS_H

// ======================================================================
