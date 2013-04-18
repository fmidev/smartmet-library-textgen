// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::PeriodPhraseFactory
 */
// ======================================================================

#ifndef TEXTGEN_PERIODPHRASEFACTORY_H
#define TEXTGEN_PERIODPHRASEFACTORY_H

#include <string>
class TextGenTime;
namespace TextGen { class WeatherPeriod; class WeatherArea; }

namespace TextGen
{
  class Sentence;

  namespace PeriodPhraseFactory
  {

	Sentence create(const std::string & theType,
					const std::string & theVariable,
					const TextGenTime & theForecastTime,
					const TextGen::WeatherPeriod & thePeriod);
	Sentence create(const std::string & theType,
					const std::string & theVariable,
					const TextGenTime & theForecastTime,
					const TextGen::WeatherPeriod & thePeriod,
					const TextGen::WeatherArea & theArea);

  } // namespace PeriodPhraseFactory

} // namespace TextGen

#endif // TEXTGEN_PERIODPHRASEFACTORY_H

// ======================================================================
