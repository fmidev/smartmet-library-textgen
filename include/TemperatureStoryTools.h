// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::TemperatureStoryTools
 */
// ======================================================================

#ifndef TEXTGEN_TEMPERATURESTORYTOOLS_H
#define TEXTGEN_TEMPERATURESTORYTOOLS_H

#include <string>

namespace WeatherAnalysis
{
  class WeatherPeriod;
}

namespace TextGen
{
  class Sentence;

  namespace TemperatureStoryTools
  {
	const char * temperature_comparison_phrase(int theMean1,
											   int theMean2,
											   const std::string & theVariable);

	const TextGen::Sentence temperature_sentence(int theMinimum,
												 int theMean,
												 int theMaximum,
												 int theMinInterval,
												 bool theZeroFlag);

  } // namespace TemperatureStoryTools
} // namespace TextGen

#endif // TEXTGEN_TEMPERATURESTORYTOOLS_H

// ======================================================================
