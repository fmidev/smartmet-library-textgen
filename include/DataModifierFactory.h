// ======================================================================
/*!
 * \file
 * \brief Interface of namespace WeatherAnalysis::DataModifierFactory
 */
// ======================================================================
/*!
 * \namespace WeatherAnalysis::DataModifierFactory
 *
 * \brief Creating NFmiDataModifier objects
 *
 * Newbase does not have a factory, and it wouldn't recognize
 * weatheranalysis enumerations anyway.
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_DATAMODIFIERFACTORY_H
#define WEATHERANALYSIS_DATAMODIFIERFACTORY_H

#include "WeatherFunction.h"
#include "boost/shared_ptr.hpp"

#include "NFmiDataModifier.h"

namespace WeatherAnalysis
{
  class WeatherLimits;

  namespace DataModifierFactory
  {
	boost::shared_ptr<NFmiDataModifier> create(WeatherFunction theFunction);

	boost::shared_ptr<NFmiDataModifier> create(WeatherFunction theFunction,
											   const WeatherLimits & theLimits);

  } // namespace DataModifierFactory
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_DATAMODIFIERFACTORY_H

// ======================================================================
