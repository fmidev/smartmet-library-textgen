// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::NullCalculator
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::NullCalculator
 *
 * \brief Dummy calculator
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_NULLCALCULATOR_H
#define WEATHERANALYSIS_NULLCALCULATOR_H

#include "Calculator.h"

namespace WeatherAnalysis
{
  class Acceptor;

  class NullCalculator : public Calculator
  {
  public:

	virtual ~NullCalculator() { }
	virtual void operator()(float theValue);
	virtual float operator()() const;
	virtual void acceptor(const Acceptor & theAcceptor);
	virtual boost::shared_ptr<Calculator> clone() const;
	virtual void reset();
	
  }; // class Calculator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_NULLCALCULATOR_H

// ======================================================================
