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

#include "NFmiFastQueryInfo.h"
#include "NFmiTime.h"

namespace WeatherAnalysis
{
  namespace QueryDataTools
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Find integration index range
	 *
	 * The NFmiFastQueryInfo Time and ValidTime methods are
	 * too slow to be used in loops, it is much better to use
	 * the TimeIndex method. This function calculates
	 * the desired loop indexes so that if you set the returned
	 * start index, the querydata valid time is the first one
	 * greater than or equal to the desired start time.
	 * The end index is the first time greater than the 
	 * end time.
	 *
	 * \param theQI The query info
	 * \param theStartTime The desired integration start time
	 * \param theEndTime The desired end time
	 * \param theStartIndex The variable into which the start index is stored
	 * \param theEndIndex The variable into which the end index is stored
	 * \return True, if both indexes were found
	 */
	// ----------------------------------------------------------------------

	bool findIndices(NFmiFastQueryInfo & theQI,
					 const NFmiTime & theStartTime,
					 const NFmiTime & theEndTime,
					 unsigned long & theStartIndex,
					 unsigned long & theEndIndex)
	{
	  const unsigned long invalid = static_cast<unsigned long>(-1);

	  theStartIndex = invalid;
	  theEndIndex = invalid;

	  theQI.FirstTime();
	  do
		{
		  if(theQI.IsValidTime() && theQI.ValidTime()>=theStartTime)
			{
			  theStartIndex = theQI.TimeIndex();
			  break;
			}
		}
	  while(theQI.NextTime());

	  if(theStartIndex == invalid)
		return false;

	  do
		{
		  if(theQI.IsValidTime() && theQI.ValidTime()>theEndTime)
			{
			  theEndIndex = theQI.TimeIndex();
			  break;
			}
		}
	  while(theQI.NextTime());

	  return (theStartIndex != invalid && theEndIndex != invalid);

	}

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
