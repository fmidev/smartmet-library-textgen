// ======================================================================
/*!
 * \file  FrostStoryAk.cpp
 * \brief FrostStory version by AKa 2009.
 *
 * NOTE: Do NOT rename this to 'FrostStory.cpp'; that already exists under
 *      TextGen all-in-one 'source/' directory!
 */
 
// ======================================================================
/*!
 * \class TextGen::FrostStory
 *
 * \brief Generates stories on frost
 *
 * \see http://wiki.weatherproof.fi/index.php?title=Textgen:_frost_overview
 */
// ======================================================================

#include "FrostStoryAk.h"

#include "AnalysisSources.h"
#include "Paragraph.h"
#include "TextGenError.h"
#include "WeatherResult.h"
#include "GridForecaster.h"

using namespace std;

struct my_story {
    typedef TextGen::Paragraph (*story_f)( const TextGen::FrostStory &me );

    const string name;
    const story_f f;
    
    my_story( const char *name_, story_f f_ ) : name(name_), f(f_) {}
};

static const my_story known_stories[]= {
    my_story( STORY_OVERVIEW_TEXT, TextGen::FrostStory::overview_text ),
    my_story( STORY_OVERVIEW_NUMERIC, TextGen::FrostStory::overview_numeric )
};

/*
* Give the function for performing certain story (or NULL if 'name' not recognized)
*/
static my_story::story_f story_func( const string &name ) {
    for( unsigned i=0; i<sizeof(known_stories) / sizeof(*known_stories); i++ ) {
        if (known_stories[i].name == name) {
            return known_stories[i].f;
        }
    }
    return 0;
}  


namespace TextGen
{
    const string FrostStory::PREFIX= "textgen::" FROST_OVERVIEW;   // configuration prefix

    /*
    */
    bool FrostStory::is_frost_season( const NFmiTime &time ) const {

        // Jos hallakauden ulkopuolella (päivämäärärajat) tai jos kasvukausi ei ole vielä
        // alkanut, ei anneta hallatiedotteita.
        //
        unsigned jd_start= require_time(SEASON_START).GetJulianDay();  // is really: day of year
        unsigned jd_end= require_time(SEASON_END).GetJulianDay();

        unsigned jd= time.GetJulianDay();

        if ((jd<jd_start) || (jd>jd_end)) {
            return false;   // hallakauden ulkopuolella
        }

        // Tarkista, onko kasvukausi jo alkanut (lämpösumma > 0 ainakin yhdessä alueen pisteessä)
        //
        // 652	EffectiveTemperatureSum		kasvukauden lämpösumma
        //
	   WeatherAnalysis::GridForecaster forecaster;

        WeatherAnalysis::WeatherResult tsum_max= 
            forecaster.analyze( T_SUM_MAX,
                                itsSources,
                                WeatherAnalysis::EffectiveTemperatureSum,
                                WeatherAnalysis::Maximum,   // area function
                                WeatherAnalysis::Maximum,   // time function
                                itsArea,
                                itsPeriod );
        if (tsum_max.value() <= 0) {
            return false;
        }

        // Jos koko alueella pakkasta, ei mainita hallasta.
        //
        WeatherAnalysis::WeatherResult t_max= 
            forecaster.analyze( T_MAX,
                                itsSources,
                                WeatherAnalysis::Temperature,
                                WeatherAnalysis::Maximum,   // area function
                                WeatherAnalysis::Maximum,   // time function
                                itsArea,
                                itsPeriod );
        
        if (t_max.value() <= 0) {
            return false;
        }
        
        return true;
    }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test whether the given story is known to this class
   *
   * \param theName The story name
   * \return True if this class can generate the story
   */
  // ----------------------------------------------------------------------
  
  bool FrostStory::hasStory(const string &name)
  {
    return story_func(name) != 0;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Generate the desired story
   *
   * Throws if the story name is not recognized.
   *
   * \param theName The story name
   * \return Paragraph containing the story
   */
  // ----------------------------------------------------------------------.
  
  const Paragraph FrostStory::makeStory(const string &name) const
  {
    my_story::story_f f= story_func(name);
    if (f) {
        return f(*this);
    }

    throw TextGenError("FrostStory cannot make story "+name);
  }


} // namespace TextGen
  
// ======================================================================
  
