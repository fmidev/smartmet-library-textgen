// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::PoDictionary
 */
// ======================================================================

#pragma once

#include "Dictionary.h"

#include <memory>
#include <string>

namespace TextGen
{
class PoDictionary : public Dictionary
{
 public:
  using size_type = Dictionary::size_type;

  ~PoDictionary() override;
  PoDictionary();
#ifdef NO_COMPILER_OPTIMIZE
  PoDictionary(const PoDictionary& theDict);
  PoDictionary& operator=(const PoDictionary& theDict);
#endif

  void init(const std::string& theLanguage) override;
  const std::string& language() const override;
  bool contains(const std::string& theKey) const override;
  std::string find(const std::string& theKey) const override;
  void insert(const std::string& theKey, const std::string& thePhrase) override;

  size_type size() const override;
  bool empty() const override;
  void changeLanguage(const std::string& theLanguage) override;

 private:
  class Pimple;
  std::shared_ptr<Pimple> itsPimple;

};  // class PoDictionary

}  // namespace TextGen

// ======================================================================
