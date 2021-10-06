// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::HtmlTextFormatter
 */
// ======================================================================

#pragma once

#include "TextFormatter.h"

namespace TextGen
{
class HtmlTextFormatter : public TextFormatter
{
 public:
  ~HtmlTextFormatter() override {}
  void dictionary(const boost::shared_ptr<Dictionary>& theDict) override;

  std::string format(const Glyph& theGlyph) const override;

  // override for all composites
  std::string visit(const Glyph& theGlyph) const override;
  std::string visit(const Integer& theInteger) const override;
  std::string visit(const Real& theReal) const override;
  std::string visit(const IntegerRange& theRange) const override;
  std::string visit(const Sentence& theSentence) const override;
  std::string visit(const Paragraph& theParagraph) const override;
  std::string visit(const Header& theHeader) const override;
  std::string visit(const Document& theDocument) const override;
  std::string visit(const WeatherTime& theTime) const override;
  std::string visit(const TimePeriod& thePeriod) const override;

  std::string visit(const SectionTag& theSectionTag) const override;
  std::string visit(const StoryTag& theStoryTag) const override;

  std::string name() const { return "html"; }

 private:
  boost::shared_ptr<Dictionary> itsDictionary;
  mutable std::string itsSectionVar;
  mutable std::string itsStoryVar;

};  // class HtmlTextFormatter
}  // namespace TextGen

// ======================================================================
