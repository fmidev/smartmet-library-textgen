// ======================================================================
/*!
 * \file
 *�\brief Implementation of method TextGen::PrecipitationStory::twodays
 */
// ======================================================================

#include "PrecipitationStory.h"
#include "DefaultAcceptor.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "Integer.h"
#include "MathTools.h"
#include "MessageLogger.h"
#include "NullPeriodGenerator.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "UnitFactory.h"
#include "WeatherPeriodTools.h"
#include "WeatherResult.h"
#include "PeriodPhraseFactory.h"

#include "boost/lexical_cast.hpp"

using namespace WeatherAnalysis;
using namespace boost;
using namespace std;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on maximum probability of precipitation
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  
  Paragraph PrecipitationStory::pop_twodays() const
  {
	MessageLogger log("PrecipitationStory::pop_twodays");

	using MathTools::to_precision;

	Paragraph paragraph;

	const int starthour = Settings::optional_hour(itsVar+"::day::starthour",0);
	const int endhour = Settings::optional_hour(itsVar+"::day::endhour",0);

	const int maxstarthour = Settings::optional_hour(itsVar+"::day::maxstarthour",starthour);
	const int minendhour   = Settings::optional_hour(itsVar+"::day::minendhour",endhour);

	const int limit     = Settings::require_percentage(itsVar+"::limit");
	const int precision = Settings::require_percentage(itsVar+"::precision");

	const int limit_significantly_greater = Settings::require_percentage(itsVar+"::comparison::significantly_greater");
	const int limit_significantly_smaller = Settings::require_percentage(itsVar+"::comparison::significantly_smaller");
	const int limit_greater = Settings::require_percentage(itsVar+"::comparison::greater");
	const int limit_smaller = Settings::require_percentage(itsVar+"::comparison::smaller");
	const int limit_somewhat_greater = Settings::require_percentage(itsVar+"::comparison::somewhat_greater");
	const int limit_somewhat_smaller = Settings::require_percentage(itsVar+"::comparison::somewhat_smaller");

	const int days = WeatherPeriodTools::countPeriods(itsPeriod,
													  starthour,
													  endhour,
													  maxstarthour,
													  minendhour);

	WeatherPeriod firstperiod = WeatherPeriodTools::getPeriod(itsPeriod,
															  1,
															  starthour,
															  endhour,
															  maxstarthour,
															  minendhour);

	GridForecaster forecaster;

	WeatherResult result = forecaster.analyze(itsVar+"::fake::day1::maximum",
											  itsSources,
											  PrecipitationProbability,
											  Maximum,
											  Maximum,
											  itsArea,
											  firstperiod);
	
	if(result.value() == kFloatMissing)
	  throw TextGenError("PrecipitationProbability not available");

	log << "PoP Maximum(Maximum) for day 1 " << result << endl;

	const int pop1 = to_precision(result.value(),precision);

	Sentence sentence;

	if(pop1 >= limit)
	  {
		sentence << "sateen todenn�k�isyys"
				 << "on"
				 << PeriodPhraseFactory::create("today",
												itsVar,
												itsForecastTime,
												firstperiod)
				 << Integer(pop1)
				 << *UnitFactory::create(Percent);
	  }

	if(days==2)
	  {
		WeatherPeriod secondperiod = WeatherPeriodTools::getPeriod(itsPeriod,
																   2,
																   starthour,
																   endhour,
																   maxstarthour,
																   minendhour);

		WeatherResult result2 = forecaster.analyze(itsVar+"::fake::day2::maximum",
												   itsSources,
												   PrecipitationProbability,
												   Maximum,
												   Maximum,
												   itsArea,
												   secondperiod);
		
		if(result2.value() == kFloatMissing)
		  throw TextGenError("PrecipitationProbability not available");

		log << "PoP Maximum(Maximum) for day 2 " << result2 << endl;

		const int pop2 = to_precision(result2.value(),precision);

		if(pop2 >= limit)
		  {
			if(sentence.empty())
			  {
				sentence << "sateen todenn�k�isyys"
						 << "on"
						 << PeriodPhraseFactory::create("today",
														itsVar,
														itsForecastTime,
														secondperiod)
						 << Integer(pop2)
						 << *UnitFactory::create(Percent);
			  }
			else
			  {
				sentence << Delimiter(",")
						 << PeriodPhraseFactory::create("next_day",
														itsVar,
														itsForecastTime,
														secondperiod);
				if(pop2 - pop1 >= limit_significantly_greater)
				  sentence << "huomattavasti suurempi";
				else if(pop2 - pop1 >= limit_greater)
				  sentence << "suurempi";
				else if(pop2 - pop1 >= limit_somewhat_greater)
				  sentence << "hieman suurempi";
				else if(pop1 - pop2 >= limit_significantly_smaller)
				  sentence << "huomattavasti pienempi";
				else if(pop1 - pop2 >= limit_smaller)
				  sentence << "pienempi";
				else if(pop1 - pop2 >= limit_somewhat_smaller)
				  sentence << "hieman pienempi";
				else
				  sentence << "sama";
			  }
		  }
	  }

	paragraph << sentence;
	log << paragraph;
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
  