// ======================================================================
/*!
 * \file
 * \brief Interface of class TemplateAcceptor
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::TemplateAcceptor
 *
 * \brief TemplateAcceptor is a templated acceptor
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_TEMPLATEACCEPTOR_H
#define WEATHERANALYSIS_TEMPLATEACCEPTOR_H

#include "Acceptor.h"

namespace WeatherAnalysis
{

  template <class T>
  class TemplateAcceptor : public Acceptor
  {
  public:

	virtual ~TemplateAcceptor() { }
	TemplateAcceptor(const T & theFunctor) : itsFunctor(theFunctor) { }
	  
	virtual bool accept(float theValue) const
	{
	  return itsFunctor(theValue);
	}

	virtual boost::shared_ptr<Acceptor> clone() const
	{
	  return new TemplateAcceptor(*this);
	}

  private:

	TemplateAcceptor(void);

	T itsFunctor;

  }; // class TemplateAcceptor

  typedef TemplateAcceptor<bool (*)(float)> FunctionAcceptor;

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_TEMPLATEACCEPTOR_H

// ======================================================================
