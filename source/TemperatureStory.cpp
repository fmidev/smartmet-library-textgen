// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::TemperatureStory
 */
// ======================================================================
/*!
 * \class TextGen::TemperatureStory
 *
 * \brief Generates stories on temperature
 *
 * \see page_tarinat
 */
// ======================================================================

#include "TemperatureStory.h"
#include "Paragraph.h"
#include "TextGenError.h"

using namespace WeatherAnalysis;
using namespace std;

namespace TextGen
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------
  
  TemperatureStory::~TemperatureStory()
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
  
  TemperatureStory::TemperatureStory(const NFmiTime & theForecastTime,
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
  
  bool TemperatureStory::hasStory(const string & theName)
  {
	if(theName == "temperature_mean")
	  return true;
	if(theName == "temperature_meanmax")
	  return true;
	if(theName == "temperature_meanmin")
	  return true;
	if(theName == "temperature_dailymax")
	  return true;
	if(theName == "temperature_nightlymin")
	  return true;
	if(theName == "temperature_weekly_minmax")
	  return true;
	if(theName == "temperature_weekly_averages")
	  return true;
	if(theName == "temperature_day")
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
  
  const Paragraph TemperatureStory::makeStory(const string & theName) const
  {
	if(theName == "temperature_mean")
	  return mean();
	if(theName == "temperature_meanmax")
	  return meanmax();
	if(theName == "temperature_meanmin")
	  return meanmin();
	if(theName == "temperature_dailymax")
	  return dailymax();
	if(theName == "temperature_nightlymin")
	  return nightlymin();
	if(theName == "temperature_weekly_minmax")
	  return weekly_minmax();
	if(theName == "temperature_weekly_averages")
	  return weekly_averages();
	if(theName == "temperature_day")
	  return day();

	throw TextGenError("TemperatureStory cannot make story "+theName);

  }


} // namespace TextGen
  
// ======================================================================
