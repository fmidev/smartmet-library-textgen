// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::PoDictionaries
 */
// ======================================================================

#pragma once
#ifdef UNIX

#include "Dictionary.h"

#include <memory>
#include <string>

namespace TextGen
{
class PoDictionaries : public Dictionary
{
 public:
  using size_type = Dictionary::size_type;

  ~PoDictionaries() override;
  PoDictionaries();
#ifdef NO_COMPILER_OPTIMIZE
  PoDictionaries(const PoDictionaries& theDict);
  PoDictionaries& operator=(const PoDictionaries& theDict);
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

};  // class PoDictionaries

}  // namespace TextGen

#endif  // UNIX

// ======================================================================
