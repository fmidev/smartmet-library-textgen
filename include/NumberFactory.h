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
 * Note that we return always on purpose a Sentence so that
 * we may append the number to a sentence.
 *
 */
// ======================================================================

#ifndef TEXTGEN_NUMBERFACTORY_H
#define TEXTGEN_NUMBERFACTORY_H

#include "boost/shared_ptr.hpp"

namespace TextGen
{
  class Sentence;

  namespace NumberFactory
  {

	boost::shared_ptr<Sentence> create(int theNumber);
	boost::shared_ptr<Sentence> create(int theLoLimit, int theHiLimit);

  } // namespace NumberFactory
} // namespace TextGen

#endif // TEXTGEN_NUMBERFACTORY_H

// ======================================================================
