// ======================================================================
/*!
 * \file
 * \brief Interface of namespace WeatherAnalysis::PrecipitationPeriodTools
 */
// ======================================================================

#ifndef WEATHERANALYSIS_PRECIPITATIONPERIODTOOLS_H
#define WEATHERANALYSIS_PRECIPITATIONPERIODTOOLS_H

#include <list>
#include <string>

class NFmiTime;

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherPeriod;
  
  namespace PrecipitationPeriodTools
  {

	std::list<NFmiTime> findRainTimes(const AnalysisSources & theSources,
									  const WeatherArea & theArea,
									  const WeatherPeriod & thePeriod,
									  const std::string & theVar);

  } // namespace PrecipitationPeriodTools
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_PRECIPITATIONPERIODTOOLS_H

// ======================================================================
