// ======================================================================
/*!
 * \file
 * \brief Interface of class DefaultAcceptor
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::DefaultAcceptor
 *
 * \brief DefaultAcceptor is a class which accepts everything but kFloatMissing
 *
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
