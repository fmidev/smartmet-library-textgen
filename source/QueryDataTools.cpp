// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace QueryDataTools
 */
// ======================================================================
/*!
 * \namespace WeatherAnalysis::QueryDataTools
 *
 * \brief Various small querydata related utility functions
 *
 * The namespace contains various useful utility functions
 * operating on querydata. The functions are not directly
 * available in newbase, but we do not wish to increase
 * the size of newbase unnecessarily with utility functions
 * which are mostly useful only in the text generator.
 *
 */
// ======================================================================

#include "QueryDataTools.h"

#include "newbase/NFmiFastQueryInfo.h"
#include "newbase/NFmiTime.h"

namespace WeatherAnalysis
{
  namespace QueryDataTools
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Set the first integration time
	 *
	 * This effectively sets active the first time greater than or equal
	 * to the given time. If there is no such time, false is returned.
	 *
	 * This is needed because newbase does not provide an equivalent
	 * interface.
	 *
	 * \param theQI The query info
	 * \param theTime The time to set
	 * \return True if the time was set succesfully
	 */
	// ----------------------------------------------------------------------
	
	bool firstTime(NFmiFastQueryInfo & theQI,
				   const NFmiTime & theTime)
	{
	  theQI.FirstTime();
	  do
		{
		  if(theQI.IsValidTime() && theQI.ValidTime()>=theTime)
			return true;
		}
	  while(theQI.NextTime());
	  return false;
	}
	
  } // namespace QueryDataTools
} // namespace WeatherAnalysis

// ======================================================================
