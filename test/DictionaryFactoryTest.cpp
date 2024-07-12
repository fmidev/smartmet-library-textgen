#include "DictionaryFactory.h"
#include <regression/tframe.h>

#include <newbase/NFmiSettings.h>

#include <memory>

#include <iostream>
#include <string>

#include <boost/locale.hpp>

using namespace std;

namespace DictionaryFactoryTest
{
//! Test create
void create(void)
{
  using namespace TextGen;

  // Should succeed
  std::shared_ptr<Dictionary> dict1(DictionaryFactory::create("null"));
  std::shared_ptr<Dictionary> dict2(DictionaryFactory::create("basic"));
  std::shared_ptr<Dictionary> dict3(DictionaryFactory::create("file"));

  // Should throw
  try
  {
    std::shared_ptr<Dictionary> dict4(DictionaryFactory::create("foobar"));
    TEST_FAILED("create(foobar) should have thrown");
  }
  catch (...)
  {
  }

  TEST_PASSED();
}

//! The actual test driver
class tests : public tframe::tests
{
  //! Overridden message separator
  virtual const char* error_message_prefix() const { return "\n\t"; }
  //! Main test suite
  void test(void) { TEST(create); }
};  // class tests

}  // namespace DictionaryFactoryTest

int main(void)
{
  boost::locale::generator generator;
  std::locale::global(generator(""));

  NFmiSettings::Init();
  NFmiSettings::Set("textgen::database", "textgen2");

  cout << endl << "DictionaryFactory tester" << endl << "========================" << endl;
  DictionaryFactoryTest::tests t;
  return t.run();
}
