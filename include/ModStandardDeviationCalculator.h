// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::ModStandardDeviationCalculator
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::ModStandardDeviationCalculator
 *
 * \brief Modular standard deviation intergator
 *
 * The modular standard deviation is calculated using the Mitsuta algorithm
 * for wind direction standard deviations.
 *
 * Reference: Mori, Y., 1986.<br>
 * <em>Evaluation of Several Single-Pass Estimators of the StandardDeviation and
 *     the Standard Deviation of Wind Direction.</em><br>
 * J Climate Appl. Metro., 25, 1387-1397.
 *
 * Some information can also be found with Google (Mitsuta wind direction).
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
	float itsSum;
	float itsSquaredSum;
	float itsPreviousDirection;

  }; // class ModStandardDeviationCalculator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MODSTANDARDDEVIATIONCALCULATOR_H

// ======================================================================
