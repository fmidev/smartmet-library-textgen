// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::PositiveValueAcceptor
 */
// ======================================================================
/*!
 * \class TextGen::PositiveValueAcceptor
 *
 * \brief PositiveValueAcceptor accepts only positive values
 *
 */
// ======================================================================

#include "PositiveValueAcceptor.h"
#include <macgyver/Exception.h>
#include <newbase/NFmiGlobals.h>

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Clone
 */
// ----------------------------------------------------------------------

Acceptor* PositiveValueAcceptor::clone() const
{
  try
  {
    return new PositiveValueAcceptor(*this);
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
 * Only positive value is accepted, , but value kFloatMissing is never accepted.

 * \param theValue The value to be accepted
 * \return True if the value is accepted
 */
// ----------------------------------------------------------------------

bool PositiveValueAcceptor::accept(float theValue) const
{
  try
  {
    return (theValue != kFloatMissing && theValue > 0.0);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

}  // namespace TextGen

// ======================================================================
