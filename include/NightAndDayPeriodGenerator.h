// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::NightAndDayPeriodGenerator
 */
// ======================================================================

#ifndef WEATHERANALYSIS_NIGHTANDDAYPERIODGENERATOR_H
#define WEATHERANALYSIS_NIGHTANDDAYPERIODGENERATOR_H

#include "WeatherPeriodGenerator.h"
#include "WeatherPeriod.h"

#include <string>
#include <vector>

namespace WeatherAnalysis
{
  class WeatherPeriod;

  class NightAndDayPeriodGenerator : public WeatherPeriodGenerator
  {
  public:
	typedef WeatherPeriodGenerator::size_type size_type;

	NightAndDayPeriodGenerator(const WeatherPeriod & theMainPeriod,
							   int theDayStartHour,
							   int theDayEndHour,
							   int theDayMaxStartHour,
							   int theDayMinEndHour,
							   int theNightMaxStartHour,
							   int theNightMinEndHour);
	
	NightAndDayPeriodGenerator(const WeatherPeriod & theMainPeriod,
							   const std::string & theVariable);
	
	virtual ~NightAndDayPeriodGenerator() { }
	virtual bool undivided() const;
	virtual size_type size() const;
	virtual const WeatherPeriod period() const;
	virtual const WeatherPeriod period(size_type thePeriod) const;

  private:

	NightAndDayPeriodGenerator(void);
	void init();

	const WeatherPeriod itsMainPeriod;
	const int itsDayStartHour;
	const int itsDayEndHour;
	const int itsDayMaxStartHour;
	const int itsDayMinEndHour;
	const int itsNightMaxStartHour;
	const int itsNightMinEndHour;

	std::vector<WeatherPeriod> itsPeriods;


  }; // class NightAndDayPeriodGenerator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_NIGHTANDDAYPERIODGENERATOR_H

// ======================================================================

