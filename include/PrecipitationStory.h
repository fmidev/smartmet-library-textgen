// ======================================================================
/*!
 * \file
 * \brief Interface of class PrecipitationStory
 */
// ======================================================================
/*!
 * \class TextGen::PrecipitationStory
 *
 * \brief Generates stories on precipitation
 *
 */
// ======================================================================

#ifndef TEXTGEN_PRECIPITATIONSTORY_H
#define TEXTGEN_PRECIPITATIONSTORY_H

#include "AnalysisSources.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"

namespace TextGen
{
  class Paragraph;

  class PrecipitationStory
  {

  public:

	~PrecipitationStory();
	PrecipitationStory(const WeatherAnalysis::AnalysisSources & theSources,
					   const WeatherAnalysis::WeatherArea & theArea,
					   const WeatherAnalysis::WeatherPeriod & thePeriod);

	static bool hasStory(const std::string & theName);
	Paragraph makeStory(const std::string & theName) const;

  private:

	Paragraph total(void) const;

	PrecipitationStory();
	PrecipitationStory(const PrecipitationStory & theStory);
	PrecipitationStory & operator=(const PrecipitationStory & theStory);

	const WeatherAnalysis::AnalysisSources & itsSources;
	const WeatherAnalysis::WeatherArea & itsArea;
	const WeatherAnalysis::WeatherPeriod & itsPeriod;
	

  }; // class PrecipitationStory
}

#endif // TEXTGEN_PRECIPITATIONSTORY_H

// ======================================================================
