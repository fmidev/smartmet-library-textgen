// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherLimits
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WeatherLimits
 *
 * \brief Representation of valid value ranges
 *
 * The purpose of the WeatherLimits class is to establish value
 * limits mainly for probability calculations.
 *
 * Example:
 * \code
 * using namespace WeatherAnalysis;
 * WeatherLimits limits;
 * limits.lowerLimit(-2);
 * limits.upperLimit(6);
 *
 * if(limits.isInside(0))
 * {
 *   ...
 * }
 * \endcode
 * By default the limiting values themselves are allowed, this
 * can be changed using an additional parameter as follows.
 * \code
 * limits.lowerLimit(-2,false);
 * limits.upperLimit(6,false);
 * // now inLimits would return false for -2 and 6
 * \endcode
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERLIMITS_H
#define WEATHERANALYSIS_WEATHERLIMITS_H

namespace WeatherAnalysis
{
  class WeatherLimits
  {
  public:

#ifdef NO_COMPILER_GENERATED
	~WeatherLimits();
	WeatherLimits(const WeatherLimits & theLimits);
	WeatherLimits & operator=(const WeatherLimits & theLimits);
#endif
	WeatherLimits();

	void lowerLimit(float theValue, bool isExact=true);
	void upperLimit(float theValue, bool isExact=true);

	float lowerLimit() const;
	float upperLimit() const;

	bool hasLowerLimit() const;
	bool hasUpperLimit() const;

	bool inLimits(float theValue) const;

  private:
	
	bool itHasLowerLimit;
	bool itHasUpperLimit;
	bool itsLowerLimitExact;
	bool itsUpperLimitExact;
	float itsLowerLimit;
	float itsUpperLimit;

  }; // class WeatherLimits

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERLIMITS_H

// ======================================================================
