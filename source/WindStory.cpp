// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::WindStory
 */
// ======================================================================

#include "WindStory.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "IntegerRange.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "TimeTools.h"
#include "UnitFactory.h"
#include "WeatherPeriod.h"
#include "WeatherPeriodTools.h"
#include "WeatherResult.h"

#include "boost/lexical_cast.hpp"

using namespace WeatherAnalysis;
using namespace boost;
using namespace std;

namespace
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Wind direction accuracy class
   */
  // ----------------------------------------------------------------------
   
  enum WindDirectionAccuracy
	{
	  good_accuracy,
	  moderate_accuracy,
	  bad_accuracy
	};

  // ----------------------------------------------------------------------
  /*!
   * \brief Calculate the wind direction accuracy class
   *
   * \param theAccuracy The accuracy
   * \param theVariable The control variable
   * \return The accuracy class
   */
  // ----------------------------------------------------------------------

  WindDirectionAccuracy direction_accuracy(double theAccuracy,
										   const string & theVariable)
  {
	using namespace Settings;
	
	const double accurate_limit = optional_double(theVariable+"::direction::accurate",0.8);
	const double variable_limit = optional_double(theVariable+"::direction::variable_limit",0.4);
	
	if(theAccuracy>=accurate_limit) return good_accuracy;
	if(theAccuracy>=variable_limit) return moderate_accuracy;
	return bad_accuracy;

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Calculate the generic wind 8th direction from angle
   *
   * \param theDirection The direction value
   * \return The direction in units of 1-8 (north ... north-west)
   */
  // ----------------------------------------------------------------------

  int direction8th(double theDirection)
  {
	return 1+(int(theDirection/45.0+0.5) % 8);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return a sentence on wind direction
   *
   * \param theDirection The direction
   * \param theVariable The control variable
   * \return The sentence
   */
  // ----------------------------------------------------------------------

  TextGen::Sentence
  direction_sentence(const WeatherResult & theDirection,
						  const string & theVariable)
  {
	TextGen::Sentence sentence;

	const int n = direction8th(theDirection.value());
	switch(direction_accuracy(theDirection.accuracy(),theVariable))
	  {
	  case good_accuracy:
		sentence << lexical_cast<string>(n)+"-tuulta";
		break;
	  case moderate_accuracy:
		sentence << lexical_cast<string>(n)+"-puoleista tuulta";
			break;
	  case bad_accuracy:
		sentence << "suunnaltaan vaihtelevaa" << "tuulta";
		break;
	  }
	return sentence;
  }


  // ----------------------------------------------------------------------
  /*!
   * \brief Return a sentence on wind speed range
   *
   * \param theMinSpeed The minimum speed
   * \param theMaxSpeed The maximum speed
   * \param theVariable The control variable
   * \return The sentence
   */
  // ----------------------------------------------------------------------

  TextGen::Sentence
  speed_range_sentence(const WeatherResult & theMinSpeed,
					   const WeatherResult & theMaxSpeed,
					   const string & theVariable)
  {
	using namespace TextGen;

	Sentence sentence;

	IntegerRange range(FmiRound(theMinSpeed.value()),
					   FmiRound(theMaxSpeed.value()));

	sentence << range << *UnitFactory::create(MetersPerSecond);

	return sentence;

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return a sentence on wind speed and direction
   *
   * The sentence is of form "[suuntatuulta] X...Y m/s"
   *
   * \param theMinSpeed The minimum speed
   * \param theMaxSpeed The maximum speed
   * \param theDirection The direction
   * \param theVariable The control variable
   * \return The sentence
   */
  // ----------------------------------------------------------------------

  TextGen::Sentence
  directed_speed_sentence(const WeatherResult & theMinSpeed,
						  const WeatherResult & theMaxSpeed,
						  const WeatherResult & theDirection,
						  const string & theVariable)
  {
	TextGen::Sentence sentence;

	sentence << direction_sentence(theDirection,theVariable)
			 << speed_range_sentence(theMinSpeed,theMaxSpeed,theVariable);

	return sentence;
	
  }

} // namespace anonymous


namespace TextGen
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------
  
  WindStory::~WindStory()
  {
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theForecastTime The forecast time
   * \param theSources The analysis sources
   * \param theArea The area to be analyzed 
   * \param thePeriod The period to be analyzed
   * \param theVariable The associated configuration variable
  */
  // ----------------------------------------------------------------------
  
  WindStory::WindStory(const NFmiTime & theForecastTime,
					   const AnalysisSources & theSources,
					   const WeatherArea & theArea,
					   const WeatherPeriod & thePeriod,
					   const string & theVariable)
	: itsForecastTime(theForecastTime)
	, itsSources(theSources)
	, itsArea(theArea)
	, itsPeriod(thePeriod)
	, itsVar(theVariable)
  {
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Test whether the given story is known to this class
   *
   * \param theName The story name
   * \return True if this class can generate the story
   */
  // ----------------------------------------------------------------------
  
  bool WindStory::hasStory(const string & theName)
  {
	if(theName == "wind_overview")
	  return true;
	if(theName == "wind_simple_overview")
	  return true;
	return false;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Generate the desired story
   *
   * Throws if the story name is not recognized.
   *
   * \param theName The story name
   * \return Paragraph containing the story
   */
  // ----------------------------------------------------------------------.
  
  Paragraph WindStory::makeStory(const string & theName) const
  {
	if(theName == "wind_overview")
	  return overview();
	if(theName == "wind_simple_overview")
	  return simple_overview();
	throw TextGenError("WindStory cannot make story "+theName);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return wind short_overview story
   *
   * \return Paragraph containing the story
   */
  // ----------------------------------------------------------------------

  Paragraph WindStory::simple_overview() const
  {
	MessageLogger log("WeatherStory::short_overview");

	// Establish options

	using namespace Settings;

	const int day_starthour = optional_hour(itsVar+"::day::starthour",6);
	const int day_maxstarthour = optional_hour(itsVar+"::day::maxstarthour",day_starthour);
	const int night_starthour = optional_hour(itsVar+"::night::starthour",18);
	const int night_maxstarthour = optional_hour(itsVar+"::night::maxstarthour",night_starthour);
	const int night_endhour = optional_hour(itsVar+"::night::endhour",6);
	const int night_minendhour = optional_hour(itsVar+"::night::minendhour",night_endhour);

	// Generate the story

	Paragraph paragraph;

	GridForecaster forecaster;

	// The period until next morning should always be possible
	
	const int starthour = itsPeriod.localStartTime().GetHour();
	WeatherPeriod morning = WeatherPeriodTools::getPeriod(itsPeriod,
														  1,
														  starthour,
														  night_endhour,
														  starthour,
														  night_minendhour);

	if(itsPeriod.localStartTime().GetHour() > day_maxstarthour)
	  {
		// Story until next morning

		Sentence sentence;

		sentence << PeriodPhraseFactory::create("until_morning",
												itsVar,
												itsForecastTime,
												morning);

		WeatherResult minspeed
		  = forecaster.analyze(itsVar+"::fake::until_morning::speed::minimum",
							   itsSources,
							   WindSpeed,
							   Minimum,
							   Mean,
							   itsArea,
							   morning);

		WeatherResult maxspeed
		  = forecaster.analyze(itsVar+"::fake::until_morning::speed::maximum",
							   itsSources,
							   WindSpeed,
							   Maximum,
							   Mean,
							   itsArea,
							   morning);

		WeatherResult direction
		  = forecaster.analyze(itsVar+"::fake::until_morning::direction",
							   itsSources,
							   WindDirection,
							   Mean,
							   Mean,
							   itsArea,
							   morning);

		sentence << directed_speed_sentence(minspeed,
											maxspeed,
											direction,
											itsVar);
		paragraph << sentence;
	  }
	else
	  {
		// First until night, then until morning

		Sentence sentence;

		// from now until night
		WeatherPeriod day = WeatherPeriodTools::getPeriod(itsPeriod,
														  1,
														  starthour,
														  night_starthour);


		WeatherResult minspeed
		  = forecaster.analyze(itsVar+"::fake::until_tonight::speed::minimum",
							   itsSources,
							   WindSpeed,
							   Minimum,
							   Mean,
							   itsArea,
							   day);

		WeatherResult maxspeed
		  = forecaster.analyze(itsVar+"::fake::until_tonight::speed::maximum",
							   itsSources,
							   WindSpeed,
							   Maximum,
							   Mean,
							   itsArea,
							   day);

		WeatherResult direction
		  = forecaster.analyze(itsVar+"::fake::until_tonight::direction",
							   itsSources,
							   WindDirection,
							   Mean,
							   Mean,
							   itsArea,
							   day);

		sentence << PeriodPhraseFactory::create("until_tonight",
												itsVar,
												itsForecastTime,
												day);
		
		sentence << directed_speed_sentence(minspeed,
											maxspeed,
											direction,
											itsVar);


		sentence << Delimiter(",");
		
		// next night
		WeatherPeriod night = WeatherPeriodTools::getPeriod(itsPeriod,
															1,
															night_starthour,
															night_endhour,
															night_maxstarthour,
															night_minendhour);


		minspeed
		  = forecaster.analyze(itsVar+"::fake::tonight::speed::minimum",
							   itsSources,
							   WindSpeed,
							   Minimum,
							   Mean,
							   itsArea,
							   night);

		maxspeed
		  = forecaster.analyze(itsVar+"::fake::tonight::speed::maximum",
							   itsSources,
							   WindSpeed,
							   Maximum,
							   Mean,
							   itsArea,
							   night);

		direction
		  = forecaster.analyze(itsVar+"::fake::tonight::direction",
							   itsSources,
							   WindDirection,
							   Mean,
							   Mean,
							   itsArea,
							   night);

		sentence << PeriodPhraseFactory::create("tonight",
												itsVar,
												itsForecastTime,
												night);

		sentence << directed_speed_sentence(minspeed,
											maxspeed,
											direction,
											itsVar);

		paragraph << sentence;

	  }

	// Remaining time after the morning

	if(morning.localEndTime().IsLessThan(itsPeriod.localEndTime()))
	  {
		const WeatherPeriod remainder(morning.localEndTime(),
									  itsPeriod.localEndTime());

		WeatherResult minspeed
		  = forecaster.analyze(itsVar+"::fake::remaining_days::speed::minimum",
							   itsSources,
							   WindSpeed,
							   Minimum,
							   Mean,
							   itsArea,
							   remainder);

		WeatherResult maxspeed
		  = forecaster.analyze(itsVar+"::fake::remaining_days::speed::maximum",
							   itsSources,
							   WindSpeed,
							   Maximum,
							   Mean,
							   itsArea,
							   remainder);

		WeatherResult direction
		  = forecaster.analyze(itsVar+"::fake::remaining_days::direction",
							   itsSources,
							   WindDirection,
							   Mean,
							   Mean,
							   itsArea,
							   remainder);

		Sentence sentence;
		sentence << PeriodPhraseFactory::create("remaining_days",
												itsVar,
												itsForecastTime,
												remainder);

		sentence << directed_speed_sentence(minspeed,
											maxspeed,
											direction,
											itsVar);

		paragraph << sentence;

	  }

	log << paragraph;
	return paragraph;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return wind overview story
   *
   * \return Paragraph containing the story
   */
  // ----------------------------------------------------------------------

  Paragraph WindStory::overview() const
  {
	MessageLogger log("WeatherStory::overview");

	Paragraph paragraph;
	log << paragraph;
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
