// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::WeatherArea
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERAREA_H
#define WEATHERANALYSIS_WEATHERAREA_H

#include "WeatherHistory.h"

#include <newbase/NFmiPoint.h>
#include <newbase/NFmiSvgPath.h>

#include <boost/shared_ptr.hpp>
#include <string>

namespace WeatherAnalysis
{
  class WeatherArea
  {
  public:

	enum Type
	  {
		Full,
		Land,
		Coast,
		Inland,
		Northern,
		Southern,
		Eastern,
		Western
	  };

  public:

#ifdef NO_COMPILER_GENERATED
	~WeatherArea();
	WeatherArea(const WeatherArea & theArea);
	WeatherArea & operator=(const WeatherArea & theArea);
#endif

	//WeatherArea & operator=(const WeatherArea & theArea);
	WeatherArea(const WeatherArea & theArea);

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

	Type type() const;
	void type(Type theType);

	bool operator<(const WeatherArea & theOther) const;
	bool operator==(const WeatherArea & theOther) const;
	bool operator!=(const WeatherArea & theOther) const;
	bool identicalArea(const WeatherArea & theOther) const;

	const WeatherHistory& history() const { return itsHistory; }
	WeatherHistory& history() { return itsHistory; }

	void setLongitudeDivisionLine(const double& theLongitude);
	void setLatitudeDivisionLine(const double& theLatitude);
	const double& getLongitudeDivisionLine() const { return itsLongitude; }
	const double& getLatitudeDivisionLine() const { return itsLatitude; }
	bool longitudeDivisionLineSet() const { return itsLongitude < 180.0 && itsLongitude > -180.0; }
	bool latitudeDivisionLineSet() const { return itsLatitude < 90.0 && itsLatitude > -90.0; }

  private:

	WeatherArea();
	void parse_specs(const std::string & theSpecs);

	Type itsType;
	bool itsPointFlag;
	bool itsNamedFlag;

	std::string itsName;
	NFmiPoint itsPoint;
	// By using shared pointers copying WeatherArea is much faster
	boost::shared_ptr<NFmiSvgPath> itsPolygon;
	float itsRadius;

	std::string itsSortKey;		// for sorting purposes only

	mutable WeatherHistory itsHistory;  // for handling time phrases
	double itsLatitude; //  this is utilized when the area is split horizontally
	double itsLongitude; // this is utilized when the area is split vertically
	
  }; // class WeatherArea

} // namespace WeatherAnalysis


#endif // WEATHERANALYSIS_WEATHERAREA_H

// ======================================================================

