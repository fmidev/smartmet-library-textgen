// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::ValueAcceptor
 */
// ======================================================================

#ifndef WEATHERANALYSIS_VALUEACCEPTOR_H
#define WEATHERANALYSIS_VALUEACCEPTOR_H

#include "Acceptor.h"

namespace WeatherAnalysis
{
  class ValueAcceptor : public Acceptor
  {
  public:

	virtual ~ValueAcceptor() { }
	virtual bool accept(float theValue) const;
	virtual Acceptor * clone() const;

	ValueAcceptor();
	void value(float theValue);

  private:

	float itsValue;

  }; // class ValueAcceptor
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_VALUEACCEPTOR_H

// ======================================================================
