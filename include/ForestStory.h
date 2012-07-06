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

namespace TextGen
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
				const TextGen::AnalysisSources & theSources,
				const TextGen::WeatherArea & theArea,
				const TextGen::WeatherPeriod & thePeriod,
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
	const TextGen::AnalysisSources & itsSources;
	const TextGen::WeatherArea & itsArea;
	const TextGen::WeatherPeriod & itsPeriod;
	const std::string itsVar;
	
  }; // class ForestStory
}

#endif // TEXTGEN_FORESTSTORY_H

// ======================================================================
