// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::PositiveValueAcceptor
 */
// ======================================================================

#ifndef WEATHERANALYSIS_POSITIVEVALUEACCEPTOR_H
#define WEATHERANALYSIS_POSITIVEVALUEACCEPTOR_H

#include "Acceptor.h"

namespace WeatherAnalysis
{
  class PositiveValueAcceptor : public Acceptor
  {
  public:

	virtual ~PositiveValueAcceptor() { }
	virtual bool accept(float theValue) const;
	virtual boost::shared_ptr<Acceptor> clone() const;

  }; // class PositiveValueAcceptor
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_POSITIVEVALUEACCEPTOR_H

// ======================================================================
