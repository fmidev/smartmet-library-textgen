// ======================================================================
/*!
 * \file FrostTextOverview.cpp
 * \brief Textgen story for one night (following night, or the night after that)
 *
 * Reference: <http://wiki.weatherproof.fi/index.php?title=Textgen:_frost_overview>
 */
// ======================================================================

#include "FrostStoryAk.h"

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

using MathTools::to_precision;


/*
*/
static string halla_teksti( unsigned prob, bool is_severe ) {

    const char *texts[6][2]= {
        // 0..5%
        { "", "" },     // ei mainintaa
        // 6..25% (10,20)
        { "hallanaroilla alueilla mahdollisesti hallaa",
          "hallanaroilla alueilla mahdollisesti ankaraa hallaa" },
        // 26..45% (30,40)
        { "mahdollisesti hallaa",
          "mahdollisesti ankaraa hallaa" },
        // 46..65% (50,60)
        { "paikoittain hallaa tai selkeill\xe4 alueilla hallaa",
          "paikoittain ankaraa hallaa" },
        // 66..85% (70,80)
        { "monin paikoin hallaa",
          "monin paikoin ankaraa hallaa" },
        // 86..100% (90,100)
        { "yleisesti hallaa",
          "yleisesti ankaraa hallaa" }
    };

    int index= ((prob+14)/20);
    assert( index<=5 );

    return texts[index][ is_severe ? 1:0 ];
}


namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on one night frost
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  
  Paragraph FrostStoryAk::overview_text( const FrostStoryAk &me ) {
	MessageLogger log( FROST_OVERVIEW STORY_OVERVIEW_TEXT );

	Paragraph paragraph;

	if (!me.is_frost_season( NFmiTime() )) {
		log << "Not frost season.";
		return paragraph;
	  }

	const int starthour    = optional_hour( me.PREFIX, NIGHT_START_HOUR, 18 );
	const int endhour      = optional_hour( me.PREFIX, NIGHT_END_HOUR, 8 );

	const int precision = require_percentage( me.PREFIX, PRECISION );

    // Montako yötä periodissa on? (0/1/2)
    //
	const unsigned nights = WeatherAnalysis::WeatherPeriodTools::countPeriods( me.itsPeriod,
														 starthour,
														 endhour,
														 /*max*/starthour,
														 /*min*/endhour );
	if (nights==0) {
	   log << "Period has no nights.";
		return paragraph;
    }

    // Kumpi öistä on tarkoitus ottaa?
    //
    const unsigned night_index= Settings::optional_bool( SECOND_NIGHT, false ) ? 2:1;

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
							WeatherAnalysis::Mean,       // area function
							WeatherAnalysis::Maximum,    // time function
							me.itsArea,
							night_period );

	WeatherAnalysis::WeatherResult severe_frost = 
	   forecaster.analyze( (night_index==2) ? NIGHT2_SEVERE_MEAN : NIGHT1_SEVERE_MEAN,
							me.itsSources,
							WeatherAnalysis::SevereFrost,
							WeatherAnalysis::Mean,       // area function
							WeatherAnalysis::Maximum,    // time function
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
	unsigned prob_frost = to_precision( frost.value(), precision );                 // 0|10|20|...|90|100
	unsigned prob_severe_frost = to_precision( severe_frost.value(), precision );    // -''-

    assert( prob_severe_frost <= prob_frost );      // ankaran osuus on mukana tavallisessa hallassa

    Sentence sentence;

    unsigned frost_low_limit= require_int( me.PREFIX, FROST_LOW_LIMIT );
    unsigned severe_low_limit= require_int( me.PREFIX, SEVERE_LOW_LIMIT );

    if ((prob_frost<=5) || (prob_frost < frost_low_limit)) {
        // Say nothing, we're below the low limit

    } else if ((prob_severe_frost >= prob_frost-10) && (prob_severe_frost >= severe_low_limit)) {
        // Enough severe frost to only report it
        //
        sentence << halla_teksti( prob_severe_frost, true );        

    } else {
        sentence << halla_teksti( prob_frost, false );
        
        if (prob_severe_frost > severe_low_limit) {
            sentence << "joka paikoin on ankaraa";
        }
    }

    paragraph << sentence;

	log << paragraph;
	return paragraph;
}


} // namespace TextGen
  
// ======================================================================
  
