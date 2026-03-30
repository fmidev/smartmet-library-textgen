// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::WeatherFact
 */
// ======================================================================
/*!
 * \class TextGen::WeatherFact
 *
 * \brief Representation of a single fact about weather
 *
 * The purpose of a single WeatherFact object is to specify
 * a particular analysis type on the weather, and to provide
 * the result of the analysis along with its accuracy estimate.
 *
 */
// ======================================================================

#include "WeatherFact.h"
#include <macgyver/Exception.h>

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Destructor
 */
// ----------------------------------------------------------------------

WeatherFact::~WeatherFact() = default;
// ----------------------------------------------------------------------
/*!
 * \brief Copy constructor
 *
 * \param theFact The object to copy
 */
// ----------------------------------------------------------------------

WeatherFact::WeatherFact(const WeatherFact& theFact)

    = default;

// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 *
 * \param theParameter The weather analysis parameter
 * \param theFunction The weather analysis function
 * \param thePeriod The weather analysis period
 * \param theArea The weather analysis area
 * \param theResult The weather analysis result
 */
// ----------------------------------------------------------------------

WeatherFact::WeatherFact(WeatherParameter theParameter,
                         WeatherFunction theFunction,
                         WeatherPeriod thePeriod,
                         const WeatherArea& theArea,
                         const WeatherResult& theResult)
    : itsParameter(theParameter),
      itsFunction(theFunction),
      itsPeriod(std::move(thePeriod)),
      itsArea(theArea),
      itsResult(theResult)
{
}

// ----------------------------------------------------------------------
/*!
 * \brief Assignment operator
 *
 * \param theFact The object to copy
 * \return The object assigned to
 */
// ----------------------------------------------------------------------

WeatherFact& WeatherFact::operator=(const WeatherFact& theFact)
{
  try
  {
    if (this != &theFact)
    {
      itsParameter = theFact.itsParameter;
      itsFunction = theFact.itsFunction;
      itsPeriod = theFact.itsPeriod;
      itsArea = theFact.itsArea;
      itsResult = theFact.itsResult;
    }
    return *this;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Parameter accessor
 *
 * \return The weather analysis parameter
 */
// ----------------------------------------------------------------------

WeatherParameter WeatherFact::parameter() const
{
  try
  {
    return itsParameter;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Function accessor
 *
 * \return The weather analysis function
 */
// ----------------------------------------------------------------------

WeatherFunction WeatherFact::function() const
{
  try
  {
    return itsFunction;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Period accessor
 *
 * \return The weather analysis period
 */
// ----------------------------------------------------------------------

const WeatherPeriod& WeatherFact::period() const
{
  try
  {
    return itsPeriod;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Area accessor
 *
 * \return The weather analysis area
 */
// ----------------------------------------------------------------------

const WeatherArea& WeatherFact::area() const
{
  try
  {
    return itsArea;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Result accessor
 *
 * \return The weather analysis result
 */
// ----------------------------------------------------------------------

const WeatherResult& WeatherFact::result() const
{
  try
  {
    return itsResult;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
}  // namespace TextGen

// ----------------------------------------------------------------------
/*!
 * \brief Equality comparison for WeatherFact
 *
 * \param theLhs The first fact
 * \param theRhs The second fact
 * \return True if the facts are equal
 */
// ----------------------------------------------------------------------

bool operator==(const TextGen::WeatherFact& theLhs, const TextGen::WeatherFact& theRhs)
{
  try
  {
    return (theLhs.parameter() == theRhs.parameter() && theLhs.function() == theRhs.function() &&
            theLhs.period() == theRhs.period() && theLhs.area() == theRhs.area() &&
            theLhs.result() == theRhs.result());
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Inequality comparison for WeatherFact
 *
 * \param theLhs The first fact
 * \param theRhs The second fact
 * \return True if the facts are not equal
 */
// ----------------------------------------------------------------------

bool operator!=(const TextGen::WeatherFact& theLhs, const TextGen::WeatherFact& theRhs)
{
  try
  {
    return !(theLhs == theRhs);
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ======================================================================
