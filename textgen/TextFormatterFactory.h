// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::TextFormatterFactory
 */
// ======================================================================

#pragma once

#include <string>

namespace TextGen
{
class TextFormatter;

namespace TextFormatterFactory
{
TextFormatter* create(const std::string& theType);

}  // namespace TextFormatterFactory
}  // namespace TextGen

// ======================================================================
