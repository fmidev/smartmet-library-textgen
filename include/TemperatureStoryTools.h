// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::TemperatureStoryTools
 */
// ======================================================================

#ifndef TEXTGEN_TEMPERATURESTORYTOOLS_H
#define TEXTGEN_TEMPERATURESTORYTOOLS_H

#include <string>
#include "AnalysisSources.h"
#include <newbase/NFmiTime.h>

namespace WeatherAnalysis
{
  class WeatherPeriod;
  class WeatherResult;
  class AnalysisSources;
  class WeatherArea;
  class WeatherPeriodGenerator;
}

namespace TextGen
{
  class Sentence;

  namespace TemperatureStoryTools
  {
	using namespace WeatherAnalysis;

	enum fractile_id{
	  FRACTILE_02,
	  FRACTILE_12,
	  FRACTILE_37,
	  FRACTILE_50,
	  FRACTILE_63,
	  FRACTILE_88,
	  FRACTILE_98,
	  FRACTILE_100,
	  FRACTILE_UNDEFINED
	};

	enum fractile_type_id{
	  MIN_FRACTILE,
	  MEAN_FRACTILE,
	  MAX_FRACTILE
	};
	
	const char * temperature_comparison_phrase(int theMean1,
											   int theMean2,
											   const std::string & theVariable);

	TextGen::Sentence temperature_sentence(int theMinimum,
										   int theMean,
										   int theMaximum,
										   int theMinInterval,
										   bool theZeroFlag,
										   const std::string & theRangeSeparator);

	TextGen::Sentence temperature_sentence2(int theMinimum,
											int theMean,
											int theMaximum,
											int theMinInterval,
											bool theZeroFlag,
											bool& interval,
											int& intervalStart,
											int& intervalEnd,
											const std::string & theRangeSeparator,
											const bool& theRoundTheNumber);

	// ----------------------------------------------------------------------
	/*!
	 * \brief calculate Minimum, Maximum and Mean temperatures of 
	 * areal maximum temperatures
	 */
	// ----------------------------------------------------------------------

	void min_max_mean_temperature(const std::string& theVar,
								  const WeatherAnalysis::AnalysisSources& theSources,
								  const WeatherAnalysis::WeatherArea& theArea,
								  const WeatherAnalysis::WeatherPeriod& thePeriod,
								  WeatherAnalysis::WeatherResult& theMin,
								  WeatherAnalysis::WeatherResult& theMax,
								  WeatherAnalysis::WeatherResult& theMean);


	void min_max_mean_temperature(const std::string& theVar,
								  const AnalysisSources& theSources,
								  const WeatherArea& theArea,
								  const WeatherPeriodGenerator& thePeriods,
								  const bool& theIsWinterHalf,
								  WeatherResult& theMin,
								  WeatherResult& theMax,
								  WeatherResult& theMean);

	// ----------------------------------------------------------------------
	/*!
	 * \brief calculate morning temperature
	 */
	// ----------------------------------------------------------------------

	void morning_temperature(const std::string& theVar,
							 const WeatherAnalysis::AnalysisSources& theSources,
							 const WeatherAnalysis::WeatherArea& theArea,
							 const WeatherAnalysis::WeatherPeriod& thePeriod,
							 WeatherAnalysis::WeatherResult& theMin,
							 WeatherAnalysis::WeatherResult& theMax,
							 WeatherAnalysis::WeatherResult& theMean);

	// ----------------------------------------------------------------------
	/*!
	 * \brief returns  afternoon period
	 */
	// ----------------------------------------------------------------------

	WeatherPeriod get_afternoon_period(const std::string& theVar, const NFmiTime& theTime);

	// ----------------------------------------------------------------------
	/*!
	 * \brief calculate afternoon temperature
	 */
	// ----------------------------------------------------------------------

	void afternoon_temperature(const std::string& theVar,
							   const WeatherAnalysis::AnalysisSources& theSources,
							   const WeatherAnalysis::WeatherArea& theArea,
							   const WeatherAnalysis::WeatherPeriod& thePeriod,
							   WeatherAnalysis::WeatherResult& theMin,
							   WeatherAnalysis::WeatherResult& theMax,
							   WeatherAnalysis::WeatherResult& theMean);

	void afternoon_temperature(const std::string& theVar,
							   const WeatherAnalysis::AnalysisSources& theSources,
							   const WeatherAnalysis::WeatherArea& theArea,
							   const WeatherPeriodGenerator &thePeriods,
							   WeatherAnalysis::WeatherResult& theMin,
							   WeatherAnalysis::WeatherResult& theMax,
							   WeatherAnalysis::WeatherResult& theMean);

	// ----------------------------------------------------------------------
	/*!
	 * \brief clamp the big temperature interval into smaller according to configuration file settings
	 */
	// ----------------------------------------------------------------------

	void clamp_temperature(const std::string& theVar,
						   const bool& isWinter,
						   const bool& isDay,
						   int& theMinimum,
						   int& theMaximum);


	// ----------------------------------------------------------------------
	/*!
	 * \brief determines the fractile of the given temperature
	 */
	// ----------------------------------------------------------------------

	fractile_id get_fractile(const std::string& theVar,
							 const float& theTemperature,
							 const AnalysisSources& theSources,
							 const WeatherArea& theArea,  
							 const WeatherPeriod& thePeriod,
							 const fractile_type_id& theFractileType);

	// ----------------------------------------------------------------------
	/*!
	 * \brief determines the temperature of the given fractile id
	 */
	// ----------------------------------------------------------------------

	WeatherResult get_fractile_temperature(const std::string& theVar,
										   const fractile_id& theFractileId,
										   const AnalysisSources& theSources,
										   const WeatherArea& theArea,  
										   const WeatherPeriod& thePeriod,
										   const fractile_type_id& theFractileType);

	// ----------------------------------------------------------------------
	/*!
	 * \brief returns fractile as a readable string
	 */
	// ----------------------------------------------------------------------

	const char* fractile_name(const fractile_id& id);

	// ----------------------------------------------------------------------
	/*!
	 * \brief returns fractile range as a readable string
	 */
	// ----------------------------------------------------------------------

	const char* fractile_range(const fractile_id& id);

	// ----------------------------------------------------------------------
	/*!
	 * \brief sort out temperature range interval
	 */
	// ----------------------------------------------------------------------
	void temperature_range(const int& theTemperature1, 
						   const int& theTemperature2,
						   int& intervalStart,
						   int& intervalEnd);

	// ----------------------------------------------------------------------
	/*!
	 * \brief returns temperature range sentence
	 */
	// ----------------------------------------------------------------------
	Sentence temperature_range(const int& theTemperature1, 
							   const int& theTemperature2,
							   const std::string& theRangeSeparator,
							   int& intervalStart,
							   int& intervalEnd);
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief sorts out temperature interval lower and upper limit 
	 * and returns bool if range expression is used
	 */
	// ----------------------------------------------------------------------
	bool sort_out_temperature_interval(int theMinimum,
									   int theMean,
									   int theMaximum,
									   int theMinInterval,
									   bool theZeroFlag,
									   int& intervalStart,
									   int& intervalEnd,
									   const bool& theRoundTheNumber);


  } // namespace TemperatureStoryTools
} // namespace TextGen

#endif // TEXTGEN_TEMPERATURESTORYTOOLS_H

// ======================================================================
