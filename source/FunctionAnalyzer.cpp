// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::FunctionAnalyzer
 */
// ======================================================================

#include "FunctionAnalyzer.h"

#include "AnalysisSources.h"
#include "WeatherArea.h"
#include "WeatherLimits.h"
#include "WeatherPeriod.h"
#include "WeatherResult.h"

#include "NFmiDataModifierDummy.h"

using namespace std;

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Regular analyzis interface
   *
   * \param theSources The analysis sources
   * \param theLimits The analysis limits
   * \param thePeriod The analysis period
   * \param theArea The analysis area
   * \param theDataName The name of the data
   * \param theParameterName The parameter name
   * \return The analysis result
   */
  // ----------------------------------------------------------------------

  WeatherResult
  FunctionAnalyzer::analyze(const AnalysisSources & theSources,
							const WeatherLimits & theLimits,
							const WeatherPeriod & thePeriod,
							const WeatherArea & theArea,
							const string & theDataName,
							const string & theParameterName) const
  {
	// This is static since this is a dummy, avoids repeated creation
	static NFmiDataModifierDummy dummy;

	// The 0-interval implies the interval is meaningless, and hence dummy
	return analyze(theSources,
				   theLimits,
				   thePeriod,
				   theArea,
				   theDataName,
				   theParameterName,
				   0,
				   dummy);
  }

} // namespace WeatherAnalysis

// ======================================================================
