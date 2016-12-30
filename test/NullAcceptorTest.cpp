#include <regression/tframe.h>
#include "NullAcceptor.h"
#include <newbase/NFmiGlobals.h>

#include <iostream>
#include <string>

using namespace std;

namespace NullAcceptorTest
{
// ----------------------------------------------------------------------
/*!
 * \brief Test NullAcceptor
 */
// ----------------------------------------------------------------------

void accept()
{
  using namespace TextGen;

  NullAcceptor acc;

  if (!acc.accept(0)) TEST_FAILED("Should accept 0");

  if (!acc.accept(kFloatMissing)) TEST_FAILED("Should accept kFloatMissing");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief The actual test driver
 */
// ----------------------------------------------------------------------

class tests : public tframe::tests
{
  //! Overridden message separator
  virtual const char* error_message_prefix() const { return "\n\t"; }
  //! Main test suite
  void test(void) { TEST(accept); }
};  // class tests

}  // namespace NullAcceptorTest

int main(void)
{
  using namespace NullAcceptorTest;

  cout << endl << "NullAcceptor tests" << endl << "=====================" << endl;

  tests t;
  return t.run();
}
