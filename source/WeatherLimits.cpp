// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherLimits
 */
// ======================================================================

#include "WeatherLimits.h"
#include <cassert>
#include <stdexcept>

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  // ----------------------------------------------------------------------

  WeatherLimits::WeatherLimits()
	: itHasLowerLimit(false)
	, itHasUpperLimit(false)
	, itsLowerLimitExact(true)
	, itsUpperLimitExact(true)
	, itsLowerLimit(0)
	, itsUpperLimit(0)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set a lower limit
   *
   * \param theValue The value of the lower limit
   * \param isExact True if the lower limit itself is an allowed value
   */
  // ----------------------------------------------------------------------

  void WeatherLimits::lowerLimit(float theValue, bool isExact)
  {
	itHasLowerLimit = true;
	itsLowerLimitExact = isExact;
	itsLowerLimit = theValue;

	if(itHasUpperLimit)
	  assert(itsLowerLimit<=itsUpperLimit);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set an upper limit
   *
   * \param theValue The value of the upper limit
   * \param isExact True if the upper limit itself is an allowed value
   */
  // ----------------------------------------------------------------------

  void WeatherLimits::upperLimit(float theValue, bool isExact)
  {
	itHasUpperLimit = true;
	itsUpperLimitExact = isExact;
	itsUpperLimit = theValue;

	if(itHasLowerLimit)
	  assert(itsLowerLimit<=itsUpperLimit);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test whether a lower limit has been set
   *
   * \return True if a lower limit is set
   */
  // ----------------------------------------------------------------------

  bool WeatherLimits::hasLowerLimit() const
  {
	return itHasLowerLimit;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test whether a upper limit has been set
   *
   * \return True if a upper limit is set
   */
  // ----------------------------------------------------------------------

  bool WeatherLimits::hasUpperLimit() const
  {
	return itHasUpperLimit;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the lower limit
   *
   * Throws if no lower limit has been set, always test using hasLowerLimit
   * first.
   *
   * \return The lower limit
   */
  // ----------------------------------------------------------------------

  float WeatherLimits::lowerLimit() const
  {
	if(itHasLowerLimit)
	  return itsLowerLimit;

	throw std::runtime_error("WeatherLimits:: Trying to access lower limit which is not set");
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the upper limit
   *
   * Throws if no upper limit has been set, always test using hasUpperLimit
   * first.
   *
   * \return The upper limit
   */
  // ----------------------------------------------------------------------

  float WeatherLimits::upperLimit() const
  {
	if(itHasUpperLimit)
	  return itsUpperLimit;

	throw std::runtime_error("WeatherLimits:: Trying to access upper limit which is not set");
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test whether value is within limits
   *
   * \param theValue The value to test
   * \return True if the value is within limits
   */
  // ----------------------------------------------------------------------

  bool WeatherLimits::inLimits(float theValue) const
  {
	if(itHasLowerLimit)
	  {
		if(itsLowerLimitExact)
		  {
			if(theValue < itsLowerLimit)
			  return false;
		  }
		else
		  {
			if(theValue <= itsLowerLimit)
			  return false;
		  }
	  }

	if(itHasUpperLimit)
	  {
		if(itsUpperLimitExact)
		  {
			if(theValue > itsUpperLimit)
			  return false;
		  }
		else
		  {
			if(theValue >= itsUpperLimit)
			  return false;
		  }
	  }

	return true;
  }


} // namespace WeatherAnalysis

// ======================================================================
