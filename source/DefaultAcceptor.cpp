// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::DefaultAcceptor
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::DefaultAcceptor
 *
 * \brief DefaultAcceptor is a class which accepts everything but kFloatMissing
 *
 */
// ======================================================================

#include "DefaultAcceptor.h"
#include "newbase/NFmiGlobals.h"

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Clone
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<Acceptor> DefaultAcceptor::clone() const
  {
	return boost::shared_ptr<Acceptor>(new DefaultAcceptor(*this));
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Accept or reject a value
   *
   * The value kFloatMissing is never accepted.
   *
   * \param theValue The value to be accepted
   * \return True if the value is accepted
   */
  // ----------------------------------------------------------------------

  bool DefaultAcceptor::accept(float theValue) const
  {
	return(theValue != kFloatMissing);
  }

} // namespace WeatherAnalysis

// ======================================================================
