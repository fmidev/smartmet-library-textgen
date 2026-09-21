// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::QualifiedDirectionPhrase
 */
// ======================================================================

#pragma once

#include "Dictionary.h"
#include "Phrase.h"
#include <boost/algorithm/string/replace.hpp>
#include <memory>
#include <string>

namespace TextGen
{
// A wind direction with a qualifier such as veering or backing, realized as one glyph so that
// it fits a single slot of a sentence template. The qualifier phrase is a dictionary key with
// two optional placeholders: [1] is replaced by the turn-to form of the direction
// ("pohjoiseen", "to the north") and [2] by the noun form ("1-tuulta", "northerly wind").
// Each language uses the form its grammar needs: Finnish "[1] kääntyvää tuulta", English
// "veering [2]". Unlike a flat composite phrase, a translation may ignore either placeholder.
class QualifiedDirectionPhrase : public Phrase
{
 public:
  QualifiedDirectionPhrase(const std::string& theQualifier,
                           std::string theTurnTo,
                           std::string theNoun)
      : Phrase(theQualifier), itsTurnTo(std::move(theTurnTo)), itsNoun(std::move(theNoun))
  {
  }

  std::shared_ptr<Glyph> clone() const override
  {
    return std::make_shared<QualifiedDirectionPhrase>(*this);
  }

  std::string realize(const Dictionary& theDictionary) const override
  {
    std::string text = Phrase::realize(theDictionary);
    if (text.find("[1]") != std::string::npos)
      boost::algorithm::replace_first(text, "[1]", theDictionary.find(itsTurnTo));
    if (text.find("[2]") != std::string::npos)
      boost::algorithm::replace_first(text, "[2]", theDictionary.find(itsNoun));
    return text;
  }

 private:
  std::string itsTurnTo;
  std::string itsNoun;
};
}  // namespace TextGen

// ======================================================================
