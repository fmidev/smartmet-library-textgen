// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace FunctionAnalyzerFactory
 */
// ======================================================================

#include "FunctionAnalyzerFactory.h"
#include "MaximumAnalyzer.h"
#include "MinimumAnalyzer.h"
#include "MeanAnalyzer.h"
#include "WeatherAnalysisError.h"

namespace WeatherAnalysis
{
  namespace FunctionAnalyzerFactory
  {
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief Create a new regular function analyzer
	 */
	// ----------------------------------------------------------------------
	
	std::auto_ptr<FunctionAnalyzer> create(const WeatherFunction & theFunction)
	{
	  switch(theFunction)
		{
		case Maximum:
		  return std::auto_ptr<FunctionAnalyzer>(new MaximumAnalyzer);
		case Minimum:
		  return std::auto_ptr<FunctionAnalyzer>(new MinimumAnalyzer);
		case Mean:
		  return std::auto_ptr<FunctionAnalyzer>(new MeanAnalyzer);
		}
	  throw WeatherAnalysisError("FunctionAnalyzerFactory does not support requested function");
	}

  } // namespace FunctionAnalyzerFactory
} // namespace WeatherAnalysis

// ======================================================================
