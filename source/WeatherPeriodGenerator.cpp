// ======================================================================
/*!
 * \file
 * \brief Implementation of abstract class WeatherAnalysis::WeatherPeriodGenerator
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WeatherPeriodGenerator
 *
 * \brief Generates a sequence of periods
 *
 * Expected usage is something like
 * \code
 * WeatherPeriodGenerator * generator = ...;
 *
 * for(WeatherPeriodGenerator::size_type i=1; i<generator->size(); i++)
 * {
 *   WeatherPeriod period = generator->period(i);
 *   ...
 * }
 * \endcode
 */
// ----------------------------------------------------------------------

#include "WeatherPeriodGenerator.h"

