// ======================================================================
/*!
 * \file
 * \brief Interface of class RangeAcceptor
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::RangeAcceptor
 *
 * \brief RangeAcceptor accepths a range of values
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_RANGEACCEPTOR_H
#define WEATHERANALYSIS_RANGEACCEPTOR_H

#include "Acceptor.h"

namespace WeatherAnalysis
{
  class RangeAcceptor : public Acceptor
  {
  public:

	virtual ~RangeAcceptor() { }
	virtual bool accept(float theValue) const;
	virtual boost::shared_ptr<Acceptor> clone() const;

	RangeAcceptor();
	void lowerLimit(float theLoLimit);
	void upperLimit(float theUpperLimit);

  private:

	float itsLoLimit;
	float itsHiLimit;

  }; // class RangeAcceptor
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_RANGEACCEPTOR_H

// ======================================================================
