
// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace WeatherAnalysis::QueryDataIntegrator
 */
// ======================================================================

#ifdef _MSC_VER
 #pragma warning(disable : 4786) // STL name length warnings off
#endif

#include "QueryDataIntegrator.h"
#include "Calculator.h"

#include "NFmiFastQueryInfo.h"
#include "NFmiIndexMask.h"
#include "NFmiIndexMaskSource.h"
#include "NFmiTime.h"

namespace
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

  bool first_integration_time(NFmiFastQueryInfo & theQI,
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

} // namespace anonymous

namespace WeatherAnalysis
{
  namespace QueryDataIntegrator
  {
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief Integrate over time
	 *
	 * Integrate over time with current location, parameter and level.
	 *
	 * If the start time does not exist in the data, a missing value
	 * is returned.
	 *
	 * \param theQI The query info
	 * \param theStartTime The start time of the integration.
	 * \param theEndTime The end time of the integration.
	 * \param theTimeCalculator The calculator for accumulating the result
	 * \return The accumulation result
	 */
	// ----------------------------------------------------------------------
	
	float Integrate(NFmiFastQueryInfo & theQI,
					const NFmiTime & theStartTime,
					const NFmiTime & theEndTime,
					Calculator & theTimeCalculator)
	{
	  theTimeCalculator.reset();

	  if(!first_integration_time(theQI,theStartTime))
		return kFloatMissing;
	  
	  do
		{
		  const float tmp = theQI.FloatValue();
		  theTimeCalculator(tmp);
		}
	  while(theQI.NextTime() && theQI.Time()<=theEndTime);
	  
	  return theTimeCalculator();
	}
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief Integrate over time with subintervals
	 *
	 * Integrate over time with current location, parameter and level.
	 * The time interval is divided into subintervals and are integrated
	 * separately.
	 *
	 * If the start time does not exist in the data, a missing value
	 * is returned.
	 *
	 * If the given interval is negative, a missing value is returned.
	 * If the interval is zero, no subintervals are created and
	 * the sub modifier is ignored.
	 *
	 * \param theQI The query info
	 * \param theStartTime The start time of the integration.
	 * \param theEndTime The end time of the integration.
	 * \param theInterval The sub interval in hours
	 * \param theSubTimeCalculator The calculator for accumulating the subresult
	 * \param theMainTimeCalculator The calculator for subresults
	 * \return The accumulation result
	 */
	// ----------------------------------------------------------------------
	
	float Integrate(NFmiFastQueryInfo & theQI,
					const NFmiTime & theStartTime,
					const NFmiTime & theEndTime,
					int theInterval,
					Calculator & theSubTimeCalculator,
					Calculator & theMainTimeCalculator)
	{
	  // Safety against bad loop
	  if(theInterval<0)
		return kFloatMissing;
	  // Default case
	  if(theInterval==0)
		return Integrate(theQI,theStartTime,theEndTime,theMainTimeCalculator);
	  
	  theMainTimeCalculator.reset();
	  
	  NFmiTime time1(theStartTime);
	  
	  if(!first_integration_time(theQI,time1))
		return kFloatMissing;
	  
	  do
		{
		  NFmiTime time2(time1);
		  time2.ChangeByHours(theInterval);
		  
		  theSubTimeCalculator.reset();
		  
		  do
			{
			  const float tmp = theQI.FloatValue();
			  theSubTimeCalculator(tmp);
			}
		  while(theQI.NextTime() && theQI.Time()<time2);
		  
		  const float subresult = theSubTimeCalculator();
		  theMainTimeCalculator(subresult);
		  
		}
	  while(theQI.IsValidTime() && theQI.Time()<=theEndTime);
	  
	  return theMainTimeCalculator();
	}
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief Integrate over space
	 *
	 * Integrate over space with current time, parameter and level.
	 *
	 * It is assumed that the index mask was generated from a grid
	 * equivalent to the one in the query data.
	 *
	 * \param theQI The query info
	 * \param theIndexMask The mask containing the space indices
	 * \param theSpaceCalculator The modifier for accumulating the result
	 * \return The accumulation result
	 */
	// ----------------------------------------------------------------------
	
	float Integrate(NFmiFastQueryInfo & theQI,
					const NFmiIndexMask & theIndexMask,
					Calculator & theSpaceCalculator)
	{
	  if(theIndexMask.empty())
		return kFloatMissing;
	  
	  theSpaceCalculator.reset();
	  
	  for(NFmiIndexMask::const_iterator it = theIndexMask.begin();
		  it != theIndexMask.end();
		  ++it)
		{
		  // possible -1 is handled by IndexFloatValue
		  const unsigned long idx = theQI.Index(theQI.ParamIndex(),
												*it,
												theQI.LevelIndex(),
												theQI.TimeIndex());
		  const float tmp = theQI.GetFloatValue(idx);
		  theSpaceCalculator(tmp);
		}
	  
	  return theSpaceCalculator();
	}
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief Integrate over space and time
	 *
	 * Integrate over space and time with current parameter and level.
	 * Note that the integration order is different from the Integrate
	 * command where the order of time and space arguments are reversed.
	 * Here space integration is performed first, then time integration.
	 *
	 * If the start time does not exist in the data, a missing value
	 * is returned.
	 *
	 * \param theQI The query info
	 * \param theIndexMask The  mask containing the space indices
	 * \param theSpaceCalculator  The calculator for accumulating space result
	 * \param theStartTime The start time of the integration.
	 * \param theEndTime The end time of the integration.
	 * \param theTimeCalculator The calculator for accumulating the time result
	 * \return The accumulation result
	 */
	// ----------------------------------------------------------------------
	
	float Integrate(NFmiFastQueryInfo & theQI,
					const NFmiIndexMask & theIndexMask,
					Calculator & theSpaceCalculator,
					const NFmiTime & theStartTime,
					const NFmiTime & theEndTime,
					Calculator & theTimeCalculator)
	{
	  theTimeCalculator.reset();
	  
	  if(!first_integration_time(theQI,theStartTime))
		return kFloatMissing;
	  if(theIndexMask.empty())
		return kFloatMissing;
	  
	  do
		{
		  const float tmp = Integrate(theQI,theIndexMask,theSpaceCalculator);
		  theTimeCalculator(tmp);
		}
	  while(theQI.NextTime() && theQI.Time()<=theEndTime);
	  
	  return theTimeCalculator();
	}
	
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief Integrate over time and space
	 *
	 * Integrate over time and space with current parameter and level.
	 * Note that the integration order is different from the Integrate
	 * command where the order of time and space arguments are reversed.
	 * Here time integration is performed first, then space integration.
	 *
	 * If the start time does not exist in the data, a missing value
	 * is returned.
	 *
	 * \param theQI The query info
	 * \param theStartTime The start time of the integration.
	 * \param theEndTime The end time of the integration.
	 * \param theTimeCalculator The calculator for accumulating the time result
	 * \param theIndexMask The mask containing the space indices
	 * \param theSpaceCalculator The calculator for accumulating the space result
	 * \return The accumulation result
	 */
	// ----------------------------------------------------------------------
	
	float Integrate(NFmiFastQueryInfo & theQI,
					const NFmiTime & theStartTime,
					const NFmiTime & theEndTime,
					Calculator & theTimeCalculator,
					const NFmiIndexMask & theIndexMask,
					Calculator & theSpaceCalculator)
	{
	  theSpaceCalculator.reset();
	  
	  if(theIndexMask.empty())
		return kFloatMissing;
	  
	  for(NFmiIndexMask::const_iterator it = theIndexMask.begin();
		  it != theIndexMask.end();
		  ++it)
		{
		  theTimeCalculator.reset();
		  
		  if(!first_integration_time(theQI,theStartTime))
			return kFloatMissing;
		  
		  do
			{
			  // possible -1 is handled by IndexFloatValue
			  const unsigned long idx = theQI.Index(theQI.ParamIndex(),
													*it,
													theQI.LevelIndex(),
													theQI.TimeIndex());
			  const float tmp = theQI.GetFloatValue(idx);
			  
			  theTimeCalculator(tmp);
			}
		  while(theQI.NextTime() && theQI.Time()<=theEndTime);
		  
		  const float timeresult = theTimeCalculator();
		  theSpaceCalculator(timeresult);
		}
	  
	  return theSpaceCalculator();
	}
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief Integrate over space and time with time dependent masks
	 *
	 * Integrate over space and time with current parameter and level.
	 * The space mask is time dependent and is obtained from a
	 * NFmiIndexMaskSource.
	 *
	 * If the start time does not exist in the data, a missing value
	 * is returned.
	 *
	 * \param theQI The query info
	 * \param theMaskSource The masks for any specific time
	 * \param theSpaceCalculator The calculator for accumulating the space result
	 * \param theStartTime The start time of the integration.
	 * \param theEndTime The end time of the integration.
	 * \param theTimeCalculator The calculator for accumulating the time result
	 * \return The accumulation result
	 */
	// ----------------------------------------------------------------------
	
	float Integrate(NFmiFastQueryInfo & theQI,
					const NFmiIndexMaskSource & theMaskSource,
					Calculator & theSpaceCalculator,
					const NFmiTime & theStartTime,
					const NFmiTime & theEndTime,
					Calculator & theTimeCalculator)
	{
	  theTimeCalculator.reset();
	  
	  if(!first_integration_time(theQI,theStartTime))
		return kFloatMissing;
	  
	  do
		{
		  const NFmiIndexMask & mask = theMaskSource.Find(theQI.Time());
		  const float tmp = Integrate(theQI,mask,theSpaceCalculator);
		  theTimeCalculator(tmp);
		}
	  while(theQI.NextTime() && theQI.Time()<=theEndTime);
	  
	  return theTimeCalculator();
	}
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief Integrate over time, time and space
	 *
	 * Integrate over time and space with current parameter and level.
	 * Note that the integration order is different from the Integrate
	 * command where the order of time and space arguments are reversed.
	 * Here time integration is performed first, then space integration.
	 * Also, the time integration is split into two parts with separate
	 * modifiers. For example, one may divide a 5 day period into 24 hour
	 * segments, and then calculate the mean of 24 hour maximums in time,
	 * and then mean in space again.
	 *
	 * If the start time does not exist in the data, a missing value
	 * is returned.
	 *
	 * If the given interval is negative, a missing value is returned.
	 * If the interval is zero, no subintervals are created and
	 * the sub modifier is ignored.
	 *
	 * \param theQI The query info
	 * \param theStartTime The start time of the integration.
	 * \param theEndTime The end time of the integration.
	 * \param theInterval The number of hours in a single interval
	 * \param theSubTimeCalculator The calculator for accumulating sub periods
	 * \param theMainTimeCalculator The calculator for accumulating sub period results
	 * \param theIndexMask The mask containing the space indices
	 * \param theSpaceCalculator The calculator for accumulating the space result
	 * \return The accumulation result
	 */
	// ----------------------------------------------------------------------
	
	float Integrate(NFmiFastQueryInfo & theQI,
					const NFmiTime & theStartTime,
					const NFmiTime & theEndTime,
					int theInterval,
					Calculator & theSubTimeCalculator,
					Calculator & theMainTimeCalculator,
					const NFmiIndexMask & theIndexMask,
					Calculator & theSpaceCalculator)
	{
	  // Safety against bad loop
	  if(theInterval<0)
		return kFloatMissing;
	  
	  // Don't create subintervals unless necessary
	  if(theInterval==0)
		return Integrate(theQI,
						 theStartTime,theEndTime,theMainTimeCalculator,
						 theIndexMask,theSpaceCalculator);
	  
	  theSpaceCalculator.reset();
	  
	  if(theIndexMask.empty())
		return kFloatMissing;
	  
	  for(NFmiIndexMask::const_iterator it = theIndexMask.begin();
		  it != theIndexMask.end();
		  ++it)
		{
		  theMainTimeCalculator.reset();
		  
		  NFmiTime time1(theStartTime);
		  
		  if(!first_integration_time(theQI,time1))
			return kFloatMissing;
		  
		  do
			{
			  NFmiTime time2(time1);
			  time2.ChangeByHours(theInterval);
			  
			  theSubTimeCalculator.reset();
			  
			  do
				{
				  // possible -1 is handled by IndexFloatValue
				  const unsigned long idx = theQI.Index(theQI.ParamIndex(),
														*it,
														theQI.LevelIndex(),
														theQI.TimeIndex());
				  const float tmp = theQI.GetFloatValue(idx);
				  
				  theSubTimeCalculator(tmp);
				}
			  while(theQI.NextTime() && theQI.Time()<time2);
			  
			  const float subtimeresult = theSubTimeCalculator();
			  theMainTimeCalculator(subtimeresult);
			  
			  time1 = time2;
			  
			}
		  
		  while(theQI.IsValidTime() && theQI.Time()<theEndTime);
		  
		  const float timeresult = theMainTimeCalculator();
		  theSpaceCalculator(timeresult);
		}
	  
	  return theSpaceCalculator();
	}
	
  } // namespace QueryDataIntegrator
} // namespace WeatherAnalysis

// ======================================================================
  
