// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::HeaderFactory
 */
// ======================================================================

#ifndef TEXTGEN_HEADERFACTORY_H
#define TEXTGEN_HEADERFACTORY_H

#include <string>

namespace WeatherAnalysis
{
  class WeatherArea;
  class WeatherPeriod;
}


namespace TextGen
{
  class Header;

  namespace HeaderFactory
  {
	const Header create(const WeatherAnalysis::WeatherArea & theArea,
						const WeatherAnalysis::WeatherPeriod & thePeriod,
						const std::string & theVariable);

  } // namespace HeaderFactory
} // namespace TextGen


#endif // TEXTGEN_HEADERFACTORY_H

// ======================================================================
