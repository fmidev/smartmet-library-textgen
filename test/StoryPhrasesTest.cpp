#include "regression/tframe.h"
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
   * \brief Test generic time phrases
   */
  // ----------------------------------------------------------------------

  void timephrases()
  {
	require("t�n��n");
	require("huomenna");
	require("ylihuomenna");
	require("ensi y�n�");
	require("huomisaamuna");
	require("p�iv�ll�");

	require("y�ll�");
	require("aamuy�ll�");
	require("aamulla");
	require("aamup�iv�ll�");
	require("keskip�iv�ll�");
	require("iltap�iv�ll�");
	require("illalla");
	require("iltay�ll�");
	require("keskiy�ll�");

	require("y�st� alkaen");
	require("aamuy�st� alkaen");
	require("aamusta alkaen");
	require("aamup�iv�st� alkaen");
	require("keskip�iv�st� alkaen");
	require("iltap�iv�st� alkaen");
	require("illasta alkaen");
	require("iltay�st� alkaen");
	require("keskiy�st� alkaen");

	require("1-na","maanantaina");
	require("2-na","tiistaina");
	require("3-na","keskiviikkona");
	require("4-na","torstaina");
	require("5-na","perjantaina");
	require("6-na","lauantaina");
	require("7-na","sunnuntaina");

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

	require("1-alkaen","maanantaista alkaen");
	require("2-alkaen","tiistaista alkaen");
	require("3-alkaen","keskiviikosta alkaen");
	require("4-alkaen","torstaista alkaen");
	require("5-alkaen","perjantaista alkaen");
	require("6-alkaen","lauantaista alkaen");
	require("7-alkaen","sunnuntaista alkaen");

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
   * \brief PrecipitationStory::sums() requirements
   */
  // ----------------------------------------------------------------------

  void precipitation_sums()
  {
	require("ensimm�isen 12 tunnin sadem��r�");
	require("seuraavan 12 tunnin");
	require("seuraavan 24 tunnin sadem��r�");
	require("on");
	require("noin");
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
   * \brief WindStory::overview() requirements
   */
  // ----------------------------------------------------------------------

  void wind_overview()
  {
	require("m/s");
	require("metri� sekunnissa");
	require("tyynt�");
	require("heikkoa");
	require("kohtalaista");
	require("navakkaa");
	require("kovaa");
	require("myrsky�");
	require("hirmumyrsky�");
	require("tyyneksi");
	require("heikoksi");
	require("kohtalaiseksi");
	require("navakaksi");
	require("kovaksi");
	require("myrskyksi");
	require("hirmumyrskyksi");
	require("tuuli");
	require("tuulta");
	require("y�ll�");
	require("tuuli tyyntyy");
	require("tuuli heikkenee");
	require("tuuli voimistuu");
	require("edelleen");
	require("heikkenev��");
	require("voimistuvaa");
	require("suunnaltaan vaihtelevaa");

	require("1-tuulta","pohjoistuulta");
	require("2-tuulta","koillistuulta");
	require("3-tuulta","it�tuulta");
	require("4-tuulta","kaakkoistuulta");
	require("5-tuulta","etel�tuulta");
	require("6-tuulta","lounaistuulta");
	require("7-tuulta","l�nsituulta");
	require("8-tuulta","luoteistuulta");

	require("1-tuuleksi","pohjoistuuleksi");
	require("2-tuuleksi","koillistuuleksi");
	require("3-tuuleksi","it�tuuleksi");
	require("4-tuuleksi","kaakkoistuuleksi");
	require("5-tuuleksi","etel�tuuleksi");
	require("6-tuuleksi","lounaistuuleksi");
	require("7-tuuleksi","l�nsituuleksi");
	require("8-tuuleksi","luoteistuuleksi");

	require("1-puoleista tuulta","pohjoisen puoleista tuulta");
	require("2-puoleista tuulta","koillisen puoleista tuulta");
	require("3-puoleista tuulta","id�n puoleista tuulta");
	require("4-puoleista tuulta","kaakon puoleista tuulta");
	require("5-puoleista tuulta","etel�n puoleista tuulta");
	require("6-puoleista tuulta","lounaan puoleista tuulta");
	require("7-puoleista tuulta","l�nnen puoleista tuulta");
	require("8-puoleista tuulta","luoteen puoleista tuulta");

	require("1-puoleiseksi tuuleksi","pohjoisen puoleiseksi tuuleksi");
	require("2-puoleiseksi tuuleksi","koillisen puoleiseksi tuuleksi");
	require("3-puoleiseksi tuuleksi","id�n puoleiseksi tuuleksi");
	require("4-puoleiseksi tuuleksi","kaakon puoleiseksi tuuleksi");
	require("5-puoleiseksi tuuleksi","etel�n puoleiseksi tuuleksi");
	require("6-puoleiseksi tuuleksi","lounaan puoleiseksi tuuleksi");
	require("7-puoleiseksi tuuleksi","l�nnen puoleiseksi tuuleksi");
	require("8-puoleiseksi tuuleksi","luoteen puoleiseksi tuuleksi");

	require("1-na","maanantaina");
	require("2-na","tiistaina");
	require("3-na","keskiviikkona");
	require("4-na","torstaina");
	require("5-na","perjantaina");
	require("6-na","lauantaina");
	require("7-na","sunnuntaina");

	require("1-alkaen","maanantaista alkaen");
	require("2-alkaen","tiistaista alkaen");
	require("3-alkaen","keskiviikosta alkaen");
	require("4-alkaen","torstaista alkaen");
	require("5-alkaen","perjantaista alkaen");
	require("6-alkaen","lauantaista alkaen");
	require("7-alkaen","sunnuntaista alkaen");

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
   * \brief FrostStory::twonights() requirements
   */
  // ----------------------------------------------------------------------

  void frost_twonights()
  {
	require("hallan todenn�k�isyys");
	require("ankaran hallan todenn�k�isyys");
	require("on");
	require("seuraavana y�n�");
	require("sama");
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
	  TEST(timephrases);

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
	  TEST(precipitation_sums);

	  TEST(weather_shortoverview);

	  TEST(wind_overview);

	  TEST(frost_mean);
	  TEST(frost_maximum);
	  TEST(frost_twonights);

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
