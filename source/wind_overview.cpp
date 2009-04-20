// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::WindStory::overview
 *
 * AKa 20-Apr-2009: Taking over this; going to implement logic received
 *      from Mika H. as email (Excel sheet).
 */
// ======================================================================

#include "WindStory.h"
#include "MessageLogger.h"
#include "Paragraph.h"

using namespace WeatherAnalysis;
using namespace std;

/*
* Function 
*/
Sentence f_navakka_tyyni( TuulenSuunta alku, TuulenSuunta loppu ) {
    Sentence s;

    (void)alku; (void)loppu;
    
    // "Navakka xxxpuoleinen tuuli heikkenee"
    s << "Navakka" << alku << "tuuli" << "heikkenee";

    return s;
}

/*
* Taulukko, jonka perusteella teksti syntyy.
*/
    // [aluksi][lopuksi]
table= {
    ["tyyntä"]["tyyntä"]= f_tuuli_on_heikkoa,
    ["tyyntä"]["heikkoa"]= f_tuuli_on_heikkoa,
    ["heikkoa"]["tyyntä"]= f_tuuli_on_heikkoa,
    ...
}

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Return wind overview story
   *
   * \return Paragraph containing the story
   */
  // ----------------------------------------------------------------------

  const Paragraph WindStory::overview() const
  {
	MessageLogger log("WeatherStory::overview");

    // Generate the story
    //
	Paragraph paragraph;

	GridForecaster forecaster;

    // TBD: Jotain forecasterista
    
    TuulenSuunta ts_alku, ts_loppu;
    TuulenVoima tv_alku, tv_loppu;

    Sentence sent;

    if (ts_alku == ts_loppu) {
        sent= Tuuli_ei_kaanny( ts_alku, tv_alku, tv_loppu );
    } else {
        sent= Tuuli_kaantyy( ts_alku, ts_loppu, tv_alku, tv_loppu );
    }
    
	// log << "WindSpeed Minimum(Mean)  = " << minresult << endl;

	//Sentence sentence;
	//sentence << WindStoryTools::directed_speed_sentence(minresult,
	//													maxresult,
	//													meanresult,
	//													dirresult,
	//													itsVar);
	//paragraph << sentence;

	log << paragraph;
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
