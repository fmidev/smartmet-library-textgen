// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::WindStory::daily_ranges
 */
// ======================================================================

#include "WindStory.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "HourPeriodGenerator.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "Sentence.h"
#include "Settings.h"
#include "WeatherResult.h"
#include "WindStoryTools.h"

#include "boost/lexical_cast.hpp"

#include <vector>

using namespace TextGen::WindStoryTools;
using namespace WeatherAnalysis;
using namespace boost;
using namespace std;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Return wind_daily_ranges story
   *
   * \return Paragraph containing the story
   */
  // ----------------------------------------------------------------------

  Paragraph WindStory::daily_ranges() const
  {
	MessageLogger log("WeatherStory::daily_ranges");

	// Establish options

	using namespace Settings;

	// const int mininterval = optional_int(itsVar+"::mininterval",0);

	// Generate the story

	Paragraph paragraph;

	GridForecaster forecaster;

	// All day periods

	HourPeriodGenerator periods(itsPeriod,itsVar);
	const int ndays = periods.size();

	// Calculate wind speeds for max 3 days

	vector<WeatherResult> meanspeeds;
	vector<WeatherResult> maxspeeds;
	vector<WeatherResult> minspeeds;
	vector<WeatherResult> directions;
	for(int day=1; day<min(ndays,3); day++)
	  {
		const WeatherPeriod period(periods.period(day));

		const string daystr = "day"+lexical_cast<string>(day);

		const WeatherResult minspeed =
		  forecaster.analyze(itsVar+"::fake::speed::"+daystr+"::minimum",
							 itsSources,
							 WindSpeed,
							 Minimum,
							 Mean,
							 itsArea,
							 period);

		const WeatherResult maxspeed =
		  forecaster.analyze(itsVar+"::fake::speed::"+daystr+"::maximum",
							 itsSources,
							 WindSpeed,
							 Maximum,
							 Mean,
							 itsArea,
							 period);

		const WeatherResult meanspeed =
		  forecaster.analyze(itsVar+"::fake::speed::"+daystr+"::mean",
							 itsSources,
							 WindSpeed,
							 Mean,
							 Mean,
							 itsArea,
							 period);

		// FIX THIS TO CALCULATE QUALITY!!!
		const WeatherResult direction =
		  forecaster.analyze(itsVar+"::fake::direction::"+daystr+"::mean",
							 itsSources,
							 WindSpeed,
							 Mean,
							 Mean,
							 itsArea,
							 period);

		log << "WindSpeed Minimum(Mean) " << daystr << " = " << minspeed << endl;
		log << "WindSpeed Maximum(Mean) " << daystr << " = " << maxspeed << endl;
		log << "WindSpeed Mean(Mean) " << daystr << " = " << meanspeed << endl;

		minspeeds.push_back(minspeed);
		maxspeeds.push_back(maxspeed);
		meanspeeds.push_back(meanspeed);
		directions.push_back(direction);

	  }

	log << paragraph;
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
