// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::WindStoryTools
 */
// ======================================================================
/*!
 * \namespace TextGen::WindStoryTools
 *
 * \brief Utility enumerations and functions for WindStory methods
 *
 */
// ======================================================================

#ifndef TEXTGEN_WINDSTORYTOOLS_H
#define TEXTGEN_WINDSTORYTOOLS_H

#include <string>

namespace WeatherAnalysis
{
  class WeatherResult;
}

namespace TextGen
{
  class Sentence;

  namespace WindStoryTools
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Wind direction accuracy class
	 */
	// ----------------------------------------------------------------------
	
	enum WindDirectionAccuracy
	  {
		good_accuracy,
		moderate_accuracy,
		bad_accuracy
	  };


	bool similar_speed_range(int theMinSpeed1, int theMaxSpeed1,
							 int theMinSpeed2, int theMaxSpeed2,
							 const std::string & theVariable);

	WindDirectionAccuracy direction_accuracy(double theAccuracy,
											 const std::string & theVariable);

	int direction8th(double theDirection);

	Sentence direction_sentence(const WeatherAnalysis::WeatherResult & theDirection,
								const std::string & theVariable);

	Sentence speed_range_sentence(const WeatherAnalysis::WeatherResult & theMinSpeed,
								  const WeatherAnalysis::WeatherResult & theMaxSpeed,
								  const std::string & theVariable);

	Sentence directed_speed_sentence(const WeatherAnalysis::WeatherResult & theMinSpeed,
									 const WeatherAnalysis::WeatherResult & theMaxSpeed,
									 const WeatherAnalysis::WeatherResult & theDirection,
									 const std::string & theVariable);

  } // namespace WindStoryTools
} // namespace TextGen

#endif // TEXTGEN_WINDSTORYTOOLS_H

// ======================================================================
