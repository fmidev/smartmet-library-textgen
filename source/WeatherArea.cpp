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
 * For convenience a searchpath textgen::mappath will be used for the SVG polygons,
 * and a .svg suffix will be tested if necessary.
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
 * Note that the path may consist of a single moveto followed by
 * a closepath, but in that case the radius is always positive to
 * indicate a circular area around the moveto point.
 *
 * Implementation note:
 *
 * Each WeatherArea will contain a sort key based on the
 * arguments given in the constructor. This allows one to place
 * the objects into an associative container.
 *
 */
// ======================================================================

#include "WeatherArea.h"
// textgen
#include "LocationSource.h"
#include "Settings.h"
#include "WeatherAnalysisError.h"
// newbase
#include "NFmiFileSystem.h"
#include "NFmiStringTools.h"

using namespace std;

namespace
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Create a path for a point
   *
   * Note that the closepath is required so that NFmiIndexMask
   * will calculate distances from the edges (a null edge here)
   * properly.
   */
  // ----------------------------------------------------------------------

  void make_point_path(NFmiSvgPath & thePath,
					   const NFmiPoint & thePoint)
  {
	NFmiSvgPath::Element element1(NFmiSvgPath::kElementMoveto,
								  thePoint.X(),
								  thePoint.Y());
	NFmiSvgPath::Element element2(NFmiSvgPath::kElementClosePath,
								  0,
								  0);
	thePath.push_back(element1);
	thePath.push_back(element2);
  }

}

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
	, itsSortKey(theName+'='+theSpecs)
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
	, itsSortKey(theSpecs)
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
	, itsSortKey(NFmiStringTools::Convert(thePoint.X()) +
				 ',' +
				 NFmiStringTools::Convert(thePoint.Y()) +
				 (theRadius == 0 ? "" : ":" +  NFmiStringTools::Convert(theRadius)))
  {
	if(theRadius < 0)
	  throw WeatherAnalysisError("A weather point cannot have a negative expansion radius");
	if(theRadius > 0)
	  make_point_path(itsPolygon,thePoint);
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
	, itsSortKey(theName +
				 '=' +
				 NFmiStringTools::Convert(thePoint.X()) +
				 ',' +
				 NFmiStringTools::Convert(thePoint.Y()) +
				 (theRadius == 0 ? "" : ":" +  NFmiStringTools::Convert(theRadius)))
  {
	if(theRadius < 0)
	  throw WeatherAnalysisError("A weather point cannot have a negative expansion radius");
	if(theRadius > 0)
	  make_point_path(itsPolygon,thePoint);
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

	const string searchpath = Settings::optional_string("textgen::mappath",".");

	string filename = spec;
	if(NFmiFileSystem::FileExists(filename))
	  ;
	else if(NFmiFileSystem::FileExists(filename = spec + ".svg"))
	  ;
	else if(NFmiFileSystem::FileExists(filename = searchpath + '/' + spec))
	  ;
	else if(NFmiFileSystem::FileExists(filename = searchpath + '/' + spec + ".svg"))
	  ;

	if(NFmiFileSystem::FileExists(filename))
	  {
		itsPointFlag = false;

		ifstream in(filename.c_str(), ios::in);
		if(!in)
		  throw WeatherAnalysisError("Could not open map file '"+filename+"' for reading");
		in >> itsPolygon;
		in.close();

		if(itsPolygon.empty())
		  throw WeatherAnalysisError("Map file '"+filename+"' does not contain an acceptable SVG path");
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
	  make_point_path(itsPolygon,itsPoint);

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
	return itsSortKey < theOther.itsSortKey;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Equality comparison for WeatherAnalysis::WeatherArea
   *
   * \param theOther The area to compare with
   * \return True if \c this is lexicographically less than theOther
   */
  // ----------------------------------------------------------------------

  bool WeatherArea::operator==(const WeatherArea & theOther) const
  {
	return itsSortKey == theOther.itsSortKey;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Inequality comparison for WeatherAnalysis::WeatherArea
   *
   * \param theOther The area to compare with
   * \return True if \c this is lexicographically less than theOther
   */
  // ----------------------------------------------------------------------

  bool WeatherArea::operator!=(const WeatherArea & theOther) const
  {
	return itsSortKey != theOther.itsSortKey;
  }


} // namespace WeatherAnalysis

// ======================================================================
