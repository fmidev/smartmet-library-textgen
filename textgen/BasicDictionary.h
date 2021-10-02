// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::BasicDictionary
 */
// ======================================================================

#pragma once

#include "Dictionary.h"

#include <map>
#include <string>

namespace TextGen
{
class BasicDictionary : public Dictionary
{
 public:
  using size_type = Dictionary::size_type;

  // Compiler generated:
  //
  // BasicDictionary();
  // BasicDictionary(const BasicDictionary & theDict);
  // BasicDictionary & operator=(const BasicDictionary & theDict);

  ~BasicDictionary() override;
  void init(const std::string& theLanguage) override;
  const std::string& language() const override;
  bool contains(const std::string& theKey) const override;
  const std::string& find(const std::string& theKey) const override;
  void insert(const std::string& theKey, const std::string& thePhrase) override;

  size_type size() const override;
  bool empty() const override;

 private:
  using StorageType = std::map<std::string, std::string>;
  StorageType itsData;
  std::string itsLanguage;

};  // class BasicDictionary

}  // namespace TextGen

// ======================================================================
