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

	RainPeriods rainperiods = PrecipitationPeriodTools::analyze(itsSources,
																itsArea,
																itsPeriod,
																itsVar);

	log << "Found " << rainperiods.size() << " rainy periods" << endl;

	{
	  for(RainPeriods::const_iterator it=rainperiods.begin();
		  it!=rainperiods.end();
		  it++)
		{
		  log << "Period: "
			  << it->localStartTime()
			  << " ... "
			  << it->localEndTime()
			  << endl;
		}
	}


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
		const RainPeriods::size_type noverlap = overlaps[day-1].size();
		const RainPeriods::size_type ninclusive = inclusives[day-1].size();

		log << "Day " << day << " overlap   : " << noverlap << endl;
		log << "Day " << day << " inclusive : " << ninclusive << endl;

		if(ninclusive==noverlap) // all rains within the same day
		  {
			if(ninclusive==0)
			  {
				// find sequence of non-rainy days, report all at once
				int day2 = day;
				for(; day2<n; day2++)
				  {
					if(overlaps[day2].size()!=0 || inclusives[day2].size()!=0)
					  break;
				  }
				
				WeatherPeriod period(generator.period(day).localStartTime(),
									 generator.period(day2).localEndTime());
				
				if(day != day2)
				  log << "Cloudiness only for days " << day << '-' << day2 << endl;
				else
				  log << "Cloudiness only for day " << day << endl;
				
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
			s << "ajoittain sateista";
			paragraph << s;
		  }
	  }

	log << paragraph;
	return paragraph;
  }


} // namespace TextGen

// ======================================================================
