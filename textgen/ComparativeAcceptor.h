// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::ComparativeAcceptor
 */
// ======================================================================

#pragma once

#include <calculator/Acceptor.h>
#include <newbase/NFmiGlobals.h>

namespace TextGen
{
enum comparative_operator
{
  LESS_THAN,
  LESS_OR_EQUAL,
  EQUAL,
  GREATER_OR_EQUAL,
  GREATER_THAN,
  VOID_OPERATOR
};

class ComparativeAcceptor : public Acceptor
{
 public:
  ~ComparativeAcceptor() override = default;
  bool accept(float theValue) const override;
  Acceptor* clone() const override;

  ComparativeAcceptor(const float& theLimit = kFloatMissing,
                      const comparative_operator& theOperator = VOID_OPERATOR);
  void setLimit(float theLimit);
  void setOperator(const comparative_operator& theOperator);

 private:
  float itsLimit;
  comparative_operator itsOperator;

};  // class ComparativeAcceptor
}  // namespace TextGen

// ======================================================================
