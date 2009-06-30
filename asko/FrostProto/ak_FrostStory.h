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

#include <newbase/NFmiTime.h>

// Configuration keys
//
#define TEXTGEN "textgen::"
#define FROST_OVERVIEW TEXTGEN "frost_overview::"

#define SEASON_START FROST_OVERVIEW "season_start"
#define SEASON_END FROST_OVERVIEW "season_end"

#define PRECISION FROST_OVERVIEW "precision"
#define LASTNIGHT FROST_OVERVIEW "last_night"
#define FROST_LOW_LIMIT FROST_OVERVIEW "low_limit"

#define NIGHT_START_HOUR TEXTGEN FROST_OVERVIEW "night::starthour"
#define NIGHT_END_HOUR   TEXTGEN FROST_OVERVIEW "night::endhour"

// Testing overrides
//
#define DAY1_MEAN FROST_OVERVIEW "test::day1::mean"
#define DAY1_SEVERE_MEAN FROST_OVERVIEW "test::day1::severe_mean"

#define DAY2_MEAN FROST_OVERVIEW "test::day2::mean"
#define DAY2_SEVERE_MEAN FROST_OVERVIEW "test::day2::severe_mean"

// Story names
//
#define STORY_OVERVIEW_TEXT "overview_text"
#define STORY_OVERVIEW_NUMERIC "overview_numeric"


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
	Paragraph overview_text() const;
	Paragraph overview_numeric() const;
	   // ...

    bool is_frost_season( NFmiTime t_now ) const;
    bool is_frost_season() const {
        NFmiTime t_now;
        return is_frost_season( t_now );
    }

    const std::string PREFIX;   // configuration prefix ("textgen::")

    int require_int(const std::string &s)        const { return ::Settings::require_int(PREFIX+s); }
    bool require_bool(const std::string &s)      const { return ::Settings::require_bool(PREFIX+s); }
    int require_hour(const std::string &s)       const { return ::Settings::require_hour(PREFIX+s); }
    int require_percentage(const std::string &s) const { return ::Settings::require_percentage(PREFIX+s); }
    NFmiTime require_time(const std::string &s) const { return ::Settings::require_time(PREFIX+s); }

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
