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
  using size_type = WeatherPeriodGenerator::size_type;

  MorningAndEveningPeriodGenerator() = delete;
  MorningAndEveningPeriodGenerator(WeatherPeriod theMainPeriod,
                                   int theMorningStartHour,
                                   int theDayStartHour,
                                   int theEveningStartHour,
                                   int theNightStartHour,
                                   int theMorningMaxStartHour,
                                   int theDayMaxStartHour,
                                   int theEveningMaxStartHour,
                                   int theNightMaxStartHour);

  MorningAndEveningPeriodGenerator(WeatherPeriod theMainPeriod, const std::string& theVariable);

  ~MorningAndEveningPeriodGenerator() override = default;
  bool undivided() const override;
  size_type size() const override;
  WeatherPeriod period() const override;
  WeatherPeriod period(size_type thePeriod) const override;

 private:
  void init();

  WeatherPeriod itsMainPeriod;
  int itsMorningStartHour;
  int itsDayStartHour;
  int itsEveningStartHour;
  int itsNightStartHour;
  int itsMorningMaxStartHour;
  int itsDayMaxStartHour;
  int itsEveningMaxStartHour;
  int itsNightMaxStartHour;

  std::vector<WeatherPeriod> itsPeriods;

};  // class MorningAndEveningPeriodGenerator

}  // namespace TextGen

// ======================================================================
