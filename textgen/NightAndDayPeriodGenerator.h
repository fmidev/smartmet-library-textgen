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

  NightAndDayPeriodGenerator() = delete;
  NightAndDayPeriodGenerator(WeatherPeriod theMainPeriod,
                             int theDayStartHour,
                             int theDayEndHour,
                             int theDayMaxStartHour,
                             int theDayMinEndHour,
                             int theNightMaxStartHour,
                             int theNightMinEndHour);

  NightAndDayPeriodGenerator(WeatherPeriod theMainPeriod, const std::string& theVariable);

  ~NightAndDayPeriodGenerator() override = default;
  bool undivided() const override;
  size_type size() const override;
  WeatherPeriod period() const override;
  WeatherPeriod period(size_type thePeriod) const override;

  bool isday(size_type thePeriod) const;
  bool isnight(size_type thePeriod) const { return !isday(thePeriod); }

 private:
  void init();

  WeatherPeriod itsMainPeriod;
  int itsDayStartHour;
  int itsDayEndHour;
  int itsDayMaxStartHour;
  int itsDayMinEndHour;
  int itsNightStartHour;
  int itsNightEndHour;
  int itsNightMaxStartHour;
  int itsNightMinEndHour;

  std::vector<WeatherPeriod> itsPeriods;

};  // class NightAndDayPeriodGenerator

}  // namespace TextGen

// ======================================================================
