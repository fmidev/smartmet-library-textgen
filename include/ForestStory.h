// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::ForestStory
 */
// ======================================================================

#ifndef TEXTGEN_FORESTSTORY_H
#define TEXTGEN_FORESTSTORY_H

#include "Story.h"
#include <string>

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherPeriod;
}

class NFmiTime;

namespace TextGen
{
  class Paragraph;

  class ForestStory : public Story
  {

  public:

	virtual ~ForestStory();
	ForestStory(const NFmiTime & theForecastTime,
				const WeatherAnalysis::AnalysisSources & theSources,
				const WeatherAnalysis::WeatherArea & theArea,
				const WeatherAnalysis::WeatherPeriod & thePeriod,
				const std::string & theVariable);
	
	static bool hasStory(const std::string & theName);
	virtual Paragraph makeStory(const std::string & theName) const;
	
  private:
	
	Paragraph forestfireindex_twodays() const;
	Paragraph evaporation_day() const;
	Paragraph forestfirewarning_county() const;

	ForestStory();
	ForestStory(const ForestStory & theStory);
	ForestStory & operator=(const ForestStory & theStory);

	const NFmiTime itsForecastTime;
	const WeatherAnalysis::AnalysisSources & itsSources;
	const WeatherAnalysis::WeatherArea & itsArea;
	const WeatherAnalysis::WeatherPeriod & itsPeriod;
	const std::string itsVar;
	
  }; // class ForestStory
}

#endif // TEXTGEN_FORESTSTORY_H

// ======================================================================
