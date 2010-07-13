// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::AndAcceptor
 */
// ======================================================================

#ifndef WEATHERANALYSIS_ANDACCEPTOR_H
#define WEATHERANALYSIS_ANDACCEPTOR_H

#include "Acceptor.h"
#include <functional>

namespace WeatherAnalysis
{
  class AndAcceptor : public Acceptor
  {
  public:

	AndAcceptor(const AndAcceptor & theOther);
	AndAcceptor(const Acceptor & theLhs, const Acceptor & theRhs);
	virtual ~AndAcceptor() { delete itsLhs; delete itsRhs; }
	virtual bool accept(float theValue) const;
	virtual Acceptor * clone() const;

  private:
	
	AndAcceptor();

	Acceptor * itsLhs;
	Acceptor * itsRhs;

  }; // class AndAcceptor
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_ANDACCEPTOR_H

// ======================================================================
