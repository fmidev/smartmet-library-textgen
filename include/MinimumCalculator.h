// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::MinimumCalculator
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MINIMUMCALCULATOR_H
#define WEATHERANALYSIS_MINIMUMCALCULATOR_H

#include "Calculator.h"
#include <boost/shared_ptr.hpp>

namespace WeatherAnalysis
{
  class MinimumCalculator : public Calculator
  {
  public:

	MinimumCalculator();
	virtual ~MinimumCalculator() { }
	virtual void operator()(float theValue);
	virtual float operator()() const;
	virtual void acceptor(const Acceptor & theAcceptor);
	virtual Calculator * clone() const;
	virtual void reset();

  private:

	boost::shared_ptr<Acceptor> itsAcceptor;
	long itsCounter;
	float itsMinimum;

  }; // class MinimumCalculator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MINIMUMCALCULATOR_H

// ======================================================================
