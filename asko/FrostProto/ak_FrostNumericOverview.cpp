// ======================================================================
/*!
 * \file ak_FrostNumericOverview.cpp
 * \brief Textgen story for one night (following night, or the night after that)
 *
 * Reference: <http://wiki.weatherproof.fi/index.php?title=Textgen:_frost_overview>
 */
// ======================================================================

#include "ak_FrostStory.h"

// Textgen headers
//
#include "Delimiter.h"
#include "GridForecaster.h"
#include "Integer.h"
#include "MathTools.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "UnitFactory.h"
#include "WeatherPeriodTools.h"
#include "WeatherResult.h"

using namespace std;
//using namespace WeatherAnalysis;

using MathTools::to_precision;

// Our configuration subpath
//
const string CONF_SUBPATH= "frost_overview::";


namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on one night frost
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  
  Paragraph AK_FrostStory::numeric_overview() const {
	MessageLogger log("AK_FrostStory::frost_numeric_overview");

	Paragraph paragraph;

	if (!is_frost_season()) {
		log << "Frost season is not on";
		return paragraph;
	  }

	const int starthour    = require_hour( "night::starthour" );
	const int endhour      = require_hour( "night::endhour" );
	const int maxstarthour = optional_hour( "night::maxstarthour",starthour );
	const int minendhour   = optional_hour( "night::minendhour",endhour );

	const int precision   = require_percentage( "precision" );

    // Montako yötä periodissa on? (0/1/2)
    //
	const int nights = WeatherAnalysis::WeatherPeriodTools::countPeriods( itsPeriod,
														 starthour,
														 endhour,
														 maxstarthour,
														 minendhour );
	if (nights==0) {
		//log << paragraph;     // (ei siinä vielä mitään ole)
		return paragraph;
    }

    // Kumpi öistä on tarkoitus ottaa?
    //
    const bool last_night= Settings::optional_bool( CONF_SUBPATH +"last_night", false );

    if (last_night && (nights<2)) {
        throw TextGenError( "Asking for second night but period only has one." );
    }

	// Calculate frost probability

	WeatherAnalysis::GridForecaster forecaster;

    // TBD: Miksi täällä on tuollaista 'feikkiä'?  --AKa
    //
	WeatherAnalysis::WeatherPeriod some = WeatherAnalysis::WeatherPeriodTools::getPeriod( itsPeriod,
														  last_night ? 2:1,
														  starthour,
														  endhour,
														  maxstarthour,
														  minendhour );

	WeatherAnalysis::WeatherResult frost = forecaster.analyze( string("fake::") + (last_night ? "day2":"day1") +"::mean",
											  itsSources,
											  WeatherAnalysis::Frost,
											  WeatherAnalysis::Mean,     // TBD: onko tämä oikein?
											  WeatherAnalysis::Maximum,  // -''-
											  itsArea,
											  some );

	WeatherAnalysis::WeatherResult severefrost = forecaster.analyze( string("fake::") + (last_night ? "day2":"day1") +"::severe_mean",
												    itsSources,
												    WeatherAnalysis::SevereFrost,
												    WeatherAnalysis::Mean,      // TBD: onko tämä oikein?
												    WeatherAnalysis::Maximum,   // -''-
												    itsArea,
												    some );
	
	if (frost.value()==kFloatMissing || severefrost.value()==kFloatMissing) {
        throw TextGenError("Frost is not available");
    }

	log << "Frost Mean(Maximum) for day " << (last_night?2:1) << ": " << frost << endl;
	log << "SevereFrost Mean(Maximum) for day " << (last_night?2:1) << ": " << severefrost << endl;

/*
* Tekstuaalisen esityksen teko
*
Anna-Kaisalta (wikissä):
<<
Hallan todennäköisyys 10-20% -> sanonta "hallanaroilla alueilla hallanvaara" tai "hallanaroilla alueilla mahdollisesti hallaa" tai "alavilla mailla hallanvaara"
Hallan todennäköisyys 30-40% -> sanonta "Yön alin lämpötila on ... ja hallanvaara" tai "mahdollisesti hallaa"
Hallan todennäköisyys 50-60% -> sanonta "Lämpötila on yöllä ... ja paikoin hallaa tai selkeillä alueilla hallaa"
Hallan todennäköisyys 70-80% -> sanonta ""...monin paikoin hallaa"
Hallan todennäköisyys 90-100% -> sanonta "yleisesti hallaa"
<<
*/
	int prob_frost = to_precision( frost.value(), precision );                 // 0|10|20|...|90|100
	int prob_severe_frost = to_precision( severefrost.value(), precision );    // -''-

    Sentence sentence;

    int frost_low_limit= require_int( "frost::lowlimit" );
    int severe_frost_low_limit= require_int( "severefrost::lowlimit" );

    if ((prob_frost<=5) || (prob_frost < frost_low_limit)) {
        // Say nothing, we're below the low limit

    } else {
        sentence << "hallan todennäköisyys" << Integer(prob_frost) << "%";
    }

    if ((prob_severe_frost<=5) || (prob_severe_frost < severe_frost_low_limit)) {
        // Say nothing, no severe frost

    } else {
        sentence << "ankaran hallan todennäköisyys" << Integer(severe_frost_low_limit) << "%";
    }

    paragraph << sentence;

	log << paragraph;
	return paragraph;
}


} // namespace TextGen
  
// ======================================================================
  
