// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::WeatherStory::overview
 */
// ======================================================================

#include "WeatherStory.h"
#include "CloudinessStory.h"
#include "HourPeriodGenerator.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PrecipitationPeriodTools.h"
#include "Sentence.h"
#include "TimeTools.h"

using namespace WeatherAnalysis;
using namespace std;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate overview on weather
   *
   * \return The story
   *
   * \see page_weather_overview
   *
   * \todo Much missing
   */
  // ----------------------------------------------------------------------

  Paragraph WeatherStory::overview() const
  {
	MessageLogger log("WeatherStory::overview");

	using namespace PrecipitationPeriodTools;

	Paragraph paragraph;

	// we want the last day to extend up to midnight regardless
	// of the actual period length, otherwise we risk badly
	// formed forecasts when using full-day descriptions

	WeatherPeriod rainperiod(itsPeriod.localStartTime(),
							 TimeTools::dayEnd(itsPeriod.localEndTime()));

	RainPeriods rainperiods = analyze(itsSources,
									  itsArea,
									  itsPeriod,
									  itsVar);

	log << "Found " << rainperiods.size() << " rainy periods" << endl;

	if(rainperiods.size() == 0)
	  {
		CloudinessStory story(itsForecastTime,
							  itsSources,
							  itsArea,
							  itsPeriod,
							  itsVar);
		paragraph << story.makeStory("cloudiness_overview");
		log << paragraph;
		return paragraph;
	  }

	// process each day separately

	HourPeriodGenerator generator(rainperiod, itsVar+"::day");
	
	const int n = generator.size();
	for(int day=1; day<=n; day++)
	  {
		WeatherPeriod period = generator.period(day);

		RainPeriods overlap = overlappingPeriods(rainperiods,period);
		RainPeriods inclusive = inclusivePeriods(rainperiods,period);

		const RainPeriods::size_type noverlap = overlap.size();
		const RainPeriods::size_type ninclusive = inclusive.size();

		log << "Day " << day << " overlapping rains = " << noverlap << endl;
		log << "Day " << day << " inclusive rains = " << ninclusive << endl;

		if(noverlap==0 && ninclusive==0)
		  {
			CloudinessStory story(itsForecastTime,
								  itsSources,
								  itsArea,
								  period,
								  itsVar);
			paragraph << story.makeStory("cloudiness_overview");
		  }
		else if(noverlap==0 && ninclusive==1)
		  {
			Sentence s;
			s << "sadetta";
			paragraph << s;
		  }
		else if(noverlap==0 && ninclusive>0)
		  {
			Sentence s;
			s << "ajoittain sateista";
			paragraph << s;
		  }
		else
		  {
			Sentence s;
			s << "sateista";
			paragraph << s;
		  }
	  }

	log << paragraph;
	return paragraph;
  }


} // namespace TextGen

// ======================================================================
