// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::WindStory
 */
// ======================================================================
/*!
 * \class TextGen::WindStory
 *
 * \brief Generates stories on wind speed and direction
 *
 * \see page_tarinat
 */
// ======================================================================

#ifndef TEXTGEN_WINDSTORY_H
#define TEXTGEN_WINDSTORY_H

#include "AnalysisSources.h"
#include "Story.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"

#include <string>

class NFmiTime;

namespace TextGen
{
  class Paragraph;

  class WindStory : public Story
  {

  public:

	virtual ~WindStory();
	WindStory(const NFmiTime & theForecastTime,
			  const WeatherAnalysis::AnalysisSources & theSources,
			  const WeatherAnalysis::WeatherArea & theArea,
			  const WeatherAnalysis::WeatherPeriod & thePeriod,
			  const std::string & theVariable);

	static bool hasStory(const std::string & theName);
	virtual Paragraph makeStory(const std::string & theName) const;

  private:

	WindStory();
	WindStory(const WindStory & theStory);
	WindStory & operator=(const WindStory & theStory);

	Paragraph simple_overview() const;
	Paragraph overview() const;

	const NFmiTime itsForecastTime;
	const WeatherAnalysis::AnalysisSources & itsSources;
	const WeatherAnalysis::WeatherArea & itsArea;
	const WeatherAnalysis::WeatherPeriod & itsPeriod;
	const std::string itsVar;

  }; // class WindStory
}

#endif // TEXTGEN_WINDSTORY_H

// ======================================================================
