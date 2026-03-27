// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::OrAcceptor
 */
// ======================================================================
/*!
 * \class TextGen::OrAcceptor
 *
 * \brief OrAcceptor combines two Acceptors with a logical OR
 *
 */
// ======================================================================

#include "OrAcceptor.h"
#include <macgyver/Exception.h>

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 *
 * \param theLhs The first acceptor
 * \param theRhs The second acceptor
 */
// ----------------------------------------------------------------------

OrAcceptor::OrAcceptor(const Acceptor& theLhs, const Acceptor& theRhs)
    : itsLhs(theLhs.clone()), itsRhs(theRhs.clone())
{
}

// ----------------------------------------------------------------------
/*!
 * \brief Copy constructor
 *
 * \param theOther The acceptor to copy
 */
// ----------------------------------------------------------------------

OrAcceptor::OrAcceptor(const OrAcceptor& theOther)
    : itsLhs(theOther.itsLhs->clone()), itsRhs(theOther.itsRhs->clone())
{
}

// ----------------------------------------------------------------------
/*!
 * \brief Clone
 */
// ----------------------------------------------------------------------

Acceptor* OrAcceptor::clone() const
{
  try
  {
    return new OrAcceptor(*this);
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

bool OrAcceptor::accept(float theValue) const
{
  try
  {
    return itsLhs->accept(theValue) || itsRhs->accept(theValue);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

}  // namespace TextGen

// ======================================================================
