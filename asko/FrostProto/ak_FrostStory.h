// ======================================================================
/*!
 * \file
 * \brief ...
 */
// ======================================================================

#ifndef TEXTGEN_AK_FROSTSTORY_H
#define TEXTGEN_AK_FROSTSTORY_H

#include "Story.h"
#include <string>

class NFmiTime;

namespace TextGen
{
  class Paragraph;

  class AK_FrostStory : public Story
  {

  public:
	AK_FrostStory( const NFmiTime & theForecastTime,
			    const WeatherAnalysis::AnalysisSources & theSources,
			    const WeatherAnalysis::WeatherArea & theArea,
			    const WeatherAnalysis::WeatherPeriod & thePeriod,
			    const std::string & theVariable );
	virtual ~AK_FrostStory() {}

	static bool hasStory( const std::string & theName );
	virtual const Paragraph makeStory( const std::string & theName ) const;

  private:
	const Paragraph mean() const;
	const Paragraph maximum() const;
	const Paragraph range() const;
	const Paragraph twonights() const;
	const Paragraph day() const;

	AK_FrostStory();
	AK_FrostStory( const AK_FrostStory & theStory );
	AK_FrostStory & operator=( const AK_FrostStory & theStory );

	const NFmiTime itsForecastTime;
	const WeatherAnalysis::AnalysisSources & itsSources;
	const WeatherAnalysis::WeatherArea & itsArea;
	const WeatherAnalysis::WeatherPeriod & itsPeriod;
	const std::string itsVar;

  }; // class AK_FrostStory
}

#endif // TEXTGEN_FROSTSTORY_H

// ======================================================================
