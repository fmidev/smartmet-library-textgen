// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::NullPeriodGenerator
 */
// ======================================================================

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
	virtual const WeatherPeriod period() const;
	virtual const WeatherPeriod period(size_type thePeriod) const;

  private:

	NullPeriodGenerator(void);

	const WeatherPeriod itsMainPeriod;

  }; // class NullPeriodGenerator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_NULLPERIODGENERATOR_H

// ======================================================================

