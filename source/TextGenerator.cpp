// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::TextGenerator
 */
// ======================================================================
/*!
 * \class TextGen::TextGenerator
 *
 * \brief The main text generator driver
 *
 */
// ======================================================================

#include "TextGenerator.h"
#include "AnalysisSources.h"
#include "CoastMaskSource.h"
#include "Document.h"
#include "Header.h"
#include "HeaderFactory.h"
#include "HourPeriodGenerator.h"
#include "InlandMaskSource.h"
#include "LandMaskSource.h"
#include "LatestWeatherSource.h"
#include "MaskSource.h"
#include "MessageLogger.h"
#include "NullMaskSource.h"
#include "Paragraph.h"
#include "RegularMaskSource.h"
#include "SectionTag.h"
#include "Settings.h"
#include "StoryFactory.h"
#include "StoryTag.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"
#include "WeatherPeriodFactory.h"

#include "NFmiStringTools.h"
#include "NFmiTime.h"

using namespace WeatherAnalysis;
using namespace std;
using namespace boost;

namespace TextGen
{

  namespace
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Generate contents from given contents list
	 *
	 * \param theContents The string with content variables
	 * \param theVar The control variable prefix
	 * \param theForecastTime The forecast time
	 * \param theSources The analysis sources
	 * \param theArea The weather area
	 * \param thePeriod The weather period
	 * \return A paragraph
	 */
	// ----------------------------------------------------------------------

	const Paragraph make_contents(const string & theContents,
								  const string & theVar,
								  const NFmiTime & theForecastTime,
								  const AnalysisSources & theSources,
								  const WeatherArea & theArea,
								  const WeatherPeriod & thePeriod)
	{
	  const vector<string> contents = NFmiStringTools::Split(theContents);

	  Paragraph paragraph;

	  for(vector<string>::const_iterator iter = contents.begin(); iter != contents.end(); ++iter)
		{
		  const string storyvar = theVar+"::story::"+*iter;
		  paragraph << StoryTag(storyvar);
		  Paragraph p = StoryFactory::create(theForecastTime,
											 theSources,
											 theArea,
											 thePeriod,
											 *iter,
											 storyvar);
		  paragraph << p;
		}

	  return paragraph;

	}


  } // namespace anonymous




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
	shared_ptr<WeatherSource> weathersource(new LatestWeatherSource());
	itsSources.setWeatherSource(weathersource);

	typedef shared_ptr<MaskSource> mask_source;

	mask_source masksource(new RegularMaskSource());
	itsSources.setMaskSource(masksource);

	// land mask is optional - by default the area is land so the source can be shared
	const string land_name = Settings::optional_string("textgen::mask::land","");
	if(land_name.empty())
	  itsSources.setLandMaskSource(masksource);
	else
	  itsSources.setLandMaskSource(mask_source(new LandMaskSource(WeatherArea(land_name))));

	// coast mask is optional - by default there is no coast
	const string coast_name = Settings::optional_string("textgen::mask::coast","");
	if(coast_name.empty())
	  {
		mask_source nullsource(new NullMaskSource);
		itsSources.setCoastMaskSource(nullsource);
		itsSources.setInlandMaskSource(nullsource);
	  }
	else
	  {
		const WeatherArea coast(coast_name);
		itsSources.setCoastMaskSource(mask_source(new CoastMaskSource(coast)));
		itsSources.setInlandMaskSource(mask_source(new InlandMaskSource(coast)));
	  }


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
   *    -# Append the header to the document
   *    -# If there are no subperiods
   *       -# For each story name in textgen::name::content
   *           -# Generate the story
   *           -# Append the story to the output paragraph
   *       -# Append the paragraph to the document
   *    -# Else for each subperiod
   *       -# For each story name in textgen::name::dayX::content
   *           -# Generate the story
   *           -# Append the story to the output paragraph
   * -# Return the document
   *
   * \param theArea The weather area
   *
   */
  // ----------------------------------------------------------------------

  const Document TextGenerator::generate(const WeatherArea & theArea) const
  {
	MessageLogger log("TextGenerator::generate");

	const vector<string> paragraphs = NFmiStringTools::Split(Settings::require_string("textgen::sections"));

	Document doc;
	for(vector<string>::const_iterator it = paragraphs.begin();
		it != paragraphs.end();
		++it)
	  {
		doc << SectionTag("textgen::"+*it);

		const string periodvar = "textgen::"+*it+"::period";
		const WeatherPeriod period = WeatherPeriodFactory::create(time(),
																  periodvar);

		const string headervar = "textgen::"+*it+"::header";
		Header header = HeaderFactory::create(theArea,period,headervar);
		if(!header.empty())
		  doc << header;

		const bool subs = Settings::optional_bool("textgen::"+*it+"::subperiods",false);

		if(!subs)
		  {
			const string contents = Settings::require("textgen::"+*it+"::content");
			log << "contents " << contents << endl;
			doc << make_contents(contents,
								 "textgen::"+*it,
								 itsPimple->itsForecastTime,
								 itsPimple->itsSources,
								 theArea,
								 period);
		  }
		else
		  {
			// Generate subparagraphs for each day
			HourPeriodGenerator generator(period,"textgen::"+*it+"::subperiod::day");
			
			const string defaultvar = "textgen::"+*it;
			
			for(HourPeriodGenerator::size_type day=1; day<=generator.size(); day++)
			  {
				const WeatherPeriod subperiod = generator.period(day);

				const string dayvar = defaultvar+"::day"+NFmiStringTools::Convert(day);

				const bool hasday = Settings::isset(dayvar+"::content");

				doc << make_contents(hasday ?
									 Settings::require_string(dayvar+"::content") :
									 Settings::require_string(defaultvar+"::content"),
									 hasday ? dayvar : defaultvar,
									 itsPimple->itsForecastTime,
									 itsPimple->itsSources,
									 theArea,
									 subperiod);
			  }
		  }
	  }
	return doc;

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set a new forecast data
   *
   * \param theSources new data
   */
  // ----------------------------------------------------------------------
  void TextGenerator::sources(const AnalysisSources & theSources)
  {
	itsPimple->itsSources = theSources;
  }


} // namespace TextGen

// ======================================================================



