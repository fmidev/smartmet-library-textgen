// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::PeriodPhraseFactory
 */
// ======================================================================
/*!
 * \class TextGen::PeriodPhraseFactory
 *
 * \brief Tools for generating phrases to describe periods
 *
 * \see page_periodifraasit
 *
 */
// ======================================================================

#ifndef TEXTGEN_PERIODPHRASEFACTORY_H
#define TEXTGEN_PERIODPHRASEFACTORY_H

namespace TextGen
{

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

