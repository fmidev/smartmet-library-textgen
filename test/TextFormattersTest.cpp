// ======================================================================
/*!
 * \file
 * \brief Regression tests for the plain, html, wml, speech, debug and sonera text formatters
 */
// ======================================================================

#include "DebugTextFormatter.h"
#include "DictionaryFactory.h"
#include "Document.h"
#include "Header.h"
#include "HtmlTextFormatter.h"
#include "Integer.h"
#include "IntegerRange.h"
#include "Paragraph.h"
#include "PlainTextFormatter.h"
#include "Sentence.h"
#include "SoneraTextFormatter.h"
#include "SpeechTextFormatter.h"
#include "WmlTextFormatter.h"
#include <boost/locale.hpp>
#include <calculator/Settings.h>
#include <newbase/NFmiSettings.h>
#include <regression/tframe.h>
#include <iostream>
#include <string>

using namespace std;

namespace TextFormattersTest
{
std::shared_ptr<TextGen::Dictionary> dict;

// "Weather" header and a paragraph with a number and a range
TextGen::Document document()
{
  using namespace TextGen;
  Sentence s1;
  s1 << "lampotila"
     << "on" << Integer(-5) << "astetta";
  Sentence s2;
  s2 << "tuuli" << IntegerRange(3, 7) << "metria sekunnissa";
  Paragraph p;
  p << s1 << s2;
  Header h;
  h << "saa";
  Document d;
  d << h << p;
  return d;
}

template <typename Formatter>
void require(const std::string& theLanguage, const std::string& theExpected)
{
  dict->init(theLanguage);
  Formatter formatter;
  formatter.dictionary(dict);
  const std::string result = formatter.format(document());
  if (result != theExpected)
    TEST_FAILED("Expected '" + theExpected + "', got '" + result + "'");
}

// ----------------------------------------------------------------------

void plain()
{
  require<TextGen::PlainTextFormatter>(
      "en", "The weather\n\nTemperature is -5 degrees. Wind 3-7 meters per second.\n");
  require<TextGen::PlainTextFormatter>(
      "fi", "Sää\n\nLämpötila on -5 astetta. Tuuli 3-7 metriä sekunnissa.\n");
  TEST_PASSED();
}

void html()
{
  require<TextGen::HtmlTextFormatter>("en",
                                      "<div><h1>The weather</h1>\n\n<p>Temperature is -5 degrees. "
                                      "Wind 3-7 meters per second.</p></div>");
  TEST_PASSED();
}

void wml()
{
  require<TextGen::WmlTextFormatter>("en",
                                     "<p><b>The weather</b><br/></p>\n\n<p>Temperature is -5 "
                                     "degrees. Wind 3-7 meters per second.<br/></p>");
  TEST_PASSED();
}

void speech()
{
  require<TextGen::SpeechTextFormatter>(
      "en", "The weather. Temperature is -5 degrees. Wind 3-7 meters per second.");
  require<TextGen::SpeechTextFormatter>(
      "fi", "Sää. Lämpötila on -5 astetta. Tuuli 3-7 metriä sekunnissa.");
  TEST_PASSED();
}

void debug()
{
  // The debug formatter shows the untranslated phrases
  require<TextGen::DebugTextFormatter>(
      "en", "Saa\n: Lampotila\non\n-5\nastetta\n.\nTuuli\n3-7\nmetria sekunnissa\n.\n");
  TEST_PASSED();
}

void sonera()
{
  require<TextGen::SoneraTextFormatter>(
      "en",
      "r1,the weather,temperature,0is,minus,005,degrees,wind,003,0to,007,meters per second,;\n");
  TEST_PASSED();
}

// ----------------------------------------------------------------------

class tests : public tframe::tests
{
  const char* error_message_prefix() const override { return "\n\t"; }
  void test() override
  {
    TEST(plain);
    TEST(html);
    TEST(wml);
    TEST(speech);
    TEST(debug);
    TEST(sonera);
  }
};

}  // namespace TextFormattersTest

int main()
{
  boost::locale::generator generator;
  std::locale::global(generator(""));

  NFmiSettings::Init();
  Settings::set(NFmiSettings::ToString());
  Settings::set("textgen::podictionaries", "../po");

  TextFormattersTest::dict.reset(TextGen::DictionaryFactory::create("po"));

  cout << endl << "Text formatter tests" << endl << "====================" << endl;
  TextFormattersTest::tests t;
  return t.run();
}
