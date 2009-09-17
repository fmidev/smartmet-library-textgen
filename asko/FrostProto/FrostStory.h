// ======================================================================
/*!
 * \file
 * \brief ...
 */
// ======================================================================

#ifndef TEXTGEN_FROSTSTORY_H
#define TEXTGEN_FROSTSTORY_H

#include "Story.h"
#include "Settings.h"

#include <string>

#include <newbase/NFmiTime.h>

// Configuration keys
//
#define TEXTGEN         "textgen::"
#define FROST_OVERVIEW  TEXTGEN "frost_overview::"

#define SEASON_START    FROST_OVERVIEW "season_start"
#define SEASON_END      FROST_OVERVIEW "season_end"

#define PRECISION       FROST_OVERVIEW "precision"
#define SECOND_NIGHT    FROST_OVERVIEW "second_night"     // was: "last_night" (misleading word)
#define FROST_LOW_LIMIT FROST_OVERVIEW "low_limit"

#define NIGHT_START_HOUR TEXTGEN FROST_OVERVIEW "night::starthour"
#define NIGHT_END_HOUR   TEXTGEN FROST_OVERVIEW "night::endhour"

// Testing overrides
//
#define NIGHT1_MEAN       FROST_OVERVIEW "test::night1::mean"     // was: "day1"
#define NIGHT1_SEVERE_MEAN FROST_OVERVIEW "test::night1::severe_mean"

#define NIGHT2_MEAN       FROST_OVERVIEW "test::night2::mean"     // was: "day2"
#define NIGHT2_SEVERE_MEAN FROST_OVERVIEW "test::night2::severe_mean"

// Story names
//
#define STORY_OVERVIEW_TEXT    "overview_text"
#define STORY_OVERVIEW_NUMERIC "overview_numeric"

#define FROST_STORY_OVERVIEW_NUMERIC "FrostStory::frost_overview_numeric"

namespace TextGen
{
  class Paragraph;

  class FrostStory : public Story {
  public:
	FrostStory( const NFmiTime &forecast_time,
			    const WeatherAnalysis::AnalysisSources &sources,
			    const WeatherAnalysis::WeatherArea &area,
			    const WeatherAnalysis::WeatherPeriod &period )
	: itsForecastTime(forecast_time)
	, itsSources(sources)
	, itsArea(area)
	, itsPeriod(period) {}

	/*virtual*/ ~FrostStory() {}

	static bool hasStory( const std::string &name );
	
	// NAG: The 'const' here is absolutely unnecessary, but we must have it to 
	//     go along with the virtual fingerprint of the functin. Bad.  --AKa 17-Sep-2009
	//
	/*virtual*/ const Paragraph makeStory( const std::string &name ) const;

	static Paragraph overview_text( const FrostStory &me );
	static Paragraph overview_numeric( const FrostStory &me );
	   // ...

  private:
    static bool is_frost_season( const NFmiTime &time );
#if 0
    bool is_frost_season() const {
        return is_frost_season( NFmiTime() /*initializes to current time*/ );
    }
#endif

    static const std::string PREFIX;   // configuration prefix ("textgen::")

    static int require_int(const std::string &s)        { return ::Settings::require_int(PREFIX+s); }
    static bool require_bool(const std::string &s)      { return ::Settings::require_bool(PREFIX+s); }
    static int require_hour(const std::string &s)       { return ::Settings::require_hour(PREFIX+s); }
    static int require_percentage(const std::string &s) { return ::Settings::require_percentage(PREFIX+s); }
    static NFmiTime require_time(const std::string &s) { return ::Settings::require_time(PREFIX+s); }

    static bool optional_bool(const std::string &s, bool def) { return ::Settings::optional_bool(PREFIX+s,def); }
    static int optional_hour(const std::string &s, int def)   { return ::Settings::optional_hour(PREFIX+s,def); }

	FrostStory();   // is not
	FrostStory( const FrostStory & );   // is not
	FrostStory & operator=( const FrostStory & );    // is not

	const NFmiTime itsForecastTime;
	const WeatherAnalysis::AnalysisSources & itsSources;
	const WeatherAnalysis::WeatherArea & itsArea;
	const WeatherAnalysis::WeatherPeriod & itsPeriod;

  };  // FrostStory
}   // namespace

#endif

// ======================================================================
