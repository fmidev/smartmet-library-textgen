// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::SumCalculator
 */
// ======================================================================

#ifndef WEATHERANALYSIS_SUMCALCULATOR_H
#define WEATHERANALYSIS_SUMCALCULATOR_H

#include "Calculator.h"
#include "Acceptor.h"
#include "boost/shared_ptr.hpp"

namespace WeatherAnalysis
{
  class SumCalculator : public Calculator
  {
  public:

	SumCalculator();
	virtual ~SumCalculator() { }
	virtual void operator()(float theValue);
	virtual float operator()() const;
	virtual void acceptor(const Acceptor & theAcceptor);
	virtual boost::shared_ptr<Calculator> clone() const;
	virtual void reset();

  private:

	boost::shared_ptr<Acceptor> itsAcceptor;
	long itsCounter;
	double itsSum;

  }; // class SumCalculator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_SUMCALCULATOR_H

// ======================================================================
