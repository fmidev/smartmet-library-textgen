// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::FileDictionary
 */
// ======================================================================

#pragma once

#include "Dictionary.h"

#include <memory>
#include <string>

namespace TextGen
{
class FileDictionary : public Dictionary
{
 public:
  using size_type = Dictionary::size_type;

  ~FileDictionary() override;
  FileDictionary();
#ifdef NO_COMPILER_OPTIMIZE
  FileDictionary(const FileDictionary& theDict);
  FileDictionary& operator=(const FileDictionary& theDict);
#endif

  void init(const std::string& theLanguage) override;
  const std::string& language() const override;
  bool contains(const std::string& theKey) const override;
  const std::string& find(const std::string& theKey) const override;
  void insert(const std::string& theKey, const std::string& thePhrase) override;

  size_type size() const override;
  bool empty() const override;
  void changeLanguage(const std::string& theLanguage) override;

 private:
  class Pimple;
  std::shared_ptr<Pimple> itsPimple;

};  // class FileDictionary

}  // namespace TextGen

// ======================================================================
