// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::StandardDeviationCalculator
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::StandardDeviationCalculator
 *
 * \brief Standarddeviation intergator
 *
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
