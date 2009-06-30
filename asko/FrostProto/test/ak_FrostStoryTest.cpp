/*
* Test for 'AK_FrostStory'
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
void require( const TextGen::Story & story,
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
   * \brief Test AK_FrostStory::mean()
   */
  // ----------------------------------------------------------------------

void frost_mean() {
	WeatherAnalysis::AnalysisSources sources;
	WeatherAnalysis::WeatherArea area("25,60");
	NFmiTime t1(2000,1,1);
	NFmiTime t2(2000,1,2);
	WeatherAnalysis::WeatherPeriod period( t1, t2 );
	TextGen::AK_FrostStory story( t1, sources, area, period, "mean" );

	const string id = "ak_frost_mean";

	NFmiSettings::Set( "mean::precision", "10" );
	NFmiSettings::Set( "mean::frost_limit", "20" );
	NFmiSettings::Set( "mean::severe_frost_limit", "10" );

	NFmiSettings::Set( "mean::fake::mean", "0,0" );
	NFmiSettings::Set( "mean::fake::severe_mean", "0,0" );
	require( story, FI, id, "" );
	require( story, SV, id, "" );
	require( story, EN, id, "" );

	NFmiSettings::Set( "mean::fake::mean", "10,0" );
	NFmiSettings::Set( "mean::fake::severe_mean", "0,0" );
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
	NFmiSettings::Set( "mean::fake::mean", "20,0" );
	NFmiSettings::Set( "mean::fake::severe_mean", "0,0" );
#ifdef NEW_SPECS
	require( story, FI, id, "Hallanaroilla alueilla hallanvaara.");
	require( story, SV, id, "...TBD...");
	require( story, EN, id, "...TBD...");
#else
	require( story, FI, id, "Hallan todenn\xe4k\xf6isyys on 20%.");
	require( story, SV, id, "Sannolikheten f\xf6r nattfrost \xe4r 20%.");
	require( story, EN, id, "Probability of frost is 20%.");
#endif

	NFmiSettings::Set( "mean::fake::severe_mean", "10,0" );
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
	NFmiSettings::Set( "mean::fake::mean", "30,0" );
	NFmiSettings::Set( "mean::fake::severe_mean", "0,0" );
#ifdef NEW_SPECS
    require( story, FI, id, "Mahdollisesti hallaa." );
    // TBD: SV, EN
#endif

	NFmiSettings::Set( "mean::fake::severe_mean", "10,0" );
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
	NFmiSettings::Set( "mean::fake::mean", "50,0" );
	NFmiSettings::Set( "mean::fake::severe_mean", "0,0" );
#ifdef NEW_SPECS
    require( story, FI, id, "Paikoin hallaa tai selkeill\xe4 alueilla hallaa." );
    // TBD: SV, EN
#endif
	NFmiSettings::Set( "mean::fake::severe_mean", "10,0" );
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
	NFmiSettings::Set( "mean::fake::mean", "70,0" );
	NFmiSettings::Set( "mean::fake::severe_mean", "0,0" );
#ifdef NEW_SPECS
    require( story, FI, id, "Monin paikoin hallaa." );
    // TBD: SV, EN
#endif
	NFmiSettings::Set( "mean::fake::severe_mean", "10,0" );
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
	NFmiSettings::Set( "mean::fake::mean", "90,0" );
	NFmiSettings::Set( "mean::fake::severe_mean", "0,0" );
#ifdef NEW_SPECS
    require( story, FI, id, "Yleisesti hallaa." );
    // TBD: SV, EN
#endif
	NFmiSettings::Set( "mean::fake::severe_mean", "10,0" );
#ifdef NEW_SPECS
    require( story, FI, id, "Yleisesti hallaa, joka voi olla ankaraa." );
    // TBD: SV, EN
#endif
    
	TEST_PASSED();
}

  // ----------------------------------------------------------------------
  /*!
   * \brief Test AK_FrostStory::maximum()
   */
  // ----------------------------------------------------------------------

void frost_maximum() {
	WeatherAnalysis::AnalysisSources sources;
	WeatherAnalysis::WeatherArea area("25,60");
	NFmiTime t1(2000,1,1);
	NFmiTime t2(2000,1,2);
	WeatherAnalysis::WeatherPeriod period( t1, t2 );
	TextGen::AK_FrostStory story( t1, sources, area, period, "maximum" );

	const string id = "ak_frost_maximum";

	NFmiSettings::Set( "maximum::precision", "10" );
	NFmiSettings::Set( "maximum::frost_limit", "20" );
	NFmiSettings::Set( "maximum::severe_frost_limit", "10" );

	NFmiSettings::Set( "maximum::fake::maximum", "0,0" );
	NFmiSettings::Set( "maximum::fake::severe_maximum", "0,0" );
	require( story, FI, id, "" );
	require( story, SV, id, "" );
	require( story, EN, id, "" );

	NFmiSettings::Set( "maximum::fake::maximum", "10,0" );
	NFmiSettings::Set( "maximum::fake::severe_maximum", "0,0" );
	require( story, FI, id, "" );
	require( story, SV, id, "" );
	require( story, EN, id, "" );

	NFmiSettings::Set( "maximum::fake::maximum", "20,0" );
	NFmiSettings::Set( "maximum::fake::severe_maximum", "0,0" );
	require( story, FI, id, "Hallan todenn\xe4k\xf6isyys on 20%.");
	require( story, SV, id, "Sannolikheten f\xf6r nattfrost \xe4r 20%.");
	require( story, EN, id, "Probability of frost is 20%.");

	NFmiSettings::Set("maximum::fake::maximum","20,0");
	NFmiSettings::Set("maximum::fake::severe_maximum","10,0");
	require( story, FI, id, "Ankaran hallan todenn\xe4k\xf6isyys on 10%.");
	require( story, SV, id, "Sannolikheten f\xf6r str\xe4ng nattfrost \xe4r 10%.");
	require( story, EN, id, "Probability of severe frost is 10%.");

	TEST_PASSED();
}

  // ----------------------------------------------------------------------
  /*!
   * \brief Test AK_FrostStory::range()
   */
  // ----------------------------------------------------------------------

void frost_range() {

	WeatherAnalysis::AnalysisSources sources;
	WeatherAnalysis::WeatherArea area("25,60");
	NFmiTime t1(2000,1,1);
	NFmiTime t2(2000,1,2);
	WeatherAnalysis::WeatherPeriod period( t1, t2 );
	TextGen::AK_FrostStory story( t1, sources, area, period, "range" );

	const string id = "ak_frost_range";

	NFmiSettings::Set("range::precision","10");
	NFmiSettings::Set("range::frost_limit","50");
	NFmiSettings::Set("range::severe_frost_limit","30");

	NFmiSettings::Set("range::fake::minimum","0,0");
	NFmiSettings::Set("range::fake::maximum","0,0");
	NFmiSettings::Set("range::fake::severe_minimum","0,0");
	NFmiSettings::Set("range::fake::severe_maximum","0,0");
	require(story,FI,id,"");
	require(story,SV,id,"");
	require(story,EN,id,"");

	NFmiSettings::Set("range::fake::minimum","50,0");
	NFmiSettings::Set("range::fake::maximum","50,0");
	NFmiSettings::Set("range::fake::severe_minimum","0,0");
	NFmiSettings::Set("range::fake::severe_maximum","0,0");
	require(story,FI,id,"Hallan todenn\xe4k\xf6isyys on 50%.");
	require(story,SV,id,"Sannolikheten f\xf6r nattfrost \xe4r 50%.");
	require(story,EN,id,"Probability of frost is 50%.");

	NFmiSettings::Set("range::fake::minimum","40,0");
	NFmiSettings::Set("range::fake::maximum","60,0");
	NFmiSettings::Set("range::fake::severe_minimum","0,0");
	NFmiSettings::Set("range::fake::severe_maximum","0,0");
	require(story,FI,id,"Hallan todenn\xe4k\xf6isyys on 40-60%.");
	require(story,SV,id,"Sannolikheten f\xf6r nattfrost \xe4r 40-60%.");
	require(story,EN,id,"Probability of frost is 40-60%.");

	NFmiSettings::Set("range::fake::minimum","50,0");
	NFmiSettings::Set("range::fake::maximum","70,0");
	NFmiSettings::Set("range::fake::severe_minimum","10,0");
	NFmiSettings::Set("range::fake::severe_maximum","20,0");
	require(story,FI,id,"Hallan todenn\xe4k\xf6isyys on 50-70%.");
	require(story,SV,id,"Sannolikheten f\xf6r nattfrost \xe4r 50-70%.");
	require(story,EN,id,"Probability of frost is 50-70%.");

	NFmiSettings::Set("range::fake::minimum","40,0");
	NFmiSettings::Set("range::fake::maximum","60,0");
	NFmiSettings::Set("range::fake::severe_minimum","30,0");
	NFmiSettings::Set("range::fake::severe_maximum","40,0");
	require(story,FI,id,"Ankaran hallan todenn\xe4k\xf6isyys on 30-40%.");
	require(story,SV,id,"Sannolikheten f\xf6r str\xe4ng nattfrost \xe4r 30-40%.");
	require(story,EN,id,"Probability of severe frost is 30-40%.");

	TEST_PASSED();
}

  // ----------------------------------------------------------------------
  /*!
   * \brief Test AK_FrostStory::twonights()
   */
  // ----------------------------------------------------------------------

void frost_twonights() {

	WeatherAnalysis::AnalysisSources sources;
	WeatherAnalysis::WeatherArea area("25,60");

	const string id = "ak_frost_twonights";

	NFmiSettings::Set("twonights::precision","10");
	NFmiSettings::Set("twonights::frost_limit","20");
	NFmiSettings::Set("twonights::severe_frost_limit","10");

	NFmiSettings::Set("twonights::night::starthour","21");
	NFmiSettings::Set("twonights::night::endhour","09");

	// One night
	{
	  NFmiTime t1(2003,6,3,12,0);
	  NFmiTime t2(2003,6,4,12,0);
	  WeatherAnalysis::WeatherPeriod period( t1, t2 );
	  TextGen::AK_FrostStory story( t1, sources, area, period, "twonights" );

	  NFmiSettings::Set("twonights::fake::day1::mean","10,0");
	  NFmiSettings::Set("twonights::fake::day1::severe_mean","0,0");
	  require(story,FI,id,"");
	  require(story,SV,id,"");
	  require(story,EN,id,"");

	  NFmiSettings::Set("twonights::fake::day1::mean","20,0");
	  NFmiSettings::Set("twonights::fake::day1::severe_mean","0,0");
	  require(story,FI,id,"Hallan todenn\xe4k\xf6isyys on keskiviikon vastaisena y\xf6n\xe4 20%.");
	  require(story,SV,id,"Sannolikheten f\xf6r nattfrost \xe4r natten mot onsdagen 20%.");
	  require(story,EN,id,"Probability of frost is on Wednesday night 20%.");

	  NFmiSettings::Set("twonights::fake::day1::mean","80,0");
	  NFmiSettings::Set("twonights::fake::day1::severe_mean","20,0");
	  require(story,FI,id,"Ankaran hallan todenn\xe4k\xf6isyys on keskiviikon vastaisena y\xf6n\xe4 20%.");
	  require(story,SV,id,"Sannolikheten f\xf6r str\xe4ng nattfrost \xe4r natten mot onsdagen 20%.");
	  require(story,EN,id,"Probability of severe frost is on Wednesday night 20%.");

	}

	// Two nights, nothing on second day
	{
	  NFmiTime t1(2003,6,3,12,0);
	  NFmiTime t2(2003,6,5,12,0);
	  WeatherAnalysis::WeatherPeriod period( t1, t2 );
	  TextGen::AK_FrostStory story( t1, sources, area, period, "twonights" );

	  NFmiSettings::Set("twonights::fake::day1::mean","10,0");
	  NFmiSettings::Set("twonights::fake::day1::severe_mean","0,0");
	  NFmiSettings::Set("twonights::fake::day2::mean","0,0");
	  NFmiSettings::Set("twonights::fake::day2::severe_mean","0,0");
	  require(story,FI,id,"");
	  require(story,SV,id,"");
	  require(story,EN,id,"");

	  NFmiSettings::Set("twonights::fake::day1::mean","30,0");
	  NFmiSettings::Set("twonights::fake::day1::severe_mean","0,0");
	  NFmiSettings::Set("twonights::fake::day2::mean","30,0");
	  NFmiSettings::Set("twonights::fake::day2::severe_mean","0,0");
	  require(story,FI,id,"Hallan todenn\xe4k\xf6isyys on keskiviikon vastaisena y\xf6n\xe4 30%, seuraavana y\xf6n\xe4 sama.");
	  require(story,SV,id,"Sannolikheten f\xf6r nattfrost \xe4r natten mot onsdagen 30%, f\xf6ljande natt densamma.");
	  require(story,EN,id,"Probability of frost is on Wednesday night 30%, the following night the same.");

	  NFmiSettings::Set("twonights::fake::day1::mean","30,0");
	  NFmiSettings::Set("twonights::fake::day1::severe_mean","0,0");
	  NFmiSettings::Set("twonights::fake::day2::mean","40,0");
	  NFmiSettings::Set("twonights::fake::day2::severe_mean","0,0");
	  require(story,FI,id,"Hallan todenn\xe4k\xf6isyys on keskiviikon vastaisena y\xf6n\xe4 30%, seuraavana y\xf6n\xe4 40%.");
	  require(story,SV,id,"Sannolikheten f\xf6r nattfrost \xe4r natten mot onsdagen 30%, f\xf6ljande natt 40%.");
	  require(story,EN,id,"Probability of frost is on Wednesday night 30%, the following night 40%.");

	  NFmiSettings::Set("twonights::fake::day1::mean","20,0");
	  NFmiSettings::Set("twonights::fake::day1::severe_mean","0,0");
	  NFmiSettings::Set("twonights::fake::day2::mean","0,0");
	  NFmiSettings::Set("twonights::fake::day2::severe_mean","0,0");
	  require(story,FI,id,"Hallan todenn\xe4k\xf6isyys on keskiviikon vastaisena y\xf6n\xe4 20%, seuraava y\xf6 on l\xe4mpim\xe4mpi.");
	  require(story,SV,id,"Sannolikheten f\xf6r nattfrost \xe4r natten mot onsdagen 20%, f\xf6ljande natt \xe4r varmare.");
	  require(story,EN,id,"Probability of frost is on Wednesday night 20%, the following night is warmer.");

	  NFmiSettings::Set("twonights::fake::day1::mean","80,0");
	  NFmiSettings::Set("twonights::fake::day1::severe_mean","20,0");
	  NFmiSettings::Set("twonights::fake::day2::mean","0,0");
	  NFmiSettings::Set("twonights::fake::day2::severe_mean","0,0");
	  require(story,FI,id,"Ankaran hallan todenn\xe4k\xf6isyys on keskiviikon vastaisena y\xf6n\xe4 20%, seuraava y\xf6 on huomattavasti l\xe4mpim\xe4mpi.");
	  require(story,SV,id,"Sannolikheten f\xf6r str\xe4ng nattfrost \xe4r natten mot onsdagen 20%, f\xf6ljande natt \xe4r betydligt varmare.");
	  require(story,EN,id,"Probability of severe frost is on Wednesday night 20%, the following night is significantly warmer.");

	}

	// First day frost
	{
	  NFmiTime t1(2003,6,3,12,0);
	  NFmiTime t2(2003,6,5,12,0);
	  WeatherAnalysis::WeatherPeriod period( t1, t2 );
	  TextGen::AK_FrostStory story( t1, sources, area, period, "twonights" );

	  NFmiSettings::Set("twonights::fake::day1::mean","20,0");
	  NFmiSettings::Set("twonights::fake::day1::severe_mean","0,0");

	  NFmiSettings::Set("twonights::fake::day2::mean","0,0");
	  NFmiSettings::Set("twonights::fake::day2::severe_mean","0,0");
	  require(story,FI,id,"Hallan todenn\xe4k\xf6isyys on keskiviikon vastaisena y\xf6n\xe4 20%, seuraava y\xf6 on l\xe4mpim\xe4mpi.");
	  require(story,SV,id,"Sannolikheten f\xf6r nattfrost \xe4r natten mot onsdagen 20%, f\xf6ljande natt \xe4r varmare.");
	  require(story,EN,id,"Probability of frost is on Wednesday night 20%, the following night is warmer.");

	  NFmiSettings::Set("twonights::fake::day2::mean","30,0");
	  NFmiSettings::Set("twonights::fake::day2::severe_mean","00,0");
	  require(story,FI,id,"Hallan todenn\xe4k\xf6isyys on keskiviikon vastaisena y\xf6n\xe4 20%, seuraavana y\xf6n\xe4 30%.");
	  require(story,SV,id,"Sannolikheten f\xf6r nattfrost \xe4r natten mot onsdagen 20%, f\xf6ljande natt 30%.");
	  require(story,EN,id,"Probability of frost is on Wednesday night 20%, the following night 30%.");

	  NFmiSettings::Set("twonights::fake::day2::mean","80,0");
	  NFmiSettings::Set("twonights::fake::day2::severe_mean","20,0");
	  require(story,FI,id,"Hallan todenn\xe4k\xf6isyys on keskiviikon vastaisena y\xf6n\xe4 20%, seuraavana y\xf6n\xe4 ankaran hallan todenn\xe4k\xf6isyys on 20%.");
	  require(story,SV,id,"Sannolikheten f\xf6r nattfrost \xe4r natten mot onsdagen 20%, f\xf6ljande natt sannolikheten f\xf6r str\xe4ng nattfrost \xe4r 20%.");
	  require(story,EN,id,"Probability of frost is on Wednesday night 20%, the following night probability of severe frost is 20%.");

	}

	TEST_PASSED();
}

  // ----------------------------------------------------------------------
  /*!
   * \brief Test AK_FrostStory::day()
   */
  // ----------------------------------------------------------------------

void frost_day() {

	WeatherAnalysis::AnalysisSources sources;
	WeatherAnalysis::WeatherArea area("25,60");
	NFmiTime t1(2000,6,1,18);
	NFmiTime t2(2000,6,2,6);
	WeatherAnalysis::WeatherPeriod period( t1, t2 );
	TextGen::AK_FrostStory story( t1, sources, area, period, "day" );

	const string id = "ak_frost_day";

	NFmiSettings::Set("day::night::starthour","18");
	NFmiSettings::Set("day::night::endhour","6");

	NFmiSettings::Set("day::precision","10");
	NFmiSettings::Set("day::frost_limit","50");
	NFmiSettings::Set("day::severe_frost_limit","30");

	NFmiSettings::Set("day::fake::area::frost","0,0");
	NFmiSettings::Set("day::fake::area::severe_frost","0,0");
	NFmiSettings::Set("day::fake::coast::value","0,0");
	NFmiSettings::Set("day::fake::inland::value","0,0");
	require(story,FI,id,"");
	require(story,SV,id,"");
	require(story,EN,id,"");

	NFmiSettings::Set("day::fake::area::frost","60,0");
	NFmiSettings::Set("day::fake::area::severe_frost","0,0");
	NFmiSettings::Set("day::fake::coast::value","60,0");
	NFmiSettings::Set("day::fake::inland::value","60,0");
	require(story,FI,id,"Hallan todenn\xe4k\xf6isyys on 60%.");
	require(story,SV,id,"Sannolikheten f\xf6r nattfrost \xe4r 60%.");
	require(story,EN,id,"Probability of frost is 60%.");

	NFmiSettings::Set("day::fake::area::frost","60,0");
	NFmiSettings::Set("day::fake::area::severe_frost","40,0");
	NFmiSettings::Set("day::fake::coast::value","40,0");
	NFmiSettings::Set("day::fake::inland::value","40,0");
	require(story,FI,id,"Ankaran hallan todenn\xe4k\xf6isyys on 40%.");
	require(story,SV,id,"Sannolikheten f\xf6r str\xe4ng nattfrost \xe4r 40%.");
	require(story,EN,id,"Probability of severe frost is 40%.");

	NFmiSettings::Set("day::fake::area::frost","60,0");
	NFmiSettings::Set("day::fake::area::severe_frost","40,0");
	NFmiSettings::Set("day::fake::coast::value","10,0");
	NFmiSettings::Set("day::fake::inland::value","60,0");
	require(story,FI,id,"Ankaran hallan todenn\xe4k\xf6isyys on 60%, rannikolla 10%.");
	require(story,SV,id,"Sannolikheten f\xf6r str\xe4ng nattfrost \xe4r 60%, vid kusten 10%.");
	require(story,EN,id,"Probability of severe frost is 60%, on the coastal area 10%.");

	NFmiSettings::Set("day::fake::area::frost","60,0");
	NFmiSettings::Set("day::fake::area::severe_frost","90,0");
	NFmiSettings::Set("day::fake::coast::value","90,0");
	NFmiSettings::Set("day::fake::inland::value","90,0");
	require(story,FI,id,"");
	require(story,SV,id,"");
	require(story,EN,id,"");


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
	  TEST( frost_mean );
	  TEST( frost_maximum );
	  TEST( frost_range );
	  TEST( frost_twonights );
	  TEST( frost_day );
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
