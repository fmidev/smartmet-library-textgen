// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::StoryFactory
 */
// ======================================================================
/*!
 * \namespace TextGen::StoryFactory
 *
 * \brief Generates brief paragraphs of desired type
 */
// ======================================================================

#ifndef TEXTGEN_STORYFACTORY_H
#define TEXTGEN_STORYFACTORY_H

#include <string>

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
	Paragraph create(const WeatherAnalysis::AnalysisSources & theSources,
					 const WeatherAnalysis::WeatherArea & theArea,
					 const WeatherAnalysis::WeatherPeriod & thePeriod,
					 const std::string & theName);

  } // namespace StoryFactory
} // namespace TextGen

#endif // TEXTGEN_STORYFACTORY_H

// ======================================================================
