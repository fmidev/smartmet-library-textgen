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
#include "Number.h"
#include "NumberRange.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "WeatherFunction.h"
#include "WeatherParameter.h"
#include "WeatherResult.h"

#include "boost/lexical_cast.hpp"

using namespace WeatherAnalysis;
using namespace std;
using namespace boost;

namespace
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Round the input value to the given integer precision
   *
   * \param theValue The floating value
   * \param thePrecision The precision in range 0-100, should divide 100 evenly
   * \return The rounded value
   */
  // ----------------------------------------------------------------------

  int round_to_precision(float theValue, int thePrecision)
  {
	if(thePrecision <=0 || thePrecision > 100)
	  return FmiRound(theValue);
	const int value = FmiRound(theValue/thePrecision)*thePrecision;
	return value;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the first night start hour >= the given time
   *
   * \param theTime The reference time
   * \param theStartHour The hour when nights start
   */
  // ----------------------------------------------------------------------

  NFmiTime night_start(const NFmiTime & theTime, int theStartHour)
  {
	NFmiTime ret(theTime);
	ret.SetHour(theStartHour);
	if(ret.IsLessThan(theTime))
	  ret.ChangeByDays(1);
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the night end time for the given start time
   *
   * \param theTime The night start time
   * \param theEndHour The night end hour
   */
  // ----------------------------------------------------------------------

  NFmiTime night_end(const NFmiTime & theTime, int theEndHour)
  {
	NFmiTime ret(theTime);
	if(theEndHour <= theTime.GetHour())
	  ret.ChangeByDays(1);
	ret.SetHour(theEndHour);
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Count the number of nights in the given period
   *
   * \param thePeriod The period
   * \param theStartHour The start hour of nights
   * \param theEndHour The end hour of nights
   * \return The number of nights
   */
  // ----------------------------------------------------------------------

  int night_count(const WeatherPeriod & thePeriod,
				  int theStartHour,
				  int theEndHour)
  {
	int days = 0;

	NFmiTime start = night_start(thePeriod.localStartTime(),theStartHour);
	NFmiTime end = night_end(start,theEndHour);

	while(!thePeriod.localEndTime().IsLessThan(end))
	  {
		++days;
		start = night_start(end,theStartHour);
		end = night_end(start,theEndHour);
	  }
	return days;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return phrase "viikonpäivän vastaisena yönä" for the period
   *
   * \param thePeriod The night period
   * \return The phrase
   */
  // ----------------------------------------------------------------------

  string night_against_phrase(const WeatherPeriod & thePeriod)
  {
	string ret = lexical_cast<string>(thePeriod.localEndTime().GetWeekday());
	ret += "-vastaisena yönä";
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the desired night period
   *
   * \param thePeriod The period from which to get the night
   * \param theStartHour The night start hour
   * \param theEndHour The night end hour
   * \param theNight Which night to get, 1 = first
   * \return The desired period
   */
  // ----------------------------------------------------------------------

  WeatherPeriod night(const WeatherPeriod & thePeriod,
					  int theStartHour,
					  int theEndHour,
					  int theNight)
  {
	if(theNight<1)
	  throw TextGen::TextGenError("FrostStory: Cannot request night < 1 from night()");

	NFmiTime start = night_start(thePeriod.localStartTime(),theStartHour);
	NFmiTime end = night_end(start,theEndHour);

	while(--theNight > 0)
	  {
		start = night_start(end,theStartHour);
		end = night_end(start,theEndHour);
	  }
	return WeatherPeriod(start,end);
  }

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
	TextGen::Sentence sentence;
	sentence << "ankaran hallan todennäköisyys"
			 << "on"
			 << night_against_phrase(thePeriod)
			 << TextGen::Number<int>(theProbability)
			 << TextGen::Delimiter("%");
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
	TextGen::Sentence sentence;
	sentence << "hallan todennäköisyys"
			 << "on"
			 << night_against_phrase(thePeriod)
			 << TextGen::Number<int>(theProbability)
			 << TextGen::Delimiter("%");
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
   * \param theSources The analysis sources
   * \param theArea The area to be analyzed 
   * \param thePeriod The period to be analyzed
   * \param theVariable The associated configuration variable
  */
  // ----------------------------------------------------------------------
  
  FrostStory::FrostStory(const AnalysisSources & theSources,
						 const WeatherArea & theArea,
						 const WeatherPeriod & thePeriod,
						 const string & theVariable)
	: itsSources(theSources)
	, itsArea(theArea)
	, itsPeriod(thePeriod)
	, itsVariable(theVariable)
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
	Paragraph paragraph;
	Sentence sentence;

	const string var1 = itsVariable+"::precision";
	const string var2 = itsVariable+"::severe_frost_limit";
	const string var3 = itsVariable+"::frost_limit";

	const int precision = Settings::require_percentage(var1);
	const int severelimit = Settings::require_percentage(var2);
	const int normallimit = Settings::require_percentage(var3);

	GridForecaster forecaster;

	WeatherResult frost = forecaster.analyze(itsVariable+"::fake::mean",
											 itsSources,
											 Frost,
											 Mean,
											 Maximum,
											 itsPeriod,
											 itsArea);

	if(frost.value() == kFloatMissing)
	  throw TextGenError("Frost is not available");

	// Quick exit if the mean is zero
	
	if(frost.value() == 0)
	  return paragraph;

	// Severe frost

	WeatherResult severefrost = forecaster.analyze(itsVariable+"::fake::severe_mean",
												   itsSources,
												   SevereFrost,
												   Mean,
												   Maximum,
												   itsPeriod,
												   itsArea);

	if(severefrost.value() == kFloatMissing)
	  throw TextGenError("SevereFrost is not available");

	const int frost_value = round_to_precision(frost.value(),precision);

	const int severe_frost_value = round_to_precision(severefrost.value(),precision);

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
	Paragraph paragraph;
	Sentence sentence;

	const string var1 = itsVariable+"::precision";
	const string var2 = itsVariable+"::severe_frost_limit";
	const string var3 = itsVariable+"::frost_limit";

	const int precision = Settings::require_percentage(var1);
	const int severelimit = Settings::require_percentage(var2);
	const int normallimit = Settings::require_percentage(var3);

	GridForecaster forecaster;

	WeatherResult frost = forecaster.analyze(itsVariable+"::fake::maximum",
											 itsSources,
											 Frost,
											 Maximum,
											 Maximum,
											 itsPeriod,
											 itsArea);

	if(frost.value() == kFloatMissing)
	  throw TextGenError("Frost is not available");

	// Quick exit if the mean is zero
	
	if(frost.value() == 0)
	  return paragraph;

	// Severe frost

	WeatherResult severefrost = forecaster.analyze(itsVariable+"::fake::severe_maximum",
												   itsSources,
												   SevereFrost,
												   Maximum,
												   Maximum,
												   itsPeriod,
												   itsArea);

	if(severefrost.value() == kFloatMissing)
	  throw TextGenError("SevereFrost is not available");

	const int frost_value = round_to_precision(frost.value(),precision);

	const int severe_frost_value = round_to_precision(severefrost.value(),precision);

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
	Paragraph paragraph;
	Sentence sentence;

	const string var1 = itsVariable+"::precision";
	const string var2 = itsVariable+"::severe_frost_limit";
	const string var3 = itsVariable+"::frost_limit";

	const int precision = Settings::require_percentage(var1);
	const int severelimit = Settings::require_percentage(var2);
	const int normallimit = Settings::require_percentage(var3);

	GridForecaster forecaster;

	WeatherResult maxfrost = forecaster.analyze(itsVariable+"::fake::maximum",
												itsSources,
												Frost,
												Maximum,
												Maximum,
												itsPeriod,
												itsArea);

	if(maxfrost.value() == kFloatMissing)
	  throw TextGenError("Maximum Frost is not available");

	// Quick exit if maximum frost probability is zero
	
	if(maxfrost.value() == 0)
	  return paragraph;

	// Minimum frost

	WeatherResult minfrost = forecaster.analyze(itsVariable+"::fake::minimum",
												itsSources,
												Frost,
												Minimum,
												Maximum,
												itsPeriod,
												itsArea);

	if(minfrost.value() == kFloatMissing)
	  throw TextGenError("Minimum Frost is not available");

	// Maximum severe frost

	WeatherResult maxseverefrost = forecaster.analyze(itsVariable+"::fake::severe_maximum",
													  itsSources,
													  SevereFrost,
													  Maximum,
													  Maximum,
													  itsPeriod,
													  itsArea);

	if(maxseverefrost.value() == kFloatMissing)
	  throw TextGenError("Maximum SevereFrost is not available");

	WeatherResult minseverefrost = forecaster.analyze(itsVariable+"::fake::severe_minimum",
													  itsSources,
													  SevereFrost,
													  Minimum,
													  Maximum,
													  itsPeriod,
													  itsArea);

	if(minseverefrost.value() == kFloatMissing)
	  throw TextGenError("Minimum SevereFrost is not available");

	const int frost_min = round_to_precision(minfrost.value(),precision);
	const int frost_max = round_to_precision(maxfrost.value(),precision);

	const int severe_frost_min = round_to_precision(minseverefrost.value(),precision);
	const int severe_frost_max = round_to_precision(maxseverefrost.value(),precision);

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
	Paragraph paragraph;

	const int starthour   = Settings::require_hour(itsVariable+"::starthour");
	const int endhour     = Settings::require_hour(itsVariable+"::endhour");

	const int precision   = Settings::require_percentage(itsVariable+"::precision");
	const int severelimit = Settings::require_percentage(itsVariable+"::severe_frost_limit");
	const int normallimit = Settings::require_percentage(itsVariable+"::frost_limit");

	const int nights = night_count(itsPeriod,starthour,endhour);

	if(nights==0)
	  return paragraph;

	// Calculate frost probability

	GridForecaster forecaster;

	WeatherPeriod night1 = night(itsPeriod,starthour,endhour,1);

	WeatherResult frost = forecaster.analyze(itsVariable+"::fake::day1::mean",
											 itsSources,
											 Frost,
											 Mean,
											 Maximum,
											 night1,
											 itsArea);

	WeatherResult severefrost = forecaster.analyze(itsVariable+"::fake::day1::severe_mean",
												   itsSources,
												   SevereFrost,
												   Mean,
												   Maximum,
												   night1,
												   itsArea);
	
	if(frost.value()==kFloatMissing || severefrost.value()==kFloatMissing)
	  throw TextGenError("Frost is not available");

	if(nights==1)
	  {
		const int value = round_to_precision(frost.value(),precision);
		const int severevalue = round_to_precision(severefrost.value(),precision);

		if(severevalue >= severelimit)
		  paragraph << severe_frost_sentence(night1,severevalue);
		else if(value >= normallimit)
		  paragraph << frost_sentence(night1,value);

	  }
	else
	  {

		WeatherPeriod night2 = night(itsPeriod,starthour,endhour,2);

		WeatherResult frost2 = forecaster.analyze(itsVariable+"::fake::day2::mean",
												  itsSources,
												  Frost,
												  Mean,
												  Maximum,
												  night2,
												  itsArea);

		WeatherResult severefrost2 = forecaster.analyze(itsVariable+"::fake::day2::severe_mean",
														itsSources,
														SevereFrost,
														Mean,
														Maximum,
														night2,
														itsArea);
		
		if(frost2.value()==kFloatMissing || severefrost2.value()==kFloatMissing)
		  throw TextGenError("Frost is not available");

		const int value1 = round_to_precision(frost.value(),precision);
		const int severevalue1 = round_to_precision(severefrost.value(),precision);

		const int value2 = round_to_precision(frost2.value(),precision);
		const int severevalue2 = round_to_precision(severefrost2.value(),precision);
		
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
  
