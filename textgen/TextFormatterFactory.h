// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::TextFormatterFactory
 */
// ======================================================================

#pragma once

#include "TextFormatter.h"

namespace TextGen
{
namespace TextFormatterFactory
{
TextFormatter* create(const std::string& theType);

}  // namespace TextFormatterFactory
}  // namespace TextGen


// ======================================================================
