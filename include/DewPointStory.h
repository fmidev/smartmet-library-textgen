// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::DewPointStory
 */
// ======================================================================

#ifndef TEXTGEN_DEWPOINTSTORY_H
#define TEXTGEN_DEWPOINTSTORY_H

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

  class DewPointStory : public Story
  {

  public:

	virtual ~DewPointStory();
	DewPointStory(const NFmiTime & theForecastTime,
				  const WeatherAnalysis::AnalysisSources & theSources,
				  const WeatherAnalysis::WeatherArea & theArea,
				  const WeatherAnalysis::WeatherPeriod & thePeriod,
				  const std::string & theVariable);

	static bool hasStory(const std::string & theName);
	virtual const Paragraph makeStory(const std::string & theName) const;

  private:

	const Paragraph range() const;

	DewPointStory();
	DewPointStory(const DewPointStory & theStory);
	DewPointStory & operator=(const DewPointStory & theStory);

	const NFmiTime itsForecastTime;
	const WeatherAnalysis::AnalysisSources & itsSources;
	const WeatherAnalysis::WeatherArea & itsArea;
	const WeatherAnalysis::WeatherPeriod & itsPeriod;
	const std::string itsVar;

  }; // class DewPointStory
}

#endif // TEXTGEN_DEWPOINTSTORY_H

// ======================================================================
