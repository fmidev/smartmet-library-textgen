#include "tframe.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Phrase.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;


namespace HeaderPhrasesTest
{
  shared_ptr<TextGen::Dictionary> dict;

  void require(const string & theKey,
			   const string & theValue)
  {
	using namespace TextGen;
	Phrase p(theKey);
	const string result = p.realize(*dict);
	if(result != theValue)
	  {
		const string msg = ("'" + theKey + "'"
							+ " = '"
							+ p.realize(*dict)
							+ "' is wrong, should be '"
							+ theValue
							+ "'");
		TEST_FAILED(msg.c_str());
	  }
  }
			   

	void require(const string & theKey)
  {
	require(theKey,theKey);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief HeaderFactory::header_until() requirements
   */
  // ----------------------------------------------------------------------

  void header_until()
  {
	require("odotettavissa");

	require("1-aamuun","maanantaiaamuun");
	require("2-aamuun","tiistaiaamuun");
	require("3-aamuun","keskiviikkoaamuun");
	require("4-aamuun","torstaiaamuun");
	require("5-aamuun","perjantaiaamuun");
	require("6-aamuun","lauantaiaamuun");
	require("7-aamuun","sunnuntaiaamuun");

	require("1-iltaan","maanantai-iltaan");
	require("2-iltaan","tiistai-iltaan");
	require("3-iltaan","keskiviikkoiltaan");
	require("4-iltaan","torstai-iltaan");
	require("5-iltaan","perjantai-iltaan");
	require("6-iltaan","lauantai-iltaan");
	require("7-iltaan","sunnuntai-iltaan");

	require("asti");

	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief HeaderFactory::header_from_until() requirements
   */
  // ----------------------------------------------------------------------

  void header_from_until()
  {
	require("odotettavissa");

	require("1-aamusta","maanantaiaamusta");
	require("2-aamusta","tiistaiaamusta");
	require("3-aamusta","keskiviikkoaamusta");
	require("4-aamusta","torstaiaamusta");
	require("5-aamusta","perjantaiaamusta");
	require("6-aamusta","lauantaiaamusta");
	require("7-aamusta","sunnuntaiaamusta");

	require("1-illasta","maanantai-illasta");
	require("2-illasta","tiistai-illasta");
	require("3-illasta","keskiviikkoillasta");
	require("4-illasta","torstai-illasta");
	require("5-illasta","perjantai-illasta");
	require("6-illasta","lauantai-illasta");
	require("7-illasta","sunnuntai-illasta");

	require("1-aamuun","maanantaiaamuun");
	require("2-aamuun","tiistaiaamuun");
	require("3-aamuun","keskiviikkoaamuun");
	require("4-aamuun","torstaiaamuun");
	require("5-aamuun","perjantaiaamuun");
	require("6-aamuun","lauantaiaamuun");
	require("7-aamuun","sunnuntaiaamuun");

	require("1-iltaan","maanantai-iltaan");
	require("2-iltaan","tiistai-iltaan");
	require("3-iltaan","keskiviikkoiltaan");
	require("4-iltaan","torstai-iltaan");
	require("5-iltaan","perjantai-iltaan");
	require("6-iltaan","lauantai-iltaan");
	require("7-iltaan","sunnuntai-iltaan");

	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief HeaderFactory::header_several_days() requirements
   */
  // ----------------------------------------------------------------------

  void header_several_days()
  {
	require("1-aamusta","maanantaiaamusta");
	require("2-aamusta","tiistaiaamusta");
	require("3-aamusta","keskiviikkoaamusta");
	require("4-aamusta","torstaiaamusta");
	require("5-aamusta","perjantaiaamusta");
	require("6-aamusta","lauantaiaamusta");
	require("7-aamusta","sunnuntaiaamusta");

	require("1-illasta","maanantai-illasta");
	require("2-illasta","tiistai-illasta");
	require("3-illasta","keskiviikkoillasta");
	require("4-illasta","torstai-illasta");
	require("5-illasta","perjantai-illasta");
	require("6-illasta","lauantai-illasta");
	require("7-illasta","sunnuntai-illasta");

	require("alkavan");

	require("2-vuorokauden s‰‰","kahden vuorokauden s‰‰");
	require("3-vuorokauden s‰‰","kolmen vuorokauden s‰‰");
	require("4-vuorokauden s‰‰","nelj‰n vuorokauden s‰‰");
	require("5-vuorokauden s‰‰","viiden vuorokauden s‰‰");
	require("6-vuorokauden s‰‰","kuuden vuorokauden s‰‰");
	require("7-vuorokauden s‰‰","seitsem‰n vuorokauden s‰‰");
	require("8-vuorokauden s‰‰","kahdeksan vuorokauden s‰‰");
	require("9-vuorokauden s‰‰","yhdeks‰n vuorokauden s‰‰");
	require("10-vuorokauden s‰‰","kymmenen vuorokauden s‰‰");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief HeaderFactory::header_report_area() requirements
   */
  // ----------------------------------------------------------------------

  void header_report_area()
  {
	require("s‰‰ennuste");
	require("kello");
	require("o'clock","");
	
	require("1-na","maanantaina");
	require("2-na","tiistaina");
	require("3-na","keskiviikkona");
	require("4-na","torstaina");
	require("5-na","perjantaina");
	require("6-na","lauantaina");
	require("7-na","sunnuntaina");

	require("aland:lle","Ahvenanmaalle");
	require("etela-karjala:lle","Etel‰-Karjalaan");
	require("etela-pohjanmaa:lle","Etel‰-Pohjanmaalle");
	require("etela-savo:lle","Etel‰-Savoon");
	require("hame:lle","H‰meeseen");
	require("kainuu:lle","Kainuuseen");
	require("keski-pohjanmaa:lle","Keski-Pohjanmaalle");
	require("keski-suomi:lle","Keski-Suomeen");
	require("koillismaa:lle","Koillismaalle");
	require("kymenlaakso:lle","Kymenlaaksoon");
	require("lansi-lappi:lle","L‰nsi-Lappiin");
	require("nyland:lle","Uudellemaalle");
	require("osterbotten:lle","Pohjanmaalle");
	require("paijat-hame:lle","P‰ij‰t-H‰meeseen");
	require("pirkanmaa:lle","Pirkanmaalle");
	require("pohjois-karjala:lle","Pohjois-Karjalaan");
	require("pohjois-lappi:lle","Pohjois-Lappiin");
	require("pohjois-pohjanmaa:lle","Pohjois-Pohjanmaalle");
	require("pohjois-savo:lle","Pohjois-Savoon");
	require("satakunta:lle","Satakuntaan");
	require("uusimaa:lle","Uudellemaalle");
	require("varsinais-suomi:lle","Varsinais-Suomeen");

	TEST_PASSED();
  }


  //! The actual test driver
  class tests : public tframe::tests
  {
	//! Overridden message separator
	virtual const char * const error_message_prefix() const
	{
	  return "\n\t";
	}

	//! Main test suite
	void test(void)
	{
	  TEST(header_until);
	  TEST(header_from_until);
	  TEST(header_several_days);
	  TEST(header_report_area);

	}

  }; // class tests

} // namespace HeaderPhrasesTest


int main(void)
{
  cout << endl
	   << "HeaderFactory requirements" << endl
	   << "==========================" << endl;

  HeaderPhrasesTest::dict = TextGen::DictionaryFactory::create("mysql");
  HeaderPhrasesTest::dict->init("fi");

  HeaderPhrasesTest::tests t;
  return t.run();
}
