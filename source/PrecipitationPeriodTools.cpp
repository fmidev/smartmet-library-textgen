// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace WeatherAnalysis::PrecipitationPeriodTools
 */
// ======================================================================
/*!
 * \namespace WeatherAnalysis::PrecipitationPeriodTools
 *
 * \brief Tools related to finding precipitation time intervals
 *
 * The namespace contains tools for analyzing a single time
 * interval, finding all times when it rains, joining them
 * into precipitation periods, joining precipitation periods
 * which can be considered to be the same rain period and so on.
 *
 */
// ======================================================================

#include "PrecipitationPeriodTools.h"

#include "newbase/NFmiTime.h"

#include "AnalysisSources.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"

using namespace std;

namespace WeatherAnalysis
{
  namespace PrecipitationPeriodTools
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Find all moments when it rains for given area
	 *
	 * The variables controlling the algorithm are
	 * \code
	 * ::rainytime::minimum_rain = <0->     (default = 0.1)
	 * ::rainytime::minimum_area = <0-100>  (default = 20)
	 * \endcode
	 * The first variable is the required rain amount in a single
	 * point for the point to be considered rainy. Variable
	 * \c minimum_area is the minimum required areal coverage of
	 * rainy points for the area to be considered rainy. This
	 * number should be fairly small.
	 *
	 * \param theSources The analysis sources
	 * \param theArea The relevant area
	 * \param thePeriod The time interval to be analyzed
	 * \param theVar The variable controlling the algorithm
	 * \return Sorted list of rainy times
	 */
	// ----------------------------------------------------------------------

	RainTimes findRainTimes(const AnalysisSources & theSources,
							const WeatherArea & theArea,
							const WeatherPeriod & thePeriod,
							const std::string & theVar)
	{
	  RainTimes times;
	  return times;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Join rainy times into rain periods
	 *
	 * The variables controlling the algorithm are
	 * \code
	 * ::rainyperiod::maximum_interval = <1->  (default = 1)
	 * \endcode
	 * Variable \c maximum_interval determines the maximum allowed
	 * separation between any two rainy moments in time for them
	 * to be considered to belong into the same rain period.
	 * Typically one might use a value like 3 hours, but the default
	 * is nonetheless 1 hour. This forces the used to make a
	 * conscious choice on the maximum separation.
	 *
	 * \param theTimes The rainy times to be joined
	 * \param theVar The variable controlling the algorithm
	 * \return Sorted list of rainy periods
	 */
	// ----------------------------------------------------------------------

	RainPeriods findRainPeriods(const RainTimes & theTimes,
								const std::string & theVar)
	{
	  RainPeriods periods;
	  return periods;
	}


  } // namespace PrecipitationPeriodTools
} // namespace WeatherAnalysis

// ======================================================================
