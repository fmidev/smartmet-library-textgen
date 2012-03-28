// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::CloudinessStory
 */
// ======================================================================

#ifndef TEXTGEN_CLOUDINESSSTORY_H
#define TEXTGEN_CLOUDINESSSTORY_H

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

  class CloudinessStory : public Story
  {

  public:

	virtual ~CloudinessStory();
	CloudinessStory(const NFmiTime & theForecastTime,
					const WeatherAnalysis::AnalysisSources & theSources,
					const WeatherAnalysis::WeatherArea & theArea,
					const WeatherAnalysis::WeatherPeriod & thePeriod,
					const std::string & theVariable);

	static bool hasStory(const std::string & theName);
	virtual Paragraph makeStory(const std::string & theName) const;

  private:

	const Paragraph overview(void) const;

	CloudinessStory();
	CloudinessStory(const CloudinessStory & theStory);
	CloudinessStory & operator=(const CloudinessStory & theStory);

	const NFmiTime itsForecastTime;
	const WeatherAnalysis::AnalysisSources & itsSources;
	const WeatherAnalysis::WeatherArea & itsArea;
	const WeatherAnalysis::WeatherPeriod & itsPeriod;
	const std::string itsVar;

  }; // class CloudinessStory
}

#endif // TEXTGEN_CLOUDINESSSTORY_H

// ======================================================================
