// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::Calculator
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::Calculator
 *
 * \brief Base class for data calculators
 *
 * This is a text generator specific replacement for the hideous
 * NFmiDataModifier hierarchy. I will not support it any longer (Mika).
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_CALCULATOR_H
#define WEATHERANALYSIS_CALCULATOR_H

#include "boost/shared_ptr.hpp"

namespace WeatherAnalysis
{
  class Acceptor;

  class Calculator
  {
  public:

	Calculator() { }
	virtual ~Calculator() { }
	virtual void operator()(float theValue) = 0;
	virtual float operator()() const = 0;
	virtual void acceptor(const Acceptor & theAcceptor) = 0;
	virtual boost::shared_ptr<Calculator> clone() const = 0;
	virtual void reset() = 0;
	
  }; // class Calculator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_CALCULATOR_H

// ======================================================================
