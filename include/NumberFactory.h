// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::NumberFactory
 */
// ======================================================================
/*!
 * \namespace TextGen::NumberFactory
 *
 * \brief Provides Number and NumberRange creation services.
 *
 * The purpose of the factory is to decouple the formatting
 * of numbers from their creation. The desired formatting is
 * specified by a global Settings variable.
 *
 */
// ======================================================================

#ifndef TEXTGEN_NUMBERFACTORY_H
#define TEXTGEN_NUMBERFACTORY_H

#include "Glyph.h"
#include "boost/shared_ptr.hpp"

namespace TextGen
{
  namespace NumberFactory
  {

	boost::shared_ptr<Glyph> create(int theNumber);
	boost::shared_ptr<Glyph> create(int theLoLimit, int theHiLimit);

  } // namespace NumberFactory
} // namespace TextGen

#endif // TEXTGEN_NUMBERFACTORY_H

// ======================================================================
