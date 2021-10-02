// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::DatabaseDictionary
 */
// ======================================================================

#pragma once
#ifdef UNIX

#include "Dictionary.h"
#include <boost/shared_ptr.hpp>
#include <string>

namespace TextGen
{
class DatabaseDictionary : public Dictionary
{
 public:
  typedef Dictionary::size_type size_type;

  virtual ~DatabaseDictionary();
  DatabaseDictionary();
#ifdef NO_COMPILER_OPTIMIZE
  DatabaseDictionary(const DatabaseDictionary& theDict);
  DatabaseDictionary& operator=(const DatabaseDictionary& theDict);
#endif

  virtual void init(const std::string& theLanguage);
  virtual void getDataFromDB(const std::string& theLanguage,
                             std::map<std::string, std::string>& theDataStorage) = 0;
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

};  // class DatabaseDictionary

}  // namespace TextGen
#endif  // UNIX

// ======================================================================
