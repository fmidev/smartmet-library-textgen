// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::MeanCalculator
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MEANCALCULATOR_H
#define WEATHERANALYSIS_MEANCALCULATOR_H

#include "Calculator.h"
#include <boost/shared_ptr.hpp>

namespace WeatherAnalysis
{
  class MeanCalculator : public Calculator
  {
  public:

	MeanCalculator();
	virtual ~MeanCalculator() { }
	virtual void operator()(float theValue);
	virtual float operator()() const;
	virtual void acceptor(const Acceptor & theAcceptor);
	virtual Calculator * clone() const;
	virtual void reset();

  private:

	boost::shared_ptr<Acceptor> itsAcceptor;
	long itsCounter;
	double itsSum;

  }; // class MeanCalculator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MEANCALCULATOR_H

// ======================================================================
