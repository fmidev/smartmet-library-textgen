// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class WeatherAnalysis::WeatherPeriodGenerator
 */
// ======================================================================

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
	virtual bool undivided() const = 0;
	virtual size_type size() const = 0;
	virtual WeatherPeriod period() const = 0;
	virtual WeatherPeriod period(size_type thePeriod) const = 0;

  }; // class WeatherPeriodGenerator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERPERIODGENERATOR_H

// ======================================================================

