// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::DictionaryFactory
 */
// ======================================================================

#ifndef TEXTGEN_DICTIONARYFACTORY_H
#define TEXTGEN_DICTIONARYFACTORY_H

#include "Dictionary.h"
#include <boost/shared_ptr.hpp>

namespace TextGen
{
  namespace DictionaryFactory
  {
	boost::shared_ptr<Dictionary> create(const std::string & theType);

  } // namespace DictionaryFactory
} // namespace TextGen

#endif // TEXTGEN_DICTIONARYFACTORY_H

// ======================================================================

