// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::ValueAcceptor
 */
// ======================================================================

#pragma once

#include <calculator/Acceptor.h>

namespace TextGen
{
class ValueAcceptor : public Acceptor
{
 public:
  ~ValueAcceptor() override {}
  bool accept(float theValue) const override;
  Acceptor* clone() const override;

  ValueAcceptor();
  void value(float theValue);

 private:
  float itsValue;

};  // class ValueAcceptor
}  // namespace TextGen

// ======================================================================
