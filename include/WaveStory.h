// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::WaveStory
 */
// ======================================================================

#ifndef TEXTGEN_WAVESTORY_H
#define TEXTGEN_WAVESTORY_H

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

  class WaveStory : public Story
  {

  public:

	virtual ~WaveStory();
	WaveStory(const NFmiTime & theForecastTime,
			  const WeatherAnalysis::AnalysisSources & theSources,
			  const WeatherAnalysis::WeatherArea & theArea,
			  const WeatherAnalysis::WeatherPeriod & thePeriod,
			  const std::string & theVariable);

	static bool hasStory(const std::string & theName);
	virtual Paragraph makeStory(const std::string & theName) const;

  private:

	WaveStory();
	WaveStory(const WaveStory & theStory);
	WaveStory & operator=(const WaveStory & theStory);

	Paragraph range() const;

	const NFmiTime itsForecastTime;
	const WeatherAnalysis::AnalysisSources & itsSources;
	const WeatherAnalysis::WeatherArea & itsArea;
	const WeatherAnalysis::WeatherPeriod & itsPeriod;
	const std::string itsVar;

  }; // class WaveStory
}

#endif // TEXTGEN_WAVESTORY_H

// ======================================================================
