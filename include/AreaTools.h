// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::AreaTools
 */
// ======================================================================

#ifndef TEXTGEN_AREATOOLS_H
#define TEXTGEN_AREATOOLS_H

#include <string>

#include "WeatherArea.h"
#include "UserWeatherSource.h"

using namespace std;
using namespace boost;
using namespace WeatherAnalysis;

namespace TextGen
{
  namespace AreaTools
  {
	enum forecast_area_id{NO_AREA = 0x0,
						  COASTAL_AREA = 0x1, 
						  INLAND_AREA = 0x2,
						  FULL_AREA = 0x4};

	  const bool isPartOfArea(const UserWeatherSource& theWeatherSource,
							  const WeatherArea& theWeatherArea1,
							  const WeatherArea& theWeatherArea2);


	  const bool isPartOfArea(const WeatherArea& theWeatherArea1,
							  const std::string& theArea2SvgFile,
							  const std::string theQueryData);

  } // namespace AreaTools
} // namespace TextGen

#endif // TEXTGEN_AREATOOLS_H

// ======================================================================
