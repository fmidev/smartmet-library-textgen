// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::SevereFrostAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::SevereFrostAnalyzer
 *
 * \brief SevereFrost analysis functions
 *
 */
// ======================================================================

#include "SevereFrostAnalyzer.h"
#include "AnalysisSources.h"
#include "NullPeriodGenerator.h"
#include "RegularFunctionAnalyzer.h"
#include "WeatherResult.h"

using namespace std;
using namespace boost;

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Analyze severefrost in an area
   *
   * \param theSources Analysis sources
   * \param theDataType The source type to be used
   * \param theAreaFunction The area function to analyze
   * \param theTimeFunction The time function to analyze
   * \param theSubTimeFunction The subtime function to analyze
   * \param theArea The area
   * \param thePeriods The time period generator
   * \param theAreaAcceptor The acceptor for data in area integrals
   * \param theTimeAcceptor The acceptor for data in time integrals
   * \param theTester The optional acceptor for Percentage calculations
   */
  // ----------------------------------------------------------------------
  
  const WeatherResult
  SevereFrostAnalyzer::analyze(const AnalysisSources & theSources,
							   const WeatherDataType & theDataType,
							   const WeatherFunction & theAreaFunction,
							   const WeatherFunction & theTimeFunction,
							   const WeatherFunction & theSubTimeFunction,
							   const WeatherArea & theArea,
							   const WeatherPeriodGenerator & thePeriods,
							   const Acceptor & theAreaAcceptor,
							   const Acceptor & theTimeAcceptor,
							   const Acceptor & theTester) const
  {
	const string varname = "textgen::severefrost";
	const string parname = "SevereFrostProbability";

	auto_ptr<FunctionAnalyzer> analyzer(new RegularFunctionAnalyzer(theAreaFunction,theTimeFunction,theSubTimeFunction));

	return analyzer->analyze(theSources,theDataType,
							 theArea,thePeriods,
							 theAreaAcceptor,theTimeAcceptor,theTester,
							 varname,parname);

  }

} // namespace WeatherAnalysis

// ======================================================================
