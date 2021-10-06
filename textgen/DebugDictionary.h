// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::DebugDictionary
 */
// ======================================================================

#pragma once

#include "Dictionary.h"

#include <string>

namespace TextGen
{
class DebugDictionary : public Dictionary
{
 public:
  using size_type = Dictionary::size_type;

  // Compiler generated:
  //
  // DebugDictionary();
  // DebugDictionary(const DebugDictionary & theDict);
  // DebugDictionary & operator=(const DebugDictionary & theDict);

  ~DebugDictionary() override = default;
  void init(const std::string& theLanguage) override { itsLanguage = theLanguage; }
  const std::string& language() const override { return itsLanguage; }
  bool contains(const std::string& theKey) const override { return true; }
  const std::string& find(const std::string& theKey) const override { return theKey; }
  void insert(const std::string& theKey, const std::string& thePhrase) override {}
  size_type size() const override { return 0; }
  bool empty() const override { return false; }

 private:
  std::string itsLanguage;

};  // class DebugDictionary

}  // namespace TextGen

// ======================================================================
