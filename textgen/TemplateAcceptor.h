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
  ~TemplateAcceptor() override = default;
  TemplateAcceptor(const T& theFunctor) : itsFunctor(theFunctor) {}
  bool accept(float theValue) const override { return itsFunctor(theValue); }
  Acceptor* clone() const override { return new TemplateAcceptor(*this); }

 private:
  TemplateAcceptor() = delete;

  T itsFunctor;

};  // class TemplateAcceptor

using FunctionAcceptor = TemplateAcceptor<bool (*)(float)>;

}  // namespace TextGen

// ======================================================================
