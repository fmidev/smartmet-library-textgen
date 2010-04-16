// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TemperatureTools
 */
// ======================================================================

#ifndef TEMPERATURETOOLS_H
#define TEMPERATURETOOLS_H

using namespace std;
#include <string>

namespace WeatherAnalysis
{
  class WeatherResult;
  class WeatherPeriod;
  class AnalysisSources;
  class WeatherArea;
}

namespace WeatherAnalysis
{
  namespace TemperatureTools
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief calculate Minimum, Maximum and Mean temperatures of 
	 * areal maximum temperatures
	 */
	// ----------------------------------------------------------------------

	void min_max_mean_temperature(const string& theVar,
								  const WeatherAnalysis::AnalysisSources& theSources,
								  const WeatherAnalysis::WeatherArea& theArea,
								  const WeatherAnalysis::WeatherPeriod& thePeriod,
								  WeatherAnalysis::WeatherResult& theMin,
								  WeatherAnalysis::WeatherResult& theMax,
								  WeatherAnalysis::WeatherResult& theMean);


	// ----------------------------------------------------------------------
	/*!
	 * \brief calculate morning temperature, default values follow finnish convention
	 */
	// ----------------------------------------------------------------------

	void morning_temperature(const string& theVar,
							 const WeatherAnalysis::AnalysisSources& theSources,
							 const WeatherAnalysis::WeatherArea& theArea,
							 const WeatherAnalysis::WeatherPeriod& thePeriod,
							 WeatherAnalysis::WeatherResult& theMin,
							 WeatherAnalysis::WeatherResult& theMax,
							 WeatherAnalysis::WeatherResult& theMean);

	// ----------------------------------------------------------------------
	/*!
	 * \brief calculate afternoon temperature, default values follow finnish convention
	 */
	// ----------------------------------------------------------------------

	void afternoon_temperature(const string& theVar,
							   const WeatherAnalysis::AnalysisSources& theSources,
							   const WeatherAnalysis::WeatherArea& theArea,
							   const WeatherAnalysis::WeatherPeriod& thePeriod,
							   WeatherAnalysis::WeatherResult& theMin,
							   WeatherAnalysis::WeatherResult& theMax,
							   WeatherAnalysis::WeatherResult& theMean);

	// ----------------------------------------------------------------------
	/*!
	 * \brief clamp the big temperature interval into smaller according to configuration file settings
	 */
	// ----------------------------------------------------------------------

	void clamp_temperature(const string& theVar,
						   const bool& isWinter,
						   const bool& isDay,
						   int& theMinimum,
						   int& theMaximum);

  } // namespace TemperatureTools
} // namespace WeatherAnalysis

#endif // TEMPERATURETOOLS_H

// ======================================================================
