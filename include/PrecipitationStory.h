// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::PrecipitationStory
 */
// ======================================================================

#ifndef TEXTGEN_PRECIPITATIONSTORY_H
#define TEXTGEN_PRECIPITATIONSTORY_H

#include "Story.h"

class NFmiTime;

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherPeriod;
}

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
	virtual const Paragraph makeStory(const std::string & theName) const;

  private:

	const Paragraph total() const;
	const Paragraph total_day() const;
	const Paragraph range() const;
	const Paragraph classification() const;
	const Paragraph pop_twodays() const;
	const Paragraph pop_days() const;
	const Paragraph pop_max() const;
	const Paragraph sums() const;
	const Paragraph daily_sums() const;

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
