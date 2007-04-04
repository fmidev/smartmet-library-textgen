// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::FrostStoryTools
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
	bool is_frost_season();

	const Sentence frost_sentence(const WeatherAnalysis::WeatherPeriod & thePeriod,
								  int theProbability);

	const Sentence severe_frost_sentence(const WeatherAnalysis::WeatherPeriod & thePeriod,
										 int theProbability);

  } // namespace FrostStoryTools
} // namespace TextGen

#endif // TEXTGEN_FROSTSTORYTOOLS_H

// ======================================================================
