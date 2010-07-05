// ======================================================================
/*!
 * \file
 * \brief Interface of namespace WeatherAnalysis::CalculatorFactory
 */
// ======================================================================

#ifndef WEATHERANALYSIS_CALCULATORFACTORY_H
#define WEATHERANALYSIS_CALCULATORFACTORY_H

#include "WeatherFunction.h"
#include <boost/shared_ptr.hpp>

#include "Calculator.h"

namespace WeatherAnalysis
{
  class Acceptor;

  namespace CalculatorFactory
  {
	boost::shared_ptr<Calculator> create(WeatherFunction theFunction);

	boost::shared_ptr<Calculator> create(WeatherFunction theFunction,
										 int theModulo);

	boost::shared_ptr<Calculator> create(WeatherFunction theFunction,
										 const Acceptor & theTester);

	boost::shared_ptr<Calculator> create(WeatherFunction theFunction,
										 const Acceptor & theTester,
										 int theModulo);

  } // namespace CalculatorFactory
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_CALCULATORFACTORY_H

// ======================================================================
