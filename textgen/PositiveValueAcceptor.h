// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::PositiveValueAcceptor
 */
// ======================================================================

#pragma once

#include <calculator/Acceptor.h>

namespace TextGen
{
class PositiveValueAcceptor : public Acceptor
{
 public:
  virtual ~PositiveValueAcceptor() {}
  virtual bool accept(float theValue) const;
  virtual Acceptor* clone() const;

};  // class PositiveValueAcceptor
}  // namespace TextGen


// ======================================================================
