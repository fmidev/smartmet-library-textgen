// ======================================================================
/*!
 * \file
 * \brief Interface of TextGen::TextFormatterFactory namespace
 */
// ======================================================================
/*!
 * \namespace TextGen::TextFormatterFactory
 *
 * \brief TextFormatter creation services
 *
 * The responsibility of the TextFormatterFactory namespace is to provide
 * TextFormatter creation services.
 *
 * For example,
 * \code
 * boost::shared_ptr<TextFormatter> form1 = TextFormatterFactory::create("plain");
 * boost::shared_ptr<TextFormatter> form2 = TextFormatterFactory::create("html");
 * \endcode
 *
 */
// ======================================================================

#ifndef TEXTGEN_TEXTFORMATTERFACTORY_H
#define TEXTGEN_TEXTFORMATTERFACTORY_H

#include "TextFormatter.h"
#include "boost/shared_ptr.hpp"

namespace TextGen
{
  namespace TextFormatterFactory
  {
	boost::shared_ptr<TextFormatter> create(const std::string & theType);

  } // namespace TextFormatterFactory
} // namespace TextGen

#endif // TEXTGEN_TEXTFORMATTERFACTORY_H

// ======================================================================

