// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::Story
 */
// ======================================================================

#pragma once

#include <calculator/AnalysisSources.h>
#include <calculator/WeatherArea.h>
#include <calculator/WeatherPeriod.h>

namespace TextGen
{
class Paragraph;

class Story
{
 public:
  Story() = default;
  virtual ~Story();
  virtual Paragraph makeStory(const std::string& theName) const = 0;

};  // class Story
}  // namespace TextGen

// ======================================================================
