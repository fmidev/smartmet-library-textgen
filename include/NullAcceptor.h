// ======================================================================
/*!
 * \file
 * \brief Interface of class NullAcceptor
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::NullAcceptor
 *
 * \brief Acceptor is a class which accepts everything but kFloatMissing
 *
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

	virtual ~NullAcceptor() { }
	virtual bool accept(float theValue) const;
	virtual boost::shared_ptr<Acceptor> clone() const;

  }; // class NullAcceptor
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_NULLACCEPTOR_H

// ======================================================================
