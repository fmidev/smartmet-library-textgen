// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::FileDictionaries
 */
// ======================================================================

#pragma once
#ifdef UNIX

#include "Dictionary.h"

#include <boost/shared_ptr.hpp>
#include <string>

namespace TextGen
{
class FileDictionaries : public Dictionary
{
 public:
  typedef Dictionary::size_type size_type;

  virtual ~FileDictionaries();
  FileDictionaries();
#ifdef NO_COMPILER_OPTIMIZE
  FileDictionaries(const FileDictionaries& theDict);
  FileDictionaries& operator=(const FileDictionaries& theDict);
#endif

  void init(const std::string& theLanguage);
  virtual const std::string& language(void) const;
  virtual bool contains(const std::string& theKey) const;
  virtual const std::string& find(const std::string& theKey) const;
  virtual void insert(const std::string& theKey, const std::string& thePhrase);

  virtual size_type size(void) const;
  virtual bool empty(void) const;
  void changeLanguage(const std::string& theLanguage) override;

 private:
  class Pimple;
  boost::shared_ptr<Pimple> itsPimple;

};  // class FileDictionaries

}  // namespace TextGen

#endif  // UNIX

// ======================================================================
