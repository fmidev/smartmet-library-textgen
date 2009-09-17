/*
* Test for 'FrostStory' stories:
*       frost_text_overview
*       frost_numeric_overview
*
* Author:
*       AKa 26-Jun-2009
*/

#include "FrostStory.h"

// These are from 'textgen' includes
//
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "PlainTextFormatter.h"
#include "Story.h"

#include <newbase/NFmiSettings.h>

#include <regression/tframe.h>

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;

static TextGen::PlainTextFormatter FI, SV, EN;

/*
*/  
static void require( const TextGen::Story &story,
			  const TextGen::PlainTextFormatter &lang,
			  const string &story_name,
			  const string &expected ) {

	const string value= story.makeStory(story_name).realize(lang);

	if (value != expected) {
		TEST_FAILED( value + " != " + expected );
    }
}

/*
* Set the libaries to think these are the frost results.
*
* NOTE: THIS WAY OF TESTING THE SYSTEM IS _FUNDAMENTALLY_FLAWED_. One should
*       use actual data, certain times and scenarios to run tests, not telling
*       "don't look at the data, here are the results". This only tests the
*       latter part of textgen, NOT the whole tube.     --AKa 30-Jun-2009
*/
static void fake_results( const char *normal, const char *severe ) {
    if (normal) {
        NFmiSettings::Set( NIGHT1_MEAN, normal );
        NFmiSettings::Set( NIGHT2_MEAN, normal );
    } else {
        NFmiSettings::Set( NIGHT1_SEVERE_MEAN, severe );
        NFmiSettings::Set( NIGHT2_SEVERE_MEAN, severe );
    }
}


  // ----------------------------------------------------------------------
  /*!
   * \brief Test for frost_text_overview()
   */
  // ----------------------------------------------------------------------

static void overview_text() {
	WeatherAnalysis::AnalysisSources sources;
	WeatherAnalysis::WeatherArea area("25,60");
	NFmiTime t1(2000,1,1);
	NFmiTime t2(2000,1,2);
	WeatherAnalysis::WeatherPeriod period( t1, t2 );
	TextGen::FrostStory story( t1, sources, area, period );

	const string id = STORY_OVERVIEW_TEXT;

    fake_results( "0,0", "0,0" );
	require( story, FI, id, "" );
	require( story, SV, id, "" );
	require( story, EN, id, "" );

    fake_results( "10,0", "0,0" );
	require( story, FI, id, "" );
	require( story, SV, id, "" );
	require( story, EN, id, "" );

    //--
    // Halla 20|30%
    //
    // Kaisa wikissä:
    // <<
    // Hallan todennäköisyys 10-20% -> sanonta "hallanaroilla alueilla hallanvaara" tai "hallanaroilla alueilla mahdollisesti hallaa" tai "alavilla mailla hallanvaara"
    // <<
    //
    fake_results( "20,0", "0,0" );
	require( story, FI, id, "Hallanaroilla alueilla hallanvaara.");
	require( story, SV, id, "...TBD...");
	require( story, EN, id, "...TBD...");

    fake_results( NULL, "10,0" );
	require( story, FI, id, "Hallanaroilla alueilla hallanvaara. Paikoin ankaraa hallaa.");
	require( story, SV, id, "...TBD...");
	require( story, EN, id, "...TBD...");

    //--
    // Halla 30|40%
    //
    // Kaisa wikissä:
    // <<
    // Hallan todennäköisyys 30-40% -> sanonta "Yön alin lämpötila on ... ja hallanvaara" tai "mahdollisesti hallaa"
    // <<
    //
    fake_results( "30,0", "0,0" );
    require( story, FI, id, "Mahdollisesti hallaa." );
    // TBD: SV, EN

    fake_results( NULL, "10,0" );
    require( story, FI, id, "Mahdollisesti hallaa, joka paikoin voi olla ankaraa." );
    // TBD: SV, EN

    //--
    // Halla 50|60%
    //
    // Kaisa wikissä:
    // <<
    // Hallan todennäköisyys 50-60% -> sanonta "Lämpötila on yöllä ... ja paikoin hallaa tai selkeillä alueilla hallaa"
    // <<
    //
    fake_results( "50,0", "0,0" );
    require( story, FI, id, "Paikoin hallaa tai selkeill\xe4 alueilla hallaa." );
    // TBD: SV, EN

    fake_results( NULL, "10,0" );
    require( story, FI, id, "Paikoin hallaa tai selkeillä alueilla hallaa, joka voi olla ankaraa." );
    // TBD: SV, EN

    //--
    // Halla 70|80%
    //
    // Kaisa wikissä:
    // <<
    // "...monin paikoin hallaa"
    // <<
    //
    fake_results( "70,0", "0,0" );
    require( story, FI, id, "Monin paikoin hallaa." );
    // TBD: SV, EN

    fake_results( NULL, "10,0" );
    require( story, FI, id, "Monin paikoin hallaa, joka voi olla ankaraa." );
    // TBD: SV, EN

    //--
    // Halla 90|100%
    //
    // Kaisa wikissä:
    // <<
    // "...yleisesti hallaa"
    // <<
    //
    fake_results( "90,0", "0,0" );
    require( story, FI, id, "Yleisesti hallaa." );
    // TBD: SV, EN

    fake_results( NULL, "10,0" );
    require( story, FI, id, "Yleisesti hallaa, joka voi olla ankaraa." );
    // TBD: SV, EN
    
	TEST_PASSED();
}


  // ----------------------------------------------------------------------
  /*!
   * \brief Test for overview_numeric()
   */
  // ----------------------------------------------------------------------

static void overview_numeric() {
	WeatherAnalysis::AnalysisSources sources;
	WeatherAnalysis::WeatherArea area("25,60");
	NFmiTime t1(2000,1,1);
	NFmiTime t2(2000,1,2);
	WeatherAnalysis::WeatherPeriod period( t1, t2 );
	TextGen::FrostStory story( t1, sources, area, period );

	const string id = STORY_OVERVIEW_NUMERIC;

    fake_results( "0,0", "0,0" );
	require( story, FI, id, "" );
	require( story, SV, id, "" );
	require( story, EN, id, "" );

    fake_results( "30,0", "0,0" );
	require( story, FI, id, "..." );   // TBD
	require( story, SV, id, "..." );
	require( story, EN, id, "..." );

    fake_results( "30,0", "10,0" );
	require( story, FI, id, "..." );   // TBD
	require( story, SV, id, "..." );
	require( story, EN, id, "..." );

    TEST_PASSED();
}


/*
* Integration to 'tframe'
*/
class tests : public tframe::tests {

	//! Overridden message separator
	//
	// Note: 'tframe.h' needing the second 'const' in the return value is PLAIN STUPID.
	//       It essentially is like 'const bool', meaning the returned pointer (not the
	//       contents) is never ever to be reused for anything else. Totally unneeded
	//       (but we have to dance accordingly, to match its virtual function signature).
	//         --AKa 26-Jun-2009
	//
	//       ps. "Mikael Kilpeläinen" of 2002 seems to be behind the tframe.
	//
    virtual const char * const /*<-unneeded!*/ error_message_prefix() const {
        return "\n\t";
	}

	//! Main test suite
	//
	void test() {
	  TEST( overview_text );
	  TEST( overview_numeric );
	}
};


/*
*/
static shared_ptr<TextGen::Dictionary> dict( const char *lang ) {
    shared_ptr<TextGen::Dictionary> d= TextGen::DictionaryFactory::create("multimysql");
    d->init(lang);
    return d;
}


/*
*/
int main(void)
{
    // This SHOULD come from '/smartmet/cnf/fmi.conf' automatically, but for some reason
    // does not seem to.
    //
#if 1
    NFmiSettings::Set( "textgen::host", "base.weatherproof.fi" );
    NFmiSettings::Set( "textgen::user", "textgen" );
    NFmiSettings::Set( "textgen::passwd", "w1w2w3" );   // TBD: NOT GOOD TO HAVE PW HERE
    NFmiSettings::Set( "textgen::database", "textgen" );
    
    NFmiSettings::Set( "textgen::filedictionaries", "/smartmet/share/textgendata/dictionaries" );
    NFmiSettings::Set( "textgen::mappath", "/smartmet/share/textgendata/maps" );
    NFmiSettings::Set( "textgen::coordinates", "/smartmet/share/coordinates/kaikki.txt" );
#endif
    
	NFmiSettings::Set( SEASON_START, "200004010000" );    // year does not count
	NFmiSettings::Set( SEASON_END,   "200009300000" );    // year does not count

	NFmiSettings::Set( PRECISION, "10" );

    cout << endl
	   << "FrostStory tests" << endl
	   << "================" << endl;

    // NOTE: SQL configuration needs to be in place in '/smartmet/cnf/fmi.conf'

	FI.dictionary( dict("fi") );
	SV.dictionary( dict("sv") );
	EN.dictionary( dict("en") );

    NFmiSettings::Set("textgen::frostseason","true");

    tests t;
    return t.run();
}
