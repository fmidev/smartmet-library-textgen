// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::UnitFactory
 */
// ======================================================================

#ifndef TEXTGEN_UNITFACTORY_H
#define TEXTGEN_UNITFACTORY_H

#include "boost/shared_ptr.hpp"

namespace TextGen
{
  class Sentence;

  enum Units
	{
	  DegreesCelsius,
	  MetersPerSecond,
	  Millimeters,
	  Percent,
	  HectoPascal
	};

  namespace UnitFactory
  {

	boost::shared_ptr<Sentence> create(Units theUnit);

  } // namespace NumberFactory
} // namespace TextGen

#endif // TEXTGEN_UNITFACTORY_H

// ======================================================================
