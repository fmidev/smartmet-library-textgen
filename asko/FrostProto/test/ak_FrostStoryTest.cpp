/*
* Test for 'AK_FrostStory' stories:
*       frost_text_overview
*       frost_numeric_overview
*
* Author:
*       AKa 26-Jun-2009
*/

#include "ak_FrostStory.h"

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

#define NEW_SPECS

static TextGen::PlainTextFormatter FI, SV, EN;

/*
*/  
static void require( const TextGen::Story & story,
			  const TextGen::PlainTextFormatter &lang,
			  const string & story_name,
			  const string & expected ) {

	const string value = story.makeStory( story_name ).realize(lang);

	if (value != expected) {
		TEST_FAILED( value + " != " + expected );
    }
}

  // ----------------------------------------------------------------------
  /*!
   * \brief Test for frost_text_overview()
   */
  // ----------------------------------------------------------------------

static void frost_text_overview() {
	WeatherAnalysis::AnalysisSources sources;
	WeatherAnalysis::WeatherArea area("25,60");
	NFmiTime t1(2000,1,1);
	NFmiTime t2(2000,1,2);
	WeatherAnalysis::WeatherPeriod period( t1, t2 );
	TextGen::AK_FrostStory story( t1, sources, area, period );

	const string id = "frost_text_overview";

	NFmiSettings::Set( "precision", "10" );
	NFmiSettings::Set( "frost_limit", "20" );
	NFmiSettings::Set( "severe_frost_limit", "10" );

	NFmiSettings::Set( "fake::mean", "0,0" );
	NFmiSettings::Set( "fake::severe_mean", "0,0" );
	require( story, FI, id, "" );
	require( story, SV, id, "" );
	require( story, EN, id, "" );

	NFmiSettings::Set( "fake::mean", "10,0" );
	NFmiSettings::Set( "fake::severe_mean", "0,0" );
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
	NFmiSettings::Set( "fake::mean", "20,0" );
	NFmiSettings::Set( "fake::severe_mean", "0,0" );
#ifdef NEW_SPECS
	require( story, FI, id, "Hallanaroilla alueilla hallanvaara.");
	require( story, SV, id, "...TBD...");
	require( story, EN, id, "...TBD...");
#else
	require( story, FI, id, "Hallan todenn\xe4k\xf6isyys on 20%.");
	require( story, SV, id, "Sannolikheten f\xf6r nattfrost \xe4r 20%.");
	require( story, EN, id, "Probability of frost is 20%.");
#endif

	NFmiSettings::Set( "fake::severe_mean", "10,0" );
#ifdef NEW_SPECS
	require( story, FI, id, "Hallanaroilla alueilla hallanvaara. Paikoin ankaraa hallaa.");
	require( story, SV, id, "...TBD...");
	require( story, EN, id, "...TBD...");
#else
	require( story, FI, id, "Ankaran hallan todenn\xe4k\xf6isyys on 10%." );
	require( story, SV, id, "Sannolikheten f\xf6r str\xe4ng nattfrost \xe4r 10%." );
	require( story, EN, id, "Probability of severe frost is 10%." );
#endif

    //--
    // Halla 30|40%
    //
    // Kaisa wikissä:
    // <<
    // Hallan todennäköisyys 30-40% -> sanonta "Yön alin lämpötila on ... ja hallanvaara" tai "mahdollisesti hallaa"
    // <<
    //
	NFmiSettings::Set( "fake::mean", "30,0" );
	NFmiSettings::Set( "fake::severe_mean", "0,0" );
#ifdef NEW_SPECS
    require( story, FI, id, "Mahdollisesti hallaa." );
    // TBD: SV, EN
#endif

	NFmiSettings::Set( "fake::severe_mean", "10,0" );
#ifdef NEW_SPECS
    require( story, FI, id, "Mahdollisesti hallaa, joka paikoin voi olla ankaraa." );
    // TBD: SV, EN
#endif

    //--
    // Halla 50|60%
    //
    // Kaisa wikissä:
    // <<
    // Hallan todennäköisyys 50-60% -> sanonta "Lämpötila on yöllä ... ja paikoin hallaa tai selkeillä alueilla hallaa"
    // <<
    //
	NFmiSettings::Set( "fake::mean", "50,0" );
	NFmiSettings::Set( "fake::severe_mean", "0,0" );
#ifdef NEW_SPECS
    require( story, FI, id, "Paikoin hallaa tai selkeill\xe4 alueilla hallaa." );
    // TBD: SV, EN
#endif
	NFmiSettings::Set( "fake::severe_mean", "10,0" );
#ifdef NEW_SPECS
    require( story, FI, id, "Paikoin hallaa tai selkeillä alueilla hallaa, joka voi olla ankaraa." );
    // TBD: SV, EN
#endif

    //--
    // Halla 70|80%
    //
    // Kaisa wikissä:
    // <<
    // "...monin paikoin hallaa"
    // <<
    //
	NFmiSettings::Set( "fake::mean", "70,0" );
	NFmiSettings::Set( "fake::severe_mean", "0,0" );
#ifdef NEW_SPECS
    require( story, FI, id, "Monin paikoin hallaa." );
    // TBD: SV, EN
#endif
	NFmiSettings::Set( "fake::severe_mean", "10,0" );
#ifdef NEW_SPECS
    require( story, FI, id, "Monin paikoin hallaa, joka voi olla ankaraa." );
    // TBD: SV, EN
#endif

    //--
    // Halla 90|100%
    //
    // Kaisa wikissä:
    // <<
    // "...yleisesti hallaa"
    // <<
    //
	NFmiSettings::Set( "fake::mean", "90,0" );
	NFmiSettings::Set( "fake::severe_mean", "0,0" );
#ifdef NEW_SPECS
    require( story, FI, id, "Yleisesti hallaa." );
    // TBD: SV, EN
#endif
	NFmiSettings::Set( "fake::severe_mean", "10,0" );
#ifdef NEW_SPECS
    require( story, FI, id, "Yleisesti hallaa, joka voi olla ankaraa." );
    // TBD: SV, EN
#endif
    
	TEST_PASSED();
}


  // ----------------------------------------------------------------------
  /*!
   * \brief Test for frost_numeric_overview()
   */
  // ----------------------------------------------------------------------

static void frost_numeric_overview() {
	WeatherAnalysis::AnalysisSources sources;
	WeatherAnalysis::WeatherArea area("25,60");
	NFmiTime t1(2000,1,1);
	NFmiTime t2(2000,1,2);
	WeatherAnalysis::WeatherPeriod period( t1, t2 );
	TextGen::AK_FrostStory story( t1, sources, area, period );

	const string id = "frost_numeric_overview";

	NFmiSettings::Set( "precision", "10" );
	NFmiSettings::Set( "frost_limit", "20" );
	NFmiSettings::Set( "severe_frost_limit", "10" );

	NFmiSettings::Set( "fake::mean", "0,0" );
	NFmiSettings::Set( "fake::severe_mean", "0,0" );
	require( story, FI, id, "" );
	require( story, SV, id, "" );
	require( story, EN, id, "" );

	NFmiSettings::Set( "fake::mean", "30,0" );
	NFmiSettings::Set( "fake::severe_mean", "0,0" );
	require( story, FI, id, "..." );   // TBD
	require( story, SV, id, "..." );
	require( story, EN, id, "..." );

	NFmiSettings::Set( "fake::mean", "30,0" );
	NFmiSettings::Set( "fake::severe_mean", "10,0" );
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
	  TEST( frost_text_overview );
	  TEST( frost_numeric_overview );
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
    cout << endl
	   << "AK_FrostStory tests" << endl
	   << "===================" << endl;

    // NB: SQL configuration needs to be in place in '/smartmet/cnf/fmi.conf'

	FI.dictionary( dict("fi") );
	SV.dictionary( dict("sv") );
	EN.dictionary( dict("en") );

    NFmiSettings::Set("textgen::ak_frostseason","true");

    tests t;
    return t.run();
}
