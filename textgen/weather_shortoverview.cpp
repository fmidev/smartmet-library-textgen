// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::WeatherStory::shortoverview
 */
// ======================================================================

#include "Delimiter.h"
#include "Integer.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "UnitFactory.h"
#include "WeatherStory.h"
#include "WeekdayTools.h"
#include <calculator/GridForecaster.h>
#include <calculator/HourPeriodGenerator.h>
#include <calculator/MathTools.h>
#include <calculator/NullPeriodGenerator.h>
#include <calculator/RangeAcceptor.h>
#include <calculator/Settings.h>
#include <calculator/WeatherPeriodTools.h>
#include <calculator/WeatherResult.h>
#include <calculator/WeatherResultTools.h>
#include <macgyver/Exception.h>

#include <boost/lexical_cast.hpp>
#include <memory>

#include <vector>

using namespace TextGen;
using namespace std;

namespace
{
struct RainSentenceResult
{
  TextGen::Sentence sentence;
  bool unstable = false;
};

// Build precipitation sentence based on per-day rain counts
RainSentenceResult rain_sentence_phrase(int rainy_days, int partly_rainy_days, int days,
                                        int r_unstable,
                                        const TextGen::WeatherPeriod& last_rainy_period,
                                        const TextGen::WeatherPeriod& last_partly_rainy_period)
{
  RainSentenceResult result;
  if (rainy_days == 0 && partly_rainy_days == 0)
  {
    result.sentence << "poutaa";
  }
  else if (rainy_days == 1 && partly_rainy_days == 1)
  {
    result.sentence << TextGen::WeekdayTools::on_weekday(last_rainy_period.localStartTime())
                    << "sadetta";
  }
  else if (rainy_days == 0 && partly_rainy_days == 1)
  {
    result.sentence << TextGen::WeekdayTools::on_weekday(last_partly_rainy_period.localStartTime())
                    << "paikoin" << "sadetta";
  }
  else if (100 * static_cast<float>(rainy_days) / days >= r_unstable)
  {
    result.sentence << "saa on epavakaista";
    result.unstable = true;
  }
  else
  {
    result.sentence << "ajoittain sateista";
  }
  return result;
}

// Build cloudiness description sentence fragment based on n1/n2/n3 percentages
TextGen::Sentence cloudiness_phrase(float n1, float n2, float n3,
                                    int single_limit, int double_limit)
{
  TextGen::Sentence s;
  if (n1 >= single_limit)
    s << "enimmakseen" << "selkeaa";
  else if (n2 >= single_limit)
    s << "enimmakseen" << "puolipilvista";
  else if (n3 >= single_limit)
    s << "enimmakseen" << "pilvista";
  else if (n1 < double_limit)
    s << "enimmakseen" << "pilvista" << "tai" << "puolipilvista";
  else if (n3 < double_limit)
    s << "enimmakseen" << "selkeaa" << "tai" << "puolipilvista";
  else
    s << "vaihtelevaa pilvisyytta";
  return s;
}
}  // namespace

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Generator brief overview story on weather
 *
 * \return The story
 *
 * \see page_weather_shortoverview
 *
 * \todo Precipitation form output is missing
 */
// ----------------------------------------------------------------------

Paragraph WeatherStory::shortoverview() const
{
  try
  {
    MessageLogger log("WeatherStory::shortoverview");

    using namespace Settings;
    using namespace WeatherPeriodTools;

    Paragraph paragraph;
    Sentence c_sentence;
    Sentence r_sentence;

    const bool c_fullrange = optional_bool(itsVar + "::cloudiness::fullrange", true);
    const int c_starthour = optional_hour(itsVar + "::cloudiness::day::starthour", 0);
    const int c_endhour = optional_hour(itsVar + "::cloudiness::day::endhour", 0);
    const int c_maxstarthour = optional_hour(itsVar + "::cloudiness::day::maxstarthour", c_starthour);
    const int c_minendhour = optional_hour(itsVar + "::cloudiness::day::minendhour", c_endhour);

    const int c_clear = optional_percentage(itsVar + "::cloudiness::clear", 40);
    const int c_cloudy = optional_percentage(itsVar + "::cloudiness::cloudy", 70);

    const int c_single_limit = optional_percentage(itsVar + "::cloudiness::single_limit", 60);
    const int c_double_limit = optional_percentage(itsVar + "::cloudiness::double_limit", 20);

    const int r_starthour = optional_hour(itsVar + "::precipitation::day::starthour", 0);
    const int r_endhour = optional_hour(itsVar + "::precipitation::day::endhour", 0);
    const int r_maxstarthour =
        optional_hour(itsVar + "::precipitation::day::maxstarthour", r_starthour);
    const int r_minendhour = optional_hour(itsVar + "::precipitation::day::minendhour", r_endhour);

    const double r_rainy = optional_double(itsVar + "::precipitation::rainy", 1);
    const double r_partly_rainy = optional_double(itsVar + "::precipitation::partly_rainy", 0.1);
    const int r_unstable = optional_percentage(itsVar + "::precipitation::unstable", 50);

    GridForecaster forecaster;

    // Generate cloudiness story first

    {
      RangeAcceptor n1limits;
      RangeAcceptor n3limits;
      n1limits.upperLimit(c_clear);
      n3limits.lowerLimit(c_cloudy);

      std::shared_ptr<WeatherPeriodGenerator> periods;
      if (c_fullrange)
        periods = std::shared_ptr<WeatherPeriodGenerator>(new NullPeriodGenerator(itsPeriod));
      else
        periods = std::shared_ptr<WeatherPeriodGenerator>(
            new HourPeriodGenerator(itsPeriod, c_starthour, c_endhour, c_maxstarthour, c_minendhour));

      const WeatherResult n1result = forecaster.analyze(itsVar + "::fake::clear_percentage",
                                                        itsSources,
                                                        Cloudiness,
                                                        Mean,
                                                        (c_fullrange ? Percentage : Mean),
                                                        (c_fullrange ? NullFunction : Percentage),
                                                        itsArea,
                                                        *periods,
                                                        DefaultAcceptor(),
                                                        DefaultAcceptor(),
                                                        n1limits);

      const WeatherResult n3result = forecaster.analyze(itsVar + "::fake::cloudy_percentage",
                                                        itsSources,
                                                        Cloudiness,
                                                        Mean,
                                                        (c_fullrange ? Percentage : Mean),
                                                        (c_fullrange ? NullFunction : Percentage),
                                                        itsArea,
                                                        *periods,
                                                        DefaultAcceptor(),
                                                        DefaultAcceptor(),
                                                        n3limits);
      WeatherResultTools::checkMissingValue(
          "weather_shortoverview", Cloudiness, {n1result, n3result});

      log << "Cloudiness clear  Mean(Mean) = " << n1result << '\n';
      log << "Cloudiness cloudy Mean(Mean) = " << n3result << '\n';

      // n1+n2+n3 = 100
      const float n1 = n1result.value();
      const float n3 = n3result.value();
      const float n2 = 100 - n1 - n3;

      c_sentence << cloudiness_phrase(n1, n2, n3, c_single_limit, c_double_limit);
    }

    // Sentence on rain

    {
      HourPeriodGenerator generator(itsPeriod, r_starthour, r_endhour, r_maxstarthour, r_minendhour);

      WeatherPeriod last_rainy_period = generator.period(1);
      WeatherPeriod last_partly_rainy_period = generator.period(1);

      const int days = generator.size();
      int rainy_days = 0;
      int partly_rainy_days = 0;

      for (HourPeriodGenerator::size_type i = 1; i <= generator.size(); i++)
      {
        WeatherPeriod period = generator.period(i);

        const string day = "day" + std::to_string(i);
        const string var = itsVar + "::fake::" + day + "::precipitation";

        const WeatherResult result =
            forecaster.analyze(var, itsSources, Precipitation, Mean, Sum, itsArea, period);

        WeatherResultTools::checkMissingValue("weather_shortoverview", Precipitation, result);

        log << "Precipitation Mean(Sum) day " << i << " = " << result << '\n';

        if (result.value() >= r_rainy)
        {
          ++rainy_days;
          last_rainy_period = period;
        }
        if (result.value() >= r_partly_rainy)
        {
          ++partly_rainy_days;
          last_partly_rainy_period = period;
        }
      }

      const RainSentenceResult rain = rain_sentence_phrase(
          rainy_days, partly_rainy_days, days, r_unstable, last_rainy_period, last_partly_rainy_period);
      r_sentence << rain.sentence;

      if (rain.unstable)
        paragraph << r_sentence;
      else
      {
        c_sentence << Delimiter(",") << r_sentence;
        paragraph << c_sentence;
      }
    }

    log << paragraph;
    return paragraph;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

}  // namespace TextGen

// ======================================================================
