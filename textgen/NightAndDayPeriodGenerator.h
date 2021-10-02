// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::NightAndDayPeriodGenerator
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

class NightAndDayPeriodGenerator : public WeatherPeriodGenerator
{
 public:
  using size_type = WeatherPeriodGenerator::size_type;

  NightAndDayPeriodGenerator(const WeatherPeriod& theMainPeriod,
                             int theDayStartHour,
                             int theDayEndHour,
                             int theDayMaxStartHour,
                             int theDayMinEndHour,
                             int theNightMaxStartHour,
                             int theNightMinEndHour);

  NightAndDayPeriodGenerator(const WeatherPeriod& theMainPeriod, const std::string& theVariable);

  ~NightAndDayPeriodGenerator() override {}
  bool undivided() const override;
  size_type size() const override;
  WeatherPeriod period() const override;
  WeatherPeriod period(size_type thePeriod) const override;

  bool isday(size_type thePeriod) const;
  bool isnight(size_type thePeriod) const { return !isday(thePeriod); }

 private:
  NightAndDayPeriodGenerator();
  void init();

  const WeatherPeriod itsMainPeriod;
  const int itsDayStartHour;
  const int itsDayEndHour;
  const int itsDayMaxStartHour;
  const int itsDayMinEndHour;
  const int itsNightStartHour;
  const int itsNightEndHour;
  const int itsNightMaxStartHour;
  const int itsNightMinEndHour;

  std::vector<WeatherPeriod> itsPeriods;

};  // class NightAndDayPeriodGenerator

}  // namespace TextGen

// ======================================================================
