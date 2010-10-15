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
		Inland
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

	const WeatherHistory& history() const { return itsHistory; }
	WeatherHistory& history() { return itsHistory; }

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
	
  }; // class WeatherArea

} // namespace WeatherAnalysis


#endif // WEATHERANALYSIS_WEATHERAREA_H

// ======================================================================

