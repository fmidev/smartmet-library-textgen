// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherStory
 */
// ======================================================================

#include "WeatherStory.h"
#include "Paragraph.h"
#include "Settings.h"
#include "TextGenError.h"
#include "WeatherPeriodTools.h"

using namespace WeatherAnalysis;
using namespace boost;
using namespace std;

namespace TextGen
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------
  
  WeatherStory::~WeatherStory()
  {
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theForecastTime The forecast time
   * \param theSources The analysis sources
   * \param theArea The area to be analyzed 
   * \param thePeriod The period to be analyzed
   * \param theVariable The associated configuration variable
  */
  // ----------------------------------------------------------------------
  
  WeatherStory::WeatherStory(const NFmiTime & theForecastTime,
									 const AnalysisSources & theSources,
									 const WeatherArea & theArea,
									 const WeatherPeriod & thePeriod,
									 const string & theVariable)
	: itsForecastTime(theForecastTime)
	, itsSources(theSources)
	, itsArea(theArea)
	, itsPeriod(thePeriod)
	, itsVar(theVariable)
  {
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Test whether the given story is known to this class
   *
   * \param theName The story name
   * \return True if this class can generate the story
   */
  // ----------------------------------------------------------------------
  
  bool WeatherStory::hasStory(const string & theName)
  {
	if(theName == "weather_shortoverview")
	  return true;
	return false;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Generate the desired story
   *
   * Throws if the story name is not recognized.
   *
   * \param theName The story name
   * \return Paragraph containing the story
   */
  // ----------------------------------------------------------------------.
  
  Paragraph WeatherStory::makeStory(const string & theName) const
  {
	if(theName == "weather_shortoverview")
	  return shortoverview();
	throw TextGenError("WeatherStory cannot make story "+theName);

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generator brief overview story on weather
   *
   * \return The story
   *
   * \see page_weather_shortoverview
   */
  // ----------------------------------------------------------------------

  Paragraph WeatherStory::shortoverview() const
  {
	using namespace Settings;
	using namespace WeatherPeriodTools;

	Paragraph paragraph;

#if 0
	const bool c_fullrange = optional_bool(itsVar+"::cloudiness::fullrange",true);
	const int c_starthour = optional_hour(itsVar+"::cloudiness::starthour",0);
	const int c_endhour = optional_hour(itsVar+"::cloudiness::endhour",0);
	const int c_maxstarthour = optional_hour(itsVar+"::cloudiness::maxstarthour",c_starthour);
	const int c_minendhour = optional_hour(itsVar+"::cloudiness::minendhour",c_endhour);

	const int c_clear = optional_percentage(itsVar+"::cloudiness::clear",40);
	const int c_cloudy = optional_percentage(itsVar+"::cloudiness::cloudy",70);

	const int c_single_limit = optional_percentage(itsVar+"::cloudiness::single_limit",60);
	const int c_double_limit = optional_percentage(itsVar+"::cloudiness::double_limit",20);

	const int r_starthour = optional_hour(itsVar+"::precipitation::starthour",0);
	const int r_endhour = optional_hour(itsVar+"::precipitation::endhour",0);
	const int r_maxstarthour = optional_hour(itsVar+"::precipitation::maxstarthour",r_starthour);
	const int r_minendhour = optional_hour(itsVar+"::precipitation::minendhour",r_endhour);

	const double r_rainy = optional_double(itsVar+"::precipitation::rainy",1);
	const double r_partly_rainy = optional_double(itsVar+"::precipitation::partly_rainy",0.1);
	const int c_unstable = optional_percentage(itsVar+"::cloudiness::unstable",50);
#endif
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
