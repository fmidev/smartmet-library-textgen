// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::MorningAndEveningPeriodGenerator
 */
// ======================================================================

#pragma once

#include <calculator/WeatherPeriod.h>
#include <calculator/WeatherPeriodGenerator.h>

#include <string>
#include <vector>

namespace TextGen
{
class WeatherPeriod;

class MorningAndEveningPeriodGenerator : public WeatherPeriodGenerator
{
 public:
  typedef WeatherPeriodGenerator::size_type size_type;

  MorningAndEveningPeriodGenerator(const WeatherPeriod& theMainPeriod,
                                   int theMorningStartHour,
                                   int theDayStartHour,
                                   int theEveningStartHour,
                                   int theNightStartHour,
                                   int theMorningMaxStartHour,
                                   int theDayMaxStartHour,
                                   int theEveningMaxStartHour,
                                   int theNightMaxStartHour);

  MorningAndEveningPeriodGenerator(const WeatherPeriod& theMainPeriod,
                                   const std::string& theVariable);

  ~MorningAndEveningPeriodGenerator() override {}
  bool undivided() const override;
  size_type size() const override;
  WeatherPeriod period() const override;
  WeatherPeriod period(size_type thePeriod) const override;

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

};  // class MorningAndEveningPeriodGenerator

}  // namespace TextGen

// ======================================================================
