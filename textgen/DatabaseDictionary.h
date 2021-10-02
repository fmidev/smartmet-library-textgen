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
  using size_type = Dictionary::size_type;

  ~DatabaseDictionary() override;
  DatabaseDictionary();
#ifdef NO_COMPILER_OPTIMIZE
  DatabaseDictionary(const DatabaseDictionary& theDict);
  DatabaseDictionary& operator=(const DatabaseDictionary& theDict);
#endif

  void init(const std::string& theLanguage) override;
  virtual void getDataFromDB(const std::string& theLanguage,
                             std::map<std::string, std::string>& theDataStorage) = 0;
  const std::string& language() const override;
  bool contains(const std::string& theKey) const override;
  const std::string& find(const std::string& theKey) const override;
  void insert(const std::string& theKey, const std::string& thePhrase) override;

  size_type size() const override;
  bool empty() const override;
  void changeLanguage(const std::string& theLanguage) override;

 private:
  class Pimple;
  boost::shared_ptr<Pimple> itsPimple;

};  // class DatabaseDictionary

}  // namespace TextGen
#endif  // UNIX

// ======================================================================
