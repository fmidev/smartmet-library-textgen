// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::PeriodPhraseFactory
 */
// ======================================================================

#ifndef TEXTGEN_PERIODPHRASEFACTORY_H
#define TEXTGEN_PERIODPHRASEFACTORY_H

#include <string>
class NFmiTime;
namespace WeatherAnalysis { class WeatherPeriod; }

namespace TextGen
{
  class Sentence;

  namespace PeriodPhraseFactory
  {

	Sentence create(const std::string & theType,
					const std::string & theVariable,
					const NFmiTime & theForecastTime,
					const WeatherAnalysis::WeatherPeriod & thePeriod);

  } // namespace PeriodPhraseFactory

} // namespace TextGen

#endif // TEXTGEN_PERIODPHRASEFACTORY_H

// ======================================================================
