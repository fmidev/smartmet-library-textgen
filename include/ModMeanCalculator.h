// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::ModMeanCalculator
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::ModMeanCalculator
 *
 * \brief Modular mean intergator
 *
 * The modular mean is calculated using the Mitsuta algorithm
 * for wind direction means.
 *
 * Reference: Mori, Y., 1986.<br>
 * <em>Evaluation of Several Single-Pass Estimators of the Mean and
 *     the Standard Deviation of Wind Direction.</em><br>
 * J Climate Appl. Metro., 25, 1387-1397.
 *
 * Some information can also be found with Google (Mitsuta wind direction).
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
	float itsSum;
	float itsPreviousDirection;

  }; // class ModMeanCalculator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MODMEANCALCULATOR_H

// ======================================================================
