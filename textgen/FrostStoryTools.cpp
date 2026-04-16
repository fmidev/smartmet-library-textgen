// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::FrostStoryTools
 */
// ======================================================================
/*!
 * \namespace TextGen::FrostStoryTools
 *
 * \brief Common utilities for FrostStory methods
 *
 */
// ======================================================================

#include "FrostStoryTools.h"
#include "Integer.h"
#include "Sentence.h"
#include "UnitFactory.h"
#include "WeekdayTools.h"
#include <calculator/Settings.h>
#include <calculator/WeatherPeriod.h>
#include <macgyver/Exception.h>

namespace TextGen
{
namespace FrostStoryTools
{
// ----------------------------------------------------------------------
/*!
 * \brief Return true if one is allowed to report on frost
 */
// ----------------------------------------------------------------------

bool is_frost_season()
{
  try
  {
    return Settings::require_bool("textgen::frostseason");
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Return sentence for severe frost
 *
 * \param thePeriod The night period
 * \param theProbability The probability
 * \return The sentence
 */
// ----------------------------------------------------------------------

Sentence severe_frost_sentence(const WeatherPeriod& thePeriod, int theProbability)
{
  try
  {
    Sentence sentence;
    sentence << "ankaran hallan todennakoisyys"
             << "on" << WeekdayTools::night_against_weekday(thePeriod.localEndTime())
             << Integer(theProbability) << *UnitFactory::create(Percent);
    return sentence;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed")
        .addParameter("theProbability", std::to_string(theProbability));
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Return sentence for frost
 *
 * \param thePeriod The night period
 * \param theProbability The probability
 * \return The sentence
 */
// ----------------------------------------------------------------------

Sentence frost_sentence(const WeatherPeriod& thePeriod, int theProbability)
{
  try
  {
    Sentence sentence;
    sentence << "hallan todennakoisyys"
             << "on" << WeekdayTools::night_against_weekday(thePeriod.localEndTime())
             << Integer(theProbability) << *UnitFactory::create(Percent);
    return sentence;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed")
        .addParameter("theProbability", std::to_string(theProbability));
  }
}

}  // namespace FrostStoryTools
}  // namespace TextGen

// ======================================================================
