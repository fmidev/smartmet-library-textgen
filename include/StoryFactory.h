// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::StoryFactory
 */
// ======================================================================

#ifndef TEXTGEN_STORYFACTORY_H
#define TEXTGEN_STORYFACTORY_H

#include <string>

class NFmiTime;

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherPeriod;
}

namespace TextGen
{
  class Paragraph;

  namespace StoryFactory
  {
	const Paragraph create(const NFmiTime & theForecastTime,
						   const WeatherAnalysis::AnalysisSources & theSources,
						   const WeatherAnalysis::WeatherArea & theArea,
						   const WeatherAnalysis::WeatherPeriod & thePeriod,
						   const std::string & theName,
						   const std::string & theVariable);

  } // namespace StoryFactory
} // namespace TextGen

#endif // TEXTGEN_STORYFACTORY_H

// ======================================================================
