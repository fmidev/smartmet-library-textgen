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
#include "WeekdayTools.h"
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

	using WeekdayTools::on_weekday;
	using WeekdayTools::from_weekday;

	// Establish options

	using namespace Settings;

	// Generate the story

	Paragraph paragraph;

	GridForecaster forecaster;

	// All day periods

	const HourPeriodGenerator periodgenerator(itsPeriod,itsVar);
	const int ndays = periodgenerator.size();

	// Calculate wind speeds for max 3 days

	vector<WeatherPeriod> periods;
	vector<WeatherResult> meanspeeds;
	vector<WeatherResult> maxspeeds;
	vector<WeatherResult> minspeeds;
	vector<WeatherResult> directions;
	vector<WindDirectionAccuracy> accuracies;

	for(int day=1; day<min(ndays,3); day++)
	  {
		const WeatherPeriod period(periodgenerator.period(day));

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

		const WeatherResult direction =
		  forecaster.analyze(itsVar+"::fake::direction::"+daystr+"::mean",
							 itsSources,
							 WindDirection,
							 Mean,
							 Mean,
							 itsArea,
							 period);

		log << "WindSpeed Minimum(Mean) " << daystr << " = " << minspeed << endl;
		log << "WindSpeed Maximum(Mean) " << daystr << " = " << maxspeed << endl;
		log << "WindSpeed Mean(Mean) " << daystr << " = " << meanspeed << endl;

		periods.push_back(period);
		minspeeds.push_back(minspeed);
		maxspeeds.push_back(maxspeed);
		meanspeeds.push_back(meanspeed);
		directions.push_back(direction);
		accuracies.push_back(direction_accuracy(direction.error(),itsVar));
	  }

	switch(ndays)
	  {
	  case 1:
		{
		  Sentence sentence;
		  sentence << PeriodPhraseFactory::create("today",
												  itsVar,
												  itsForecastTime,
												  periods[0])
				   << directed_speed_sentence(minspeeds[0],
											  maxspeeds[0],
											  meanspeeds[0],
											  directions[0],
											  itsVar);
		  paragraph << sentence;
		  break;
		}
	  case 2:
		{
		  Sentence sentence;

		  const WeatherResult direction =
			forecaster.analyze(itsVar+"::fake::speed::days1-2::mean",
							   itsSources,
							   WindDirection,
							   Mean,
							   Mean,
							   Mean,
							   itsArea,
							   periodgenerator);
	
		  const WindDirectionAccuracy accuracy = direction_accuracy(direction.error(),itsVar);

		  const bool similar_speeds = similar_speed_range(minspeeds[0],
														  maxspeeds[0],
														  minspeeds[1],
														  maxspeeds[1],
														  itsVar);

		  if(accuracy != bad_accuracy ||
			 (accuracies[0] == bad_accuracy &&
			  accuracies[1] == bad_accuracy))
			{
			  if(similar_speeds)
				{
				  sentence << directed_speed_sentence(minspeed(minspeeds[0],minspeeds[1]),
													  maxspeed(maxspeeds[0],maxspeeds[1]),
													  meanspeed(meanspeeds[0],meanspeeds[1]),
													  direction,
													  itsVar);
				}
			  else
				{
				  sentence << PeriodPhraseFactory::create("today",
														  itsVar,
														  itsForecastTime,
														  periods[0])
						   << directed_speed_sentence(minspeeds[0],
													  maxspeeds[0],
													  meanspeeds[0],
													  directions[0],
													  itsVar)
						   << PeriodPhraseFactory::create("next_day",
														  itsVar,
														  itsForecastTime,
														  periods[1])
						   << speed_range_sentence(minspeeds[1],
												   maxspeeds[1],
												   meanspeeds[1],
												   itsVar);
				}
			}
		  else
			{
			  sentence << PeriodPhraseFactory::create("today",
													  itsVar,
													  itsForecastTime,
													  periods[0])
					   << directed_speed_sentence(minspeeds[0],
												  maxspeeds[0],
												  meanspeeds[0],
												  directions[0],
												  itsVar)
					   << Delimiter(",")
					   << PeriodPhraseFactory::create("next_day",
													  itsVar,
													  itsForecastTime,
													  periods[1])
					   << speed_range_sentence(minspeeds[1],
											   maxspeeds[1],
											   meanspeeds[1],
											   itsVar);
			  if(similar_speeds)
				{
				  sentence << direction_sentence(directions[1],itsVar);
				}
			  else
				{
				  sentence << directed_speed_sentence(minspeeds[1],
													  maxspeeds[1],
													  meanspeeds[1],
													  directions[1],
													  itsVar);
				}
			}
		  paragraph << sentence;
		  break;
		}
	  default:
		{
		  Sentence sentence;

		  // various useful subperiods

		  const WeatherPeriod days13(periods[0].localStartTime(),
									 periods[2].localEndTime());
		  const WeatherPeriod days12(periods[0].localStartTime(),
									 periods[1].localEndTime());
		  const WeatherPeriod days23(periods[1].localStartTime(),
									 periods[2].localEndTime());

		  const WeatherResult direction13 =
			forecaster.analyze(itsVar+"::fake::speed::days1-3::mean",
							   itsSources,
							   WindDirection,
							   Mean,
							   Mean,
							   Mean,
							   itsArea,
							   HourPeriodGenerator(days13,itsVar));

		  const WeatherResult direction12 =
			forecaster.analyze(itsVar+"::fake::speed::days1-2::mean",
							   itsSources,
							   WindDirection,
							   Mean,
							   Mean,
							   Mean,
							   itsArea,
							   HourPeriodGenerator(days12,itsVar));

		  const WeatherResult direction23 =
			forecaster.analyze(itsVar+"::fake::speed::days2-3::mean",
							   itsSources,
							   WindDirection,
							   Mean,
							   Mean,
							   Mean,
							   itsArea,
							   HourPeriodGenerator(days23,itsVar));

		  const WindDirectionAccuracy accuracy13 = direction_accuracy(direction13.error(),itsVar);
		  const WindDirectionAccuracy accuracy12 = direction_accuracy(direction12.error(),itsVar);
		  const WindDirectionAccuracy accuracy23 = direction_accuracy(direction23.error(),itsVar);

		  const bool similar_speeds12 = similar_speed_range(minspeeds[0],
															maxspeeds[0],
															minspeeds[1],
															maxspeeds[1],
															itsVar);

		  const bool similar_speeds23 = similar_speed_range(minspeeds[1],
															maxspeeds[1],
															minspeeds[2],
															maxspeeds[2],
															itsVar);
		  // All pairs 1&2,2&3 and 1&3 must be similar
		  const bool similar_speeds13 = (similar_speeds12 &&
										 similar_speeds23 &&
										 similar_speed_range(minspeeds[0],
															 maxspeeds[0],
															 minspeeds[2],
															 maxspeeds[2],
															 itsVar));

		  if(accuracy13 != bad_accuracy ||
			 (accuracies[0] == bad_accuracy &&
			  accuracies[1] == bad_accuracy &&
			  accuracies[2] == bad_accuracy))
			{
			  if(!similar_speeds12)
				{
				sentence << PeriodPhraseFactory::create("today",
														itsVar,
														itsForecastTime,
														periods[0])
						 << directed_speed_sentence(minspeeds[0],
													maxspeeds[0],
													meanspeeds[0],
													direction13,
													itsVar);
				}
			  else if(!similar_speeds13)
				{
				  sentence << directed_speed_sentence(minspeed(minspeeds[0],minspeeds[1]),
													  maxspeed(maxspeeds[0],maxspeeds[1]),
													  meanspeed(meanspeeds[0],meanspeeds[1]),
													  direction13,
													  itsVar);
				}
			  else
				{
				  sentence << directed_speed_sentence(minspeed(minspeeds[0],minspeeds[1],minspeeds[2]),
													  maxspeed(maxspeeds[0],maxspeeds[1],maxspeeds[2]),
													  meanspeed(meanspeeds[0],meanspeeds[1],maxspeeds[2]),
													  direction13,
													  itsVar);
				}

			  if(!similar_speeds13)
				{
				  sentence << Delimiter(",");
				  if(similar_speeds12)
					sentence << on_weekday(periods[2].localStartTime());
				  else if(similar_speeds23)
					sentence << PeriodPhraseFactory::create("next_days",
															itsVar,
															itsForecastTime,
															periods[1]);
				  else
					sentence << PeriodPhraseFactory::create("next_day",
															itsVar,
															itsForecastTime,
															periods[1]);
				  if(similar_speeds23)
					sentence << speed_range_sentence(minspeed(minspeeds[1],minspeeds[2]),
													 maxspeed(maxspeeds[1],maxspeeds[2]),
													 meanspeed(meanspeeds[1],meanspeeds[2]),
													 itsVar);
				  else
					sentence << speed_range_sentence(minspeeds[2],
													 maxspeeds[2],
													 meanspeeds[2],
													 itsVar);
				}
			}
		  else if(accuracy12 != bad_accuracy ||
				  (accuracies[0] == bad_accuracy &&
				   accuracies[1] == bad_accuracy))
			{
			  if(!similar_speeds12)
				{
				sentence << PeriodPhraseFactory::create("today",
														itsVar,
														itsForecastTime,
														periods[0])
						 << directed_speed_sentence(minspeeds[0],
													maxspeeds[0],
													meanspeeds[0],
													direction12,
													itsVar);
				}
			  else if(!similar_speeds13)
				{
				  sentence << directed_speed_sentence(minspeed(minspeeds[0],minspeeds[1]),
													  maxspeed(maxspeeds[0],maxspeeds[1]),
													  meanspeed(meanspeeds[0],meanspeeds[1]),
													  direction12,
													  itsVar);
				}
			  else
				{
				  sentence << directed_speed_sentence(minspeed(minspeeds[0],minspeeds[1],minspeeds[2]),
													  maxspeed(maxspeeds[0],maxspeeds[1],maxspeeds[2]),
													  meanspeed(meanspeeds[0],meanspeeds[1],maxspeeds[2]),
													  direction12,
													  itsVar);
				}

			  // second day
			  if(!similar_speeds12)
				{
				  sentence << Delimiter(",")				  
						   << PeriodPhraseFactory::create("next_day",
														  itsVar,
														  itsForecastTime,
														  periods[1]);
				  if(!similar_speeds23)
					sentence << speed_range_sentence(minspeeds[1],
													 maxspeeds[1],
													 meanspeeds[1],
													 itsVar);
				  else
					sentence << speed_range_sentence(minspeed(minspeeds[1],minspeeds[2]),
													 maxspeed(maxspeeds[1],maxspeeds[2]),
													 meanspeed(meanspeeds[1],meanspeeds[2]),
													 itsVar);
				}

			  // third day
			  sentence << Delimiter(",");
			  if(!similar_speeds12)
				sentence << on_weekday(periods[2].localStartTime());
			  else
				sentence << PeriodPhraseFactory::create("next_day",
														itsVar,
														itsForecastTime,
														periods[2]);
			  if(similar_speeds23)
				sentence << direction_sentence(directions[2],itsVar);
			  else
				sentence << directed_speed_sentence(minspeeds[2],
													maxspeeds[2],
													meanspeeds[2],
													directions[2],
													itsVar);
			}
		  else if(accuracy23 != bad_accuracy ||
				  (accuracies[1] == bad_accuracy &&
				   accuracies[2] == bad_accuracy))
			{
			  sentence << PeriodPhraseFactory::create("today",
													  itsVar,
													  itsForecastTime,
													  periods[0]);
			  if(!similar_speeds12)
				sentence << directed_speed_sentence(minspeeds[0],
													maxspeeds[0],
													meanspeeds[0],
													directions[0],
													itsVar);
			  else if(!similar_speeds13)
				sentence << directed_speed_sentence(minspeed(minspeeds[0],minspeeds[1]),
													maxspeed(maxspeeds[0],maxspeeds[1]),
													meanspeed(meanspeeds[0],meanspeeds[1]),
													directions[0],
													itsVar);
			  else
				sentence << directed_speed_sentence(minspeed(minspeeds[0],minspeeds[1],minspeeds[2]),
													maxspeed(maxspeeds[0],maxspeeds[1],maxspeeds[2]),
													meanspeed(meanspeeds[0],meanspeeds[1],maxspeeds[2]),
													directions[0],
													itsVar);

			  sentence << Delimiter(",");
			  if(similar_speeds23)
				sentence << PeriodPhraseFactory::create("next_days",
														itsVar,
														itsForecastTime,
														periods[1]);
			  else
				sentence << PeriodPhraseFactory::create("next_day",
														itsVar,
														itsForecastTime,
														periods[1]);
			  sentence << direction_sentence(direction23,itsVar);
			  
			  if(!similar_speeds12)
				{
				  if(similar_speeds23)
					sentence << speed_range_sentence(minspeed(minspeeds[1],minspeeds[2]),
													 maxspeed(maxspeeds[1],maxspeeds[2]),
													 meanspeed(meanspeeds[1],meanspeeds[2]),
													 itsVar);
				  else
					sentence << speed_range_sentence(minspeeds[1],
													 maxspeeds[1],
													 meanspeeds[1],
													 itsVar);
				}
				
			  
			  if(!similar_speeds23)
				{
				  sentence << PeriodPhraseFactory::create("next_day",
														  itsVar,
														  itsForecastTime,
														  periods[2])
						   << speed_range_sentence(minspeeds[2],
												   maxspeeds[2],
												   meanspeeds[2],
												   itsVar);
				  
				}
			}
		  else
			{
			  sentence << PeriodPhraseFactory::create("today",
													  itsVar,
													  itsForecastTime,
													  periods[0]);
			  sentence << direction_sentence(directions[0],itsVar);

			  if(similar_speeds13)
				sentence << speed_range_sentence(minspeed(minspeeds[0],minspeeds[1],minspeeds[2]),
												 maxspeed(maxspeeds[0],maxspeeds[1],maxspeeds[2]),
												 meanspeed(meanspeeds[0],meanspeeds[1],meanspeeds[2]),
												 itsVar);
			  else if(similar_speeds12)
				sentence << speed_range_sentence(minspeed(minspeeds[0],minspeeds[1]),
												 maxspeed(maxspeeds[0],maxspeeds[1]),
												 meanspeed(meanspeeds[0],meanspeeds[1]),
												 itsVar);
			  else
				sentence << speed_range_sentence(minspeeds[0],
												 maxspeeds[0],
												 meanspeeds[0],
												 itsVar);
			  
			  
			  sentence << Delimiter(",");
			  sentence << PeriodPhraseFactory::create("next_day",
													  itsVar,
													  itsForecastTime,
													  periods[1]);
			  sentence << direction_sentence(directions[1],itsVar);

			  if(!similar_speeds12)
				{
				  if(similar_speeds23)
					sentence << speed_range_sentence(minspeed(minspeeds[1],minspeeds[2]),
													 maxspeed(maxspeeds[1],maxspeeds[2]),
													 meanspeed(meanspeeds[1],meanspeeds[2]),
													 itsVar);
				  else
					sentence << speed_range_sentence(minspeeds[1],
													 maxspeeds[1],
													 meanspeeds[1],
													 itsVar);
				}

			  sentence << Delimiter(",");
			  sentence << PeriodPhraseFactory::create("next_day",
													  itsVar,
													  itsForecastTime,
													  periods[2]);
			  sentence << direction_sentence(directions[2],itsVar);
			  if(!similar_speeds23)
				sentence << speed_range_sentence(minspeeds[2],
												 maxspeeds[2],
												 meanspeeds[2],
												 itsVar);
			}

		  paragraph << sentence;
		}
	  }
	

	log << paragraph;
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
