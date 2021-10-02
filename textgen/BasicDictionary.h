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
  typedef Dictionary::size_type size_type;

  // Compiler generated:
  //
  // BasicDictionary();
  // BasicDictionary(const BasicDictionary & theDict);
  // BasicDictionary & operator=(const BasicDictionary & theDict);

  ~BasicDictionary() override;
  void init(const std::string& theLanguage) override;
  const std::string& language(void) const override;
  bool contains(const std::string& theKey) const override;
  const std::string& find(const std::string& theKey) const override;
  void insert(const std::string& theKey, const std::string& thePhrase) override;

  size_type size(void) const override;
  bool empty(void) const override;

 private:
  typedef std::map<std::string, std::string> StorageType;
  StorageType itsData;
  std::string itsLanguage;

};  // class BasicDictionary

}  // namespace TextGen

// ======================================================================
