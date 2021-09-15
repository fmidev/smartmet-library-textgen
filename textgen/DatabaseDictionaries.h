// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::DatabaseDictionaries
 */
// ======================================================================

#pragma once
#ifdef UNIX

#include "Dictionary.h"

#include <boost/shared_ptr.hpp>
#include <string>

namespace TextGen
{
class DatabaseDictionaries : public Dictionary
{
 public:
  typedef Dictionary::size_type size_type;

  virtual ~DatabaseDictionaries();
  DatabaseDictionaries(const std::string& theDictionaryId);
#ifdef NO_COMPILER_OPTIMIZE
  DatabaseDictionaries(const DatabaseDictionaries& theDict);
  DatabaseDictionaries& operator=(const DatabaseDictionaries& theDict);
#endif

  void init(const std::string& theLanguage);
  virtual const std::string& language(void) const;
  virtual bool contains(const std::string& theKey) const;
  virtual const std::string& find(const std::string& theKey) const;
  virtual void insert(const std::string& theKey, const std::string& thePhrase);

  virtual size_type size(void) const;
  virtual bool empty(void) const;
  void  changeLanguage(const std::string& theLanguage) override;

private:
  class Pimple;
  boost::shared_ptr<Pimple> itsPimple;

};  // class DatabaseDictionaries

}  // namespace TextGen

#endif  // UNIX

// ======================================================================
