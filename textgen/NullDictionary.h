// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::NullDictionary
 */
// ======================================================================

#pragma once

#include "Dictionary.h"

#include <string>

namespace TextGen
{
class NullDictionary : public Dictionary
{
 public:
  using size_type = Dictionary::size_type;

  // Compiler generated:
  //
  // NullDictionary();
  // NullDictionary(const NullDictionary & theDict);
  // NullDictionary & operator=(const NullDictionary & theDict);

  ~NullDictionary() override {}
  void init(const std::string& theLanguage) override { itsLanguage = theLanguage; }
  const std::string& language() const override { return itsLanguage; }
  bool contains(const std::string& theKey) const override { return false; }
  const std::string& find(const std::string& theKey) const override;
  void insert(const std::string& theKey, const std::string& thePhrase) override;

  size_type size() const override { return 0; }
  bool empty() const override { return true; }

 private:
  std::string itsLanguage;

};  // class NullDictionary

}  // namespace TextGen

// ======================================================================
