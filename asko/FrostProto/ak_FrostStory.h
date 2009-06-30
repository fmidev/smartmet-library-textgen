// ======================================================================
/*!
 * \file
 * \brief ...
 */
// ======================================================================

#ifndef TEXTGEN_AK_FROSTSTORY_H
#define TEXTGEN_AK_FROSTSTORY_H

#include "Story.h"
#include "Settings.h"

#include <string>

class NFmiTime;

namespace TextGen
{
  class Paragraph;

  class AK_FrostStory : public Story {
  public:
	AK_FrostStory( const NFmiTime & theForecastTime,
			    const WeatherAnalysis::AnalysisSources & theSources,
			    const WeatherAnalysis::WeatherArea & theArea,
			    const WeatherAnalysis::WeatherPeriod & thePeriod );
	virtual ~AK_FrostStory() {}

	static bool hasStory( const std::string & theName );
	virtual const Paragraph makeStory( const std::string & theName ) const;

  private:
	Paragraph text_overview() const;
	Paragraph numeric_overview() const;
	   // ...

    bool is_frost_season() const;

    const std::string PREFIX;   // configuration prefix ("textgen::")

    int require_int(const std::string &s)        const { return ::Settings::require_int(PREFIX+s); }
    bool require_bool(const std::string &s)      const { return ::Settings::require_bool(PREFIX+s); }
    int require_hour(const std::string &s)       const { return ::Settings::require_hour(PREFIX+s); }
    int require_percentage(const std::string &s) const { return ::Settings::require_percentage(PREFIX+s); }

    bool optional_bool(const std::string &s, bool def) const { return ::Settings::optional_bool(PREFIX+s,def); }
    int optional_hour(const std::string &s, int def)   const { return ::Settings::optional_hour(PREFIX+s,def); }

	AK_FrostStory();   // is not
	AK_FrostStory( const AK_FrostStory & );   // is not
	AK_FrostStory & operator=( const AK_FrostStory & );    // is not

	const NFmiTime itsForecastTime;
	const WeatherAnalysis::AnalysisSources & itsSources;
	const WeatherAnalysis::WeatherArea & itsArea;
	const WeatherAnalysis::WeatherPeriod & itsPeriod;

  };  // AK_FrostStory
}   // namespace

#endif

// ======================================================================
