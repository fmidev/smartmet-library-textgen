// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::RoadStory
 */
// ======================================================================

#ifndef TEXTGEN_ROADSTORY_H
#define TEXTGEN_ROADSTORY_H

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

  class RoadStory : public Story
  {

  public:

	virtual ~RoadStory();
	RoadStory(const NFmiTime & theForecastTime,
			  const WeatherAnalysis::AnalysisSources & theSources,
			  const WeatherAnalysis::WeatherArea & theArea,
			  const WeatherAnalysis::WeatherPeriod & thePeriod,
			  const std::string & theVariable);
	
	static bool hasStory(const std::string & theName);
	virtual Paragraph makeStory(const std::string & theName) const;
	
  private:
	
	RoadStory();
	RoadStory(const RoadStory & theStory);
	RoadStory & operator=(const RoadStory & theStory);

	const NFmiTime itsForecastTime;
	const WeatherAnalysis::AnalysisSources & itsSources;
	const WeatherAnalysis::WeatherArea & itsArea;
	const WeatherAnalysis::WeatherPeriod & itsPeriod;
	const std::string itsVar;
	
  }; // class RoadStory
}

#endif // TEXTGEN_ROADSTORY_H

// ======================================================================