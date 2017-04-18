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
  virtual ~ValueAcceptor() {}
  virtual bool accept(float theValue) const;
  virtual Acceptor* clone() const;

  ValueAcceptor();
  void value(float theValue);

 private:
  float itsValue;

};  // class ValueAcceptor
}  // namespace TextGen

// ======================================================================
