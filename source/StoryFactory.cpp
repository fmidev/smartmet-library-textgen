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
	 * \param theArea The area for which to generate the story
	 * \param theName The story to create
	 */
	// ----------------------------------------------------------------------

	Paragraph create(const AnalysisSources & theSources,
					 const WeatherArea & theArea,
					 const string & theName)
	{
	  if(TemperatureStory::hasStory(theName))
		{
		  TemperatureStory story(theSources,theArea);
		  return story.makeStory(theName);
		}

	  if(PrecipitationStory::hasStory(theName))
		{
		  PrecipitationStory story(theSources,theArea);
		  return story.makeStory(theName);
		}

	  throw TextGenError("StoryFactory: Unrecognized story "+theName);
	}

  } // namespace StoryFactory
} // namespace TextGen

// ======================================================================
