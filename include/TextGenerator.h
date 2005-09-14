// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::TextGenerator
 */
// ======================================================================

#ifndef TEXTGEN_TEXTGENERATOR_H
#define TEXTGEN_TEXTGENERATOR_H

#include <boost/shared_ptr.hpp>
#include <string>

class NFmiTime;

namespace WeatherAnalysis
{
  class WeatherArea;
  class AnalysisSources;
}

namespace TextGen
{
  class Document;

  class TextGenerator
  {
  public:

	TextGenerator();
#ifdef NO_COMPILER_GENERATOR
	~TextGenerator();
	TextGenerator(const TextGenerator & theGenerator);
	TextGenerator & operator=(const TextGenerator & theGenerator);
#endif

	const NFmiTime & time() const;
	void time(const NFmiTime & theForecastTime);
	void sources(const WeatherAnalysis::AnalysisSources & theSources);

	const Document generate(const WeatherAnalysis::WeatherArea & theArea) const;

  private:

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

  }; // class TextGenerator
} // namespace TextGen

#endif // TEXTGEN_TEXTGENERATOR_H

// ======================================================================

