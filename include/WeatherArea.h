// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::WeatherArea
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERAREA_H
#define WEATHERANALYSIS_WEATHERAREA_H

#include "NFmiPoint.h"
#include "NFmiSvgPath.h"
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

	WeatherArea(const std::string & theSpecs);

	WeatherArea(const std::string & theSpecs,
				const std::string & theName);

	WeatherArea(const NFmiPoint & thePoint,
				float theRadius = 0);

	WeatherArea(const NFmiPoint & thePoint,
				const std::string & theName,
				float theRadius = 0);

	bool isPoint() const;
	bool isNamed() const;

	const std::string & name() const;
	const NFmiPoint & point() const;
	const NFmiSvgPath & path() const;
	float radius() const;

	bool operator<(const WeatherArea & theOther) const;
	bool operator==(const WeatherArea & theOther) const;
	bool operator!=(const WeatherArea & theOther) const;

  private:

	WeatherArea();
	void parse_specs(const std::string & theSpecs);

	bool itsPointFlag;
	bool itsNamedFlag;

	std::string itsName;
	NFmiPoint itsPoint;
	NFmiSvgPath itsPolygon;
	float itsRadius;

	std::string itsSortKey;		// for sorting purposes only
	
  }; // class WeatherArea

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERAREA_H

// ======================================================================

