// ======================================================================
/*!
 * \file
 * \brief Implementation for namespace TextGen::PrecipitationStoryTools
 */
// ======================================================================
/*!
 * \namespace TextGen::PrecipitationStoryTools
 *
 * \brief Tools for TextGen::PrecipitationStory and similar classes
 *
 * This namespace contains utility functions which are not
 * exclusive for handling precipitation alone. For example,
 * TextGen::WeatherStory most likely has use for these
 * functions.
 */
// ======================================================================

#include "PrecipitationStoryTools.h"
#include "AnalysisSources.h"
#include "GridForecaster.h"
#include "Integer.h"
#include "IntegerRange.h"
#include "MessageLogger.h"
#include "RangeAcceptor.h"
#include "Settings.h"
#include "Sentence.h"
#include "TextGenError.h"
#include "UnitFactory.h"
#include "ValueAcceptor.h"
#include "WeatherResult.h"

#include "NFmiGlobals.h"

#include "boost/lexical_cast.hpp"

using namespace WeatherAnalysis;
using namespace boost;
using namespace std;

namespace TextGen
{
  namespace
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Possible combinations of rain types
	 */
	// ----------------------------------------------------------------------

	enum RainType
	  {
		NONE,
		WATER,
		SLEET,
		WATER_SLEET,
		SNOW,
		SNOW_WATER,
		SNOW_SLEET,
		SNOW_SLEET_WATER
	  };

	// ----------------------------------------------------------------------
	/*!
	 * \brief Convert booleans to rain type
	 */
	// ----------------------------------------------------------------------

	RainType raintype(bool water, bool sleet, bool snow)
	{
	  return RainType(snow<<2 | sleet<<1 | water);
	}

  }

  namespace PrecipitationStoryTools
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Generate the generic rain description story
	 *
	 * \param theSources The analysis sources
	 * \param theArea The area to be analyzed
	 * \param thePeriod The rainy period to be analyzed
	 * \param theVar The control variable
	 * \param theDay The day in question
	 * \return The rain description story
	 */
	// ----------------------------------------------------------------------

	const Sentence rain_phrase(const WeatherAnalysis::AnalysisSources & theSources,
							   const WeatherAnalysis::WeatherArea & theArea,
							   const WeatherAnalysis::WeatherPeriod & thePeriod,
							   const std::string & theVar,
							   int theDay)
	{
	  Sentence s;
	  s << places_phrase(theSources,theArea,thePeriod,theVar,theDay)
		<< type_phrase(theSources,theArea,thePeriod,theVar,theDay);
	  return s;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Generate the "in some place places" story
	 *
	 * The used variables are
	 * \code
	 * ::minrain      = [0-X]    (=0.1)
	 * ::places::many = [0-100]  (=90)
	 * ::places::some = [0-100]  (=70)
	 * \endcode
	 *
	 * That is,
	 *  - If percentage >= 90, then use ""
	 *  - If percentage >= 70, then use "in many places"
	 *  - Else use "in some places"
	 *
	 * \param theSources The analysis sources
	 * \param theArea The area to be analyzed
	 * \param thePeriod The rainy period to be analyzed
	 * \param theVar The control variable
	 * \param theDay The day in question
	 * \return The "in places" description
	 */
	// ----------------------------------------------------------------------
	
	const Sentence places_phrase(const AnalysisSources & theSources,
								 const WeatherArea & theArea,
								 const WeatherPeriod & thePeriod,
								 const string & theVar,
								 int theDay)
	{
	  MessageLogger log("PrecipitationStoryTools::places_phrase");
	  
	  using namespace Settings;
	  
	  const int many_places = optional_percentage(theVar+"places::many",90);
	  const int some_places = optional_percentage(theVar+"places::some",50);
	  const double minrain  = optional_double(theVar+"::minrain",0.1);
	  
	  GridForecaster forecaster;
	  
	  RangeAcceptor rainlimits;
	  rainlimits.lowerLimit(minrain);
	  
	  const string day = lexical_cast<string>(theDay);
	  WeatherResult result = forecaster.analyze(theVar+"::fake::day"+day+"::places",
												theSources,
												Precipitation,
												Percentage,
												Maximum,
												theArea,
												thePeriod,
												DefaultAcceptor(),
												DefaultAcceptor(),
												rainlimits);
	  
	  log << "Precipitation percentage: " << result.value() << endl;
	  
	  if(result.value() == kFloatMissing)
		throw TextGenError("Precipitation percentage not available");
	  
	  Sentence s;
	  if(result.value() >= many_places)
		;
	  else if(result.value() >= some_places)
		s << "monin paikoin";
	  else
		s << "paikoin";
	  
	  return s;
	
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Generate the rain type phrase, including strength
	 *
	 * The variables used for determining the strength of the
	 * precipitation are
	 * \code
	 * ::weak_limit = [0-X] (=0.3)
	 * ::hard_limit = [0-X] (=3.0)
	 * \endcode
	 *
	 * The variables used for determining the rain form are
	 * \code
	 * ::ignore_limit  = [0-100] (=5)
	 * \endcode
	 *
	 * The variables used for determining the rain type are
	 * \code
	 * ::shower_limit = [0-100] (=80)
	 * \endcode
	 *
	 * \todo "sadekuuroja" prompti pitää lisätä
	 */
	// ----------------------------------------------------------------------

	const Sentence type_phrase(const AnalysisSources & theSources,
							   const WeatherArea & theArea,
							   const WeatherPeriod & thePeriod,
							   const string & theVar,
							   int theDay)
	{
	  MessageLogger log("PrecipitationStoryTools::type_phrase");

	  using namespace Settings;
	 
	  const double weak_limit = optional_double(theVar+"::weak_limit",0.3);
	  const double hard_limit = optional_double(theVar+"::hard_limit",3.0);
	  const int ignore_limit = optional_percentage(theVar+"::ignore_limit",5);
	  const int shower_limit = optional_percentage(theVar+"::shower_limit",80);

	  GridForecaster forecaster;
	  Sentence sentence;
	  
	  const string day = lexical_cast<string>(theDay);

	  RangeAcceptor waterfilter;
	  waterfilter.lowerLimit(0);	// 0 = drizzle
	  waterfilter.upperLimit(1);	// 1 = water

	  WeatherResult water = forecaster.analyze(theVar+"::fake::day"+day+"::water",
											   theSources,
											   PrecipitationForm,
											   Mean,
											   Percentage,
											   theArea,
											   thePeriod,
											   DefaultAcceptor(),
											   DefaultAcceptor(),
											   waterfilter);

	  ValueAcceptor sleetfilter;
	  sleetfilter.value(2);	// 2 = sleet
	  WeatherResult sleet = forecaster.analyze(theVar+"::fake::day"+day+"::sleet",
											   theSources,
											   PrecipitationForm,
											   Mean,
											   Percentage,
											   theArea,
											   thePeriod,
											   DefaultAcceptor(),
											   DefaultAcceptor(),
											   sleetfilter);

	  ValueAcceptor snowfilter;
	  snowfilter.value(3);	// 3 = snow
	  WeatherResult snow = forecaster.analyze(theVar+"::fake::day"+day+"::snow",
											   theSources,
											   PrecipitationForm,
											   Mean,
											   Percentage,
											   theArea,
											   thePeriod,
											   DefaultAcceptor(),
											   DefaultAcceptor(),
											   snowfilter);
	  
	  log << "Water percentage: " << water << endl;
	  log << "Sleet percentage: " << sleet << endl;
	  log << "Snow percentage:  " << snow << endl;

	  ValueAcceptor showerfilter;
	  showerfilter.value(2);	// 1=large scale, 2=showers
	  WeatherResult showers = forecaster.analyze(theVar+"::fake::day"+day+"::showers",
												 theSources,
												 PrecipitationType,
												 Mean,
												 Percentage,
												 theArea,
												 thePeriod,
												 DefaultAcceptor(),
												 DefaultAcceptor(),
												 showerfilter);
												 
	  log << "Shower percentage: " << showers << endl;

	  WeatherResult strength = forecaster.analyze(theVar+"::fake::day"+day+"::strength",
												  theSources,
												  Precipitation,
												  Maximum,
												  Maximum,
												  theArea,
												  thePeriod);

	  log << "Precipitation maximum: " << strength << endl;
	  
	  // Safety against weird data. Note that we always assume
	  // large scale rain, if for some reason we cannot obtain
	  // the type from the data itself. That is, we never assume
	  // rain showers.

	  if(water.value() == kFloatMissing ||
		 sleet.value() == kFloatMissing ||
		 snow.value() == kFloatMissing ||
		 strength.value() == kFloatMissing)
		{
		  sentence << "sadetta";
		  return sentence;
		}
	  
	  const bool has_water = (water.value() >= ignore_limit);
	  const bool has_sleet = (sleet.value() >= ignore_limit);
	  const bool has_snow  = (snow.value()  >= ignore_limit);

	  const bool has_showers = (showers.value() != kFloatMissing && showers.value() >= shower_limit);

	  switch(raintype(has_water,has_sleet,has_snow))
		{
		case NONE:
		  {
			log << "Rain type is NONE" << endl;
			sentence << "sadetta";
			break;
		  }
		case WATER:
		  {
			log << "Rain type is WATER" << endl;
			if(has_showers)
			  {
				if(strength.value() < weak_limit)
				  sentence << "vähäisiä kuuroja";
				else if(strength.value() >= hard_limit)
				  sentence << "ajoittain" << "voimakkaita kuuroja";
				else
				  sentence << "sadetta";	// "sadekuuroja" prompti puuttuu!
			  }
			else
			  {
				if(strength.value() < weak_limit)
				  sentence << "heikkoa sadetta";
				else if(strength.value() >= hard_limit)
				  sentence << "ajoittain" << "runsasta sadetta";
				else
				  sentence << "sadetta";
			  }
			break;
		  }
		case SLEET:
		  {
			log << "Rain type is SLEET" << endl;
			sentence << "räntäsadetta";
			break;
		  }
		case WATER_SLEET:
		  {
			log << "Rain type is WATER_SLEET" << endl;
			if(water.value() >= sleet.value())
			  sentence << "vesi-" << "tai" << "räntäsadetta";
			else
			  sentence << "räntä-" << "tai" << "vesisadetta";
			break;
		  }
		case SNOW:
		  {
			log << "Rain type is SNOW" << endl;
			if(has_showers)
			  {
				if(strength.value() < weak_limit)
				  sentence << "lumikuuroja";	// Sopiva prompti "vähäisiä lumikuuroja" puuttuu!
				else if(strength.value() >= hard_limit)
				  sentence << "sakeita lumikuuroja";
				else
				  sentence << "lumikuuroja";
			  }
			else
			  {
				if(strength.value() < weak_limit)
				  sentence << "vähäistä lumisadetta";
				else if(strength.value() >= hard_limit)
				  sentence << "ajoittain" << "sakeaa lumisadetta";
				else
				  sentence << "lumisadetta";
			  }
			break;
		  }
		case SNOW_WATER:
		case SNOW_SLEET_WATER:
		  {
			if(raintype(has_water,has_sleet,has_snow) == SNOW_WATER)
			  log << "Rain type is SNOW_WATER" << endl;
			else
			  log << "Rain type is SNOW_SLEET_WATER" << endl;

			if(has_showers)
			  {
				if(strength.value() < weak_limit)
				  sentence << "vesi-" << "tai" << "lumikuuroja";
				else if(strength.value() >= hard_limit)
				  sentence << "voimakkaita kuuroja" << "tai" << "sakeita lumikuuroja";
				else
				  sentence << "vesi-" << "tai" << "lumikuuroja";
			  }
			else
			  {
				if(strength.value() < weak_limit)
				  sentence << "heikkoa";
				else if(strength.value() >= hard_limit)
				  sentence << "ajoittain" << "kovaa";
				if(water.value() >= snow.value())
				  sentence << "vesi-" << "tai" << "lumisadetta";
				else
				  sentence << "lumi-" << "tai" << "vesisadetta";
			  }
			break;
		  }
		case SNOW_SLEET:
		  {
			log << "Rain type is SNOW_SLEET" << endl;
			if(has_showers)
			  {
				if(strength.value() < weak_limit)
				  sentence << "räntä-" << "tai" << "lumikuuroja";
				else if(strength.value() >= hard_limit)
				  sentence << "sakeita lumikuuroja" << "tai" << "räntäsadetta";
				else
				  sentence << "räntä-" << "tai" << "lumikuuroja";
			  }
			else
			  {
				if(strength.value() < weak_limit)
				  sentence << "vähäistä lumisadetta" << "tai" << "räntäsadetta";
				else if(strength.value() >= hard_limit)
				  sentence << "sakeaa lumisadetta" << "tai" << "räntäsadetta";
				else
				  {
					if(snow.value() >= sleet.value())
					  sentence << "lumi-" << "tai" << "räntäsadetta";
					else
					  sentence << "räntä-" << "tai" << "lumisadetta";
				  }
			  }
			break;
		  }
		}

	  log << sentence;

	  return sentence;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Generate a sum phrase from sum values
	 *
	 * \param theMinimum The minimum rain sum
	 * \param theMaximum The maximum rain sum
	 * \param theMean The mean rain sum
	 * \param theMinInterval The minimum allowed interval
	 * \return The sum phrase
	 */
	// ----------------------------------------------------------------------

	const Sentence sum_phrase(const WeatherResult & theMinimum,
							  const WeatherResult & theMaximum,
							  const WeatherResult & theMean,
							  int theMinInterval)
	{
	  Sentence sentence;
	  
	  const int minimum = FmiRound(theMinimum.value());
	  const int maximum = FmiRound(theMaximum.value());
	  const int mean = FmiRound(theMean.value());

	  if(minimum==0 && maximum==0)
		{
		  sentence << 0;
		}
	  else if(maximum-minimum < theMinInterval && mean!=0)
		{
		  sentence << "noin" << mean;
		}
	  else
		{
		  sentence << IntegerRange(minimum,maximum);
		}

	  sentence << *UnitFactory::create(Millimeters);

	  return sentence;

	}


  } // namespace PrecipitationStoryTools
} // namespace TextGen

// ======================================================================
