// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::CloudinessStory::overview
 */
// ======================================================================

#include "CloudinessStory.h"
#include "CloudinessStoryTools.h"
#include "GridForecaster.h"
#include "HourPeriodGenerator.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "RangeAcceptor.h"
#include "Settings.h"
#include "WeatherResult.h"

#include "boost/lexical_cast.hpp"

#include <vector>

using namespace boost;
using namespace std;
using namespace WeatherAnalysis;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on cloudiness
   *
   * \return The story
   *
   */
  // ----------------------------------------------------------------------

  Paragraph CloudinessStory::overview() const
  {
	MessageLogger log("CloudinessStory::overview");

	using namespace Settings;
	using namespace CloudinessStoryTools;

	// Establish optional control settings

	const int clear = optional_percentage(itsVar+"::clear",40);
	const int cloudy = optional_percentage(itsVar+"::cloudy",70);

	// Calculate the percentages for each day in the period
	// We do not allow forecasts longer than 3 days in order
	// to limit the complexity of the algorithm.
	
	GridForecaster forecaster;
	Paragraph paragraph;

	const HourPeriodGenerator periodgenerator(itsPeriod,itsVar+"::day");
	const int ndays = min(periodgenerator.size(),3u);

	log << "Period covers " << ndays << " days" << endl;

	if(ndays <= 0)
	  {
		log << paragraph;
		return paragraph;
	  }

	vector<WeatherPeriod> periods;
	vector<CloudinessType> types;

	for(int day=1; day<ndays; day++)
	  {
		const WeatherPeriod period(periodgenerator.period(day));
		const string daystr = "day"+lexical_cast<string>(day);

		RangeAcceptor cloudylimits;
		cloudylimits.lowerLimit(cloudy);

		RangeAcceptor clearlimits;
		clearlimits.upperLimit(clear);

		const WeatherResult cloudy_percentage =
		  forecaster.analyze(itsVar+"::fake::"+daystr+"::cloudy",
							 itsSources,
							 Cloudiness,
							 Mean,
							 Percentage,
							 itsArea,
							 period,
							 DefaultAcceptor(),
							 DefaultAcceptor(),
							 cloudylimits);

		const WeatherResult clear_percentage =
		  forecaster.analyze(itsVar+"::fake::"+daystr+"::clear",
							 itsSources,
							 Cloudiness,
							 Mean,
							 Percentage,
							 itsArea,
							 period,
							 DefaultAcceptor(),
							 DefaultAcceptor(),
							 clearlimits);

		const WeatherResult trend =
		  forecaster.analyze(itsVar+"::fake::"+daystr+"::trend",
							 itsSources,
							 Cloudiness,
							 Mean,
							 Trend,
							 itsArea,
							 period);

		log << "Cloudiness Mean(Percentage(cloudy))      " << daystr << " = " << cloudy_percentage << endl;
		log << "Cloudiness Mean(Percentage(clear))       " << daystr << " = " << clear_percentage << endl;
		log << "Cloudiness Mean(Trend) " << daystr << " = " << trend << endl;

		CloudinessType ctype = cloudinesstype(itsVar,
											  cloudy_percentage.value(),
											  clear_percentage.value(),
											  trend.value());

		periods.push_back(period);
		types.push_back(ctype);

	  }


	log << paragraph;
	return paragraph;

  }

} // namespace TextGen
  
// ======================================================================
  
