// ======================================================================
/*!
 * \file
 * \brief Implementation of class PrecipitationStory
 */
// ======================================================================

#include "PrecipitationStory.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "Number.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "TextGenError.h"
#include "WeatherFunction.h"
#include "WeatherLimits.h"
#include "WeatherResult.h"

#include "NFmiSettings.h"

#include "boost/lexical_cast.hpp"

using namespace WeatherAnalysis;
using namespace std;
using namespace boost;

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
	if(theName == "precipitation_range")
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
	if(theName == "precipitation_range")
	  return range();

	throw TextGenError("PrecipitationStory cannot make story "+theName);

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on mean total precipitation
   *
   * Sample story: "Sadesumma 10 millimetriä."
   *
   * \return The story
   *
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

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on total precipitation range
   *
   * Sample story 1: "Sadesumma yli LIMIT millimetriä."	(when min>=LIMIT)
   * Sample story 2: "Sadesumma 4 millimetriä."			(when min==max)
   * Sample story 3: "Sadesumma 1-5 millimetriä."		(otherwise)
   *
   * where LIMIT is the value of textgen::precipitation_range::maxrain

   * \return The story
   *
   * \todo Should filter out rains less than 0.1 mm in the summation
   */
  // ----------------------------------------------------------------------

  Paragraph PrecipitationStory::range() const
  {
	Paragraph paragraph;
	Sentence sentence;

	GridForecaster forecaster;

	WeatherResult minresult = forecaster.analyze(itsSources,
												 Precipitation,
												 MinSum,
												 WeatherLimits(),
												 itsPeriod,
												 itsArea);

	WeatherResult maxresult = forecaster.analyze(itsSources,
												 Precipitation,
												 MaxSum,
												 WeatherLimits(),
												 itsPeriod,
												 itsArea);

	if(minresult.value() == kFloatMissing ||
	   maxresult.value() == kFloatMissing)
	  throw TextGenError("Total precipitation not available");

	Number<int> minrain = FmiRound(minresult.value());
	Number<int> maxrain = FmiRound(minresult.value());

	const string variable = "textgen::story::precipitation_range::maxrain";
	const string varvalue = NFmiSettings::instance().require(variable);
	const int rainlimit = lexical_cast<int>(varvalue);

	if(minrain.value() >= rainlimit)
	  {
		sentence << "sadesumma"
				 << "yli"
				 << rainlimit
				 << "millimetriä";
	  }
	else if(minrain.value() == maxrain.value())
	  {
		sentence << "sadesumma"
				 << minrain
				 << "millimetriä";
	  }
	else 
	  {
		sentence << "sadesumma"
				 << minrain
				 << Delimiter("-")
				 << maxrain
				 << "millimetriä";
	  }

	paragraph << sentence;
	return paragraph;
  }



} // namespace TextGen
  
// ======================================================================
  
