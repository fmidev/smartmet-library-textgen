// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::PercentageCalculator
 */
// ======================================================================

#ifndef WEATHERANALYSIS_PERCENTAGECALCULATOR_H
#define WEATHERANALYSIS_PERCENTAGECALCULATOR_H

#include "Calculator.h"
#include "boost/shared_ptr.hpp"

namespace WeatherAnalysis
{
  class PercentageCalculator : public Calculator
  {
  public:

	PercentageCalculator();
	virtual ~PercentageCalculator() { }
	virtual void operator()(float theValue);
	virtual float operator()() const;
	virtual void acceptor(const Acceptor & theAcceptor);
	virtual boost::shared_ptr<Calculator> clone() const;
	virtual void reset();

	void condition(const Acceptor & theCondition);

  private:

	boost::shared_ptr<Acceptor> itsAcceptor;
	boost::shared_ptr<Acceptor> itsCondition;
	long itsCounter;
	long itsTotalCounter;

  }; // class PercentageCalculator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_PERCENTAGECALCULATOR_H

// ======================================================================
