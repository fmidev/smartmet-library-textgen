// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::NullMaskSource
 */
// ======================================================================
/*!
 * \class TextGen::NullMaskSource
 *
 * \brief Provides mask services to clients
 *
 * A null mask source which always returns an empty index mask.
 * Used as a substitute mask source for coasts, lands etc when
 * no coast or land area is specified.
 *
 */
// ======================================================================

#include "NullMaskSource.h"

#include <calculator/WeatherArea.h>
#include <calculator/WeatherSource.h>
#include <macgyver/Exception.h>
#include <newbase/NFmiIndexMask.h>

using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 */
// ----------------------------------------------------------------------

NullMaskSource::NullMaskSource() : itsData(new NFmiIndexMask()) {}
// ----------------------------------------------------------------------
/*!
 * \brief Return the mask for the given area
 *
 * \param theArea The weather area
 * \param theData The data name
 * \param theWeatherSource The source for weather data
 */
// ----------------------------------------------------------------------

NullMaskSource::mask_type NullMaskSource::mask(const WeatherArea& /*theArea*/,
                                               const std::string& /*theData*/,
                                               const WeatherSource& /*theWeatherSource*/) const
{
  try
  {
    return itsData;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the mask source for the given area
 *
 * \param theArea The weather area
 * \param theData The data name
 * \param theWeatherSource The source for weather data
 */
// ----------------------------------------------------------------------

NullMaskSource::masks_type NullMaskSource::masks(const WeatherArea& /*theArea*/,
                                                 const std::string& /*theData*/,
                                                 const WeatherSource& /*theWeatherSource*/) const
{
  try
  {
    throw Fmi::Exception(BCP, "NullMaskSource::masks not implemented");
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

}  // namespace TextGen

// ======================================================================
