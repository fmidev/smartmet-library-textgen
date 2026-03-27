// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::ValueAcceptor
 */
// ======================================================================
/*!
 * \class TextGen::ValueAcceptor
 *
 * \brief ValueAcceptor accepts only a single value
 *
 * This is typically used to calculate the percentage of some
 * enumerated type out of all types.
 */
// ======================================================================

#include "ValueAcceptor.h"
#include <macgyver/Exception.h>
#include <newbase/NFmiGlobals.h>

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 */
// ----------------------------------------------------------------------

ValueAcceptor::ValueAcceptor() : itsValue(kFloatMissing) {}
// ----------------------------------------------------------------------
/*!
 * \brief Clone
 */
// ----------------------------------------------------------------------

Acceptor* ValueAcceptor::clone() const
{
  try
  {
    return new ValueAcceptor(*this);
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

bool ValueAcceptor::accept(float theValue) const
{
  try
  {
    if (theValue == kFloatMissing)
      return false;
    return (theValue == itsValue);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Set the accepted value
 *
 * \param theValue The value
 */
// ----------------------------------------------------------------------

void ValueAcceptor::value(float theValue)
{
  try
  {
    itsValue = theValue;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
}  // namespace TextGen

// ======================================================================
