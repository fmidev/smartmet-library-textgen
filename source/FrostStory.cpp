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

using namespace WeatherAnalysis;
using namespace std;

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
	if(theName == "frost_maxtwonights")
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
	if(theName == "frost_maxtwonights")
	  return maxtwonights();

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
	  }
	else if(frost_value >= normallimit)
	  {
		Number<int> val(frost_value);
		sentence << "hallan todennäköisyys"
				 << "on"
				 << val
				 << Delimiter("%");
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
	  }
	else if(frost_value >= normallimit)
	  {
		Number<int> val(frost_value);
		sentence << "hallan todennäköisyys"
				 << "on"
				 << val
				 << Delimiter("%");
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

	if(severe_frost_max > severelimit)
	  {
		NumberRange<Number<int> > range(severe_frost_min,severe_frost_max);
		sentence << "ankaran hallan todennäköisyys"
				 << "on"
				 << range
				 << Delimiter("%");
	  }
	else if(frost_max > normallimit)
	  {
		NumberRange<Number<int> > range(frost_min,frost_max);
		sentence << "hallan todennäköisyys"
				 << "on"
				 << range
				 << Delimiter("%");
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
  
  Paragraph FrostStory::maxtwonights() const
  {
	Paragraph paragraph;

#if 0
	const int starthour   = Settings::require_hour(itsVariable+"::starthour");
	const int endhour     = Settings::require_hour(itsVariable+"::endhour");

	const int precision   = Settings::require_percentage(itsVariable+"::precision");
	const int severelimit = Settings::require_percentage(itsVariable+"::severe_forst_limit");
	const int normallimit = Settings::require_percentage(itsVariable+"::frost_limit");
#endif


	return paragraph;
  }


} // namespace TextGen
  
// ======================================================================
  
