#include "tframe.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Phrase.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;


namespace StoryPhrasesTest
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
   * \brief TemperatureStory::mean() requirements
   */
  // ----------------------------------------------------------------------

  void temperature_mean()
  {
	require("keskil�mp�tila");
	require("astetta");

	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief TemperatureStory::meanmax() requirements
   */
  // ----------------------------------------------------------------------

  void temperature_meanmax()
  {
	require("keskim��r�inen ylin l�mp�tila");
	require("astetta");

	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief TemperatureStory::meanmin() requirements
   */
  // ----------------------------------------------------------------------

  void temperature_meanmin()
  {
	require("keskim��r�inen alin l�mp�tila");
	require("astetta");

	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief TemperatureStory::dailymax() requirements
   */
  // ----------------------------------------------------------------------

  void temperature_dailymax()
  {
	require("p�iv�n ylin l�mp�tila");
	require("on");
	require("noin");
	require("astetta");
	require("seuraavana p�iv�n�");
	require("huomattavasti korkeampi");
	require("korkeampi");
	require("hieman korkeampi");
	require("suunnilleen sama");
	require("hieman alempi");
	require("alempi");
	require("huomattavasti alempi");

	require("1-na","maanantaina");
	require("2-na","tiistaina");
	require("3-na","keskiviikkona");
	require("4-na","torstaina");
	require("5-na","perjantaina");
	require("6-na","lauantaina");
	require("7-na","sunnuntaina");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief TemperatureStory::nightlymin() requirements
   */
  // ----------------------------------------------------------------------

  void temperature_nightlymin()
  {
	require("y�n alin l�mp�tila");
	require("on");
	require("noin");
	require("astetta");
	require("seuraavana y�n�");
	require("huomattavasti korkeampi");
	require("korkeampi");
	require("hieman korkeampi");
	require("suunnilleen sama");
	require("hieman alempi");
	require("alempi");
	require("huomattavasti alempi");

	require("1-vastaisena y�n�","maanantain vastaisena y�n�");
	require("2-vastaisena y�n�","tiistain vastaisena y�n�");
	require("3-vastaisena y�n�","keskiviikon vastaisena y�n�");
	require("4-vastaisena y�n�","torstain vastaisena y�n�");
	require("5-vastaisena y�n�","perjantain vastaisena y�n�");
	require("6-vastaisena y�n�","lauantain vastaisena y�n�");
	require("7-vastaisena y�n�","sunnuntain vastaisena y�n�");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TemperatureStory::weekly_minmax()
   */
  // ----------------------------------------------------------------------

  void temperature_weekly_minmax()
  {
	require("p�ivien ylin l�mp�tila");
	require("�iden alin l�mp�tila");
	require("on");
	require("noin");
	require("astetta");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TemperatureStory::weekly_averages()
   */
  // ----------------------------------------------------------------------

  void temperature_weekly_averages()
  {
	require("p�iv�l�mp�tila");
	require("y�l�mp�tila");
	require("on");
	require("noin");
	require("astetta");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TemperatureStory::weekly_averages_trend()
   */
  // ----------------------------------------------------------------------

  void temperature_weekly_averages_trend()
  {
	require("p�iv�l�mp�tila");
	require("y�l�mp�tila");
	require("on");
	require("noin");
	require("astetta");
	require("aluksi");
	require("lopuksi");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief PrecipitationStory::total() requirements
   */
  // ----------------------------------------------------------------------

  void precipitation_total()
  {

	require("sadesumma");
	require("millimetri�");

	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief PrecipitationStory::range() requirements
   */
  // ----------------------------------------------------------------------

  void precipitation_range()
  {
	require("sadesumma");
	require("yli");
	require("millimetri�");

	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief PrecipitationStory::classification() requirements
   */
  // ----------------------------------------------------------------------

  void precipitation_classification()
  {
	require("sadesumma");
	require("paikoin");
	require("monin paikoin");
	require("yli");
	require("millimetri�");
	require("paikoin enemm�n");
	require("monin paikoin enemm�n");

	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief WeatherStory::shortoverview() requirements
   */
  // ----------------------------------------------------------------------

  void weather_shortoverview()
  {
	require("enimm�kseen");
	require("selke��");
	require("puolipilvist�");
	require("pilvist�");
	require("vaihtelevaa pilvisyytt�");
	require("poutaa");
	require("paikoin");
	require("sadetta");
	require("ajoittain sateista");
	require("s�� on ep�vakaista");
	require("tai");
	require("r�nt�sadetta");
	require("lumisadetta");
	require("vesi-");
	require("lumi-");

	require("1-na","maanantaina");
	require("2-na","tiistaina");
	require("3-na","keskiviikkona");
	require("4-na","torstaina");
	require("5-na","perjantaina");
	require("6-na","lauantaina");
	require("7-na","sunnuntaina");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief FrostStory::mean() requirements
   */
  // ----------------------------------------------------------------------

  void frost_mean()
  {
	require("hallan todenn�k�isyys");
	require("ankaran hallan todenn�k�isyys");
	require("on");
	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief FrostStory::maximum() requirements
   */
  // ----------------------------------------------------------------------

  void frost_maximum()
  {
	require("hallan todenn�k�isyys");
	require("ankaran hallan todenn�k�isyys");
	require("on");
	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief FrostStory::maxtwonights() requirements
   */
  // ----------------------------------------------------------------------

  void frost_maxtwonights()
  {
	require("hallan todenn�k�isyys");
	require("ankaran hallan todenn�k�isyys");
	require("on");
	require("seuraavana y�n�");
	require("seuraava y�");
	require("l�mpim�mpi");
	require("huomattavasti l�mpim�mpi");

	require("1-vastaisena y�n�","maanantain vastaisena y�n�");
	require("2-vastaisena y�n�","tiistain vastaisena y�n�");
	require("3-vastaisena y�n�","keskiviikon vastaisena y�n�");
	require("4-vastaisena y�n�","torstain vastaisena y�n�");
	require("5-vastaisena y�n�","perjantain vastaisena y�n�");
	require("6-vastaisena y�n�","lauantain vastaisena y�n�");
	require("7-vastaisena y�n�","sunnuntain vastaisena y�n�");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief RelativeHumidityStory::lowest() requirements
   */
  // ----------------------------------------------------------------------

  void relative_humidity_lowest()
  {
	require("alin suhteellinen kosteus");
	require("on");
	require("huomattavasti suurempi");
	require("suurempi");
	require("hieman suurempi");
	require("sama");
	require("hieman pienempi");
	require("pienempi");
	require("huomattavasti pienempi");

	require("1-na","maanantaina");
	require("2-na","tiistaina");
	require("3-na","keskiviikkona");
	require("4-na","torstaina");
	require("5-na","perjantaina");
	require("6-na","lauantaina");
	require("7-na","sunnuntaina");

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
	  TEST(temperature_mean);
	  TEST(temperature_meanmax);
	  TEST(temperature_meanmin);
	  TEST(temperature_dailymax);
	  TEST(temperature_nightlymin);
	  TEST(temperature_weekly_minmax);
	  TEST(temperature_weekly_averages);
	  TEST(temperature_weekly_averages_trend);

	  TEST(precipitation_total);
	  TEST(precipitation_range);
	  TEST(precipitation_classification);

	  TEST(weather_shortoverview);

	  TEST(frost_mean);
	  TEST(frost_maximum);
	  TEST(frost_maxtwonights);

	  TEST(relative_humidity_lowest);

	}

  }; // class tests

} // namespace StoryPhrasesTest


int main(void)
{
  cout << endl
	   << "StoryFactory requirements" << endl
	   << "==========================" << endl;

  StoryPhrasesTest::dict = TextGen::DictionaryFactory::create("mysql");
  StoryPhrasesTest::dict->init("fi");

  StoryPhrasesTest::tests t;
  return t.run();
}
