// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::AndAcceptor
 */
// ======================================================================
/*!
 * \class TextGen::AndAcceptor
 *
 * \brief AndAcceptor combines two Acceptors with a logical AND
 *
 */
// ======================================================================

#include "AndAcceptor.h"
#include <macgyver/Exception.h>
#include <newbase/NFmiGlobals.h>

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Copy Constructor
 *
 * \param theOther The acceptor
 */
// ----------------------------------------------------------------------

AndAcceptor::AndAcceptor(const AndAcceptor& theOther)
    : itsLhs(theOther.itsLhs->clone()), itsRhs(theOther.itsRhs->clone())
{
}

// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 *
 * \param theLhs The first acceptor
 * \param theRhs The second acceptor
 */
// ----------------------------------------------------------------------

AndAcceptor::AndAcceptor(const Acceptor& theLhs, const Acceptor& theRhs)
    : itsLhs(theLhs.clone()), itsRhs(theRhs.clone())
{
}

// ----------------------------------------------------------------------
/*!
 * \brief Clone
 */
// ----------------------------------------------------------------------

Acceptor* AndAcceptor::clone() const
{
  try
  {
    return new AndAcceptor(*this);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Accept or reject a value
 *
 * The value kFloatMissing is never accepted.

 * \param theValue The value to be accepted
 * \return True if the value is accepted
 */
// ----------------------------------------------------------------------

bool AndAcceptor::accept(float theValue) const
{
  try
  {
    return itsLhs->accept(theValue) && itsRhs->accept(theValue);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

}  // namespace TextGen

// ======================================================================
