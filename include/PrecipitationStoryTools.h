// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::PrecipitationStoryTools
 */
// ======================================================================

#ifndef TEXTGEN_PRECIPITATIONSTORYTOOLS_H
#define TEXTGEN_PRECIPITATIONSTORYTOOLS_H

#include <string>
#include <utility>
#include <vector>

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherPeriod;
  class WeatherResult;
}

namespace TextGen
{
  class Sentence;

  namespace PrecipitationStoryTools
  {

	const Sentence rain_phrase(const WeatherAnalysis::AnalysisSources & theSources,
							   const WeatherAnalysis::WeatherArea & theArea,
							   const WeatherAnalysis::WeatherPeriod & thePeriod,
							   const std::string & theVar,
							   int theDay);

	const Sentence places_phrase(const WeatherAnalysis::AnalysisSources & theSources,
								 const WeatherAnalysis::WeatherArea & theArea,
								 const WeatherAnalysis::WeatherPeriod & thePeriod,
								 const std::string & theVar,
								 int theDay);

	const Sentence type_phrase(const WeatherAnalysis::AnalysisSources & theSources,
							   const WeatherAnalysis::WeatherArea & theArea,
							   const WeatherAnalysis::WeatherPeriod & thePeriod,
							   const std::string & theVar,
							   int theDay);

	const Sentence strength_phrase(const WeatherAnalysis::AnalysisSources & theSources,
								   const WeatherAnalysis::WeatherArea & theArea,
								   const WeatherAnalysis::WeatherPeriod & thePeriod,
								   const std::string & theVar,
								   int theDay);

	const Sentence sum_phrase(const WeatherAnalysis::WeatherResult & theMinimum,
							  const WeatherAnalysis::WeatherResult & theMaximum,
							  const WeatherAnalysis::WeatherResult & theMean,
							  int theMinInterval);

  } // namespace PrecipitationStoryTools
} // namespace TextGen

#endif // TEXTGEN_PRECIPITATIONSTORYTOOLS_H

// ======================================================================
