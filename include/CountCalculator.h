// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::CountCalculator
 */
// ======================================================================

#ifndef WEATHERANALYSIS_COUNTCALCULATOR_H
#define WEATHERANALYSIS_COUNTCALCULATOR_H

#include "Calculator.h"
#include <boost/shared_ptr.hpp>

namespace WeatherAnalysis
{
  class CountCalculator : public Calculator
  {
  public:

	CountCalculator();
	CountCalculator(const CountCalculator & other);
	virtual ~CountCalculator();
	virtual void operator()(float theValue);
	virtual float operator()() const;
	virtual void acceptor(const Acceptor & theAcceptor);
	virtual Calculator * clone() const;
	virtual void reset();

	void condition(const Acceptor & theCondition);

  private:

	Acceptor * itsAcceptor;
	Acceptor * itsCondition;
	long itsCounter;
	long itsTotalCounter;

  }; // class CountCalculator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_COUNTCALCULATOR_H

// ======================================================================
