// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::DictionaryFactory
 */
// ======================================================================
/*!
 * \namespace TextGen::DictionaryFactory
 *
 * \brief Dictionary creation services
 *
 * The responsibility of the DictionaryFactory namespace is to provide
 * dictionary creation services.
 *
 * For example,
 * \code
 * std::shared_ptr<Dictionary> dict1(DictionaryFactory::create("null"));
 * std::shared_ptr<Dictionary> dict2(DictionaryFactory::create("basic"));
 * std::shared_ptr<Dictionary> dict3(DictionaryFactory::create("mysql"));
 * std::shared_ptr<Dictionary> dict3(DictionaryFactory::create("file"));
 * \endcode
 *
 */
// ======================================================================

#include "DictionaryFactory.h"
#include "BasicDictionary.h"
#include "DatabaseDictionaries.h"
#include "FileDictionaries.h"
#include "FileDictionary.h"
#include "MySQLDictionary.h"
#include "NullDictionary.h"
#include "PostgreSQLDictionary.h"
#include <calculator/TextGenError.h>

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Create a dictionary of given type
 *
 * \param theType The type of the dictionary (null|basic|mysql)
 * \return An uninitialized dictionary
 */
// ----------------------------------------------------------------------

Dictionary* DictionaryFactory::create(const std::string& theType)
{
  if (theType == "null")
    return new NullDictionary();
  if (theType == "basic")
    return new BasicDictionary();
  if (theType == "file")
    return new FileDictionary();
  if (theType == "multifile")
    return new FileDictionaries();
#ifdef UNIX
  if (theType == "mysql")
    return new MySQLDictionary();
  if (theType == "multimysql")
    return new DatabaseDictionaries("mysql");
  if (theType == "postgresql")
    return new PostgreSQLDictionary();
  if (theType == "multipostgresql")
    return new DatabaseDictionaries("postgresql");
#endif  // UNIX

  throw TextGenError("Error: Unknown dictionary type " + theType);
}

}  // namespace TextGen

// ======================================================================
