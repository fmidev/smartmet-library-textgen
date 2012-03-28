// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::WeatherPeriodFactory
 */
// ======================================================================

#ifndef TEXTGEN_WEATHERPERIODFACTORY_H
#define TEXTGEN_WEATHERPERIODFACTORY_H

#include <string>

class NFmiTime;

namespace WeatherAnalysis
{
  class WeatherPeriod;
}

namespace TextGen
{

  namespace WeatherPeriodFactory
  {

	WeatherAnalysis::WeatherPeriod create(const NFmiTime & theTime,
										  const std::string & theVariable);


  } // namespace WeatherPeriodFactory

} // namespace TextGen

#endif // TEXTGEN_WEATHERPERIODFACTORY_H

// ======================================================================
