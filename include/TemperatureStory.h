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
 */
// ======================================================================

#ifndef TEXTGEN_TEMPERATURESTORY_H
#define TEXTGEN_TEMPERATURESTORY_H

#include "AnalysisSources.h"
#include "WeatherArea.h"

namespace TextGen
{
  class Paragraph;

  class TemperatureStory
  {

  public:

	~TemperatureStory();
	TemperatureStory(const WeatherAnalysis::AnalysisSources & theSources,
					 const WeatherAnalysis::WeatherArea & theArea);

	Paragraph mean(void) const;
	Paragraph meanmax(void) const;
	Paragraph meanmin(void) const;

  private:

	TemperatureStory();
	TemperatureStory(const TemperatureStory & theStory);
	TemperatureStory & operator=(const TemperatureStory & theStory);

	const WeatherAnalysis::AnalysisSources & itsSources;
	WeatherAnalysis::WeatherArea itsArea;

  }; // class TemperatureStory
}

#endif // TEXTGEN_TEMPERATURESTORY_H

// ======================================================================
