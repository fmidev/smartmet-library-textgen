// ======================================================================
/*!
 * \file
 * \brief Interface of namespace WeatherAnalysis::QueryDataIntegrator
 */
// ======================================================================
/*!
 * \namespace WeatherAnalysis::QueryDataIntegrator
 *
 * \brief Functions for integrating query data
 *
 * The QueryDataIntegrator namespace defines several methods
 * for performing data integration in space and time.
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_QUERYDATAINTEGRATOR_H
#define WEATHERANALYSIS_QUERYDATAINTEGRATOR_H

class NFmiFastQueryInfo;
class NFmiIndexMask;
class NFmiIndexMaskSource;
class NFmiTime;

namespace WeatherAnalysis
{
  class Calculator;
  class WeatherPeriodGenerator;

  namespace QueryDataIntegrator
  {
	// Integrate over time with current location & param & level
	
	float Integrate(NFmiFastQueryInfo & theQI,
					const NFmiTime & theStartTime,
					const NFmiTime & theEndTime,
					Calculator & theTimeCalculator);
	
	// Integrate over time with subintervals w/ current location/param/level
	
	float Integrate(NFmiFastQueryInfo & theQI,
					const NFmiTime & theStartTime,
					const NFmiTime & theEndTime,
					int theInterval,
					Calculator & theSubTimeCalculator,
					Calculator & theMainTimeCalculator);

	// Integrate over time with subinterval generator w/ current location/param/level
	
	float Integrate(NFmiFastQueryInfo & theQI,
					const NFmiTime & theStartTime,
					const NFmiTime & theEndTime,
					const WeatherPeriodGenerator & theGenerator,
					Calculator & theSubTimeCalculator,
					Calculator & theMainTimeCalculator);
	
	// Integrate over grid with current param & time & level
	
	float Integrate(NFmiFastQueryInfo & theQI,
					const NFmiIndexMask & theIndexMask,
					Calculator & theSpaceCalculator);
	
	// Integrate over grid and time with current param & level
	
	float Integrate(NFmiFastQueryInfo & theQI,
					const NFmiIndexMask & theIndexMask,
					Calculator & theSpaceCalculator,
					const NFmiTime & theStartTime,
					const NFmiTime & theEndTime,
					Calculator & theTimeCalculator);
	
	// Integrate over time and grid with current param & level
	
	float Integrate(NFmiFastQueryInfo & theQI,
					const NFmiTime & theStartTime,
					const NFmiTime & theEndTime,
					Calculator & theTimeCalculator,
					const NFmiIndexMask & theIndexMask,
					Calculator & theSpaceCalculator);
	
	// Integrate over grid and time with time dependend mask, current P & L
	
	float Integrate(NFmiFastQueryInfo & theQI,
					const NFmiIndexMaskSource & theMaskSource,
					Calculator & theSpaceCalculator,
					const NFmiTime & theStartTime,
					const NFmiTime & theEndTime,
					Calculator & theTimeCalculator);
	
	// Integrate over time, time and grid with current param & level
	
	float Integrate(NFmiFastQueryInfo & theQI,
					const NFmiTime & theStartTime,
					const NFmiTime & theEndTime,
					int theInterval,
					Calculator & theSubTimeCalculator,
					Calculator & theMainTimeCalculator,
					const NFmiIndexMask & theIndexMask,
					Calculator & theSpaceCalculator);

	// Integrate over time, sub time and grid with current param & level
	
	float Integrate(NFmiFastQueryInfo & theQI,
					const NFmiTime & theStartTime,
					const NFmiTime & theEndTime,
					const WeatherPeriodGenerator & theGenerator,
					Calculator & theSubTimeCalculator,
					Calculator & theMainTimeCalculator,
					const NFmiIndexMask & theIndexMask,
					Calculator & theSpaceCalculator);
	
  } // namespace QueryDataIntegrator
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_QUERYDATAINTEGRATOR_H

// ======================================================================
