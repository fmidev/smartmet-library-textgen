// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::TextGenerator
 */
// ======================================================================

#pragma once

#include <memory>
#include <string>

class TextGenPosixTime;

namespace TextGen
{
class WeatherArea;
class AnalysisSources;
}  // namespace TextGen

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

  static std::string version();

 private:
  class Pimple;
  std::shared_ptr<Pimple> itsPimple;

};  // class TextGenerator
}  // namespace TextGen

// ======================================================================
