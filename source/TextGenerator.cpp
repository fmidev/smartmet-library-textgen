// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::TextGenerator
 */
// ======================================================================

#include "TextGenerator.h"
#include "AnalysisSources.h"
#include "Document.h"
#include "Header.h"
#include "HeaderFactory.h"
#include "LandMaskSource.h"
#include "MapSource.h"
#include "MaskSource.h"
#include "Paragraph.h"
#include "RegularMaskSource.h"
#include "StoryFactory.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"
#include "WeatherPeriodFactory.h"
#include "LatestWeatherSource.h"

#include "NFmiSettings.h"
#include "NFmiStringTools.h"
#include "NFmiTime.h"

#include "boost/lexical_cast.hpp"

using namespace WeatherAnalysis;
using namespace std;
using namespace boost;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Implementation hiding pimple
   */
  // ----------------------------------------------------------------------

  class TextGenerator::Pimple
  {
  public:
	Pimple();

	AnalysisSources itsSources;
	NFmiTime itsForecastTime;

  }; // class Pimple

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * The constructor initializes reasonable defaults for all
   * the analysis sources components.
   */
  // ----------------------------------------------------------------------

  TextGenerator::Pimple::Pimple()
  {
	const string str_expansion_distance = NFmiSettings::instance().require("textgen::mask::expansion_distance");
	const double expansion_distance = lexical_cast<double>(str_expansion_distance);

	shared_ptr<WeatherSource> weathersource(new LatestWeatherSource());
	shared_ptr<MapSource> mapsource(new MapSource());
	shared_ptr<MaskSource> masksource(new RegularMaskSource(mapsource,
															expansion_distance));
	const string land_name = NFmiSettings::instance().require("textgen::mask::land");
	shared_ptr<MaskSource> landsource(new LandMaskSource(mapsource,
														 land_name,
														 expansion_distance));
	
	itsSources.setWeatherSource(weathersource);
	itsSources.setMapSource(mapsource);
	itsSources.setMaskSource(masksource);
	itsSources.setLandMaskSource(landsource);

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  // ----------------------------------------------------------------------

  TextGenerator::TextGenerator()
	: itsPimple(new Pimple())
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the forecast time
   *
   * \return The forecast time
   */
  // ----------------------------------------------------------------------

  const NFmiTime & TextGenerator::time() const
  {
	return itsPimple->itsForecastTime;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set a new forecast time
   *
   * This is mostly used to override the current time as taken from
   * the CPU clock. This is useful for both debugging and regression
   * testing.
   *
   * \param theForecastTime The time to use as the forecast time
   */
  // ----------------------------------------------------------------------

  void TextGenerator::time(const NFmiTime & theForecastTime)
  {
	itsPimple->itsForecastTime = theForecastTime;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate the text
   *
   * The algorithm consists of the following steps:
   *
   * -# Parse textgen::paragraphs for paragraph names
   * -# Initialize output document
   * -# For each paragraph name
   *    -# Generate period from textgen::name::period
   *    -# Generate header from textgen::name::header, if it exists
   *    -# Initialize output paragraph
   *    -# For each story name in textgen::name::content
   *        -# Generate the story
   *        -# Append the story to the output paragraph
   *    -# Append the header to the document
   *    -# Append the paragraph to the document
   * -# Return the document
   *
   * \param theArea The weather area
   *
   */
  // ----------------------------------------------------------------------

  Document TextGenerator::generate(const WeatherArea & theArea) const
  {
	const NFmiSettings & settings = NFmiSettings::instance();

	const list<string> paragraphs = NFmiStringTools::SplitWords(settings.require("textgen::sections"));

	Document doc;
	for(list<string>::const_iterator it = paragraphs.begin();
		it != paragraphs.end();
		++it)
	  {
		const string periodvar = "textgen::"+*it+"::period";
		const WeatherPeriod period = WeatherPeriodFactory::create(time(),
																  periodvar);

		const string headervar = "textgen::"+*it+"::header";
		Header header = HeaderFactory::create(period,headervar);
		if(!header.empty())
		  doc << header;

		const list<string> contents = NFmiStringTools::SplitWords(settings.require("textgen::"+*it+"::content"));
		if(!contents.empty())
		  {
			Paragraph paragraph;
			for(list<string>::const_iterator iter = contents.begin();
				iter != contents.end();
				++iter)
			  {
				const string storyvar = "textgen::"+*it+"::story::"+*iter;
				Paragraph p = StoryFactory::create(itsPimple->itsSources,
												   theArea,
												   period,
												   *iter,
												   storyvar);
				paragraph << p;
			  }
			doc << paragraph;
		  }
	  }

	return doc;

  }

} // namespace TextGen

// ======================================================================
