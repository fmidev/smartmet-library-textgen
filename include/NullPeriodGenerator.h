// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::NullPeriodGenerator
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::NullPeriodGenerator
 *
 * \brief Generates a sequence of periods
 *
 * This class always returns the main period as is.
 */
// ----------------------------------------------------------------------

#ifndef WEATHERANALYSIS_NULLPERIODGENERATOR_H
#define WEATHERANALYSIS_NULLPERIODGENERATOR_H

#include "WeatherPeriodGenerator.h"
#include "WeatherPeriod.h"

#include <string>

namespace WeatherAnalysis
{
  class WeatherPeriod;

  class NullPeriodGenerator : public WeatherPeriodGenerator
  {
  public:
	typedef WeatherPeriodGenerator::size_type size_type;

	NullPeriodGenerator(const WeatherPeriod & theMainPeriod);

	virtual ~NullPeriodGenerator() { }
	virtual bool undivided() const;
	virtual size_type size() const;
	virtual WeatherPeriod period(size_type thePeriod) const;

  private:

	NullPeriodGenerator(void);

	const WeatherPeriod itsMainPeriod;

  }; // class NullPeriodGenerator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_NULLPERIODGENERATOR_H

// ======================================================================

