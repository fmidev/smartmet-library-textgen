// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace WeatherAnalysis::CalculatorFactory
 */
// ======================================================================

#include "CalculatorFactory.h"
#include "CountCalculator.h"
#include "MaximumCalculator.h"
#include "MeanCalculator.h"
#include "MinimumCalculator.h"
#include "PercentageCalculator.h"
#include "RangeAcceptor.h"
#include "SumCalculator.h"
#include "TrendCalculator.h"
#include "WeatherAnalysisError.h"

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
		case Percentage:
		  return shared_ptr<Calculator>(new PercentageCalculator);
		case Count:
		  return shared_ptr<Calculator>(new CountCalculator);
		case Trend:
		  return shared_ptr<Calculator>(new TrendCalculator);
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
	 * \param theTester The tester for Percentage calculations
	 * \return The data modifier
	 */
	// ----------------------------------------------------------------------

	shared_ptr<Calculator> create(WeatherFunction theFunction,
								  const Acceptor & theTester)
	{
	  switch(theFunction)
		{
		case Mean:
		case Maximum:
		case Minimum:
		case Sum:
		case Trend:
		  return create(theFunction);
		case Percentage:
		  {
			shared_ptr<PercentageCalculator> tmp(new PercentageCalculator);
			tmp->condition(theTester);
			return tmp;
		  }
		case Count:
		  {
			shared_ptr<CountCalculator> tmp(new CountCalculator);
			tmp->condition(theTester);
			return tmp;
		  }
		}
	  
	  throw WeatherAnalysisError("CalculatorFactory failed to recognize the given function"+lexical_cast<string>(static_cast<int>(theFunction)));
	}


  } // namespace CalculatorFactory
} // namespace WeatherAnalysis

// ======================================================================
