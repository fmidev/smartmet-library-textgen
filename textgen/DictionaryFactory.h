// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::DictionaryFactory
 */
// ======================================================================

#pragma once

#include <string>

namespace TextGen
{
class Dictionary;

namespace DictionaryFactory
{
Dictionary* create(const std::string& theType);

}  // namespace DictionaryFactory
}  // namespace TextGen

// ======================================================================
