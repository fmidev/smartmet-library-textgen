// ======================================================================
/*!
 * \file
 * \brief ...
 */
// ======================================================================

#ifndef TEXTGEN_AK_FROSTSTORYTOOLS_H
#define TEXTGEN_AK_FROSTSTORYTOOLS_H

namespace WeatherAnalysis
{
  class WeatherPeriod;
}

namespace TextGen
{
  class Sentence;

  namespace AK_FrostStoryTools
  {
	bool is_frost_season();

	Sentence frost_sentence( const WeatherAnalysis::WeatherPeriod & thePeriod,
								   int theProbability );

	Sentence severe_frost_sentence( const WeatherAnalysis::WeatherPeriod & thePeriod,
										  int theProbability );
  } // AK_FrostStoryTools
} // TextGen

#endif // TEXTGEN_FROSTSTORYTOOLS_H

// ======================================================================
