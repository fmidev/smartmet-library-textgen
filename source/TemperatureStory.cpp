// ======================================================================
/*!
 * \file
 * \brief Implementation of class TemperatureStory
 */
// ======================================================================

#include "TemperatureStory.h"
#include "GridForecaster.h"
#include "Number.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "TextGenError.h"
#include "WeatherFunction.h"
#include "WeatherLimits.h"
#include "WeatherParameter.h"
#include "WeatherResult.h"

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
   * \param theSources The analysis sources
   * \param theArea The area to be analyzed 
  */
  // ----------------------------------------------------------------------
  
  TemperatureStory::TemperatureStory(const AnalysisSources & theSources,
									 const WeatherArea & theArea,
									 const WeatherPeriod & thePeriod)
	: itsSources(theSources)
	, itsArea(theArea)
	, itsPeriod(thePeriod)
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
  
  Paragraph TemperatureStory::makeStory(const string & theName) const
  {
	if(theName == "temperature_mean")
	  return mean();
	if(theName == "temperature_meanmax")
	  return meanmax();
	if(theName == "temperature_meanmin")
	  return meanmin();

	throw TextGenError("TemperatureStory cannot make story "+theName);

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on mean temperature
   *
   * \return The generated paragraph
   *
   * \todo Is throwing the best way to handle missing results?
   */
  // ----------------------------------------------------------------------
  
  Paragraph TemperatureStory::mean() const
  {
	Paragraph paragraph;
	Sentence sentence;

	GridForecaster forecaster;

	WeatherResult result = forecaster.analyze(itsSources,
											  Temperature,
											  Mean,
											  WeatherLimits(),
											  itsPeriod,
											  itsArea);

	if(result.value() == kFloatMissing)
	  throw TextGenError("Mean temperature not available");

	Number<int> num = FmiRound(result.value());

	sentence << "keskilämpötila"
			 << num
			 << "astetta";
	
	paragraph << sentence;
	return paragraph;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on mean maximum temperature
   *
   * Throws if the weather period does not evenly divide into 24 hour
   * segments.
   *
   * \return The story
   */
  // ----------------------------------------------------------------------

  Paragraph TemperatureStory::meanmax() const
  {
	Paragraph paragraph;
	Sentence sentence;
	sentence << "keskimääräinen ylin lämpötila"
			 << 20
			 << "astetta";
	paragraph << sentence;
	return paragraph;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on mean minimum temperature
   *
   * Throws if the weather period does not evenly divide into 24 hour
   * segments.
   *
   * \return The story
   */
  // ----------------------------------------------------------------------

  Paragraph TemperatureStory::meanmin() const
  {
	Paragraph paragraph;
	Sentence sentence;
	sentence << "keskimääräinen alin lämpötila"
			 << 5
			 << "astetta";
	paragraph << sentence;
	return paragraph;
  }


} // namespace TextGen
  
// ======================================================================
  
