// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::DefaultAcceptor
 */
// ======================================================================

#ifndef WEATHERANALYSIS_DEFAULTACCEPTOR_H
#define WEATHERANALYSIS_DEFAULTACCEPTOR_H

#include "Acceptor.h"

namespace WeatherAnalysis
{
  class DefaultAcceptor : public Acceptor
  {
  public:

	virtual ~DefaultAcceptor() { }
	virtual bool accept(float theValue) const;
	virtual boost::shared_ptr<Acceptor> clone() const;

  }; // class DefaultAcceptor
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_DEFAULTACCEPTOR_H

// ======================================================================
