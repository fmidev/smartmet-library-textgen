// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::StandardDeviationCalculator
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::StandardDeviationCalculator
 *
 * \brief StandardDeviation intergator
 *
 * The result is sample standard deviation, not plain population
 * standard deviation because we most often calculate the standard
 * deviation for data which is essentially discrete samples
 * of continuous data.
 */
// ======================================================================

#ifndef WEATHERANALYSIS_STANDARDDEVIATIONCALCULATOR_H
#define WEATHERANALYSIS_STANDARDDEVIATIONCALCULATOR_H

#include "Calculator.h"
#include "Acceptor.h"
#include "boost/shared_ptr.hpp"

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
	virtual boost::shared_ptr<Calculator> clone() const;
	virtual void reset();

  private:

	boost::shared_ptr<Acceptor> itsAcceptor;
	long itsCounter;
	float itsSum;
	float itsSquaredSum;

  }; // class StandardDeviationCalculator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_STANDARDDEVIATIONCALCULATOR_H

// ======================================================================
