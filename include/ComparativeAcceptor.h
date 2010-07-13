// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::ComparativeAcceptor
 */
// ======================================================================

#ifndef WEATHERANALYSIS_COMPARATIVEACCEPTOR_H
#define WEATHERANALYSIS_COMPARATIVEACCEPTOR_H

#include "Acceptor.h"
#include <newbase/NFmiGlobals.h>

namespace WeatherAnalysis
{
  enum comparative_operator{LESS_THAN, 
							LESS_OR_EQUAL, 
							EQUAL, 
							GREATER_OR_EQUAL, 
							GREATER_THAN,
							VOID_OPERATOR};

  class ComparativeAcceptor : public Acceptor
  {
  public:

	virtual ~ComparativeAcceptor() { }
	virtual bool accept(float theValue) const;
	virtual Acceptor * clone() const;

	ComparativeAcceptor(const float& theLimit = kFloatMissing,
						const comparative_operator& theOperator = VOID_OPERATOR);
	void setLimit(float theLimit);
	void setOperator(const comparative_operator& theOperator);

  private:

	float itsLimit;
	comparative_operator itsOperator;

  }; // class ComparativeAcceptor
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_COMPARATIVEACCEPTOR_H

// ======================================================================
