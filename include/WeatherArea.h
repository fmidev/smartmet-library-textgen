// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherArea
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WeatherArea
 *
 * \brief Representation of an area to be analyzed
 *
 * The area is represented by name alone. The management
 * of area polygons and masks is done elsewhere.
 *
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERAREA_H
#define WEATHERANALYSIS_WEATHERAREA_H

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

	const std::string & name() const;

  private:

	WeatherArea();

	std::string itsName;

  }; // class WeatherArea

} // namespace WeatherAnalysis


// Free functions

bool operator==(const WeatherAnalysis::WeatherArea & theLhs,
				const WeatherAnalysis::WeatherArea & theRhs);

bool operator!=(const WeatherAnalysis::WeatherArea & theLhs,
				const WeatherAnalysis::WeatherArea & theRhs);

#endif // WEATHERANALYSIS_WEATHERAREA_H

// ======================================================================

