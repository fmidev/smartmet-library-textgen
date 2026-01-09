#include "NullDictionary.h"
#include "PositiveRange.h"
#include <regression/tframe.h>

#include <newbase/NFmiSettings.h>

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/locale.hpp>

using namespace std;

namespace PositiveRangeTest
{
// ----------------------------------------------------------------------
/*!
 * \brief Test structors
 */
// ----------------------------------------------------------------------

void structors(void)
{
  using namespace TextGen;

  // Should succeed
  PositiveRange r1(12, 14);

  // Should succeed
  PositiveRange r2(r1);

  // Should succeed
  PositiveRange r3 = r1;

  PositiveRange r4(0, 0);
  r4 = r1;

  // Should fail
  try
  {
    PositiveRange(-1, 1);
  }
  catch (...)
  {
  }

  try
  {
    PositiveRange(-4, -1);
  }
  catch (...)
  {
  }

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test PositiveRange::isDelimiter
 */
// ----------------------------------------------------------------------

void isdelimiter()
{
  using namespace TextGen;

  PositiveRange r1(0, 0);
  if (r1.isDelimiter())
    TEST_FAILED("should never return true for ranges");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test PositiveRange::startValue
 */
// ----------------------------------------------------------------------

void startvalue()
{
  using namespace TextGen;

  PositiveRange r1(1, 2);
  if (r1.startValue() != 1)
    TEST_FAILED("failed to return 1 for 1-2");

  PositiveRange r2(2, 3);
  if (r2.startValue() != 2)
    TEST_FAILED("failed to return 2 for 2-3");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test PositiveRange::endValue
 */
// ----------------------------------------------------------------------

void endvalue()
{
  using namespace TextGen;

  PositiveRange r1(1, 2);
  if (r1.endValue() != 2)
    TEST_FAILED("failed to return 2 for 1-2");

  PositiveRange r2(2, 3);
  if (r2.endValue() != 3)
    TEST_FAILED("failed to return 3 for 2-3");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test PositiveRange::realize
 */
// ----------------------------------------------------------------------

void realize(void)
{
  using namespace TextGen;

  PositiveRange r1(0, 0);
  if (r1.realize(NullDictionary()) != "0")
    TEST_FAILED("realization of 0,0 failed");

  PositiveRange r2(1, 2);
  if (r2.realize(NullDictionary()) != "1-2")
    TEST_FAILED("realization of 1,2 failed");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test PositiveRange range separator
 */
// ----------------------------------------------------------------------

void rangeseparator()
{
  using namespace TextGen;

  PositiveRange range1(1, 2);

  if (range1.rangeSeparator() != "-")
    TEST_FAILED("default range separator should be -");

  if (range1.realize(NullDictionary()) != "1-2")
    TEST_FAILED("realization of 1,2 failed to produce 1-2");

  PositiveRange range2(1, 2, "...");

  if (range2.rangeSeparator() != "...")
    TEST_FAILED("range separator should be ... after setting it");

  if (range2.realize(NullDictionary()) != "1...2")
    TEST_FAILED("realization of 1,2 failed to produce 1...2");

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
  void test(void)
  {
    TEST(structors);
    TEST(isdelimiter);
    TEST(startvalue);
    TEST(endvalue);
    TEST(realize);
    TEST(rangeseparator);
  }

};  // class tests

}  // namespace PositiveRangeTest

int main(void)
{
  boost::locale::generator generator;
  std::locale::global(generator(""));

  NFmiSettings::Init();
  NFmiSettings::Set("textgen::database", "textgen2");

  cout << endl << "PositiveRange tester" << endl << "===================" << endl;
  PositiveRangeTest::tests t;
  return t.run();
}
