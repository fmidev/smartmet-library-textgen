// ======================================================================
/*!
 * \file
 *�\brief Implementation of method TextGen::FrostStory::day
 */
// ======================================================================

#include "FrostStory.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "HourPeriodGenerator.h"
#include "Integer.h"
#include "MathTools.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "UnitFactory.h"
#include "WeatherResult.h"


using namespace std;
using namespace WeatherAnalysis;

namespace TextGen
{

  namespace
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return a plain frost sentence
	 *
	 * The sentence is of type "hallan todenn�k�isyys on y�ll� 10%".
	 */
	// ----------------------------------------------------------------------

	const Sentence plain_frost_sentence(bool isSevere,
										int theValue,
										const string & theVar,
										const NFmiTime & theForecastTime,
										const WeatherPeriod & thePeriod)
	{
	  Sentence sentence;
	  sentence << (isSevere ?
				   "ankaran hallan todenn�k�isyys" :
				   "hallan todenn�k�isyys")
			   << "on"
			   << PeriodPhraseFactory::create("tonight",
											  theVar,
											  theForecastTime,
											  thePeriod)
			   << Integer(theValue)
			   << *UnitFactory::create(Percent);
	  return sentence;
	}

  } // namespace anonymous

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on one night frost
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  
  const Paragraph FrostStory::day() const
  {
	MessageLogger log("FrostStory::day");

	using MathTools::to_precision;

	Paragraph paragraph;

	// No frost during winter

	const int forecast_month = itsForecastTime.GetMonth();
	const int forecast_day = itsForecastTime.GetDay();
	if(forecast_month > 10 ||
	   forecast_month < 4 ||
	   (forecast_month == 10 && forecast_day>17))
	  {
		log << "Frost is not reported from after October 15th until 31st of March!";
		log << paragraph;
		return paragraph;
	  }
	
	HourPeriodGenerator generator(itsPeriod,itsVar+"::night");

    // Too late for this night? Return empty story then

    if(generator.size() == 0)
      return paragraph;

   // Too many days is an error

    if(generator.size() != 1)
      throw TextGenError("Cannot use frost_day story for periods longer than 1 day");

    // Exactly one period is fine

    WeatherPeriod period = generator.period(1);

	// The options

	const int precision   = Settings::optional_percentage(itsVar+"::precision",10);
	const int severelimit = Settings::optional_percentage(itsVar+"::severe_frost_limit",10);
	const int normallimit = Settings::optional_percentage(itsVar+"::frost_limit",10);
	const int obvious_frost = Settings::optional_percentage(itsVar+"::obvious_frost_limit",90);
    const int coastlimit = Settings::optional_percentage(itsVar+"::coastlimit",20);

	// Calculate frost probability for the area

	GridForecaster forecaster;

    WeatherResult areafrost = forecaster.analyze(itsVar+"::fake::area::frost",
												 itsSources,
												 Frost,
												 Mean,
												 Maximum,
												 itsArea,
												 period);

    WeatherResult areasevere = forecaster.analyze(itsVar+"::fake::area::severe_frost",
												  itsSources,
												  Frost,
												  Mean,
												  Maximum,
												  itsArea,
												  period);

	if(areafrost.value()==kFloatMissing || areasevere.value()==kFloatMissing)
	  throw TextGenError("Frost is not available");

	log << "Frost Mean(Maximum) for area " << areafrost << endl;
	log << "SevereFrost Mean(Maximum) for area " << areasevere << endl;

	// Rounded values

	const int areaf = to_precision(areafrost.value(),precision);
	const int areasf = to_precision(areasevere.value(),precision);
		
	// Abort if the story is meaningless

	if(areaf < normallimit)
	  {
		log << "Frost probability is below the required minimum - no story";
		return paragraph;
	  }
	
	if(areasf >= obvious_frost)
	  {
		log << "Severe frost probability is above the preset maximum - no story";
		return paragraph;
	  }

	// Deduce frost type

	const bool issevere = (areasf >= severelimit);
	const WeatherParameter param = (issevere ? SevereFrost : Frost);

	if(issevere)
	  log << "Forecast is severe frost";
	else
	  log << "Forecast is not severe frost";

	// Calculate coastal values

    WeatherArea coast = itsArea;
    coast.type(WeatherArea::Coast);

    WeatherResult coastfrost = forecaster.analyze(itsVar+"::fake::coast::value",
                                                  itsSources,
                                                  param,
                                                  Mean,
                                                  Maximum,
                                                  coast,
                                                  period);

	// Done if there is no coast

	if(coastfrost.value() == kFloatMissing)
	  {
		log << "There is no coastal area";

		paragraph << plain_frost_sentence(issevere,
										  issevere ? areasf : areaf,
										  itsVar,
										  itsForecastTime,
										  period);
		log << paragraph;
		return paragraph;
	  }

	// Calculate inland values
	WeatherArea inland = itsArea;
    inland.type(WeatherArea::Inland);
	
    WeatherResult inlandfrost = forecaster.analyze(itsVar+"::fake::inland::value",
												   itsSources,
												   param,
												   Mean,
												   Maximum,
												   inland,
												   period);

	// Done if there is no inland

	if(inlandfrost.value() == kFloatMissing)
	  {
		log << "There is no inland area";

		paragraph << plain_frost_sentence(issevere,
										  issevere ? areasf : areaf,
										  itsVar,
										  itsForecastTime,
										  period);
		log << paragraph;
		return paragraph;
	  }

	// We have a coast and inland

	log << "Mean(Maximum) for coast " << coastfrost << endl;
	log << "Mean(Maximum) for inland " << inlandfrost << endl;

	// What's the difference between coast and inland?

	const int inlandvalue = to_precision(inlandfrost.value(),precision);
	const int coastvalue = to_precision(coastfrost.value(),precision);

	// No significant difference
	if(abs(inlandvalue-coastvalue) < coastlimit)
	  {
		log << "Difference between coast and inland is small";

		paragraph << plain_frost_sentence(issevere,
										  issevere ? areasf : areaf,
										  itsVar,
										  itsForecastTime,
										  period);
		log << paragraph;
		return paragraph;
	  }

	// Report probabilities separately

	log << "Difference between coast and inland is significant";

	Sentence sentence = plain_frost_sentence(issevere,
											 inlandvalue,
											 itsVar,
											 itsForecastTime,
											 period);
	sentence << Delimiter(",")
			 << "rannikolla"
			 << Integer(coastvalue)
			 << *UnitFactory::create(Percent);
	paragraph << sentence;

	log << paragraph;
	return paragraph;
  }


} // namespace TextGen
  
// ======================================================================
  