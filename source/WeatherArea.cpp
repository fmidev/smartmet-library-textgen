// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::WeatherArea
 */
// ======================================================================

#include "WeatherArea.h"
#include "WeatherAnalysisError.h"

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * Construction is possible only by explicitly stating the
   * area name or coordinate.
   *
   * The void constructor is intentionally disabled.
   *
   * \param theName The name of the area
   */
  // ----------------------------------------------------------------------

  WeatherArea::WeatherArea(const std::string & theName)
	: itsNamed(true)
	, itsName(theName)
	, itsPoint(kFloatMissing,kFloatMissing)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * Construction is possible only by explicitly stating the
   * area name or coordinate.
   *
   * The void constructor is intentionally disabled.
   *
   * \param thePoint The coordinate of the weather point
   */
  // ----------------------------------------------------------------------

  WeatherArea::WeatherArea(const NFmiPoint & thePoint)
	: itsNamed(false)
	, itsName()
	, itsPoint(thePoint)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test if the area is identified by name instead of coordinates
   *
   * \return True if the area has a name
   */
  // ----------------------------------------------------------------------

  bool WeatherArea::isNamed() const
  {
	return itsNamed;
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
	if(itsNamed)
	  return itsName;
	throw WeatherAnalysisError("Trying to access name of weather point");
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Coordinate accessor
   *
   * \return The coordinate
   */
  // ----------------------------------------------------------------------

  const NFmiPoint & WeatherArea::point() const
  {
	if(!itsNamed)
	  return itsPoint;
	throw WeatherAnalysisError("Trying to coordinate of named weather area");
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Lexical less-than comparison for WeatherAnalysis::WeatherArea
   *
   * This is implemented solely for the benefit of putting WeatherArea
   * objects into standard associative containers. For example, MaskSource
   * objects need a std::map mapping a WeatherArea to a NFmiIndexMask
   * or a NFmiIndexMaskSource object.
   *
   * \param theOther The area to compare with
   * \return True if \c this is lexicographically less than theOther
   */
  // ----------------------------------------------------------------------

  bool WeatherArea::operator<(const WeatherArea & theOther) const
  {
	// we choose named < not named
	
	if(isNamed())
	  {
		if(!theOther.isNamed())
		  return true;
		return (name() < theOther.name());
	  }
	else
	  {
		if(theOther.isNamed())
		  return false;
		return (point() < theOther.point());
	  }
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
  if(theLhs.isNamed())
	{
	  if(theRhs.isNamed())
		return (theLhs.name() == theRhs.name());
	  return false;
	}
  else
	{
	  if(theRhs.isNamed())
		return false;
	  return (theLhs.point() == theRhs.point());
	}
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
