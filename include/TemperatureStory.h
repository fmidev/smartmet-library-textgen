// ======================================================================
/*!
 * \file
 * \brief Interface of class TemperatureStory
 */
// ======================================================================
/*!
 * \class TextGen::TemperatureStory
 *
 * \brief Generates stories on temperature
 *
 * \see page_tarinat
 */
// ======================================================================

#ifndef TEXTGEN_TEMPERATURESTORY_H
#define TEXTGEN_TEMPERATURESTORY_H

#include "AnalysisSources.h"
#include "Story.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"

#include <string>

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
	virtual Paragraph makeStory(const std::string & theName) const;

  private:

	Paragraph mean() const;
	Paragraph meanmax() const;
	Paragraph meanmin() const;
	Paragraph dailymax() const;
	Paragraph nightlymin() const;
	Paragraph weekly_minmax() const;

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
