// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::WeatherArea
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WeatherArea
 *
 * \brief Representation of an area to be analyzed
 *
 * The WeatherArea class represents either a point location or
 * a polygonal area. The area is described by a generic path.
 *
 * Instances are generally created from textual descriptions
 * of the form
 *
 *    -# \c filename - a path to a SVG polygon
 *    -# \c filename:radius - a path to a SVG polygon plus expansion radius
 *    -# \c location - a location name
 *    -# \c location:radius - a location name plus expansion radius
 *    -# \c lon,lat - location coordinates
 *    -# \c lon,lat:radius - location coordinates plus expansion radius
 *
 * Additionally the area may be given a name for text generation
 * purposes.
 *
 * Usage examples:
 *
 * \code
 * WeatherArea area(specifications);
 *
 * if(area.isPoint())
 * {
 *   NFmiPoint lonlat = area.point();
 *   ...
 * }
 * else
 * {
 *   NFmiSvgPath path = area.path();
 *   float radius = area.radius();
 *   ...
 * }
 * \endcode
 *
 * Note that the path may consists of a single moveto command, but in
 * that case the radius is always positive to indicate a circular
 * area around the moveto point.
 */
// ======================================================================

#include "WeatherArea.h"
// textgen
#include "LocationSource.h"
#include "WeatherAnalysisError.h"
// newbase
#include "NFmiFileSystem.h"
#include "NFmiStringTools.h"

using namespace std;

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * Construct from text description and name. The allowed descriptions
   * are of the forms
   *
   * \param theSpecs The area specification
   * \param theName The name of the area
   */
  // ----------------------------------------------------------------------

  WeatherArea::WeatherArea(const std::string & theSpecs,
						   const std::string & theName)
	: itsPointFlag()
	, itsNamedFlag(true)
	, itsName(theName)
	, itsPoint(kFloatMissing,kFloatMissing)
	, itsPolygon()
	, itsRadius(0)
  {
	parse_specs(theSpecs);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * Construct from text description. The allowed descriptions are of the
   * forms. The WeatherArea will not have a name, even though some
   * specs might imply one.
   *
   * \param theSpecs The area specification
   */
  // ----------------------------------------------------------------------

  WeatherArea::WeatherArea(const std::string & theSpecs)
	: itsPointFlag()
	, itsNamedFlag(false)
	, itsName()
	, itsPoint(kFloatMissing,kFloatMissing)
	, itsPolygon()
	, itsRadius(0)
  {
	parse_specs(theSpecs);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * Construct from explicit coordinates.
   *
   * \param thePoint The coordinate of the weather point
   * \param theRadius The expansion radius
   */
  // ----------------------------------------------------------------------

  WeatherArea::WeatherArea(const NFmiPoint & thePoint,
						   float theRadius)
	: itsPointFlag(theRadius == 0)
	, itsNamedFlag(false)
	, itsName()
	, itsPoint(thePoint)
	, itsPolygon()
	, itsRadius(theRadius)
  {
	if(theRadius < 0)
	  throw WeatherAnalysisError("A weather point cannot have a negative expansion radius");
	if(theRadius > 0)
	  {
		NFmiSvgPath::Element element(NFmiSvgPath::kElementMoveto,
									 thePoint.X(),
									 thePoint.Y());
		itsPolygon.push_back(element);
	  }
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * Construct named point from explicit coordinates
   *
   * \param thePoint The coordinate of the weather point
   * \param theName The name of the coordinate point
   * \param theRadius The expansion radius
   */
  // ----------------------------------------------------------------------

  WeatherArea::WeatherArea(const NFmiPoint & thePoint,
						   const std::string & theName,
						   float theRadius)
	: itsPointFlag(theRadius == 0)
	, itsNamedFlag(true)
	, itsName(theName)
	, itsPoint(thePoint)
	, itsPolygon()
	, itsRadius(theRadius)
  {
	if(theRadius < 0)
	  throw WeatherAnalysisError("A weather point cannot have a negative expansion radius");
	if(theRadius > 0)
	  {
		NFmiSvgPath::Element element(NFmiSvgPath::kElementMoveto,
									 thePoint.X(),
									 thePoint.Y());
		itsPolygon.push_back(element);
	  }
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test if the area is really just a point
   *
   * \return True if the area is just a point
   */
  // ----------------------------------------------------------------------

  bool WeatherArea::isPoint() const
  {
	return itsPointFlag;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test if the area has a name
   *
   * \return True if the area has a name
   */
  // ----------------------------------------------------------------------

  bool WeatherArea::isNamed() const
  {
	return itsNamedFlag;
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
	if(itsNamedFlag)
	  return itsName;
	throw WeatherAnalysisError("Trying to access name of unnamed weather area");
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
	if(itsPointFlag)
	  return itsPoint;
	throw WeatherAnalysisError("Trying to access coordinate of polygonal weather area");
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Path accessor
   *
   * \return The path
   */
  // ----------------------------------------------------------------------

  const NFmiSvgPath & WeatherArea::path() const
  {
	if(!itsPointFlag)
	  return itsPolygon;
	throw WeatherAnalysisError("Trying to access path of a point");
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Radius accessor
   *
   * \return The radius
   */
  // ----------------------------------------------------------------------

  float WeatherArea::radius() const
  {
	return itsRadius;
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

	// unnamed points can be compared too

	if(isPoint() && theOther.isPoint())
	  return (point() < theOther.point());

	throw WeatherAnalysisError("Attempt to compare incomparable weather areas");

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Parse a text description of a WeatherArea
   *
   * The allowed descriptions are of the forms
   *
   *    -# \c filename - a path to a SVG polygon
   *    -# \c filename:radius - a path to a SVG polygon plus expansion radius
   *    -# \c location - a location name
   *    -# \c location:radius - a location name plus expansion radius
   *    -# \c lon,lat - location coordinates
   *    -# \c lon,lat:radius - location coordinates plus expansion radius
   *
   */
  // ----------------------------------------------------------------------

  void WeatherArea::parse_specs(const std::string & theSpecs)
  {
	if(theSpecs.empty())
	  throw WeatherAnalysisError("Trying to construct WeatherArea from empty string description");

	vector<string> words = NFmiStringTools::Split(theSpecs,":");
	if(words.size() > 2)
	  throw WeatherAnalysisError("Too many ':' characters in WeatherArea specification '"+theSpecs+"'");

	// Parse the radius part

	if(words.size() == 2)
	  {
		try
		  {
			itsRadius = NFmiStringTools::Convert<float>(words[1]);
		  }
		catch(exception & e)
		  {
			throw WeatherAnalysisError("Expecting a valid radius after the ':' character in WeatherArea '"+theSpecs+"'");
		  }
	  }

	// Parse the location part

	const string & spec = words[0];

	if(NFmiFileSystem::FileExists(spec))
	  {
		itsPointFlag = false;

		ifstream in(spec.c_str(), ios::in);
		if(!in)
		  throw WeatherAnalysisError("Could not open map file '"+spec+"' for reading");
		in >> itsPolygon;
		in.close();

		if(itsPolygon.empty())
		  throw WeatherAnalysisError("Map file '"+spec+"' does not contain an acceptable SVG path");
		return;
	  }

	// Not a polygon - must be pointlike then

	if(itsRadius < 0)
	  throw WeatherAnalysisError("Location '"+spec+"' cannot have negative expansion radius");

	if(LocationSource::instance().hasCoordinates(spec))
	  {
		itsPoint = LocationSource::instance().coordinates(spec);
	  }
	else
	  {
		try
		  {
			vector<float> tmp = NFmiStringTools::Split<vector<float> >(spec);
			itsPoint = NFmiPoint(tmp[0],tmp[1]);
		  }
		catch(exception & e)
		  {
			throw WeatherAnalysisError("Location '"+spec+"' has no known coordinates");
		  }
	  }

	itsPointFlag = (itsRadius == 0);
	if(!itsPointFlag)
	  {
		NFmiSvgPath::Element element(NFmiSvgPath::kElementMoveto,
									 itsPoint.X(),
									 itsPoint.Y());
		itsPolygon.push_back(element);
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

  if(theLhs.isPoint())
	{
	  if(theRhs.isPoint())
		return false;
	  return (theLhs.point() == theRhs.point());
	}

  return false;
  
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
