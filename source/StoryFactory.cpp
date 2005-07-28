// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::StoryFactory
 */
// ======================================================================
/*!
 * \namespace TextGen::StoryFactory
 *
 * \brief Generates brief paragraphs of desired type
 */
// ======================================================================

#include "StoryFactory.h"
#include "AnalysisSources.h"
#include "CloudinessStory.h"
#include "Delimiter.h"
#include "DewPointStory.h"
#include "ForestStory.h"
#include "FrostStory.h"
#include "Paragraph.h"
#include "PrecipitationStory.h"
#include "PressureStory.h"
#include "RelativeHumidityStory.h"
#include "RoadStory.h"
#include "TemperatureStory.h"
#include "TextGenError.h"
#include "WeatherStory.h"
#include "WindStory.h"

#include "NFmiTime.h"

using namespace WeatherAnalysis;
using namespace std;

namespace TextGen
{
  namespace StoryFactory
  {
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief Create a story on the desired subject
	 *
	 * Throws if the given name is not recognized.
	 *
	 * \param theForecastTime The forecast time
	 * \param theSources The associated analysis sources
	 * \param theArea The area for which to generate the story
	 * \param thePeriod The period for which to generate the story
	 * \param theName The story to create
	 * \param theVariable The configuration variable prefix
	 */
	// ----------------------------------------------------------------------

	const Paragraph create(const NFmiTime & theForecastTime,
						   const AnalysisSources & theSources,
						   const WeatherArea & theArea,
						   const WeatherPeriod & thePeriod,
						   const string & theName,
						   const string & theVariable)
	{
	  if(theName == "none")
		{
		  Paragraph paragraph;
		  return paragraph;
		}

	  if(TemperatureStory::hasStory(theName))
		{
		  TemperatureStory story(theForecastTime,theSources,theArea,thePeriod,theVariable);
		  return story.makeStory(theName);
		}

	  if(PrecipitationStory::hasStory(theName))
		{
		  PrecipitationStory story(theForecastTime,theSources,theArea,thePeriod,theVariable);
		  return story.makeStory(theName);
		}

	  if(CloudinessStory::hasStory(theName))
		{
		  CloudinessStory story(theForecastTime,theSources,theArea,thePeriod,theVariable);
		  return story.makeStory(theName);
		}

	  if(WeatherStory::hasStory(theName))
		{
		  WeatherStory story(theForecastTime,theSources,theArea,thePeriod,theVariable);
		  return story.makeStory(theName);
		}

	  if(WindStory::hasStory(theName))
		{
		  WindStory story(theForecastTime,theSources,theArea,thePeriod,theVariable);
		  return story.makeStory(theName);
		}

	  if(FrostStory::hasStory(theName))
		{
		  FrostStory story(theForecastTime,theSources,theArea,thePeriod,theVariable);
		  return story.makeStory(theName);
		}

	  if(RelativeHumidityStory::hasStory(theName))
		{
		  RelativeHumidityStory story(theForecastTime,theSources,theArea,thePeriod,theVariable);
		  return story.makeStory(theName);
		}

	  if(RoadStory::hasStory(theName))
		{
		  RoadStory story(theForecastTime,theSources,theArea,thePeriod,theVariable);
		  return story.makeStory(theName);
		}

	  if(ForestStory::hasStory(theName))
		{
		  ForestStory story(theForecastTime,theSources,theArea,thePeriod,theVariable);
		  return story.makeStory(theName);
		}

	  if(DewPointStory::hasStory(theName))
		{
		  DewPointStory story(theForecastTime,theSources,theArea,thePeriod,theVariable);
		  return story.makeStory(theName);
		}

	  if(PressureStory::hasStory(theName))
		{
		  PressureStory story(theForecastTime,theSources,theArea,thePeriod,theVariable);
		  return story.makeStory(theName);
		}

	  throw TextGenError("StoryFactory: Unrecognized story '"+theName+"'");
	}

  } // namespace StoryFactory
} // namespace TextGen

// ======================================================================
