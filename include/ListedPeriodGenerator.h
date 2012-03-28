// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::ListedPeriodGenerator
 */
// ======================================================================

#ifndef WEATHERANALYSIS_LISTEDPERIODGENERATOR_H
#define WEATHERANALYSIS_LISTEDPERIODGENERATOR_H

#include "WeatherPeriodGenerator.h"
#include "WeatherPeriod.h"

#include <string>
#include <vector>

namespace WeatherAnalysis
{
  class WeatherPeriod;

  class ListedPeriodGenerator : public WeatherPeriodGenerator
  {
  public:
	typedef WeatherPeriodGenerator::size_type size_type;

	ListedPeriodGenerator(const WeatherPeriod & theMainPeriod);
		
	virtual ~ListedPeriodGenerator() { }
	virtual bool undivided() const;
	virtual size_type size() const;
	virtual WeatherPeriod period() const;
	virtual WeatherPeriod period(size_type thePeriod) const;

	void add(const WeatherPeriod & thePeriod);

  private:

	ListedPeriodGenerator(void);

	const WeatherPeriod itsMainPeriod;
	std::vector<WeatherPeriod> itsPeriods;

  }; // class ListedPeriodGenerator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_LISTEDPERIODGENERATOR_H

// ======================================================================

