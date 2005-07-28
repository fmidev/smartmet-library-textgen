// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::PressureStory
 */
// ======================================================================

#ifndef TEXTGEN_PRESSURESTORY_H
#define TEXTGEN_PRESSURESTORY_H

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

  class PressureStory : public Story
  {

  public:

	virtual ~PressureStory();
	PressureStory(const NFmiTime & theForecastTime,
				  const WeatherAnalysis::AnalysisSources & theSources,
				  const WeatherAnalysis::WeatherArea & theArea,
				  const WeatherAnalysis::WeatherPeriod & thePeriod,
				  const std::string & theVariable);

	static bool hasStory(const std::string & theName);
	virtual const Paragraph makeStory(const std::string & theName) const;

  private:

	const Paragraph mean() const;

	PressureStory();
	PressureStory(const PressureStory & theStory);
	PressureStory & operator=(const PressureStory & theStory);

	const NFmiTime itsForecastTime;
	const WeatherAnalysis::AnalysisSources & itsSources;
	const WeatherAnalysis::WeatherArea & itsArea;
	const WeatherAnalysis::WeatherPeriod & itsPeriod;
	const std::string itsVar;

  }; // class PressureStory
}

#endif // TEXTGEN_PRESSURESTORY_H

// ======================================================================
