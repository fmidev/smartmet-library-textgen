// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::RelativeHumidityStory
 */
// ======================================================================
/*!
 * \class TextGen::RelativeHumidityStory
 *
 * \brief Generates stories on relativehumidity
 *
 * \see page_tarinat
 */
// ======================================================================

#ifndef TEXTGEN_RELATIVEHUMIDITYSTORY_H
#define TEXTGEN_RELATIVEHUMIDITYSTORY_H

#include "AnalysisSources.h"
#include "Story.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"

#include <string>

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
