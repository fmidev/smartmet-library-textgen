// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::FrostStoryTools
 */
// ======================================================================
/*!
 * \namespace TextGen::FrostStoryTools
 *
 * \brief Common utilities for FrostStory methods
 *
 */
// ======================================================================

#ifndef TEXTGEN_FROSTSTORYTOOLS_H
#define TEXTGEN_FROSTSTORYTOOLS_H

namespace WeatherAnalysis
{
  class WeatherPeriod;
}

namespace TextGen
{
  class Sentence;

  namespace FrostStoryTools
  {
	Sentence frost_sentence(const WeatherAnalysis::WeatherPeriod & thePeriod,
							int theProbability);

	Sentence severe_frost_sentence(const WeatherAnalysis::WeatherPeriod & thePeriod,
								   int theProbability);

  } // namespace FrostStoryTools
} // namespace TextGen

#endif // TEXTGEN_FROSTSTORYTOOLS_H

// ======================================================================