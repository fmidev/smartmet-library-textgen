// ======================================================================
/*!
 * \file
 * \brief Implementation of class PrecipitationStory
 */
// ======================================================================

#include "PrecipitationStory.h"
#include "GridForecaster.h"
#include "Number.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "TextGenError.h"
#include "WeatherFunction.h"
#include "WeatherLimits.h"
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
  
  PrecipitationStory::~PrecipitationStory()
  {
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theSources The analysis sources
   * \param theArea The area to be analyzed
   * \param thePeriod The time interval to be analyzed
   */
  // ----------------------------------------------------------------------
  
  PrecipitationStory::PrecipitationStory(const AnalysisSources & theSources,
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
  
  bool PrecipitationStory::hasStory(const string & theName)
  {
	if(theName == "precipitation_total")
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
  
  Paragraph PrecipitationStory::makeStory(const string & theName) const
  {
	if(theName == "precipitation_total")
	  return total();

	throw TextGenError("PrecipitationStory cannot make story "+theName);

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on total precipitation
   *
   * \return The story
   *
   * \todo Should tell range instead of a single value.
   * \todo Should filter out rains less than 0.1 mm in the summation
   */
  // ----------------------------------------------------------------------

  Paragraph PrecipitationStory::total() const
  {
	Paragraph paragraph;
	Sentence sentence;

	GridForecaster forecaster;
	WeatherResult result = forecaster.analyze(itsSources,
											  Precipitation,
											  MeanSum,
											  WeatherLimits(),
											  itsPeriod,
											  itsArea);

	if(result.value() == kFloatMissing)
	  throw TextGenError("Total precipitation not available");

	Number<int> num = FmiRound(result.value());

	sentence << "sadesumma"
			 << num
			 << "millimetriä";
	paragraph << sentence;
	return paragraph;
  }



} // namespace TextGen
  
// ======================================================================
  
