// ======================================================================
/*!
 * \file
 * \brief An abstract interface for a period generator
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

#ifndef WEATHERANALYSIS_WEATHERPERIODGENERATOR_H
#define WEATHERANALYSIS_WEATHERPERIODGENERATOR_H

namespace WeatherAnalysis
{
  class WeatherPeriod;

  class WeatherPeriodGenerator
  {
  public:
	typedef unsigned int size_type;

	WeatherPeriodGenerator() { }

	virtual ~WeatherPeriodGenerator() { }
	virtual size_type size() const = 0;
	virtual WeatherPeriod period(size_type thePeriod) const = 0;


  }; // class WeatherPeriodGenerator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERPERIODGENERATOR_H

// ======================================================================

