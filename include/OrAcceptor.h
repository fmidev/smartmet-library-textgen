// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::OrAcceptor
 */
// ======================================================================

#ifndef WEATHERANALYSIS_ORACCEPTOR_H
#define WEATHERANALYSIS_ORACCEPTOR_H

#include "Acceptor.h"
#include <functional>

namespace WeatherAnalysis
{
  class OrAcceptor : public Acceptor
  {
  public:

	OrAcceptor(const OrAcceptor & theOther);
	OrAcceptor(const Acceptor & theLhs, const Acceptor & theRhs);
	virtual ~OrAcceptor() { delete itsLhs; delete itsRhs; }
	virtual bool accept(float theValue) const;
	virtual Acceptor * clone() const;

  private:
	
	OrAcceptor();

	Acceptor * itsLhs;
	Acceptor * itsRhs;

  }; // class OrAcceptor
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_ORACCEPTOR_H

// ======================================================================
