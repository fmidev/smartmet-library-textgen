// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::ModStandardDeviationCalculator
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MODSTANDARDDEVIATIONCALCULATOR_H
#define WEATHERANALYSIS_MODSTANDARDDEVIATIONCALCULATOR_H

#include "Calculator.h"
#include "boost/shared_ptr.hpp"

namespace WeatherAnalysis
{
  class ModStandardDeviationCalculator : public Calculator
  {
  public:

	ModStandardDeviationCalculator(int theModulo);
	virtual ~ModStandardDeviationCalculator() { }
	virtual void operator()(float theValue);
	virtual float operator()() const;
	virtual void acceptor(const Acceptor & theAcceptor);
	virtual boost::shared_ptr<Calculator> clone() const;
	virtual void reset();

  private:

	ModStandardDeviationCalculator();

	boost::shared_ptr<Acceptor> itsAcceptor;
	const int itsModulo;

	long itsCounter;
	double itsSum;
	double itsSquaredSum;
	double itsPreviousDirection;

  }; // class ModStandardDeviationCalculator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MODSTANDARDDEVIATIONCALCULATOR_H

// ======================================================================
