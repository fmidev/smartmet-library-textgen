// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace WeatherAnalysis::WeatherPeriodTools
 */
// ======================================================================

#include "WeatherPeriodTools.h"
#include "WeatherPeriod.h"
#include "WeatherAnalysisError.h"

#include "NFmiTime.h"

#include "boost/lexical_cast.hpp"

using namespace std;
using namespace boost;

namespace WeatherAnalysis
{
  
  namespace WeatherPeriodTools
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Count the number of specified subperiods in the period
	 *
	 * \param thePeriod The main period
	 * \param theStartHour The normal start hour of the subperiod
	 * \param theEndHour The normal end hour of the subperiod
	 * \param theMaxStartHour The maximum start hour of the subperiod
	 * \param theMinEndHour The minimum end hour of the subperiod
	 * \return The number of acceptable subperiods in the period
	 */
	// ----------------------------------------------------------------------

	int countPeriods(const WeatherPeriod & thePeriod,
					 int theStartHour,
					 int theEndHour,
					 int theMaxStartHour,
					 int theMinEndHour)
	{
	  int count = 0;

	  NFmiTime start(thePeriod.localStartTime());
	  start.ChangeByDays(-1);
	  start.SetHour(theStartHour);

	  // maximum allowed difference in hours after start of period
	  const int maxdiff1 = (theMaxStartHour >= theStartHour ?
							theMaxStartHour - theStartHour :
							theMaxStartHour + 24 - theStartHour);

	  // maximum allowed difference in hours before end of period
	  const int maxdiff2 = (theMinEndHour <= theEndHour ?
							theEndHour - theMinEndHour :
							theEndHour + 24 - theMinEndHour);

	  for(;
		  !thePeriod.localEndTime().IsLessThan(start);
		  start.ChangeByDays(1))
		{
		  NFmiTime end(start);
		  if(theEndHour <= theStartHour)
			end.ChangeByDays(1);
		  end.SetHour(theEndHour);

		  // Is period acceptable somehow?

		  int diff1 = 0;
		  if(start.IsLessThan(thePeriod.localStartTime()))
			diff1 = thePeriod.localStartTime().DifferenceInHours(start);
		  int diff2 = 0;
		  if(thePeriod.localEndTime().IsLessThan(end))
			diff2 = end.DifferenceInHours(thePeriod.localEndTime());

		  if(diff1 <= maxdiff1 && diff2 <= maxdiff2)
			count++;
		}
	  return count;

	}
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief Count the number of specified subperiods in the period
	 *
	 * Each subperiod is of the form
	 * \code
	 *  theStartHour...theMaxStartHour - theMinEndHour...theEndHour
	 * \endcode
	 *
	 * \param thePeriod The main period
	 * \param theStartHour The normal start hour of the subperiod
	 * \param theEndHour The normal end hour of the subperiod
	 * \param theMaxStartHour The maximum start hour of the subperiod
	 * \param theMinEndHour The minimum end hour of the subperiod
	 * \return The number of acceptable subperiods in the period
	 */
	// ----------------------------------------------------------------------

	WeatherPeriod getPeriod(const WeatherPeriod & thePeriod,
							int theNumber,
							int theStartHour,
							int theEndHour,
							int theMaxStartHour,
							int theMinEndHour)
	{
	  const string msg = "WeatherPeriodTools: Cannot extract subperiod "+lexical_cast<string>(theNumber);

	  if(theNumber <= 0)
		throw WeatherAnalysisError(msg);

	  NFmiTime start(thePeriod.localStartTime());
	  start.ChangeByDays(-1);
	  start.SetHour(theStartHour);

	  // maximum allowed difference in hours after start of period
	  const int maxdiff1 = (theMaxStartHour >= theStartHour ?
							theMaxStartHour - theStartHour :
							theMaxStartHour + 24 - theStartHour);

	  // maximum allowed difference in hours before end of period
	  const int maxdiff2 = (theMinEndHour <= theEndHour ?
							theEndHour - theMinEndHour :
							theEndHour + 24 - theMinEndHour);

	  for(;
		  !thePeriod.localEndTime().IsLessThan(start);
		  start.ChangeByDays(1))
		{
		  NFmiTime end(start);
		  if(theEndHour <= theStartHour)
			end.ChangeByDays(1);
		  end.SetHour(theEndHour);

		  // Is period acceptable somehow?

		  int diff1 = 0;
		  if(start.IsLessThan(thePeriod.localStartTime()))
			diff1 = thePeriod.localStartTime().DifferenceInHours(start);
		  int diff2 = 0;
		  if(thePeriod.localEndTime().IsLessThan(end))
			diff2 = end.DifferenceInHours(thePeriod.localEndTime());

		  if(diff1 <= maxdiff1 && diff2 <= maxdiff2)
			if(--theNumber == 0)
			  {
				start.ChangeByHours(diff1);
				end.ChangeByHours(-diff2);
				return WeatherPeriod(start,end);
			  }
		}

	  throw WeatherAnalysisError(msg);
	}
  }
}

// ======================================================================
