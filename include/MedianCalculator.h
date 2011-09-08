// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::MedianCalculator
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MEDIANCALCULATOR_H
#define WEATHERANALYSIS_MEDINACALCULATOR_H

#include "Calculator.h"
#include <boost/shared_ptr.hpp>
#include <vector>


namespace WeatherAnalysis
{
  class MedianCalculator : public Calculator
  {
  public:

	MedianCalculator();
	virtual ~MedianCalculator() { }
	virtual void operator()(float theValue);
	virtual float operator()() const;
	virtual void acceptor(const Acceptor & theAcceptor);
	virtual boost::shared_ptr<Calculator> clone() const;
	virtual void reset();

  private:

	boost::shared_ptr<Acceptor> itsAcceptor;
	mutable std::vector<float> theValueVector;

  }; // class MedianCalculator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MEDIANCALCULATOR_H

// ======================================================================
