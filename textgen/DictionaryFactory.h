// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::DictionaryFactory
 */
// ======================================================================

#pragma once

#include "Dictionary.h"

namespace TextGen
{
namespace DictionaryFactory
{
Dictionary* create(const std::string& theType);

}  // namespace DictionaryFactory
}  // namespace TextGen

// ======================================================================
