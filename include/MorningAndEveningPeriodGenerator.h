// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::MorningAndEveningPeriodGenerator
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

  class MorningAndEveningPeriodGenerator : public WeatherPeriodGenerator
  {
  public:
	typedef WeatherPeriodGenerator::size_type size_type;

	MorningAndEveningPeriodGenerator(const WeatherPeriod & theMainPeriod,
									 int theMorningStartHour,
									 int theDayStartHour,
									 int theEveningStartHour,
									 int theNightStartHour,
									 int theMorningMaxStartHour,
									 int theDayMaxStartHour,
									 int theEveningMaxStartHour,
									 int theNightMaxStartHour);
	
	MorningAndEveningPeriodGenerator(const WeatherPeriod & theMainPeriod,
									 const std::string & theVariable);
	
	virtual ~MorningAndEveningPeriodGenerator() { }
	virtual bool undivided() const;
	virtual size_type size() const;
	virtual WeatherPeriod period() const;
	virtual WeatherPeriod period(size_type thePeriod) const;

  private:

	MorningAndEveningPeriodGenerator(void);
	void init();

	const WeatherPeriod itsMainPeriod;
	const int itsMorningStartHour;
	const int itsDayStartHour;
	const int itsEveningStartHour;
	const int itsNightStartHour;
	const int itsMorningMaxStartHour;
	const int itsDayMaxStartHour;
	const int itsEveningMaxStartHour;
	const int itsNightMaxStartHour;

	std::vector<WeatherPeriod> itsPeriods;

  }; // class MorningAndEveningPeriodGenerator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_NIGHTANDDAYPERIODGENERATOR_H

// ======================================================================

