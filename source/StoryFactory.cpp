// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::StoryFactory
 */
// ======================================================================

#include "StoryFactory.h"
#include "AnalysisSources.h"
#include "Paragraph.h"
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
	  throw TextGenError("StoryFactory: Unrecognized story "+theName);
	}

  } // namespace StoryFactory
} // namespace TextGen

// ======================================================================
