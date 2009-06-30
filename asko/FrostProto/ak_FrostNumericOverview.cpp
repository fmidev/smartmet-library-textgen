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


namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on one night frost
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  
  Paragraph AK_FrostStory::overview_numeric() const {
	MessageLogger log("AK_FrostStory::frost_overview_numeric");

	Paragraph paragraph;

	if (!is_frost_season()) {
		log << "Frost season is not on";
		return paragraph;
	  }

    // TBD: Voisiko tämän tehdä jotenkin automaattisemmin?  'NFmiTime':lla on 'isNight'-metodi; mitä se käyttää?
    //
#ifdef NIGHT_START_HOUR
	const int starthour    = optional_hour( NIGHT_START_HOUR, 18 );
	const int endhour      = optional_hour( NIGHT_END_HOUR, 8 );
#endif

	const int precision = require_percentage( PRECISION );

    // Montako yötä periodissa on? (0/1/2)
    //
	const int nights = WeatherAnalysis::WeatherPeriodTools::countPeriods( itsPeriod,
														 starthour,
														 endhour,
														 /*max*/starthour,
														 /*min*/endhour );
	if (nights==0) {
		//log << paragraph;     // (ei siinä vielä mitään ole)
		return paragraph;
    }

    // Kumpi öistä on tarkoitus ottaa?
    //
    const bool last_night= Settings::optional_bool( LASTNIGHT, false );

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
														  /*max*/starthour,
														  /*min*/endhour );

	WeatherAnalysis::WeatherResult frost = forecaster.analyze( last_night ? DAY2_MEAN:DAY1_MEAN,
											  itsSources,
											  WeatherAnalysis::Frost,
											  WeatherAnalysis::Mean,     // TBD: onko tämä oikein?
											  WeatherAnalysis::Maximum,  // -''-
											  itsArea,
											  some );

	WeatherAnalysis::WeatherResult severefrost = forecaster.analyze( last_night ? DAY2_SEVERE_MEAN:DAY1_SEVERE_MEAN,
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

    int frost_low_limit= require_int( FROST_LOW_LIMIT );

    if ((prob_frost<=5) || (prob_frost < frost_low_limit)) {
        // Say nothing, we're below the low limit

    } else if (prob_severe_frost >= prob_frost-10) {
        // Enough severe frost to only report it
        //
        sentence << "ankaran hallan todennäköisyys" << Integer(prob_severe_frost) << "%";

    } else {
        sentence << "hallan todennäköisyys" << Integer(prob_frost) << "%";

        if (prob_severe_frost>5) {
            // Mention that some of the frost is severe

            sentence << "ankaran hallan todennäköisyys" << Integer(prob_severe_frost) << "%";
        }
    }

    paragraph << sentence;

	log << paragraph;
	return paragraph;
}


} // namespace TextGen
  
// ======================================================================
  
