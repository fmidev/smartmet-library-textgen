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
#include <stdexcept>

#include <newbase/NFmiTime.h>

// Configuration keys
//
#define FROST_OVERVIEW "frost_overview::"

#define SEASON_START    "season_start"
#define SEASON_END      "season_end"

#define PRECISION       "precision"
#define SECOND_NIGHT    "second_night"     // was: "last_night" (misleading word)
#define FROST_LOW_LIMIT "frost_limit"
#define SEVERE_LOW_LIMIT "severe_frost_limit"

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
#define STORY_OVERVIEW_TEXT    "frost_overview_text"
#define STORY_OVERVIEW_NUMERIC "frost_overview_numeric"

namespace TextGen
{
  class Paragraph;

  // NB: Do NOT name this 'FrostStory'; there is already a story by that name in textgen
  //     (and it needs the namespaces to be unique).    -- AKa 30-Sep-2009
  //
  class FrostStoryAk : public Story {
  public:
	FrostStoryAk( const NFmiTime &forecast_time,
			    const WeatherAnalysis::AnalysisSources &sources,
			    const WeatherAnalysis::WeatherArea &area,
			    const WeatherAnalysis::WeatherPeriod &period,
			    const std::string &var )
	: itsForecastTime(forecast_time)
	, itsSources(sources)
	, itsArea(area)
	, itsPeriod(period)
	, PREFIX(var) {}

	/*virtual*/ ~FrostStoryAk() {}

	static bool hasStory( const std::string &name );
	
	// NAG: The 'const' here is absolutely unnecessary, but we must have it to 
	//     go along with the virtual fingerprint of the functin. Bad.  --AKa 17-Sep-2009
	//
	/*virtual*/ const Paragraph makeStory( const std::string &name ) const;

	static Paragraph overview_text( const FrostStoryAk &me );
	static Paragraph overview_numeric( const FrostStoryAk &me );
	   // ...

  private:
    bool is_frost_season( const NFmiTime &time ) const;

    static int require_int( const std::string &PRE, const std::string &s )
        { return ::Settings::require_int(PRE+s); }
    static bool require_bool( const std::string &PRE, const std::string &s )
        { return ::Settings::require_bool(PRE+s); }
    static int require_hour( const std::string &PRE, const std::string &s )
        { return ::Settings::require_hour(PRE+s); }
    static int require_percentage( const std::string &PRE, const std::string &s )
        { return ::Settings::require_percentage(PRE+s); }

    static bool optional_bool(const std::string &PRE, const std::string &s, bool def)
        { return ::Settings::optional_bool(PRE+s,def); }
    static int optional_hour(const std::string &PRE, const std::string &s, int def)
        { return ::Settings::optional_hour(PRE+s,def); }
    static std::string optional_string(const std::string &PRE, const std::string &s, const std::string &def)
        { return ::Settings::optional_string(PRE+s,def); }

	FrostStoryAk();   // is not
	FrostStoryAk( const FrostStoryAk & );   // is not
	FrostStoryAk & operator=( const FrostStoryAk & );    // is not

	const NFmiTime itsForecastTime;
	const WeatherAnalysis::AnalysisSources & itsSources;
	const WeatherAnalysis::WeatherArea & itsArea;
	const WeatherAnalysis::WeatherPeriod & itsPeriod;
    const std::string PREFIX;       // i.e. "textgen::part1::story::frost_overview_numeric"

  };  // FrostStoryAk
}   // namespace

#endif

// ======================================================================
