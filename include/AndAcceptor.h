// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::AndAcceptor
 */
// ======================================================================

#ifndef WEATHERANALYSIS_ANDACCEPTOR_H
#define WEATHERANALYSIS_ANDACCEPTOR_H

#include "Acceptor.h"
#include <boost/shared_ptr.hpp>
#include <functional>

namespace WeatherAnalysis
{
  class AndAcceptor : public Acceptor
  {
  public:

	AndAcceptor(const Acceptor & theLhs, const Acceptor & theRhs);
	virtual ~AndAcceptor() { }
	virtual bool accept(float theValue) const;
	virtual Acceptor * clone() const;

  private:
	
	AndAcceptor();

	boost::shared_ptr<Acceptor> itsLhs;
	boost::shared_ptr<Acceptor> itsRhs;

  }; // class AndAcceptor
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_ANDACCEPTOR_H

// ======================================================================
