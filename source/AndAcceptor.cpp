// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::AndAcceptor
 */
// ======================================================================

#include "AndAcceptor.h"
#include "NFmiGlobals.h"

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

  AndAcceptor::AndAcceptor(const Acceptor & theLhs,
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

  boost::shared_ptr<Acceptor> AndAcceptor::clone() const
  {
	return boost::shared_ptr<Acceptor>(new AndAcceptor(*this));
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

  bool AndAcceptor::accept(float theValue) const
  {
	return itsLhs->accept(theValue) && itsRhs->accept(theValue);
  }

} // namespace WeatherAnalysis

// ======================================================================
