// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::RelativeHumidityStory
 */
// ======================================================================

#ifndef TEXTGEN_RELATIVEHUMIDITYSTORY_H
#define TEXTGEN_RELATIVEHUMIDITYSTORY_H

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

  class RelativeHumidityStory : public Story
  {

  public:

	virtual ~RelativeHumidityStory();
	RelativeHumidityStory(const NFmiTime & theForecastTime,
						  const WeatherAnalysis::AnalysisSources & theSources,
						  const WeatherAnalysis::WeatherArea & theArea,
						  const WeatherAnalysis::WeatherPeriod & thePeriod,
						  const std::string & theVariable);
	
	static bool hasStory(const std::string & theName);
	virtual Paragraph makeStory(const std::string & theName) const;

  private:

	Paragraph lowest() const;

	RelativeHumidityStory();
	RelativeHumidityStory(const RelativeHumidityStory & theStory);
	RelativeHumidityStory & operator=(const RelativeHumidityStory & theStory);

	const NFmiTime itsForecastTime;
	const WeatherAnalysis::AnalysisSources & itsSources;
	const WeatherAnalysis::WeatherArea & itsArea;
	const WeatherAnalysis::WeatherPeriod & itsPeriod;
	const std::string itsVar;

  }; // class RelativeHumidityStory
}

#endif // TEXTGEN_RELATIVEHUMIDITYSTORY_H

// ======================================================================
