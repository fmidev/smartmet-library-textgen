// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace WeatherAnalysis::FunctionAnalyzerFactory
 */
// ======================================================================

#include "FunctionAnalyzerFactory.h"
#include "MeanAnalyzer.h"
#include "MaxMaximumAnalyzer.h"
#include "MeanMaximumAnalyzer.h"
#include "MinMaximumAnalyzer.h"
#include "MaxMinimumAnalyzer.h"
#include "MeanMinimumAnalyzer.h"
#include "MinMinimumAnalyzer.h"
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
		case Mean:
		  return boost::shared_ptr<FunctionAnalyzer>(new MeanAnalyzer);
		case MaxMaximum:
		  return boost::shared_ptr<FunctionAnalyzer>(new MaxMaximumAnalyzer);
		case MeanMaximum:
		  return boost::shared_ptr<FunctionAnalyzer>(new MeanMaximumAnalyzer);
		case MinMaximum:
		  return boost::shared_ptr<FunctionAnalyzer>(new MinMaximumAnalyzer);
		case MaxMinimum:
		  return boost::shared_ptr<FunctionAnalyzer>(new MaxMinimumAnalyzer);
		case MeanMinimum:
		  return boost::shared_ptr<FunctionAnalyzer>(new MeanMinimumAnalyzer);
		case MinMinimum:
		  return boost::shared_ptr<FunctionAnalyzer>(new MinMinimumAnalyzer);
		}
	  throw WeatherAnalysisError("FunctionAnalyzerFactory does not support requested function");
	}

  } // namespace FunctionAnalyzerFactory
} // namespace WeatherAnalysis

// ======================================================================
