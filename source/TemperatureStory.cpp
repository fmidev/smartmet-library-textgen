// ======================================================================
/*!
 * \file
 * \brief Implementation of class TemperatureStory
 */
// ======================================================================

#include "TemperatureStory.h"
#include "DefaultAcceptor.h"
#include "GridForecaster.h"
#include "Number.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "WeatherFunction.h"
#include "WeatherParameter.h"
#include "WeatherPeriodTools.h"
#include "WeatherResult.h"

using namespace WeatherAnalysis;
using namespace std;

namespace TextGen
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------
  
  TemperatureStory::~TemperatureStory()
  {
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theSources The analysis sources
   * \param theArea The area to be analyzed 
   * \param thePeriod The period to be analyzed
   * \param theVariable The associated configuration variable
  */
  // ----------------------------------------------------------------------
  
  TemperatureStory::TemperatureStory(const AnalysisSources & theSources,
									 const WeatherArea & theArea,
									 const WeatherPeriod & thePeriod,
									 const string & theVariable)
	: itsSources(theSources)
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
  
  bool TemperatureStory::hasStory(const string & theName)
  {
	if(theName == "temperature_mean")
	  return true;
	if(theName == "temperature_meanmax")
	  return true;
	if(theName == "temperature_meanmin")
	  return true;
	if(theName == "temperature_dailymax")
	  return true;
	if(theName == "temperature_nightlymin")
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
  
  Paragraph TemperatureStory::makeStory(const string & theName) const
  {
	if(theName == "temperature_mean")
	  return mean();
	if(theName == "temperature_meanmax")
	  return meanmax();
	if(theName == "temperature_meanmin")
	  return meanmin();
	if(theName == "temperature_dailymax")
	  return dailymax();
	if(theName == "temperature_nightlymin")
	  return nightlymin();

	throw TextGenError("TemperatureStory cannot make story "+theName);

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on mean temperature
   *
   * \return The generated paragraph
   *
   * \todo Is throwing the best way to handle missing results?
   *
   * \see page_temperature_mean
   */
  // ----------------------------------------------------------------------
  
  Paragraph TemperatureStory::mean() const
  {
	Paragraph paragraph;
	Sentence sentence;

	GridForecaster forecaster;

	WeatherResult result = forecaster.analyze(itsVar+"::fake::mean",
											  itsSources,
											  Temperature,
											  Mean,
											  Mean,
											  itsPeriod,
											  itsArea);

	if(result.value() == kFloatMissing)
	  throw TextGenError("Mean temperature not available");

	Number<int> num = FmiRound(result.value());

	sentence << "keskilämpötila"
			 << num
			 << "astetta";
	
	paragraph << sentence;
	return paragraph;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on mean maximum temperature
   *
   * Throws if the weather period does not evenly divide into 24 hour
   * segments.
   *
   * \return The story
   *
   * \see page_temperature_meanmax
   */
  // ----------------------------------------------------------------------

  Paragraph TemperatureStory::meanmax() const
  {
	Paragraph paragraph;
	Sentence sentence;

	GridForecaster forecaster;

	WeatherResult result = forecaster.analyze(itsVar+"::fake::mean",
											  itsSources,
											  MaxTemperature,
											  Mean,
											  Mean,
											  itsPeriod,
											  itsArea);

	if(result.value() == kFloatMissing)
	  throw TextGenError("Mean daily maximum temperature not available");

	Number<int> num = FmiRound(result.value());

	sentence << "keskimääräinen ylin lämpötila"
			 << num
			 << "astetta";
	paragraph << sentence;
	return paragraph;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on mean minimum temperature
   *
   * Throws if the weather period does not evenly divide into 24 hour
   * segments.
   *
   * \return The story
   *
   * \see page_temperature_meanmin
   */
  // ----------------------------------------------------------------------

  Paragraph TemperatureStory::meanmin() const
  {
	Paragraph paragraph;
	Sentence sentence;

	GridForecaster forecaster;

	WeatherResult result = forecaster.analyze(itsVar+"::fake::mean",
											  itsSources,
											  MinTemperature,
											  Mean,
											  Mean,
											  itsPeriod,
											  itsArea);

	if(result.value() == kFloatMissing)
	  throw TextGenError("Mean daily minimum temperature not available");

	Number<int> num = FmiRound(result.value());

	sentence << "keskimääräinen alin lämpötila"
			 << num
			 << "astetta";
	paragraph << sentence;
	return paragraph;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on daily maximum temperature
   *
   * \return The story
   *
   * \see page_temperature_dailymax
   */
  // ----------------------------------------------------------------------

  Paragraph TemperatureStory::dailymax() const
  {
	using namespace Settings;
	using namespace WeatherPeriodTools;

	Paragraph paragraph;

#if 0
	const int starthour    = require_hour(itsVar+"::starthour");
	const int endhour      = require_hour(itsVar+"::endhour");
	const int maxstarthour = optional_hour(itsVar+"::maxstarthour",starthour);
	const int minendhour   = optional_hour(itsVar+"::minendhour",endhour);

	const int mininterval  = optional_int(itsVar+"::mininterval",1);
	const bool interval_zero = optional_bool(itsVar+"::always_interval_zero",false);

	const bool prefer_dayname = optional_bool(itsVar+"::prefer_dayname",false);

	const int limit_significantly_greater = require_percentage(itsVar+"::significantly_greater");
	const int limit_significantly_smaller = require_percentage(itsVar+"::significantly_smaller");
	const int limit_greater = require_percentage(itsVar+"::greater");
	const int limit_smaller = require_percentage(itsVar+"::smaller");
	const int limit_somewhat_greater = require_percentage(itsVar+"::somewhat_greater");
	const int limit_somewhat_smaller = require_percentage(itsVar+"::somewhat_smaller");

	const int days = countPeriods(itsPeriod,
								  starthour,
								  endhour,
								  maxstarthour,
								  minendhour);


	WeatherPeriod period = getperiod(itsPeriod,
									 1,
									 starthour,
									 endhour,
									 maxstarthour,
									 minendhour);


	Sentence sentence;
	sentence << "päivän ylin lämpötila"
			 << "on";
	  
	paragraph << sentence;
#endif
	return paragraph;

  }


  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on nightly minimum temperature
   *
   * \return The story
   *
   * \see page_temperature_nightlymin
   */
  // ----------------------------------------------------------------------

  Paragraph TemperatureStory::nightlymin() const
  {
	using namespace Settings;
	using namespace WeatherPeriodTools;

	Paragraph paragraph;

#if 0
	const int starthour    = require_hour(itsVar+"::starthour");
	const int endhour      = require_hour(itsVar+"::endhour");
	const int maxstarthour = optional_hour(itsVar+"::maxstarthour",starthour);
	const int minendhour   = optional_hour(itsVar+"::minendhour",endhour);

	const int mininterval  = optional_int(itsVar+"::mininterval",1);
	const bool interval_zero = optional_bool(itsVar+"::always_interval_zero",false);

	const bool prefer_dayname = optional_bool(itsVar+"::prefer_dayname",false);

	const int limit_significantly_greater = require_percentage(itsVar+"::significantly_greater");
	const int limit_significantly_smaller = require_percentage(itsVar+"::significantly_smaller");
	const int limit_greater = require_percentage(itsVar+"::greater");
	const int limit_smaller = require_percentage(itsVar+"::smaller");
	const int limit_somewhat_greater = require_percentage(itsVar+"::somewhat_greater");
	const int limit_somewhat_smaller = require_percentage(itsVar+"::somewhat_smaller");

	const int days = countPeriods(itsPeriod,
								  starthour,
								  endhour,
								  maxstarthour,
								  minendhour);


	WeatherPeriod period = getperiod(itsPeriod,
									 1,
									 starthour,
									 endhour,
									 maxstarthour,
									 minendhour);


	Sentence sentence;
	sentence << "yön alin lämpötila"
			 << "on";
	  

	paragraph << sentence;
#endif
	return paragraph;

  }

} // namespace TextGen
  
// ======================================================================
  
