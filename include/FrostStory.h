// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::FrostStory
 */
// ======================================================================

#ifndef TEXTGEN_FROSTSTORY_H
#define TEXTGEN_FROSTSTORY_H

#include "Story.h"
#include <string>

class NFmiTime;

namespace TextGen
{
  class Paragraph;

  class FrostStory : public Story
  {

  public:

	virtual ~FrostStory();
	FrostStory(const NFmiTime & theForecastTime,
			   const WeatherAnalysis::AnalysisSources & theSources,
			   const WeatherAnalysis::WeatherArea & theArea,
			   const WeatherAnalysis::WeatherPeriod & thePeriod,
			   const std::string & theVariable);

	static bool hasStory(const std::string & theName);
	virtual const Paragraph makeStory(const std::string & theName) const;

  private:

	const Paragraph mean() const;
	const Paragraph maximum() const;
	const Paragraph range() const;
	const Paragraph twonights() const;

	FrostStory();
	FrostStory(const FrostStory & theStory);
	FrostStory & operator=(const FrostStory & theStory);

	const NFmiTime itsForecastTime;
	const WeatherAnalysis::AnalysisSources & itsSources;
	const WeatherAnalysis::WeatherArea & itsArea;
	const WeatherAnalysis::WeatherPeriod & itsPeriod;
	const std::string itsVar;

  }; // class FrostStory
}

#endif // TEXTGEN_FROSTSTORY_H

// ======================================================================
