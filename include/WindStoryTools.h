// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::WindStoryTools
 */
// ======================================================================

#ifndef TEXTGEN_WINDSTORYTOOLS_H
#define TEXTGEN_WINDSTORYTOOLS_H

#include <string>
#include <vector>

namespace WeatherAnalysis
{
  class WeatherResult;
}

namespace TextGen
{
  class Sentence;

  namespace WindStoryTools
  {

	using namespace std;

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

	enum CompassType
	  {
		eight_directions,
		sixteen_directions
	  };

	enum WindSpeedId
	  {
		TYYNI,
		HEIKKO,       // 0.5...3.5
		KOHTALAINEN,  // 3.5...7.5
		NAVAKKA,      // 7.5...13.5
		KOVA,         // 13.5...20.5
		MYRSKY,       // 20.5...32.5
		HIRMUMYRSKY   // 32.5...
	  };

  enum WindDirectionId
	{
	  POHJOINEN = 0x1,
	  POHJOINEN_KOILLINEN,
	  KOILLINEN,
	  ITA_KOILLINEN,
	  ITA,
	  ITA_KAAKKO,
	  KAAKKO,
	  ETELA_KAAKKO,
	  ETELA,
	  ETELA_LOUNAS,
	  LOUNAS,
	  LANSI_LOUNAS,
	  LANSI,
	  LANSI_LUODE,
	  LUODE,
	  POHJOINEN_LUODE,
	  POHJOISEN_PUOLEINEN,
	  KOILLISEN_PUOLEINEN,
	  IDAN_PUOLEINEN,
	  KAAKON_PUOLEINEN,
	  ETELAN_PUOLEINEN,
	  LOUNAAN_PUOLEINEN,
	  LANNEN_PUOLEINEN,
	  LUOTEEN_PUOLEINEN,
	  VAIHTELEVA,
	  MISSING_WIND_DIRECTION_ID
	};

#define HEIKKO_LOWER_LIMIT 0.5
#define HEIKKO_UPPER_LIMIT 3.5
#define KOHTALAINEN_LOWER_LIMIT 3.5
#define KOHTALAINEN_UPPER_LIMIT 7.5
#define NAVAKKA_LOWER_LIMIT 7.5
#define NAVAKKA_UPPER_LIMIT 13.5
#define KOVA_LOWER_LIMIT 13.5
#define KOVA_UPPER_LIMIT 20.5
#define MYRSKY_LOWER_LIMIT 20.5
#define MYRSKY_UPPER_LIMIT 32.5
#define HIRMUMYRSKY_LOWER_LIMIT 32.5

#define TYYNI_WORD "tyyni"
#define HEIKKO_WORD "heikko"
#define KOHTALAINEN_WORD "kohtalainen"
#define NAVAKKA_WORD "navakka"
#define KOVA_WORD "kova"
#define MYRSKY_WORD "myrsky"
#define HIRMUMYRSKY_WORD "hirmumyrsky"


// contains wind speed interval minimum and maximum values
typedef vector<pair<float, float> > wind_speed_vector;


	WindDirectionAccuracy direction_accuracy(double theAccuracy,
											 const std::string & theVariable);
	int direction8th(double theDirection);

	int direction16th(double theDirection);

	const Sentence direction_sentence(const WeatherAnalysis::WeatherResult & theDirection,
									  const std::string & theVariable);

	const std::string direction_string(const WeatherAnalysis::WeatherResult & theDirection,
									   const std::string & theVariable);

	const std::string direction16_string(const WeatherAnalysis::WeatherResult & theDirection,
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
	
	const std::string directed_speed_string(const WeatherAnalysis::WeatherResult & theMeanSpeed,
											const WeatherAnalysis::WeatherResult & theDirection,
											const std::string & theVariable);

	const std::string directed16_speed_string(const WeatherAnalysis::WeatherResult & theMeanSpeed,
											const WeatherAnalysis::WeatherResult & theDirection,
											const std::string & theVariable);

	const std::string speed_string(const WeatherAnalysis::WeatherResult& theMeanSpeed);

	const WindSpeedId wind_speed_id(const WeatherAnalysis::WeatherResult& theWindSpeed);

	const WindSpeedId wind_speed_id(const float& theWindSpeed);

	const std::string wind_speed_string(const WindSpeedId& theWindSpeedId);

	const pair<int, int> wind_speed_interval(const wind_speed_vector& theWindSpeedVector);

	const WindDirectionId wind_direction_id(const WeatherAnalysis::WeatherResult& theWindDirection, const string& theVariable);

	const std::string wind_direction_string(const WindDirectionId& theWindDirectionId);

	const float direction_difference(const float& direction1, const float& direction2);
  } // namespace WindStoryTools
} // namespace TextGen

#endif // TEXTGEN_WINDSTORYTOOLS_H

// ======================================================================
