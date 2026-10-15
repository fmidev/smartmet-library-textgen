// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::AndAcceptor
 */
// ======================================================================

#pragma once

#include <calculator/Acceptor.h>
#include <functional>

namespace TextGen
{
class AndAcceptor : public Acceptor
{
 public:
  AndAcceptor() = delete;
  AndAcceptor(const AndAcceptor& theOther);
  AndAcceptor(const Acceptor& theLhs, const Acceptor& theRhs);
  ~AndAcceptor() override
  {
    delete itsLhs;
    delete itsRhs;
  }
  AndAcceptor& operator=(const AndAcceptor&) = delete;
  bool accept(float theValue) const override;
  Acceptor* clone() const override;

 private:
  Acceptor* itsLhs;
  Acceptor* itsRhs;

};  // class AndAcceptor
}  // namespace TextGen

// ======================================================================
