// ======================================================================
/*!
 * \file
 * \brief Interface of namespace WeatherAnalysis::CalculatorFactory
 */
// ======================================================================

#ifndef WEATHERANALYSIS_CALCULATORFACTORY_H
#define WEATHERANALYSIS_CALCULATORFACTORY_H

#include "WeatherFunction.h"

#include "Calculator.h"

namespace WeatherAnalysis
{
  class Acceptor;

  namespace CalculatorFactory
  {
	Calculator * create(WeatherFunction theFunction);

	Calculator * create(WeatherFunction theFunction,
						int theModulo);

	Calculator * create(WeatherFunction theFunction,
						const Acceptor & theTester);

	Calculator * create(WeatherFunction theFunction,
						const Acceptor & theTester,
						int theModulo);

  } // namespace CalculatorFactory
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_CALCULATORFACTORY_H

// ======================================================================
