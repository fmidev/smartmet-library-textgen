// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalyzer
 */
// ======================================================================

#include "WeatherAnalyzer.h"
#include "Acceptor.h"
#include "AnalysisSources.h"
#include "Settings.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"
#include "WeatherResult.h"

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Analyze weather data for area
   *
   * \param theFakeVariable The Settings variable containing the optional
   *                        result. Useful for debugging and regression
   *                        testing.
   * \param theSources Analysis sources
   * \param theParameter The weather phenomenon to analyze
   * \param theAreaFunction The area function
   * \param theTimeFunction The time function
   * \param thePeriod The time period to analyze
   * \param theArea The name of the area to analyze
   * \param theAreaAcceptor The weather data acceptor in area integration
   * \param theTimeAcceptor The weather data acceptor in time integration
   * \param theTester The acceptor for Percentage calculations
   * \return The result of the analysis
   */
  // ----------------------------------------------------------------------

  WeatherResult
  WeatherAnalyzer::analyze(const std::string & theFakeVariable,
						   const AnalysisSources & theSources,
						   const WeatherParameter & theParameter,
						   const WeatherFunction & theAreaFunction,
						   const WeatherFunction & theTimeFunction,
						   const WeatherPeriod & thePeriod,
						   const WeatherArea & theArea,
						   const Acceptor & theAreaAcceptor,
						   const Acceptor & theTimeAcceptor,
						   const Acceptor & theTester) const
  {
	if(Settings::isset(theFakeVariable))
	  return Settings::require_result(theFakeVariable);

	return analyze(theSources,
				   theParameter,
				   theAreaFunction,
				   theTimeFunction,
				   thePeriod,
				   theArea,
				   theAreaAcceptor,
				   theTimeAcceptor,
				   theTester);
  }
  
} // namespace WeatherAnalysis

// ======================================================================
