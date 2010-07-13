// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::NullAcceptor
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::NullAcceptor
 *
 * \brief Acceptor is a class which accepts everything
 *
 */
// ======================================================================

#include "NullAcceptor.h"
#include <newbase/NFmiGlobals.h>

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Clone
   */
  // ----------------------------------------------------------------------

  Acceptor * NullAcceptor::clone() const
  {
	return new NullAcceptor(*this);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Accept or reject a value
   *
   * NullAcceptor accepts all values
   *
   * \param theValue The value to be accepted
   * \return True if the value is accepted
   */
  // ----------------------------------------------------------------------

  bool NullAcceptor::accept(float theValue) const
  {
	return true;
  }

} // namespace WeatherAnalysis

// ======================================================================
