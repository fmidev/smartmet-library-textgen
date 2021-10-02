// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::TemplateAcceptor
 */
// ======================================================================

#pragma once

#include <calculator/Acceptor.h>

namespace TextGen
{
template <class T>
class TemplateAcceptor : public Acceptor
{
 public:
  ~TemplateAcceptor() override {}
  TemplateAcceptor(const T& theFunctor) : itsFunctor(theFunctor) {}
  bool accept(float theValue) const override { return itsFunctor(theValue); }
  Acceptor* clone() const override { return new TemplateAcceptor(*this); }

 private:
  TemplateAcceptor(void);

  T itsFunctor;

};  // class TemplateAcceptor

typedef TemplateAcceptor<bool (*)(float)> FunctionAcceptor;

}  // namespace TextGen

// ======================================================================
