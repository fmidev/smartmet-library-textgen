// ======================================================================
/*!
 * \file
 * \brief Implementation of class FrostStory
 */
// ======================================================================

#include "FrostStory.h"
#include "DefaultAcceptor.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "MathTools.h"
#include "NullPeriodGenerator.h"
#include "Number.h"
#include "NumberRange.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "WeatherFunction.h"
#include "WeatherParameter.h"
#include "WeatherPeriodTools.h"
#include "WeatherResult.h"
#include "WeekdayTools.h"

#include "boost/lexical_cast.hpp"

using namespace WeatherAnalysis;
using namespace std;
using namespace boost;

namespace
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Return sentence for severe frost
   *
   * \param thePeriod The night period
   * \param theProbability The probability
   * \return The sentence
   */
  // ----------------------------------------------------------------------

  TextGen::Sentence severe_frost_sentence(const WeatherPeriod & thePeriod,
										  int theProbability)
  {
	using namespace TextGen;
	Sentence sentence;
	sentence << "ankaran hallan todennäköisyys"
			 << "on"
			 << WeekdayTools::night_against_weekday(thePeriod.localEndTime())
			 << Number<int>(theProbability)
			 << Delimiter("%");
	return sentence;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return sentence for frost
   *
   * \param thePeriod The night period
   * \param theProbability The probability
   * \return The sentence
   */
  // ----------------------------------------------------------------------

  TextGen::Sentence frost_sentence(const WeatherPeriod & thePeriod,
								   int theProbability)
  {
	using namespace TextGen;
	Sentence sentence;
	sentence << "hallan todennäköisyys"
			 << "on"
			 << WeekdayTools::night_against_weekday(thePeriod.localEndTime())
			 << Number<int>(theProbability)
			 << Delimiter("%");
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
  
  FrostStory::~FrostStory()
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
  
  FrostStory::FrostStory(const NFmiTime & theForecastTime,
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
  
  bool FrostStory::hasStory(const string & theName)
  {
	if(theName == "frost_mean")
	  return true;
	if(theName == "frost_maximum")
	  return true;
	if(theName == "frost_range")
	  return true;
	if(theName == "frost_twonights")
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
  
  Paragraph FrostStory::makeStory(const string & theName) const
  {
	if(theName == "frost_mean")
	  return mean();
	if(theName == "frost_maximum")
	  return maximum();
	if(theName == "frost_range")
	  return range();
	if(theName == "frost_twonights")
	  return twonights();

	throw TextGenError("FrostStory cannot make story "+theName);

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on mean frost
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  
  Paragraph FrostStory::mean() const
  {
	using MathTools::to_precision;

	Paragraph paragraph;
	Sentence sentence;

	const string var1 = itsVar+"::precision";
	const string var2 = itsVar+"::severe_frost_limit";
	const string var3 = itsVar+"::frost_limit";

	const int precision = Settings::require_percentage(var1);
	const int severelimit = Settings::require_percentage(var2);
	const int normallimit = Settings::require_percentage(var3);

	GridForecaster forecaster;

	WeatherResult frost = forecaster.analyze(itsVar+"::fake::mean",
											 itsSources,
											 Frost,
											 Mean,
											 Maximum,
											 itsArea,
											 itsPeriod);

	if(frost.value() == kFloatMissing)
	  throw TextGenError("Frost is not available");

	// Quick exit if the mean is zero
	
	if(frost.value() == 0)
	  return paragraph;

	// Severe frost

	WeatherResult severefrost = forecaster.analyze(itsVar+"::fake::severe_mean",
												   itsSources,
												   SevereFrost,
												   Mean,
												   Maximum,
												   itsArea,
												   itsPeriod);

	if(severefrost.value() == kFloatMissing)
	  throw TextGenError("SevereFrost is not available");

	const int frost_value = to_precision(frost.value(),precision);

	const int severe_frost_value = to_precision(severefrost.value(),precision);

	if(severe_frost_value >= severelimit)
	  {
		Number<int> val(severe_frost_value);
		sentence << "ankaran hallan todennäköisyys"
				 << "on"
				 << val
				 << Delimiter("%");
		paragraph << sentence;
	  }
	else if(frost_value >= normallimit)
	  {
		Number<int> val(frost_value);
		sentence << "hallan todennäköisyys"
				 << "on"
				 << val
				 << Delimiter("%");
		paragraph << sentence;
	  }

	return paragraph;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on maximum frost
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  
  Paragraph FrostStory::maximum() const
  {
	using MathTools::to_precision;

	Paragraph paragraph;
	Sentence sentence;

	const string var1 = itsVar+"::precision";
	const string var2 = itsVar+"::severe_frost_limit";
	const string var3 = itsVar+"::frost_limit";

	const int precision = Settings::require_percentage(var1);
	const int severelimit = Settings::require_percentage(var2);
	const int normallimit = Settings::require_percentage(var3);

	GridForecaster forecaster;

	WeatherResult frost = forecaster.analyze(itsVar+"::fake::maximum",
											 itsSources,
											 Frost,
											 Maximum,
											 Maximum,
											 itsArea,
											 itsPeriod);

	if(frost.value() == kFloatMissing)
	  throw TextGenError("Frost is not available");

	// Quick exit if the mean is zero
	
	if(frost.value() == 0)
	  return paragraph;

	// Severe frost

	WeatherResult severefrost = forecaster.analyze(itsVar+"::fake::severe_maximum",
												   itsSources,
												   SevereFrost,
												   Maximum,
												   Maximum,
												   itsArea,
												   itsPeriod);

	if(severefrost.value() == kFloatMissing)
	  throw TextGenError("SevereFrost is not available");

	const int frost_value = to_precision(frost.value(),precision);

	const int severe_frost_value = to_precision(severefrost.value(),precision);

	if(severe_frost_value >= severelimit)
	  {
		Number<int> val(severe_frost_value);
		sentence << "ankaran hallan todennäköisyys"
				 << "on"
				 << val
				 << Delimiter("%");
		paragraph << sentence;
	  }
	else if(frost_value >= normallimit)
	  {
		Number<int> val(frost_value);
		sentence << "hallan todennäköisyys"
				 << "on"
				 << val
				 << Delimiter("%");
		paragraph << sentence;
	  }

	return paragraph;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on frost probability range
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  
  Paragraph FrostStory::range() const
  {
	using MathTools::to_precision;

	Paragraph paragraph;
	Sentence sentence;

	const string var1 = itsVar+"::precision";
	const string var2 = itsVar+"::severe_frost_limit";
	const string var3 = itsVar+"::frost_limit";

	const int precision = Settings::require_percentage(var1);
	const int severelimit = Settings::require_percentage(var2);
	const int normallimit = Settings::require_percentage(var3);

	GridForecaster forecaster;

	WeatherResult maxfrost = forecaster.analyze(itsVar+"::fake::maximum",
												itsSources,
												Frost,
												Maximum,
												Maximum,
												itsArea,
												itsPeriod);

	if(maxfrost.value() == kFloatMissing)
	  throw TextGenError("Maximum Frost is not available");

	// Quick exit if maximum frost probability is zero
	
	if(maxfrost.value() == 0)
	  return paragraph;

	// Minimum frost

	WeatherResult minfrost = forecaster.analyze(itsVar+"::fake::minimum",
												itsSources,
												Frost,
												Minimum,
												Maximum,
												itsArea,
												itsPeriod);

	if(minfrost.value() == kFloatMissing)
	  throw TextGenError("Minimum Frost is not available");

	// Maximum severe frost

	WeatherResult maxseverefrost = forecaster.analyze(itsVar+"::fake::severe_maximum",
													  itsSources,
													  SevereFrost,
													  Maximum,
													  Maximum,
													  itsArea,
													  itsPeriod);

	if(maxseverefrost.value() == kFloatMissing)
	  throw TextGenError("Maximum SevereFrost is not available");

	WeatherResult minseverefrost = forecaster.analyze(itsVar+"::fake::severe_minimum",
													  itsSources,
													  SevereFrost,
													  Minimum,
													  Maximum,
													  itsArea,
													  itsPeriod);

	if(minseverefrost.value() == kFloatMissing)
	  throw TextGenError("Minimum SevereFrost is not available");

	const int frost_min = to_precision(minfrost.value(),precision);
	const int frost_max = to_precision(maxfrost.value(),precision);

	const int severe_frost_min = to_precision(minseverefrost.value(),precision);
	const int severe_frost_max = to_precision(maxseverefrost.value(),precision);

	if(severe_frost_max >= severelimit)
	  {
		NumberRange<Number<int> > range(severe_frost_min,severe_frost_max);
		sentence << "ankaran hallan todennäköisyys"
				 << "on"
				 << range
				 << Delimiter("%");
		paragraph << sentence;
	  }
	else if(frost_max >= normallimit)
	  {
		NumberRange<Number<int> > range(frost_min,frost_max);
		sentence << "hallan todennäköisyys"
				 << "on"
				 << range
				 << Delimiter("%");
		paragraph << sentence;
	  }

	return paragraph;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on 1/2 night frost
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  
  Paragraph FrostStory::twonights() const
  {
	using MathTools::to_precision;

	Paragraph paragraph;

	const int starthour    = Settings::require_hour(itsVar+"::starthour");
	const int endhour      = Settings::require_hour(itsVar+"::endhour");
	const int maxstarthour = Settings::optional_hour(itsVar+"::maxstarthour",starthour);
	const int minendhour   = Settings::optional_hour(itsVar+"::minendhour",endhour);

	const int precision   = Settings::require_percentage(itsVar+"::precision");
	const int severelimit = Settings::require_percentage(itsVar+"::severe_frost_limit");
	const int normallimit = Settings::require_percentage(itsVar+"::frost_limit");

	const int nights = WeatherPeriodTools::countPeriods(itsPeriod,
														starthour,
														endhour,
														maxstarthour,
														minendhour);

	if(nights==0)
	  return paragraph;

	// Calculate frost probability

	GridForecaster forecaster;

	WeatherPeriod night1 = WeatherPeriodTools::getPeriod(itsPeriod,
														 1,
														 starthour,
														 endhour,
														 maxstarthour,
														 minendhour);

	WeatherResult frost = forecaster.analyze(itsVar+"::fake::day1::mean",
											 itsSources,
											 Frost,
											 Mean,
											 Maximum,
											 itsArea,
											 night1);

	WeatherResult severefrost = forecaster.analyze(itsVar+"::fake::day1::severe_mean",
												   itsSources,
												   SevereFrost,
												   Mean,
												   Maximum,
												   itsArea,
												   night1);
	
	if(frost.value()==kFloatMissing || severefrost.value()==kFloatMissing)
	  throw TextGenError("Frost is not available");

	if(nights==1)
	  {
		const int value = to_precision(frost.value(),precision);
		const int severevalue = to_precision(severefrost.value(),precision);

		if(severevalue >= severelimit)
		  paragraph << severe_frost_sentence(night1,severevalue);
		else if(value >= normallimit)
		  paragraph << frost_sentence(night1,value);

	  }
	else
	  {

		WeatherPeriod night2 = WeatherPeriodTools::getPeriod(itsPeriod,
															 2,
															 starthour,
															 endhour,
															 maxstarthour,
															 minendhour);

		WeatherResult frost2 = forecaster.analyze(itsVar+"::fake::day2::mean",
												  itsSources,
												  Frost,
												  Mean,
												  Maximum,
												  itsArea,
												  night2);

		WeatherResult severefrost2 = forecaster.analyze(itsVar+"::fake::day2::severe_mean",
														itsSources,
														SevereFrost,
														Mean,
														Maximum,
														itsArea,
														night2);
		
		if(frost2.value()==kFloatMissing || severefrost2.value()==kFloatMissing)
		  throw TextGenError("Frost is not available");

		const int value1 = to_precision(frost.value(),precision);
		const int severevalue1 = to_precision(severefrost.value(),precision);

		const int value2 = to_precision(frost2.value(),precision);
		const int severevalue2 = to_precision(severefrost2.value(),precision);
		
		// We have 9 combinations:
		//
		// nada+nada		""
		// nada+frost		"Hallan todennäköisyys on tiistain vastaisena yönä x%."
		// nada+severe		"Ankaran hallan todennäköisyys on tiistain vastaisena yönä x%."
		// frost+nada		"Hallan ..., seuraava yö on lämpimämpi."
		// frost+frost		"Hallan ..., seuraavana yönä y%."
		// frost+severe		"Hallan ..., seuraavana yönä ankaran hallan todennäköisyys on y%."
		// severe+nada		"Ankaran ..., seuraava yö on huomattavasti lämpimämpi."
		// severe+frost		"Ankaran ..., seuraavana yönä hallan todennäköisyys on y%."
		// severe+severe	"Ankaran ..., seuraavana yönä y%."

		if(severevalue1 >= severelimit)		// severe + ?
		  {
			Sentence sentence;
			sentence << severe_frost_sentence(night1,severevalue1)
					 << Delimiter(",");
			
			if(severevalue2 >= severelimit)
			  {
				sentence << "seuraavana yönä"
						 << Number<int>(severevalue2)
						 << Delimiter("%");
			  }
			else if(value2 >= normallimit)
			  {
				sentence << "seuraavana yönä"
						 << "hallan todennäköisyys"
						 << "on"
						 << Number<int>(value2)
						 << Delimiter("%");
			  }
			else
			  {
				sentence << "seuraava yö"
						 << "on"
						 << "huomattavasti lämpimämpi";
			  }
			paragraph << sentence;
			
		  }
		else if(value1 >= normallimit)				// frost + ?
		  {
			Sentence sentence;
			sentence << frost_sentence(night1,value1)
					 << Delimiter(",");

			if(severevalue2 >= severelimit)
			  {
				sentence << "seuraavana yönä"
						 << "ankaran hallan todennäköisyys"
						 << "on"
						 << Number<int>(severevalue2)
						 << Delimiter("%");
			  }
			else if(value2 >= normallimit)
			  {
				sentence << "seuraavana yönä"
						 << Number<int>(value2)
						 << Delimiter("%");
			  }
			else
			  {
				sentence << "seuraava yö"
						 << "on"
						 << "lämpimämpi";
			  }
			paragraph << sentence;
			
		  }
		else										// nada + ?
		  {
			if(severevalue2 >= severelimit)
			  paragraph << severe_frost_sentence(night2,severevalue2);
			else if(value2 >= normallimit)
			  paragraph << frost_sentence(night2,value2);
		  }

	  }

	return paragraph;
  }


} // namespace TextGen
  
// ======================================================================
  
