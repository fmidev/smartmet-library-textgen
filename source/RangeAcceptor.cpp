// ======================================================================
/*!
 * \file
 * \brief Implementation of class RangeAcceptor
 */
// ======================================================================

#include "RangeAcceptor.h"
#include "NFmiGlobals.h"

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  // ----------------------------------------------------------------------

  RangeAcceptor::RangeAcceptor()
	: itsLoLimit(kFloatMissing)
	, itsHiLimit(kFloatMissing)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Clone
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<Acceptor> RangeAcceptor::clone() const
  {
	return boost::shared_ptr<Acceptor>(new RangeAcceptor(*this));
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

  bool RangeAcceptor::accept(float theValue) const
  {
	if(theValue == kFloatMissing)
	  return false;
	if(itsLoLimit != kFloatMissing && theValue < itsLoLimit)
	  return false;
	if(itsHiLimit != kFloatMissing && theValue > itsHiLimit)
	  return false;
	return true;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set a lower limit for acceptable values
   *
   * \param theLimit The limiting value
   */
  // ----------------------------------------------------------------------

  void RangeAcceptor::lowerLimit(float theLimit)
  {
	itsLoLimit = theLimit;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set an upper limit for acceptable values
   *
   * \param theLimit The limiting value
   */
  // ----------------------------------------------------------------------

  void RangeAcceptor::upperLimit(float theLimit)
  {
	itsHiLimit = theLimit;
  }

} // namespace WeatherAnalysis

// ======================================================================
