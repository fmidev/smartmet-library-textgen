// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::MySQLDictionary
 */
// ======================================================================

#pragma once
#ifdef UNIX

#include "DatabaseDictionary.h"

namespace TextGen
{
class MySQLDictionary : public DatabaseDictionary
{
 public:
  MySQLDictionary() { itsDictionaryId = "mysql"; }

 private:
  void getDataFromDB(const std::string& theLanguage,
                     std::map<std::string, std::string>& theDataStorage) override;
};  // class MySQLDictionary

}  // namespace TextGen
#endif  // UNIX

// ======================================================================
