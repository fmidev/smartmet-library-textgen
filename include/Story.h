// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::Story
 */
// ======================================================================
/*!
 * \class TextGen::Story
 *
 * \brief Generates stories on 
 *
 */
// ======================================================================

#ifndef TEXTGEN_STORY_H
#define TEXTGEN_STORY_H

#include "AnalysisSources.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"

namespace TextGen
{
  class Paragraph;

  class Story
  {

  public:

	Story() { }
	virtual ~Story() { }
	virtual Paragraph makeStory(const std::string & theName) const = 0;

  }; // class Story
}

#endif // TEXTGEN_STORY_H

// ======================================================================
