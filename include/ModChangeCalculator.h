// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::ModChangeCalculator
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::ModChangeCalculator
 *
 * \brief Modular change intergator
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MODCHANGECALCULATOR_H
#define WEATHERANALYSIS_MODCHANGECALCULATOR_H

#include "Calculator.h"
#include "boost/shared_ptr.hpp"

namespace WeatherAnalysis
{
  class ModChangeCalculator : public Calculator
  {
  public:

	ModChangeCalculator(int theModulo);
	virtual ~ModChangeCalculator() { }
	virtual void operator()(float theValue);
	virtual float operator()() const;
	virtual void acceptor(const Acceptor & theAcceptor);
	virtual boost::shared_ptr<Calculator> clone() const;
	virtual void reset();

  private:

	ModChangeCalculator();

	boost::shared_ptr<Acceptor> itsAcceptor;
	const int itsModulo;

	long itsCounter;
	float itsCumulativeChange;
	float itsLastValue;

  }; // class ModChangeCalculator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MODCHANGECALCULATOR_H

// ======================================================================
