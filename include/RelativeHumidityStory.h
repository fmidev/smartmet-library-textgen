// ======================================================================
/*!
 * \file
 * \brief Interface of class RelativeHumidityStory
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
#include "WeatherArea.h"
#include "WeatherPeriod.h"

#include <string>

namespace TextGen
{
  class Paragraph;

  class RelativeHumidityStory
  {

  public:

	~RelativeHumidityStory();
	RelativeHumidityStory(const WeatherAnalysis::AnalysisSources & theSources,
			   const WeatherAnalysis::WeatherArea & theArea,
			   const WeatherAnalysis::WeatherPeriod & thePeriod,
			   const std::string & theVariable);

	static bool hasStory(const std::string & theName);
	Paragraph makeStory(const std::string & theName) const;

  private:

	Paragraph lowest() const;

	RelativeHumidityStory();
	RelativeHumidityStory(const RelativeHumidityStory & theStory);
	RelativeHumidityStory & operator=(const RelativeHumidityStory & theStory);

	const WeatherAnalysis::AnalysisSources & itsSources;
	const WeatherAnalysis::WeatherArea & itsArea;
	const WeatherAnalysis::WeatherPeriod & itsPeriod;
	const std::string & itsVariable;

  }; // class RelativeHumidityStory
}

#endif // TEXTGEN_RELATIVEHUMIDITYSTORY_H

// ======================================================================