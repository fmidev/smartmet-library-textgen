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
	 * ::minimum_rain = <0->     (default = 0.1)
	 * ::minimum_area = <0-100>  (default = 20)
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

	std::list<NFmiTime> findRainTimes(const AnalysisSources & theSources,
									  const WeatherArea & theArea,
									  const WeatherPeriod & thePeriod,
									  const std::string & theVar)
	{
	  list<NFmiTime> times;
	  return times;
	}



  } // namespace PrecipitationPeriodTools
} // namespace WeatherAnalysis

// ======================================================================
