// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::CssTextFormatter
 */
// ======================================================================

#pragma once

#include "TextFormatter.h"
#include <set>

namespace TextGen
{
class CssTextFormatter : public TextFormatter
{
 public:
  ~CssTextFormatter() override = default;
  void dictionary(const std::shared_ptr<Dictionary>& theDict) override;

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

  std::string name() const { return "css"; }

 private:
  std::shared_ptr<Dictionary> itsDictionary;
  mutable std::set<std::string> itsUsedCssClasses;
  mutable std::string itsSectionVar;
  mutable std::string itsStoryVar;

};  // class CssTextFormatter
}  // namespace TextGen

// ======================================================================
