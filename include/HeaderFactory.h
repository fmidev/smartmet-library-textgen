// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::HeaderFactory
 */
// ======================================================================
/*!
 * \namespace TextGen::HeaderFactory
 *
 * \brief Producers for the headers of various weather periods
 *
 * The namespace consists of functions which generate a header
 * text of desired type for the given weather period.
 *
 * \see page_aikavalit
 *
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
	Header create(const WeatherAnalysis::WeatherArea & theArea,
				  const WeatherAnalysis::WeatherPeriod & thePeriod,
				  const std::string & theVariable);

  } // namespace HeaderFactory
} // namespace TextGen


#endif // TEXTGEN_HEADERFACTORY_H

// ======================================================================
