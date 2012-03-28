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
	typedef std::list<NFmiTime> RainTimes;
	typedef std::list<WeatherPeriod> RainPeriods;

	// The main function

	RainPeriods analyze(const AnalysisSources & theSources,
						const WeatherArea & theArea,
						const WeatherPeriod & thePeriod,
						const std::string & theVar);

	// Extract parts of rainy periods

	RainPeriods overlappingPeriods(const RainPeriods & thePeriods,
								   const WeatherPeriod & thePeriod);

	RainPeriods inclusivePeriods(const RainPeriods & thePeriods,
								 const WeatherPeriod & thePeriod);
	
	// Utility functions used by analyze
	
	RainTimes findRainTimes(const AnalysisSources & theSources,
							const WeatherArea & theArea,
							const WeatherPeriod & thePeriod,
							const std::string & theVar);

	RainPeriods findRainPeriods(const RainTimes & theTimes,
								const std::string & theVar);

	RainPeriods mergeNightlyRainPeriods(const RainPeriods & thePeriods,
										const std::string & theVar);

	RainPeriods mergeLargeRainPeriods(const RainPeriods & thePeriods,
									  const std::string & theVar);

  } // namespace PrecipitationPeriodTools
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_PRECIPITATIONPERIODTOOLS_H

// ======================================================================
