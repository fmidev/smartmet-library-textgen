// ======================================================================
/*!
 * \file
 * \brief Implementation of class MinimumAnalyzer
 */
// ======================================================================

#include "MinimumAnalyzer.h"
#include "AnalysisSources.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"
#include "WeatherResult.h"

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Analyze parameter minimum value
   *
   * \param theSources Analysis sources
   * \param theLimits Analysis limits, not used
   * \param thePeriod Analysis period
   * \param theArea Analysis area
   * \param theDataName The name of the data file
   * \param theParameterName The name of the parameter
   * \return The analysis result
   */
  // ----------------------------------------------------------------------

  WeatherResult
  MinimumAnalyzer::analyze(const AnalysisSources & theSources,
						   const WeatherLimits & theLimits,
						   const WeatherPeriod & thePeriod,
						   const WeatherArea & theArea,
						   const std::string & theDataName,
						   const std::string & theParameterName) const
  {
	return WeatherResult(kFloatMissing,0);
  }


} // namespace WeatherAnalysis

// ======================================================================
