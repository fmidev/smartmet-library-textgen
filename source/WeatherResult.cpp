// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::WeatherResult
 */
// ======================================================================

#include "WeatherResult.h"
#include <cassert>
#include <iostream>

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * Construction is possible only by explicitly stating the
   * value and accuracy, or by copy constructing. The void
   * constructor is intentionally disabled.
   *
   * \param theValue The value part of the result
   * \param theAccuracy The accuracy part of the result
   */
  // ----------------------------------------------------------------------

  WeatherResult::WeatherResult(float theValue, float theAccuracy)
	: itsValue(theValue)
	, itsAccuracy(theAccuracy)
  {
	assert(theAccuracy>=0 && theAccuracy<=1);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Value accessor
   *
   * Returns the value part of the result
   *
   * \return The value part of the result
   */
  // ----------------------------------------------------------------------

  float WeatherResult::value() const
  {
	return itsValue;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Accuracy accessor
   *
   * Returns the accuracy part of the result
   *
   * \return The accuracy part of the result
   */
  // ----------------------------------------------------------------------

  float WeatherResult::accuracy() const
  {
	return itsAccuracy;
  }

} // namespace WeatherAnalysis

// ----------------------------------------------------------------------
/*!
 * \brief Equality comparison for WeatherResult
 *
 * \param theLhs The first result
 * \param theRhs The second result
 * \return True if the results are equal
 */
// ----------------------------------------------------------------------

bool operator==(const WeatherAnalysis::WeatherResult & theLhs,
				const WeatherAnalysis::WeatherResult & theRhs)
{
  return (theLhs.value() == theRhs.value() &&
		  theLhs.accuracy() == theRhs.accuracy());
}

// ----------------------------------------------------------------------
/*!
 * \brief Inequality comparison for WeatherResult
 *
 * \param theLhs The first result
 * \param theRhs The second result
 * \return True if the results are not equal
 */
// ----------------------------------------------------------------------

bool operator!=(const WeatherAnalysis::WeatherResult & theLhs,
				const WeatherAnalysis::WeatherResult & theRhs)
{
  return !(theLhs == theRhs);
}

// ----------------------------------------------------------------------
/*!
 * \brief Output operator for WeatherResult
 *
 * This is intended for logging results to MessageLogger instances.
 *
 * \param theResult The result to output
 * \param theOutput The output stream
 * \return The output stream
 */
// ----------------------------------------------------------------------

std::ostream & operator<<(std::ostream & theOutput,
						  const WeatherAnalysis::WeatherResult & theResult)
{
  theOutput << '('
			<< theResult.value()
			<< ','
			<< theResult.accuracy()
			<< ')';
  return theOutput;
}

// ======================================================================
