// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::SpecialStory
 */
// ======================================================================

#ifndef TEXTGEN_SPECIALSTORY_H
#define TEXTGEN_SPECIALSTORY_H

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

  class SpecialStory : public Story
  {

  public:

	virtual ~SpecialStory();
	SpecialStory(const NFmiTime & theForecastTime,
				 const WeatherAnalysis::AnalysisSources & theSources,
				 const WeatherAnalysis::WeatherArea & theArea,
				 const WeatherAnalysis::WeatherPeriod & thePeriod,
				 const std::string & theVariable);

	static bool hasStory(const std::string & theName);
	virtual Paragraph makeStory(const std::string & theName) const;

  private:

	Paragraph none() const;
	Paragraph text() const;
#if 0
	Paragraph date() const;
	Paragraph table() const;
#endif

	SpecialStory();
	SpecialStory(const SpecialStory & theStory);
	SpecialStory & operator=(const SpecialStory & theStory);

	const NFmiTime itsForecastTime;
	const WeatherAnalysis::AnalysisSources & itsSources;
	const WeatherAnalysis::WeatherArea & itsArea;
	const WeatherAnalysis::WeatherPeriod & itsPeriod;
	const std::string itsVar;

  }; // class SpecialStory
}

#endif // TEXTGEN_SPECIALSTORY_H

// ======================================================================
