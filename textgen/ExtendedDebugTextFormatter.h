// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::ExtendedDebugTextFormatter
 */
// ======================================================================

#pragma once

#include "DebugDictionary.h"
#include "TextFormatter.h"

namespace TextGen
{
class ExtendedDebugTextFormatter : public TextFormatter
{
 public:
  ExtendedDebugTextFormatter() = default;
  ~ExtendedDebugTextFormatter() override = default;
  void dictionary(const std::shared_ptr<Dictionary>& theDict) override {}
  std::string format(const Glyph& theGlyph) const override;

  // override for all composites
  std::string visit(const Glyph& theGlyph) const override;
  std::string visit(const Integer& theInteger) const override;
  std::string visit(const Real& theReal) const override;
  std::string visit(const IntegerRange& theRange) const override;
  std::string visit(const TimePhrase& theTime) const override;
  std::string visit(const Sentence& theSentence) const override;
  std::string visit(const Paragraph& theParagraph) const override;
  std::string visit(const Header& theHeader) const override;
  std::string visit(const Document& theDocument) const override;
  std::string visit(const WeatherTime& theTime) const override;
  std::string visit(const TimePeriod& thePeriod) const override;

  std::string visit(const SectionTag& theSectionTag) const override;
  std::string visit(const StoryTag& theStoryTag) const override;

 private:
  DebugDictionary itsDictionary;
  mutable std::string itsSectionVar;
  mutable std::string itsStoryVar;

};  // class ExtendedDebugTextFormatter
}  // namespace TextGen

// ======================================================================
