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
  OrAcceptor(const OrAcceptor& theOther);
  OrAcceptor(const Acceptor& theLhs, const Acceptor& theRhs);
  virtual ~OrAcceptor()
  {
    delete itsLhs;
    delete itsRhs;
  }
  virtual bool accept(float theValue) const;
  virtual Acceptor* clone() const;

 private:
  OrAcceptor();

  Acceptor* itsLhs;
  Acceptor* itsRhs;

};  // class OrAcceptor
}  // namespace TextGen

// ======================================================================
