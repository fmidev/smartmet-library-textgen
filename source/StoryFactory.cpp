// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::StoryFactory
 */
// ======================================================================

#include "StoryFactory.h"
#include "AnalysisSources.h"
#include "Paragraph.h"
#include "PrecipitationStory.h"
#include "TemperatureStory.h"
#include "TextGenError.h"

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
	 * \param theSources The associated analysis sources
	 * \param theName The story to create
	 */
	// ----------------------------------------------------------------------

	Paragraph create(const AnalysisSources & theSources,
					 const WeatherArea & theArea,
					 const string & theName)
	{
	  if(theName == "temperature_mean")
		{
		  TemperatureStory story(theSources,theArea);
		  return story.mean();
		}
	  if(theName == "temperature_meanmax")
		{
		  TemperatureStory story(theSources,theArea);
		  return story.meanmax();
		}
	  if(theName == "temperature_meanmin")
		{
		  TemperatureStory story(theSources,theArea);
		  return story.meanmin();
		}
	  if(theName == "precipitation_total")
		{
		  PrecipitationStory story(theSources,theArea);
		  return story.total();
		}

	  throw TextGenError("StoryFactory: Unrecognized story "+theName);
	}

  } // namespace StoryFactory
} // namespace TextGen

// ======================================================================
