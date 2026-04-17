#include "PoDictionary.h"
#include <calculator/Settings.h>
#include <macgyver/Exception.h>
#include <newbase/NFmiSettings.h>
#include <regression/tframe.h>
#include <iostream>
#include <string>

#include <boost/locale.hpp>

using namespace std;

namespace PoDictionaryTest
{
//! Test structors
void structors(void)
{
  using namespace TextGen;

  {
    PoDictionary dict;
  }

  {
    PoDictionary* dict = new PoDictionary();
    delete dict;
  }

  {
    PoDictionary dict;
    PoDictionary dict2(dict);
    PoDictionary dict3 = dict;
    PoDictionary dict4;
    dict4 = dict;
  }

  TEST_PASSED();
}

//! Test init() method
void init(void)
{
  using namespace TextGen;
  PoDictionary dict;

  dict.init("fi");
  dict.init("en");

  try
  {
    dict.init("xx_nonexistent");
    TEST_FAILED("init('xx_nonexistent') should have failed");
  }
  catch (const Fmi::Exception&)
  {
  }
  TEST_PASSED();
}

//! Test language()
void language(void)
{
  using namespace TextGen;
  PoDictionary dict;

  if (!dict.language().empty())
    TEST_FAILED("language should be empty before init");

  dict.init("fi");
  if (dict.language() != "fi")
    TEST_FAILED("language should match init argument");

  TEST_PASSED();
}

//! Test empty() method
void empty(void)
{
  using namespace TextGen;
  PoDictionary dict;

  if (!dict.empty())
    TEST_FAILED("empty() should return true before init()");

  dict.init("fi");
  if (dict.empty())
    TEST_FAILED("empty() should return false after init()");

  TEST_PASSED();
}

//! Test size() method
void size(void)
{
  using namespace TextGen;
  PoDictionary dict;

  if (dict.size() != 0)
    TEST_FAILED("size() should return 0 before init()");

  dict.init("fi");
  if (dict.size() == 0)
    TEST_FAILED("size() should return > 0 after init(fi)");

  TEST_PASSED();
}

//! Test contains() method
void contains(void)
{
  using namespace TextGen;
  PoDictionary dict;

  if (dict.contains("foobar"))
    TEST_FAILED("contains(foobar) should have failed");

  dict.init("fi");
  if (dict.contains("foobar"))
    TEST_FAILED("contains(foobar) should have failed");
  if (!dict.contains("sama"))
    TEST_FAILED("contains(sama) should have succeeded");

  TEST_PASSED();
}

//! Test find() method
void find(void)
{
  using namespace TextGen;
  PoDictionary dict;

  try
  {
    dict.find("foobar");
    TEST_FAILED("find() should throw before init()");
  }
  catch (const Fmi::Exception& e)
  {
  }

  dict.init("fi");

  try
  {
    dict.find("foobar");
    TEST_FAILED("find(foobar) should have thrown");
  }
  catch (const Fmi::Exception& e)
  {
  }

  if (dict.find("sama") != "sama")
    TEST_FAILED("find(sama) should have returned sama");

  dict.init("en");
  if (dict.find("sama") != "the same")
    TEST_FAILED("find(sama) should have returned 'the same'");

  TEST_PASSED();
}

//! Test insert
void insert(void)
{
  using namespace TextGen;
  PoDictionary dict;

  try
  {
    dict.insert("foo", "bar");
    TEST_FAILED("insert(foo,bar) should throw even before init()");
  }
  catch (const Fmi::Exception& e)
  {
  }

  dict.init("fi");

  try
  {
    dict.insert("foo", "bar");
    TEST_FAILED("insert(foo,bar) should have thrown after init(fi)");
  }
  catch (const Fmi::Exception& e)
  {
  }

  TEST_PASSED();
}

//! Verify parity with the .txt dictionary for a sampling of keywords.
//! If this fails, the txt_to_po.py migration or the .po parser has drifted.
void parity_with_txt(void)
{
  using namespace TextGen;
  PoDictionary po;
  po.init("en");

  // A handful of entries that exercise apostrophes, empty translations
  // and multi-word keywords.
  if (po.find("sama") != "the same")
    TEST_FAILED("en: sama should map to 'the same'");
  if (po.find("o'clock") != "o'clock")
    TEST_FAILED("en: o'clock should map to 'o'clock'");
  if (!po.contains("meri-lappi:lle"))
    TEST_FAILED("en: missing 'meri-lappi:lle'");

  TEST_PASSED();
}

//! The actual test driver
class tests : public tframe::tests
{
  virtual const char* error_message_prefix() const { return "\n\t"; }
  void test(void)
  {
    TEST(structors);
    TEST(init);
    TEST(language);
    TEST(empty);
    TEST(size);
    TEST(contains);
    TEST(find);
    TEST(insert);
    TEST(parity_with_txt);
  }

};  // class tests

}  // namespace PoDictionaryTest

int main(void)
{
  boost::locale::generator generator;
  std::locale::global(generator(""));

  NFmiSettings::Init();
  Settings::set(NFmiSettings::ToString());

  cout << endl << "PoDictionary tester" << endl << "===================" << endl;
  PoDictionaryTest::tests t;
  return t.run();
}
