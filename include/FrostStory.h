// ======================================================================
/*!
 * \file
 * \brief Interface of class FrostStory
 */
// ======================================================================
/*!
 * \class TextGen::FrostStory
 *
 * \brief Generates stories on frost
 *
 * \see page_tarinat
 */
// ======================================================================

#ifndef TEXTGEN_FROSTSTORY_H
#define TEXTGEN_FROSTSTORY_H

#include "AnalysisSources.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"

#include <string>

namespace TextGen
{
  class Paragraph;

  class FrostStory
  {

  public:

	~FrostStory();
	FrostStory(const WeatherAnalysis::AnalysisSources & theSources,
			   const WeatherAnalysis::WeatherArea & theArea,
			   const WeatherAnalysis::WeatherPeriod & thePeriod,
			   const std::string & theVariable);

	static bool hasStory(const std::string & theName);
	Paragraph makeStory(const std::string & theName) const;

  private:

	Paragraph mean() const;
	Paragraph maximum() const;
	Paragraph maxtwonights() const;

	FrostStory();
	FrostStory(const FrostStory & theStory);
	FrostStory & operator=(const FrostStory & theStory);

	const WeatherAnalysis::AnalysisSources & itsSources;
	const WeatherAnalysis::WeatherArea & itsArea;
	const WeatherAnalysis::WeatherPeriod & itsPeriod;
	const std::string & itsVariable;

  }; // class FrostStory
}

#endif // TEXTGEN_FROSTSTORY_H

// ======================================================================
