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
#include "Story.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"

class NFmiTime;

namespace TextGen
{
  class Paragraph;

  class PrecipitationStory : public Story
  {

  public:

	virtual ~PrecipitationStory();
	PrecipitationStory(const NFmiTime & theForecastTime,
					   const WeatherAnalysis::AnalysisSources & theSources,
					   const WeatherAnalysis::WeatherArea & theArea,
					   const WeatherAnalysis::WeatherPeriod & thePeriod,
					   const std::string & theVariable);

	static bool hasStory(const std::string & theName);
	virtual Paragraph makeStory(const std::string & theName) const;

  private:

	Paragraph total(void) const;
	Paragraph range(void) const;
	Paragraph classification(void) const;

	PrecipitationStory();
	PrecipitationStory(const PrecipitationStory & theStory);
	PrecipitationStory & operator=(const PrecipitationStory & theStory);

	const NFmiTime itsForecastTime;
	const WeatherAnalysis::AnalysisSources & itsSources;
	const WeatherAnalysis::WeatherArea & itsArea;
	const WeatherAnalysis::WeatherPeriod & itsPeriod;
	const std::string itsVar;

  }; // class PrecipitationStory
}

#endif // TEXTGEN_PRECIPITATIONSTORY_H

// ======================================================================
