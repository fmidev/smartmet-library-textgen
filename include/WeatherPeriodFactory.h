// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::WeatherPeriodFactory
 */
// ======================================================================
/*!
 * \namespace TextGen::WeatherPeriodFactory
 *
 * \brief Producers for suitable weather periods
 *
 * The namespace consists of functions which given a reference time
 * in local time and a name of a conventional weather period, produce
 * the conventional weather period as a WeatherPeriod object.
 *
 * \see page_aikavalit
 * 
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
										  const std::string & theName);


  } // namespace WeatherPeriodFactory

} // namespace TextGen

#endif // TEXTGEN_WEATHERPERIODFACTORY_H

// ======================================================================
