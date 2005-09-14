// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::MaximumCalculator
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MAXIMUMCALCULATOR_H
#define WEATHERANALYSIS_MAXIMUMCALCULATOR_H

#include "Calculator.h"
#include <boost/shared_ptr.hpp>

namespace WeatherAnalysis
{
  class MaximumCalculator : public Calculator
  {
  public:

	MaximumCalculator();
	virtual ~MaximumCalculator() { }
	virtual void operator()(float theValue);
	virtual float operator()() const;
	virtual void acceptor(const Acceptor & theAcceptor);
	virtual boost::shared_ptr<Calculator> clone() const;
	virtual void reset();

  private:

	boost::shared_ptr<Acceptor> itsAcceptor;
	long itsCounter;
	float itsMaximum;

  }; // class MaximumCalculator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MAXIMUMCALCULATOR_H

// ======================================================================
