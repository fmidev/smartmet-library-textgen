// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::NullMaskSource
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::NullMaskSource
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

#include "WeatherAnalysisError.h"
#include "WeatherArea.h"
#include "WeatherSource.h"
#include <newbase/NFmiIndexMask.h>

using namespace std;
using namespace boost;

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  // ----------------------------------------------------------------------

  NullMaskSource::NullMaskSource()
	: itsData(new NFmiIndexMask())
  {
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Return the mask for the given area
   *
   * \param theArea The weather area
   * \param theData The data name
   * \param theWeatherSource The source for weather data
   */
  // ----------------------------------------------------------------------

  const NullMaskSource::mask_type
  NullMaskSource::mask(const WeatherArea & theArea,
					   const std::string & theData,
					   const WeatherSource & theWeatherSource) const
  {
	return itsData;
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

  const NullMaskSource::masks_type
  NullMaskSource::masks(const WeatherArea & theArea,
						   const std::string & theData,
						   const WeatherSource & theWeatherSource) const
  {
	throw WeatherAnalysisError("NullMaskSource::masks not implemented");
  }


} // namespace WeatherAnalysis

// ======================================================================
