// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::GridClimatology
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::GridClimatology
 *
 * \brief Provides climatology analysis services to clients
 *
 * The GridClimatology class provides weather climatology analysis
 * services. All analysis is performed by analyzing the main
 * grid climatology. Probabilities are calculated by analyzing the
 * gridded climatology.
 *
 */
// ======================================================================

#include "GridClimatology.h"
#include "Acceptor.h"
#include "AnalysisSources.h"
#include "ParameterAnalyzerFactory.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"
#include "WeatherPeriodGenerator.h"
#include "WeatherResult.h"

#include <boost/shared_ptr.hpp>

using namespace boost;

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Analyze weather climatology for area
   *
   * \param theSources Analysis sources
   * \param theParameter The weather phenomenon to analyze
   * \param theAreaFunction The area function
   * \param theTimeFunction The time function
   * \param theSubTimeFunction The time function for subperiods
   * \param theArea The name of the area to analyze
   * \param thePeriods The time period to analyze
   * \param theAreaAcceptor The weather data acceptor in area integration
   * \param theTimeAcceptor The weather data acceptor in time integration
   * \param theTester The acceptor for Percentage calculations
   * \return The result of the analysis
   */
  // ----------------------------------------------------------------------

  const WeatherResult
  GridClimatology::analyze(const AnalysisSources & theSources,
						  const WeatherParameter & theParameter,
						  const WeatherFunction & theAreaFunction,
						  const WeatherFunction & theTimeFunction,
						  const WeatherFunction & theSubTimeFunction,
						  const WeatherArea & theArea,
						  const WeatherPeriodGenerator & thePeriods,
						  const Acceptor & theAreaAcceptor,
						  const Acceptor & theTimeAcceptor,
						  const Acceptor & theTester) const
  {

	shared_ptr<ParameterAnalyzer> analyzer;
	analyzer = ParameterAnalyzerFactory::create(theParameter);

	return analyzer->analyze(theSources,
							 Climatology,
							 theAreaFunction,
							 theTimeFunction,
							 theSubTimeFunction,
							 theArea,
							 thePeriods,
							 theAreaAcceptor,
							 theTimeAcceptor,
							 theTester);
  }


} // namespace WeatherAnalysis

// ======================================================================