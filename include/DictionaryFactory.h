// ======================================================================
/*!
 * \file
 * \brief Interface of TextGen::DictionaryFactory namespace
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
 * std::auto_ptr<Dictionary> dict1 = DictionaryFactory::create("null");
 * std::auto_ptr<Dictionary> dict2 = DictionaryFactory::create("basic");
 * std::auto_ptr<Dictionary> dict3 = DictionaryFactory::create("mysql");
 * \endcode
 *
 */
// ======================================================================

#ifndef TEXTGEN_DICTIONARYFACTORY_H
#define TEXTGEN_DICTIONARYFACTORY_H

#include "Dictionary.h"
#include <memory>

namespace TextGen
{
  namespace DictionaryFactory
  {

	std::auto_ptr<Dictionary> create(const std::string & theType);

  } // namespace DictionaryFactory
} // namespace TextGen

#endif // TEXTGEN_DICTIONARYFACTORY_H

// ======================================================================

