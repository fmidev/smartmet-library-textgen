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

class TextGenPosixTime;

namespace TextGen
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
  TextGenerator(const TextGen::WeatherArea& theLandMaskArea,
                const TextGen::WeatherArea& theCoastMaskArea);
#ifdef NO_COMPILER_GENERATOR
  ~TextGenerator();
  TextGenerator(const TextGenerator& theGenerator);
  TextGenerator& operator=(const TextGenerator& theGenerator);
#endif

  const TextGenPosixTime& time() const;
  void time(const TextGenPosixTime& theForecastTime);
  void sources(const TextGen::AnalysisSources& theSources);

  Document generate(const TextGen::WeatherArea& theArea) const;

 private:
  class Pimple;
  boost::shared_ptr<Pimple> itsPimple;

};  // class TextGenerator
}  // namespace TextGen

#endif  // TEXTGEN_TEXTGENERATOR_H

// ======================================================================
