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
}

namespace TextGen
{
  class Sentence;

  namespace PrecipitationStoryTools
  {

	Sentence rain_phrase(const WeatherAnalysis::AnalysisSources & theSources,
						 const WeatherAnalysis::WeatherArea & theArea,
						 const WeatherAnalysis::WeatherPeriod & thePeriod,
						 const std::string & theVar,
						 int theDay);

	Sentence places_phrase(const WeatherAnalysis::AnalysisSources & theSources,
						   const WeatherAnalysis::WeatherArea & theArea,
						   const WeatherAnalysis::WeatherPeriod & thePeriod,
						   const std::string & theVar,
						   int theDay);

	Sentence type_phrase(const WeatherAnalysis::AnalysisSources & theSources,
						 const WeatherAnalysis::WeatherArea & theArea,
						 const WeatherAnalysis::WeatherPeriod & thePeriod,
						 const std::string & theVar,
						 int theDay);

	Sentence strength_phrase(const WeatherAnalysis::AnalysisSources & theSources,
							 const WeatherAnalysis::WeatherArea & theArea,
							 const WeatherAnalysis::WeatherPeriod & thePeriod,
							 const std::string & theVar,
							 int theDay);

  } // namespace PrecipitationStoryTools
} // namespace TextGen

#endif // TEXTGEN_PRECIPITATIONSTORYTOOLS_H

// ======================================================================
