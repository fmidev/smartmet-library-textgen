// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::TemperatureStory
 */
// ======================================================================

#ifndef TEXTGEN_TEMPERATURESTORY_H
#define TEXTGEN_TEMPERATURESTORY_H

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

  class TemperatureStory : public Story
  {

  public:

	virtual ~TemperatureStory();
	TemperatureStory(const NFmiTime & theForecastTime,
					 const WeatherAnalysis::AnalysisSources & theSources,
					 const WeatherAnalysis::WeatherArea & theArea,
					 const WeatherAnalysis::WeatherPeriod & thePeriod,
					 const std::string & theVariable);

	static bool hasStory(const std::string & theName);
	virtual const Paragraph makeStory(const std::string & theName) const;

  private:

	const Paragraph mean() const;
	const Paragraph meanmax() const;
	const Paragraph meanmin() const;
	const Paragraph dailymax() const;
	const Paragraph nightlymin() const;
	const Paragraph weekly_minmax() const;
	const Paragraph weekly_averages() const;
	const Paragraph day() const;
	const Paragraph range() const;

	TemperatureStory();
	TemperatureStory(const TemperatureStory & theStory);
	TemperatureStory & operator=(const TemperatureStory & theStory);

	const NFmiTime itsForecastTime;
	const WeatherAnalysis::AnalysisSources & itsSources;
	const WeatherAnalysis::WeatherArea & itsArea;
	const WeatherAnalysis::WeatherPeriod & itsPeriod;
	const std::string itsVar;

  }; // class TemperatureStory
}

#endif // TEXTGEN_TEMPERATURESTORY_H

// ======================================================================
