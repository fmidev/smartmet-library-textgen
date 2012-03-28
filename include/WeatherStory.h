// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::WeatherStory
 */
// ======================================================================

#ifndef TEXTGEN_WEATHERSTORY_H
#define TEXTGEN_WEATHERSTORY_H

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

  class WeatherStory : public Story
  {

  public:

	virtual ~WeatherStory();
	WeatherStory(const NFmiTime & theForecastTime,
					 const WeatherAnalysis::AnalysisSources & theSources,
					 const WeatherAnalysis::WeatherArea & theArea,
					 const WeatherAnalysis::WeatherPeriod & thePeriod,
					 const std::string & theVariable);

	static bool hasStory(const std::string & theName);
	virtual Paragraph makeStory(const std::string & theName) const;

  private:

	Paragraph overview() const;
	Paragraph forecast() const;
	Paragraph shortoverview() const;
	Paragraph thunderprobability() const;
	Paragraph thunderprobability_simplified() const;

	WeatherStory();
	WeatherStory(const WeatherStory & theStory);
	WeatherStory & operator=(const WeatherStory & theStory);

	const NFmiTime itsForecastTime;
	const WeatherAnalysis::AnalysisSources & itsSources;
	const WeatherAnalysis::WeatherArea & itsArea;
	const WeatherAnalysis::WeatherPeriod & itsPeriod;
	const std::string itsVar;

  }; // class WeatherStory
}

#endif // TEXTGEN_WEATHERSTORY_H

// ======================================================================
