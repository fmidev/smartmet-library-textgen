// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::OrAcceptor
 */
// ======================================================================

#pragma once

#include <calculator/Acceptor.h>
#include <functional>

namespace TextGen
{
class OrAcceptor : public Acceptor
{
 public:
  OrAcceptor() = delete;
  OrAcceptor(const OrAcceptor& theOther);
  OrAcceptor(const Acceptor& theLhs, const Acceptor& theRhs);
  ~OrAcceptor() override
  {
    delete itsLhs;
    delete itsRhs;
  }
  bool accept(float theValue) const override;
  Acceptor* clone() const override;

 private:
  Acceptor* itsLhs;
  Acceptor* itsRhs;

};  // class OrAcceptor
}  // namespace TextGen

// ======================================================================
