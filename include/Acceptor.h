// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::Acceptor
 */
// ======================================================================

#ifndef WEATHERANALYSIS_ACCEPTOR_H
#define WEATHERANALYSIS_ACCEPTOR_H

namespace WeatherAnalysis
{
  class Acceptor
  {
  public:

	Acceptor() { }
	virtual ~Acceptor() { }
	virtual bool accept(float theValue) const = 0;
	virtual Acceptor * clone() const = 0;

  private:

	Acceptor & operator=(const Acceptor & other);

  }; // class Acceptor
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_ACCEPTOR_H

// ======================================================================
