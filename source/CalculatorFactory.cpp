// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace WeatherAnalysis::CalculatorFactory
 */
// ======================================================================

#include "CalculatorFactory.h"
#include "WeatherLimits.h"
#include "WeatherAnalysisError.h"

#include "MeanCalculator.h"
#include "MinimumCalculator.h"
#include "MaximumCalculator.h"
#include "SumCalculator.h"
#include "RangeAcceptor.h"

#include "boost/lexical_cast.hpp"

#include <string>

using namespace boost;
using namespace std;

namespace WeatherAnalysis
{
  namespace CalculatorFactory
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Create an calculator suitable for the given WeatherFunction
	 *
	 * Throws if there is no suitable data modifier.
	 *
	 * \param theFunction The weather function
	 * \return The data modifier
	 */
	// ----------------------------------------------------------------------

	shared_ptr<Calculator> create(WeatherFunction theFunction)
	{
	  switch(theFunction)
		{
		case Mean:
		  return shared_ptr<Calculator>(new MeanCalculator);
		case Maximum:
		  return shared_ptr<Calculator>(new MaximumCalculator);
		case Minimum:
		  return shared_ptr<Calculator>(new MinimumCalculator);
		case Sum:
		  return shared_ptr<Calculator>(new SumCalculator);
		}

	  throw WeatherAnalysisError("CalculatorFactory failed to recognize the given function"+lexical_cast<string>(static_cast<int>(theFunction)));
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Create an calculator suitable for the given WeatherFunction
	 *
	 * Throws if there is no suitable data modifier.
	 *
	 * \param theFunction The weather function
	 * \param theLimits The weather limits
	 * \return The data modifier
	 */
	// ----------------------------------------------------------------------

	shared_ptr<Calculator> create(WeatherFunction theFunction,
								  const WeatherLimits & theLimits)
	{
	  shared_ptr<Calculator> ret = create(theFunction);

	  RangeAcceptor range;
	  if(theLimits.hasLowerLimit())
		range.lowerLimit(theLimits.lowerLimit());
	  if(theLimits.hasUpperLimit())
		range.upperLimit(theLimits.upperLimit());

	  ret->acceptor(range);
	  return ret;

	}

  } // namespace CalculatorFactory
} // namespace WeatherAnalysis

// ======================================================================
