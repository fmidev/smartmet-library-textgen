// ======================================================================
/*!
 * \file FrostNumericOverview.cpp
 * \brief Textgen story for one night (following night, or the night after that)
 *
 * Reference: <http://wiki.weatherproof.fi/index.php?title=Textgen:_frost_overview>
 */
// ======================================================================

#include "FrostStory.h"

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

#include <newbase/NFmiMetTime.h>

using namespace std;

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
  
  Paragraph FrostStory::overview_numeric( const FrostStory &me ) {
	MessageLogger log( FROST_STORY_OVERVIEW_NUMERIC );

	Paragraph paragraph;

	if (!is_frost_season( NFmiMetTime() )) {   // right now
		log << "Not frost season.";
		return paragraph;
	  }

    // Yön pituudet on määritelty storyn konffassa (voisihan ne toki laskeakin jostain,
    // ehkä tässä ei kuitenkaan ole tarkkuudella juuri väliä?)
    //
	const int starthour = optional_hour( NIGHT_START_HOUR, 18 );
	const int endhour   = optional_hour( NIGHT_END_HOUR, 8 );

	const int precision = require_percentage( PRECISION );

    // Montako yötä periodissa on? (0/1/2)
    //
	unsigned nights = WeatherAnalysis::WeatherPeriodTools::countPeriods( me.itsPeriod,
														 starthour,
														 endhour,
														 /*max*/starthour,
														 /*min*/endhour );
	if (nights==0) {
		log << "No nights in the period.";
		return paragraph;
    }

    // Kumpi öistä on tarkoitus ottaa?
    //
    unsigned night_index= Settings::optional_bool( SECOND_NIGHT, false ) ? 2:1;

    if (nights<night_index) {
        throw TextGenError( "Asking for second night but period only has one." );
    }

	// Calculate frost probability

	WeatherAnalysis::GridForecaster forecaster;

	WeatherAnalysis::WeatherPeriod night_period = 
	   WeatherAnalysis::WeatherPeriodTools::getPeriod( me.itsPeriod,
													  night_index,
													  starthour,
													  endhour,
													  /*max*/starthour,
													  /*min*/endhour );

	WeatherAnalysis::WeatherResult frost = 
	   forecaster.analyze( (night_index==2) ? NIGHT2_MEAN : NIGHT1_MEAN,
				           me.itsSources,
						   WeatherAnalysis::Frost,
				           WeatherAnalysis::Mean,
				           WeatherAnalysis::Maximum,
				           me.itsArea,
						   night_period );

	WeatherAnalysis::WeatherResult severe_frost = 
	   forecaster.analyze( (night_index==2) ? NIGHT2_SEVERE_MEAN : NIGHT1_SEVERE_MEAN,
				           me.itsSources,
							WeatherAnalysis::SevereFrost,
							WeatherAnalysis::Mean,
							WeatherAnalysis::Maximum,
				            me.itsArea,
							night_period );
	
	if (frost.value()==kFloatMissing || severe_frost.value()==kFloatMissing) {
        throw TextGenError("Frost is not available");
    }

	log << "Frost Mean(Maximum) for night " << night_index << ": " << frost << endl;
	log << "SevereFrost Mean(Maximum) for night " << night_index << ": " << severe_frost << endl;

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
	int prob_severe_frost = to_precision( severe_frost.value(), precision );    // -''-

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
  
