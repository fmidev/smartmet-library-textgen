// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::StandardDeviationCalculator
 */
// ======================================================================

#ifndef WEATHERANALYSIS_STANDARDDEVIATIONCALCULATOR_H
#define WEATHERANALYSIS_STANDARDDEVIATIONCALCULATOR_H

#include "Calculator.h"
#include "Acceptor.h"
#include <boost/shared_ptr.hpp>

namespace WeatherAnalysis
{
  class StandardDeviationCalculator : public Calculator
  {
  public:

	StandardDeviationCalculator();
	virtual ~StandardDeviationCalculator() { }
	virtual void operator()(float theValue);
	virtual float operator()() const;
	virtual void acceptor(const Acceptor & theAcceptor);
	virtual Calculator * clone() const;
	virtual void reset();

  private:

	boost::shared_ptr<Acceptor> itsAcceptor;
	long itsCounter;
	double itsSum;
	double itsSquaredSum;

  }; // class StandardDeviationCalculator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_STANDARDDEVIATIONCALCULATOR_H

// ======================================================================
