// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::WindStoryTools
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


	const WeatherAnalysis::WeatherResult
	minspeed(const WeatherAnalysis::WeatherResult & theSpeed1,
			 const WeatherAnalysis::WeatherResult & theSpeed2);
	
	const WeatherAnalysis::WeatherResult
	maxspeed(const WeatherAnalysis::WeatherResult & theSpeed1,
			 const WeatherAnalysis::WeatherResult & theSpeed2);
	
	const WeatherAnalysis::WeatherResult
	meanspeed(const WeatherAnalysis::WeatherResult & theSpeed1,
			  const WeatherAnalysis::WeatherResult & theSpeed2);
	
	const WeatherAnalysis::WeatherResult
	minspeed(const WeatherAnalysis::WeatherResult & theSpeed1,
			 const WeatherAnalysis::WeatherResult & theSpeed2,
			 const WeatherAnalysis::WeatherResult & theSpeed3);

	const WeatherAnalysis::WeatherResult
	maxspeed(const WeatherAnalysis::WeatherResult & theSpeed1,
			 const WeatherAnalysis::WeatherResult & theSpeed2,
			 const WeatherAnalysis::WeatherResult & theSpeed3);
	
	const WeatherAnalysis::WeatherResult
	meanspeed(const WeatherAnalysis::WeatherResult & theSpeed1,
			  const WeatherAnalysis::WeatherResult & theSpeed2,
			  const WeatherAnalysis::WeatherResult & theSpeed3);
	
	bool similar_speed_range(const WeatherAnalysis::WeatherResult & theMinSpeed1,
							 const WeatherAnalysis::WeatherResult & theMaxSpeed1,
							 const WeatherAnalysis::WeatherResult & theMinSpeed2,
							 const WeatherAnalysis::WeatherResult & theMaxSpeed2,
							 const std::string & theVariable);

	WindDirectionAccuracy direction_accuracy(double theAccuracy,
											 const std::string & theVariable);

	int direction8th(double theDirection);

	const Sentence direction_sentence(const WeatherAnalysis::WeatherResult & theDirection,
									  const std::string & theVariable);

	const Sentence speed_range_sentence(const WeatherAnalysis::WeatherResult & theMinSpeed,
										const WeatherAnalysis::WeatherResult & theMaxSpeed,
										const WeatherAnalysis::WeatherResult & theMeanSpeed,
										const std::string & theVariable);
	
	const Sentence directed_speed_sentence(const WeatherAnalysis::WeatherResult & theMinSpeed,
										   const WeatherAnalysis::WeatherResult & theMaxSpeed,
										   const WeatherAnalysis::WeatherResult & theMeanSpeed,
										   const WeatherAnalysis::WeatherResult & theDirection,
										   const std::string & theVariable);
	
  } // namespace WindStoryTools
} // namespace TextGen

#endif // TEXTGEN_WINDSTORYTOOLS_H

// ======================================================================
