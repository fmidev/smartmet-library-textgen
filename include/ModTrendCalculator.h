// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::ModTrendCalculator
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::ModTrendCalculator
 *
 * \brief ModTrend intergator
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MODTRENDCALCULATOR_H
#define WEATHERANALYSIS_MODTRENDCALCULATOR_H

#include "Calculator.h"
#include "Acceptor.h"
#include "boost/shared_ptr.hpp"

namespace WeatherAnalysis
{
  class ModTrendCalculator : public Calculator
  {
  public:

	ModTrendCalculator(int theModulo);
	virtual ~ModTrendCalculator() { }
	virtual void operator()(float theValue);
	virtual float operator()() const;
	virtual void acceptor(const Acceptor & theAcceptor);
	virtual boost::shared_ptr<Calculator> clone() const;
	virtual void reset();

  private:

	ModTrendCalculator();

	boost::shared_ptr<Acceptor> itsAcceptor;
	const int itsModulo;

	unsigned long itsCounter;
	unsigned long itsPositiveChanges;
	unsigned long itsNegativeChanges;
	unsigned long itsZeroChanges;

	float itsLastValue;

  }; // class ModTrendCalculator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MODTRENDCALCULATOR_H

// ======================================================================
