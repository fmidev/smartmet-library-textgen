// ======================================================================
/*!
 * \file
 * \brief Interface of class OrAcceptor
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::OrAcceptor
 *
 * \brief OrAcceptor combines two Acceptors with a logical OR
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_ORACCEPTOR_H
#define WEATHERANALYSIS_ORACCEPTOR_H

#include "Acceptor.h"
#include "boost/shared_ptr.hpp"
#include <functional>

namespace WeatherAnalysis
{
  class OrAcceptor : public Acceptor
  {
  public:

	OrAcceptor(const Acceptor & theLhs, const Acceptor & theRhs);
	virtual ~OrAcceptor() { }
	virtual bool accept(float theValue) const;
	virtual boost::shared_ptr<Acceptor> clone() const;

  private:
	
	OrAcceptor();

	boost::shared_ptr<Acceptor> itsLhs;
	boost::shared_ptr<Acceptor> itsRhs;

  }; // class OrAcceptor
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_ORACCEPTOR_H

// ======================================================================
