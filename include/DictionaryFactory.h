// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::DictionaryFactory
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
 * boost::shared_ptr<Dictionary> dict1 = DictionaryFactory::create("null");
 * boost::shared_ptr<Dictionary> dict2 = DictionaryFactory::create("basic");
 * boost::shared_ptr<Dictionary> dict3 = DictionaryFactory::create("mysql");
 * \endcode
 *
 */
// ======================================================================

#ifndef TEXTGEN_DICTIONARYFACTORY_H
#define TEXTGEN_DICTIONARYFACTORY_H

#include "Dictionary.h"
#include "boost/shared_ptr.hpp"

namespace TextGen
{
  namespace DictionaryFactory
  {
	boost::shared_ptr<Dictionary> create(const std::string & theType);

  } // namespace DictionaryFactory
} // namespace TextGen

#endif // TEXTGEN_DICTIONARYFACTORY_H

// ======================================================================

