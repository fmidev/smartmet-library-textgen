// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::WeatherArea
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WeatherArea
 *
 * \brief Representation of an area to be analyzed
 *
 * The area is represented either by a name or by
 * the coordinates of a single point.
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERAREA_H
#define WEATHERANALYSIS_WEATHERAREA_H

#include "newbase/NFmiPoint.h"
#include <string>

namespace WeatherAnalysis
{
  class WeatherArea
  {
  public:

#ifdef NO_COMPILER_GENERATED
	~WeatherArea();
	WeatherArea(const WeatherArea & theArea);
	WeatherArea & operator=(const WeatherArea & theArea);
#endif

	WeatherArea(const std::string & theName);
	WeatherArea(const NFmiPoint & thePoint);
	
	bool isNamed() const;

	const std::string & name() const;
	const NFmiPoint & point() const;

	bool operator<(const WeatherArea & theOther) const;

  private:

	WeatherArea();

	bool itsNamed;
	std::string itsName;
	NFmiPoint itsPoint;

  }; // class WeatherArea

} // namespace WeatherAnalysis


// Free functions

bool operator==(const WeatherAnalysis::WeatherArea & theLhs,
				const WeatherAnalysis::WeatherArea & theRhs);

bool operator!=(const WeatherAnalysis::WeatherArea & theLhs,
				const WeatherAnalysis::WeatherArea & theRhs);

#endif // WEATHERANALYSIS_WEATHERAREA_H

// ======================================================================

