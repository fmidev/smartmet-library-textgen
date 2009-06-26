// ======================================================================
/*!
 * \file
 * \brief 
 */
// ======================================================================

#include "ak_FrostStory.h"
#include "ak_FrostStoryTools.h"

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
using namespace WeatherAnalysis;
using namespace TextGen::AK_FrostStoryTools;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on 1/2 night frost
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  
  Paragraph AK_FrostStory::twonights() const {
	MessageLogger log("AK_FrostStory::twonights");

	Paragraph paragraph;

	if (!AK_FrostStoryTools::is_frost_season()) {
		log << "Frost season is not on";
		return paragraph;
	  }

	using MathTools::to_precision;

	const int starthour    = Settings::require_hour( itsVar+"::night::starthour" );
	const int endhour      = Settings::require_hour( itsVar+"::night::endhour" );
	const int maxstarthour = Settings::optional_hour( itsVar+"::night::maxstarthour",starthour );
	const int minendhour   = Settings::optional_hour( itsVar+"::night::minendhour",endhour );

	const int precision   = Settings::require_percentage( itsVar+"::precision" );
	const int severelimit = Settings::require_percentage( itsVar+"::severe_frost_limit" );
	const int normallimit = Settings::require_percentage( itsVar+"::frost_limit" );
	const int obvious_frost = Settings::optional_percentage( itsVar+"::obvious_frost_limit", 90 );

	const int nights = WeatherPeriodTools::countPeriods( itsPeriod,
														 starthour,
														 endhour,
														 maxstarthour,
														 minendhour );
	if (nights==0) {
		log << paragraph;
		return paragraph;
    }

	// Calculate frost probability

	GridForecaster forecaster;

	WeatherPeriod night1 = WeatherPeriodTools::getPeriod( itsPeriod,
														  1,
														  starthour,
														  endhour,
														  maxstarthour,
														  minendhour );

	WeatherResult frost = forecaster.analyze( itsVar+"::fake::day1::mean",
											  itsSources,
											  Frost,
											  Mean,
											  Maximum,
											  itsArea,
											  night1 );

	WeatherResult severefrost = forecaster.analyze( itsVar+"::fake::day1::severe_mean",
												    itsSources,
												    SevereFrost,
												    Mean,
												    Maximum,
												    itsArea,
												    night1 );
	
	if (frost.value()==kFloatMissing || severefrost.value()==kFloatMissing) {
        throw TextGenError("Frost is not available");
    }

	log << "Frost Mean(Maximum) for day 1 " << frost << endl;
	log << "SevereFrost Mean(Maximum) for day 1 " << severefrost << endl;

	if (nights==1) {
		const int value = to_precision( frost.value(), precision );
		const int severevalue = to_precision( severefrost.value(), precision );

		if (severevalue > obvious_frost) {
			log << "Severe frost probability 100% implies the forecast is obvious!";
        } else {
			if (severevalue >= severelimit) {
                paragraph << severe_frost_sentence( night1, severevalue );
            } else if (value >= normallimit) {
                paragraph << frost_sentence(night1,value);
            }
	   }
    } else {
		WeatherPeriod night2 = WeatherPeriodTools::getPeriod( itsPeriod,
															  2,
															  starthour,
															  endhour,
															  maxstarthour,
															  minendhour );

		WeatherResult frost2 = forecaster.analyze( itsVar+"::fake::day2::mean",
												   itsSources,
												   Frost,
												   Mean,
												   Maximum,
												   itsArea,
												   night2 );

		WeatherResult severefrost2 = forecaster.analyze( itsVar+"::fake::day2::severe_mean",
														 itsSources,
														 SevereFrost,
														 Mean,
														 Maximum,
														 itsArea,
														 night2 );
		
		if (frost2.value()==kFloatMissing || severefrost2.value()==kFloatMissing) {
            throw TextGenError("Frost is not available");
        }

		log << "Frost Mean(Maximum) for day 2 " << frost2 << endl;
		log << "SevereFrost Mean(Maximum) for day 2 " << severefrost2 << endl;
		
		const int value1 = to_precision( frost.value(), precision );
		const int severevalue1 = to_precision( severefrost.value(), precision );

		const int value2 = to_precision( frost2.value(), precision );
		const int severevalue2 = to_precision( severefrost2.value(), precision );
		
		// We have 9 combinations:
		//
		// nada+nada		""
		// nada+frost		"Hallan todennäköisyys on tiistain vastaisena yön x%."
		// nada+severe		"Ankaran hallan todennäköisyys on tiistain vastaisena yönä x%."
		// frost+nada		"Hallan ..., seuraava yö on lämpimämpi."
		// frost+frost		"Hallan ..., seuraavana yönä y%/sama."
		// frost+severe		"Hallan ..., seuraavana yönä ankaran hallan todennäköisyys on y%."
		// severe+nada		"Ankaran ..., seuraava yö on huomattavasti lämpimämpi."
		// severe+frost		"Ankaran ..., seuraavana yönä hallan todennäköisyys on y%."
		// severe+severe	"Ankaran ..., seuraavana yönä y%/sama."

		if (severevalue1 > obvious_frost && severevalue2 > obvious_frost) {
			log << "Severe frost probability 100% implies the forecast is obvious!";
		} 
		else if (severevalue1 >= severelimit) {		// severe + ?
			Sentence sentence;
			sentence << severe_frost_sentence( night1, severevalue1 )
					 << Delimiter(",");
			
			if (severevalue2 >= severelimit) {
				sentence << "seuraavana y\xf6n\xe4";
				if (severevalue1 == severevalue2) {
				    sentence << "sama";
				} else {
				  sentence << Integer(severevalue2)
						   << *UnitFactory::create(Percent);
                }
            }
			else if(value2 >= normallimit) {
				sentence << "seuraavana y\xf6n\xe4"
						 << "hallan todenn\xe4k\xf6isyys"     // SQL key (latin-1)
						 << "on"
						 << Integer(value2)
						 << *UnitFactory::create(Percent);
			}
			else {
				sentence << "seuraava y\xf6"
						 << "on"
						 << "huomattavasti l\xe4mpim\xe4mpi";
            }
			paragraph << sentence;
        }
		else if (value1 >= normallimit) {				// frost + ?
			Sentence sentence;
			sentence << frost_sentence(night1,value1)
					 << Delimiter(",");

			if (severevalue2 >= severelimit) {
				sentence << "seuraavana y\xf6n\xe4"
						 << "ankaran hallan todenn\xe4k\xf6isyys"     // SQL key (latin-1)
						 << "on"
						 << Integer(severevalue2)
						 << *UnitFactory::create(Percent);
            } 
            else if(value2 >= normallimit) {
				sentence << "seuraavana y\xf6n\xe4";
				if(value1 == value2)
				  sentence << "sama";
				else
				  sentence << Integer(value2)
						   << *UnitFactory::create(Percent);
            } else {
				sentence << "seuraava y\xf6"
						 << "on"
						 << "l\xe4mpim\xe4mpi";
            }
			paragraph << sentence;
        } else {										// nada + ?
			if(severevalue2 >= severelimit) {
			     paragraph << severe_frost_sentence(night2,severevalue2);
			} else if (value2 >= normallimit) {
			     paragraph << frost_sentence(night2,value2);
            }
        }
    }

	log << paragraph;
	return paragraph;
}


} // namespace TextGen
  
// ======================================================================
  
