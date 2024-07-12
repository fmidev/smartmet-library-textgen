// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::Glyph
 */
// ======================================================================

#pragma once

#include <memory>
#include <string>

namespace TextGen
{
class Dictionary;
class TextFormatter;

class Glyph
{
 public:
  virtual ~Glyph() = default;
  virtual std::shared_ptr<Glyph> clone() const = 0;
  virtual std::string realize(const Dictionary& theDictionary) const = 0;
  virtual std::string realize(const TextFormatter& theFormatter) const = 0;
  virtual bool isDelimiter() const = 0;

 protected:
  Glyph() = default;
};  // class Glyph
}  // namespace TextGen

// ======================================================================
