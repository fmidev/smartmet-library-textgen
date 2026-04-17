// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::PostgreSQLDictionary
 *
 * \deprecated Scheduled for removal after the PO-based dictionary release.
 * Use PoDictionary / PoDictionaries instead. The `postgresql` and
 * `multipostgresql` factory values remain accepted for one deprecation
 * cycle.
 */
// ======================================================================

#pragma once
#ifdef UNIX

#include "DatabaseDictionary.h"

namespace TextGen
{
class PostgreSQLDictionary : public DatabaseDictionary
{
 public:
  PostgreSQLDictionary() { itsDictionaryId = "postgresql"; }

 private:
  void getDataFromDB(const std::string& theLanguage,
                     std::map<std::string, std::string>& theDataStorage) override;

};  // class PostgreSQLDictionary

}  // namespace TextGen
#endif  // UNIX

// ======================================================================
