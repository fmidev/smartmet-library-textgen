// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::TrendCalculator
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::TrendCalculator
 *
 * \brief Trend intergator
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_TRENDCALCULATOR_H
#define WEATHERANALYSIS_TRENDCALCULATOR_H

#include "Calculator.h"
#include "Acceptor.h"
#include "boost/shared_ptr.hpp"

namespace WeatherAnalysis
{
  class TrendCalculator : public Calculator
  {
  public:

	TrendCalculator();
	virtual ~TrendCalculator() { }
	virtual void operator()(float theValue);
	virtual float operator()() const;
	virtual void acceptor(const Acceptor & theAcceptor);
	virtual boost::shared_ptr<Calculator> clone() const;
	virtual void reset();

  private:

	boost::shared_ptr<Acceptor> itsAcceptor;

	unsigned long itsCounter;
	unsigned long itsPositiveChanges;
	unsigned long itsNegativeChanges;
	unsigned long itsZeroChanges;

	float itsLastValue;

  }; // class TrendCalculator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_TRENDCALCULATOR_H

// ======================================================================
