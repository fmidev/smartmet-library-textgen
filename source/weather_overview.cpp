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

#include <vector>

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

	vector<RainPeriods> overlaps;
	vector<RainPeriods> inclusives;

	for(int day=1; day<=n; day++)
	  {
		WeatherPeriod period = generator.period(day);

		RainPeriods overlap = overlappingPeriods(rainperiods,period);
		RainPeriods inclusive = inclusivePeriods(rainperiods,period);

		overlaps.push_back(overlap);
		inclusives.push_back(inclusive);
	  }

	for(int day=1; day<=n; day++)
	  {
		const RainPeriods::size_type noverlap = overlaps[day].size();
		const RainPeriods::size_type ninclusive = inclusives[day].size();

		log << "Day " << day << " overlapping rains = " << noverlap << endl;
		log << "Day " << day << " inclusive rains = " << ninclusive << endl;

		if(noverlap==0)
		  {
			if(ninclusive==0)
			  {
				// find sequence of non-rainy days, report all at once
				int day2 = day;
				for(; day2<n; day2++)
				  {
					if(overlaps[day2+1].size()!=0 || inclusives[day2+1].size()!=0)
					  break;
				  }
				
				WeatherPeriod period(generator.period(day).localStartTime(),
										   generator.period(day2).localEndTime());
				
				CloudinessStory story(itsForecastTime,
									  itsSources,
									  itsArea,
									  period,
									  itsVar);
				paragraph << story.makeStory("cloudiness_overview");
				day = day2;
			  }
			else if(ninclusive==1)
			  {
				Sentence s;
				s << "sadetta";
				paragraph << s;
			  }
			else
			  {
				Sentence s;
				s << "ajoittain sateista";
				paragraph << s;
			  }
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
