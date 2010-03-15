// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::OrAcceptor
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::OrAcceptor
 *
 * \brief OrAcceptor combines two Acceptors with a logical OR
 *
 */
// ======================================================================

#include "OrAcceptor.h"

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theLhs The first acceptor
   * \param theRhs The second acceptor
   */
  // ----------------------------------------------------------------------

  OrAcceptor::OrAcceptor(const Acceptor & theLhs,
						 const Acceptor & theRhs)
	: itsLhs(theLhs.clone())
	, itsRhs(theRhs.clone())
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Clone
   */
  // ----------------------------------------------------------------------

  Acceptor * OrAcceptor::clone() const
  {
	return new OrAcceptor(*this);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Accept or reject a value
   *
   * The value kFloatMissing is never accepted.

   * \param theValue The value to be accepted
   * \return True if the value is accepted
   */
  // ----------------------------------------------------------------------

  bool OrAcceptor::accept(float theValue) const
  {
	return itsLhs->accept(theValue) || itsRhs->accept(theValue);
  }

} // namespace WeatherAnalysis

// ======================================================================
