// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherArea
 */
// ======================================================================

#include "WeatherArea.h"

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * Construction is possible only by explicitly stating the
   * area name. The void constructor is intentionally disabled.
   *
   * \param theName The name of the area
   */
  // ----------------------------------------------------------------------

  WeatherArea::WeatherArea(const std::string & theName)
	: itsName(theName)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Name accessor
   *
   * \return The area name
   */
  // ----------------------------------------------------------------------

  const std::string & WeatherArea::name() const
  {
	return itsName;
  }

} // namespace WeatherAnalysis

// ----------------------------------------------------------------------
/*!
 * \brief Equality comparison for WeatherAnalysis::WeatherArea
 *
 * \param theLhs The left hand side
 * \param theRhs The right hand side
 * \return True if the areas are equal
 */
// ----------------------------------------------------------------------

bool operator==(const WeatherAnalysis::WeatherArea & theLhs,
				const WeatherAnalysis::WeatherArea & theRhs)
{
  return(theLhs.name() == theRhs.name());
}

// ----------------------------------------------------------------------
/*!
 * \brief Inequality comparison for WeatherAnalysis::WeatherArea
 *
 * \param theLhs The left hand side
 * \param theRhs The right hand side
 * \return True if the areas are not equal
 */
// ----------------------------------------------------------------------

bool operator!=(const WeatherAnalysis::WeatherArea & theLhs,
				const WeatherAnalysis::WeatherArea & theRhs)
{
  return !(theLhs == theRhs);
}

// ======================================================================
