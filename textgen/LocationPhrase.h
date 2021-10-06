// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::LocationPhrase
 */
// ======================================================================

#pragma once

#include "Glyph.h"
#include <string>

namespace TextGen
{
class Dictionary;

class LocationPhrase : public Glyph
{
 public:
  ~LocationPhrase() override;
  LocationPhrase(std::string  theLocation);
#ifdef NO_COMPILER_GENERATED
  LocationPhrase(const LocationPhrase& theLocationPhrase);
  LocationPhrase& operator=(const LocationPhrase& theLocationPhrase);
#endif
  boost::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;
  bool isDelimiter() const override;

 private:
  LocationPhrase() = delete;
  std::string itsLocation;

};  // class LocationPhrase

}  // namespace TextGen

// ======================================================================
