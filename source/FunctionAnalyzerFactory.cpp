// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace WeatherAnalysis::FunctionAnalyzerFactory
 */
// ======================================================================

#include "FunctionAnalyzerFactory.h"
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
	
	boost::shared_ptr<FunctionAnalyzer> create(const WeatherFunction & theFunction)
	{
	  switch(theFunction)
		{

		}
	  throw WeatherAnalysisError("FunctionAnalyzerFactory does not support requested function");
	}

  } // namespace FunctionAnalyzerFactory
} // namespace WeatherAnalysis

// ======================================================================
