// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::ModMeanCalculator
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MODMEANCALCULATOR_H
#define WEATHERANALYSIS_MODMEANCALCULATOR_H

#include "Calculator.h"
#include "boost/shared_ptr.hpp"

namespace WeatherAnalysis
{
  class ModMeanCalculator : public Calculator
  {
  public:

	ModMeanCalculator(int theModulo);
	virtual ~ModMeanCalculator() { }
	virtual void operator()(float theValue);
	virtual float operator()() const;
	virtual void acceptor(const Acceptor & theAcceptor);
	virtual boost::shared_ptr<Calculator> clone() const;
	virtual void reset();

  private:

	ModMeanCalculator();

	boost::shared_ptr<Acceptor> itsAcceptor;
	const int itsModulo;

	long itsCounter;
	double itsSum;
	double itsPreviousDirection;

  }; // class ModMeanCalculator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MODMEANCALCULATOR_H

// ======================================================================
