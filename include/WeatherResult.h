// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::WeatherResult
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WeatherResult
 *
 * \brief A storage class for an analysis result and its accuracy
 *
 * An analysis results consists of a value and an associated error
 * estimate. 0 error indicates an accurate result.
 *
 * This class provides storage only, creation and interpretation
 * is left for external classes and functions.
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERRESULT_H
#define WEATHERANALYSIS_WEATHERRESULT_H

#include <iosfwd>

namespace WeatherAnalysis
{
  class WeatherResult
  {
  public:

#ifdef NO_COMPILER_GENERATED
	~WeatherResult();
	WeatherResult(const WeatherResult & theResult);
	WeatherResult & operator=(const WeatherResult & theResult);
#endif

	WeatherResult(float theValue, float theError);

	float value() const;
	float error() const;

  private:

	WeatherResult();
	float itsValue;
	float itsError;

  }; // class WeatherResult

} // namespace WeatherAnalysis

// Free functions

bool operator==(const WeatherAnalysis::WeatherResult & theLhs,
				const WeatherAnalysis::WeatherResult & theRhs);

bool operator!=(const WeatherAnalysis::WeatherResult & theLhs,
				const WeatherAnalysis::WeatherResult & theRhs);

std::ostream & operator<<(std::ostream & theOutput,
						  const WeatherAnalysis::WeatherResult & theResult);


#endif // WEATHERANALYSIS_WEATHERRESULT_H

// ======================================================================
