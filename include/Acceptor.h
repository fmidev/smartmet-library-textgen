// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::Acceptor
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::Acceptor
 *
 * \brief Acceptor is a class which either accepts or rejects a value
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_ACCEPTOR_H
#define WEATHERANALYSIS_ACCEPTOR_H

#include "boost/shared_ptr.hpp"

namespace WeatherAnalysis
{
  class Acceptor
  {
  public:

	Acceptor() { }
	virtual ~Acceptor() { }
	virtual bool accept(float theValue) const = 0;
	virtual boost::shared_ptr<Acceptor> clone() const = 0;

  }; // class Acceptor
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_ACCEPTOR_H

// ======================================================================
