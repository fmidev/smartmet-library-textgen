// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::FunctionAnalyzer
 */
// ======================================================================

#include "FunctionAnalyzer.h"

#include "Acceptor.h"
#include "AnalysisSources.h"
#include "NullCalculator.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"
#include "WeatherResult.h"

using namespace std;

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Regular analysis interface
   *
   * \param theSources The analysis sources
   * \param theAcceptor The data acceptor
   * \param thePeriod The analysis period
   * \param theArea The analysis area
   * \param theDataName The name of the data
   * \param theParameterName The parameter name
   * \return The analysis result
   */
  // ----------------------------------------------------------------------

  WeatherResult
  FunctionAnalyzer::analyze(const AnalysisSources & theSources,
							const WeatherPeriod & thePeriod,
							const WeatherArea & theArea,
							const Acceptor & theAcceptor,
							const Acceptor & theTester,
							const string & theDataName,
							const string & theParameterName) const
  {
	// This is static since this is a dummy, avoids repeated creation
	static NullCalculator dummy;

	// The 0-interval implies the interval is meaningless, and hence dummy
	return analyze(theSources,
				   thePeriod,
				   theArea,
				   theAcceptor,
				   theTester,
				   theDataName,
				   theParameterName,
				   0,
				   dummy);
  }

} // namespace WeatherAnalysis

// ======================================================================
