// ======================================================================
/*!
 * \file
 *�\brief Implementation of method TextGen::TemperatureStory::day
 */
// ======================================================================

#include "TemperatureStory.h"

#include "DefaultAcceptor.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "HourPeriodGenerator.h"
#include "Integer.h"
#include "MathTools.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "Settings.h"
#include "TemperatureStoryTools.h"
#include "TextGenError.h"
#include "UnitFactory.h"
#include "WeatherPeriodTools.h"
#include "WeatherResult.h"
#include "PeriodPhraseFactory.h"

#include "boost/lexical_cast.hpp"

using namespace WeatherAnalysis;
using namespace TextGen::TemperatureStoryTools;
using namespace boost;
using namespace std;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Calculate area results
   */
  // ----------------------------------------------------------------------

  void calculate_mean_min_max(MessageLogger & theLog,
							  const WeatherForecaster & theForecaster,
							  const string & theVar,
							  const AnalysisSources & theSources,
							  const WeatherArea & theArea,
							  const WeatherPeriod & thePeriod,
							  WeatherFunction theFunction,
							  WeatherResult & theMean,
							  WeatherResult & theMin,
							  WeatherResult & theMax)
  {
	theMean = theForecaster.analyze(theVar+"::mean",
									theSources,
									Temperature,
									Mean,
									theFunction,
									theArea,
									thePeriod);

	theMin = theForecaster.analyze(theVar+"::min",
								   theSources,
								   Temperature,
								   Minimum,
								   theFunction,
								   theArea,
								   thePeriod);

	theMax = theForecaster.analyze(theVar+"::max",
								   theSources,
								   Temperature,
								   Maximum,
								   theFunction,
								   theArea,
								   thePeriod);

	theLog << "Temperature Mean(Maximum) " << theMean << endl;
	theLog << "Temperature Min(Maximum) " << theMin << endl;
	theLog << "Temperature Max(Maximum) " << theMax << endl;

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on temperature for the day and night
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  
  const Paragraph TemperatureStory::day() const
  {
	MessageLogger log("TemperatureStory::day");
		      
	using MathTools::to_precision;
	using Settings::optional_int;
	using Settings::optional_bool;

	Paragraph paragraph;

	// Separate generators for day and night.

	HourPeriodGenerator daygenerator(itsPeriod,itsVar+"::day");
	HourPeriodGenerator nightgenerator(itsPeriod,itsVar+"::night");

	const bool hasday = (daygenerator.size() > 0);
	const bool hasnight = (nightgenerator.size() > 0);

	// Too late? Return empty story then

	if(!hasday && !hasnight)
	  {
		log << paragraph;
		return paragraph;
	  }

	// Too many days is an error

	if(daygenerator.size() > 1 || nightgenerator.size() > 1)
	  throw TextGenError("Cannot use temperature_day story for periods longer than 1 day");

	// The options

	const int day_night_limit = optional_int(itsVar+"::day_night_limit",3);
	const int coast_numeric_limit = optional_int(itsVar+"::coast_numeric_limit",8);
	const int coast_not_below = optional_int(itsVar+"::coast_not_below",-15);
	const int coast_limit = optional_int(itsVar+"::coast_limit",2);

    const int mininterval = optional_int(itsVar+"::mininterval",2);
    const bool interval_zero = optional_bool(itsVar+"::always_interval_zero",false);

	// Calculate the results

	GridForecaster forecaster;

	WeatherResult areameanday(kFloatMissing,0);
	WeatherResult areaminday(kFloatMissing,0);
	WeatherResult areamaxday(kFloatMissing,0);

	WeatherResult coastmeanday(kFloatMissing,0);
	WeatherResult coastminday(kFloatMissing,0);
	WeatherResult coastmaxday(kFloatMissing,0);

	WeatherResult inlandmeanday(kFloatMissing,0);
	WeatherResult inlandminday(kFloatMissing,0);
	WeatherResult inlandmaxday(kFloatMissing,0);

	WeatherResult areameannight(kFloatMissing,0);
	WeatherResult areaminnight(kFloatMissing,0);
	WeatherResult areamaxnight(kFloatMissing,0);

	WeatherResult coastmeannight(kFloatMissing,0);
	WeatherResult coastminnight(kFloatMissing,0);
	WeatherResult coastmaxnight(kFloatMissing,0);

	WeatherResult inlandmeannight(kFloatMissing,0);
	WeatherResult inlandminnight(kFloatMissing,0);
	WeatherResult inlandmaxnight(kFloatMissing,0);

	WeatherArea coast = itsArea;
	coast.type(WeatherArea::Coast);

	WeatherArea inland = itsArea;
	inland.type(WeatherArea::Inland);

	// Daytime results

	if(hasday)
	  {
		WeatherPeriod period = daygenerator.period(1);

		log << "Day results for area:" << endl;
		calculate_mean_min_max(log,forecaster,
							   itsVar+"::fake::day::area",itsSources,itsArea,period,Maximum,
							   areameanday,areaminday,areamaxday);

		log << "Day results for coast:" << endl;
		calculate_mean_min_max(log,forecaster,
							   itsVar+"::fake::day::coast",itsSources,coast,period,Maximum,
							   coastmeanday,coastminday,coastmaxday);

		log << "Day results for inland:" << endl;
		calculate_mean_min_max(log,forecaster,
							   itsVar+"::fake::day::inland",itsSources,inland,period,Maximum,
							   inlandmeanday,inlandminday,inlandmaxday);

	  }

	// Nighttime results

	if(hasnight)
	  {
		WeatherPeriod period = nightgenerator.period(1);

		log << "Night results for area:" << endl;
		calculate_mean_min_max(log,forecaster,
							   itsVar+"::fake::night::area",itsSources,itsArea,period,Minimum,
							   areameannight,areaminnight,areamaxnight);

		log << "Night results for coast:" << endl;
		calculate_mean_min_max(log,forecaster,
							   itsVar+"::fake::night::coast",itsSources,coast,period,Minimum,
							   coastmeannight,coastminnight,coastmaxnight);

		log << "Night results for inland:" << endl;
		calculate_mean_min_max(log,forecaster,
							   itsVar+"::fake::night::inland",itsSources,inland,period,Minimum,
							   inlandmeannight,inlandminnight,inlandmaxnight);

	  }

	// Rounded values

	const int aminday = FmiRound(areaminday.value());
	const int ameanday = FmiRound(areameanday.value());
	const int amaxday = FmiRound(areamaxday.value());
	
	const int cminday = FmiRound(coastminday.value());
	const int cmeanday = FmiRound(coastmeanday.value());
	const int cmaxday = FmiRound(coastmaxday.value());
	
	const int iminday = FmiRound(inlandminday.value());
	const int imeanday = FmiRound(inlandmeanday.value());
	const int imaxday = FmiRound(inlandmaxday.value());

	const int aminnight = FmiRound(areaminnight.value());
	const int ameannight = FmiRound(areameannight.value());
	const int amaxnight = FmiRound(areamaxnight.value());
	
	const int cminnight = FmiRound(coastminnight.value());
	const int cmeannight = FmiRound(coastmeannight.value());
	const int cmaxnight = FmiRound(coastmaxnight.value());
	
	const int iminnight = FmiRound(inlandminnight.value());
	const int imeannight = FmiRound(inlandmeannight.value());
	const int imaxnight = FmiRound(inlandmaxnight.value());

	const int bad = static_cast<int>(kFloatMissing);

	const bool hascoast = ( (cmeanday!=bad && imeanday!=bad) || (cmeannight!=bad && imeannight!=bad) );

	// See if we can report on day and night simultaneously
	// (when the difference is very small)

	const bool canmerge = (hasday && hasnight && abs(ameanday-ameannight) < day_night_limit);

	if(canmerge)
	  {
		Sentence sentence;
		sentence << "l�mp�tila"
				 << "on"
				 << PeriodPhraseFactory::create("today",
												itsVar,
												itsForecastTime,
												daygenerator.period(1));
		if(!hascoast ||
		   ameanday < coast_not_below ||
		   (abs(cmeanday - imeanday) < coast_limit && abs(cmeannight - imeannight) < coast_limit))
		  {
			sentence << temperature_sentence(min(aminday,aminnight),
											 FmiRound((ameanday+ameannight)/2),
											 max(amaxday,amaxnight),
											 mininterval,
											 interval_zero);
		  }
		else
		  {
			const int imean = FmiRound((inlandmeanday.value()+inlandmeannight.value())/2);
			const int cmean = FmiRound((coastmeanday.value()+coastmeannight.value())/2);
			sentence << temperature_sentence(min(iminday,iminnight),
											 imean,
											 max(imaxday,imaxnight),
											 mininterval,
											 interval_zero)
					 << Delimiter(",")
					 << "rannikolla"
					 << temperature_comparison_phrase(imean,cmean,itsVar);
		  }
		paragraph << sentence;

	  }

	// Report daytime

	if(hasday && !canmerge)
	  {
		Sentence sentence;

		sentence << "p�iv�n ylin l�mp�tila"
				 << "on"
				 << PeriodPhraseFactory::create("today",
												itsVar,
												itsForecastTime,
												daygenerator.period(1));
		
		// Do not report on the coast separately if there's no coast,
		// the mean temperature is very low (atleast freezing), or if the
		// difference between inland and the coast is small

		if(!hascoast ||
		   ameanday < coast_not_below ||
		   abs(cmeanday - imeanday) < coast_limit)
		  {
			sentence << temperature_sentence(aminday,ameanday,amaxday,mininterval,interval_zero);
		  }
		else
		  {
			sentence << temperature_sentence(iminday,imeanday,imaxday,mininterval,interval_zero)
					 << Delimiter(",")
					 << "rannikolla";

			if(abs(imeanday-cmeanday) >= coast_numeric_limit)
			  sentence << temperature_sentence(cminday,cmeanday,cmaxday,mininterval,interval_zero);
			else
			  sentence << temperature_comparison_phrase(imeanday,cmeanday,itsVar);
		  }
		paragraph << sentence;
	  }

	// Report nighttime

	if(hasnight && !canmerge)
	  {
		Sentence sentence;

		sentence << "y�n alin l�mp�tila"
				 << "on"
				 << PeriodPhraseFactory::create("tonight",
												itsVar,
												itsForecastTime,
												nightgenerator.period(1));
		
		// Do not report on the coast separately if there's no coast,
		// the mean temperature is very low (atleast freezing), or if the
		// difference between inland and the coast is small

		if(!hascoast ||
		   ameannight < coast_not_below ||
		   abs(cmeannight - imeannight) < coast_limit)
		  {
			sentence << temperature_sentence(aminnight,ameannight,amaxnight,mininterval,interval_zero);
		  }
		else
		  {
			sentence << temperature_sentence(iminnight,imeannight,imaxnight,mininterval,interval_zero)
					 << Delimiter(",")
					 << "rannikolla";

			if(abs(imeannight-cmeannight) >= coast_numeric_limit)
			  sentence << temperature_sentence(cminnight,cmeannight,cmaxnight,mininterval,interval_zero);
			else
			  sentence << temperature_comparison_phrase(imeannight,cmeannight,itsVar);
		  }
		paragraph << sentence;
	  }

	log << paragraph;
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
  