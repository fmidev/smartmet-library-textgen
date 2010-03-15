// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::NullCalculator
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::NullCalculator
 *
 * \brief Dummy calculator
 *
 */
// ======================================================================

#include "NullCalculator.h"
#include "DefaultAcceptor.h"
#include "WeatherAnalysisError.h"

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Integrate a new value
   *
   * \param theValue
   */
  // ----------------------------------------------------------------------

  void NullCalculator::operator()(float theValue)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the integrated value
   *
   * \return The integrated null value
   */
  // ----------------------------------------------------------------------

  float NullCalculator::operator()() const
  {
	throw WeatherAnalysisError("NullCalculator should never be asked for a result");
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Set the internal acceptor
   *
   * \param theAcceptor The acceptor to be used
   */
  // ----------------------------------------------------------------------

  void NullCalculator::acceptor(const Acceptor & theAcceptor)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Clone
   */
  // ----------------------------------------------------------------------

  Calculator * NullCalculator::clone() const
  {
	return new NullCalculator(*this);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Reset
   */
  // ----------------------------------------------------------------------

  void NullCalculator::reset()
  {
  }

} // namespace WeatherAnalysis

// ======================================================================
