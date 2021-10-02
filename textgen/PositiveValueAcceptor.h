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
  ~PositiveValueAcceptor() override {}
  bool accept(float theValue) const override;
  Acceptor* clone() const override;

};  // class PositiveValueAcceptor
}  // namespace TextGen

// ======================================================================
