// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace WeatherAnalysis::FunctionAnalyzerFactory
 */
// ======================================================================

#include "FunctionAnalyzerFactory.h"
#include "MaxMaximumAnalyzer.h"
#include "MaxMinimumAnalyzer.h"
#include "MaxSumAnalyzer.h"
#include "MeanAnalyzer.h"
#include "MeanMaximumAnalyzer.h"
#include "MeanMinimumAnalyzer.h"
#include "MeanSumAnalyzer.h"
#include "MinMaximumAnalyzer.h"
#include "MinMinimumAnalyzer.h"
#include "MinSumAnalyzer.h"
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

		case MeanSum:
		  return boost::shared_ptr<FunctionAnalyzer>(new MeanSumAnalyzer);
		case MaxSum:
		  return boost::shared_ptr<FunctionAnalyzer>(new MaxSumAnalyzer);
		case MinSum:
		  return boost::shared_ptr<FunctionAnalyzer>(new MinSumAnalyzer);

		}
	  throw WeatherAnalysisError("FunctionAnalyzerFactory does not support requested function");
	}

  } // namespace FunctionAnalyzerFactory
} // namespace WeatherAnalysis

// ======================================================================
