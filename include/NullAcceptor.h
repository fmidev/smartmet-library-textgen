// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::NullAcceptor
 */
// ======================================================================

#ifndef WEATHERANALYSIS_NULLACCEPTOR_H
#define WEATHERANALYSIS_NULLACCEPTOR_H

#include "Acceptor.h"

namespace WeatherAnalysis
{
  class NullAcceptor : public Acceptor
  {
  public:

	virtual ~NullAcceptor();
	virtual bool accept(float theValue) const;
	virtual Acceptor * clone() const;

  }; // class NullAcceptor
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_NULLACCEPTOR_H

// ======================================================================
