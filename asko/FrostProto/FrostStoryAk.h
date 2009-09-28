// ======================================================================
/*!
 * \file  FrostStoryAk.h
 * \brief FrostStory version by AKa 2009.
 *
 * NOTE: Do NOT rename this to 'FrostStory.cpp'; that already exists under
 *      TextGen all-in-one 'source/' directory!
 */
// ======================================================================

#ifndef TEXTGEN_FROSTSTORY_AK_H
#define TEXTGEN_FROSTSTORY_AK_H

#include "Story.h"
#include "Settings.h"

#include <string>

#include <newbase/NFmiTime.h>

// Configuration keys
//
#define FROST_OVERVIEW "frost_overview::"

#define SEASON_START    "season_start"
#define SEASON_END      "season_end"

#define PRECISION       "precision"
#define SECOND_NIGHT    "second_night"     // was: "last_night" (misleading word)
#define FROST_LOW_LIMIT "low_limit"

#define NIGHT_START_HOUR "night::starthour"
#define NIGHT_END_HOUR   "night::endhour"

// Testing overrides
//
#define NIGHT1_MEAN     "test::night1::mean"
#define NIGHT1_SEVERE_MEAN "test::night1::severe_mean"

#define NIGHT2_MEAN     "test::night2::mean"
#define NIGHT2_SEVERE_MEAN "test::night2::severe_mean"

#define T_SUM_MAX       "test::t_sum_max"
#define T_MAX           "test::t_max"

// Story names
//
#define STORY_OVERVIEW_TEXT    "overview_text"
#define STORY_OVERVIEW_NUMERIC "overview_numeric"

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
    bool is_frost_season( const NFmiTime &time ) const;

    static const std::string PREFIX;   // configuration prefix ("textgen::frost_overview::")

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
