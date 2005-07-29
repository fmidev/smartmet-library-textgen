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
	require("tänään");
	require("huomenna");
	require("ylihuomenna");
	require("ensi yönä");
	require("huomisaamuna");
	require("päivällä");

	require("yöllä");
	require("aamuyöllä");
	require("aamulla");
	require("aamupäivällä");
	require("keskipäivällä");
	require("iltapäivällä");
	require("illalla");
	require("iltayöllä");
	require("keskiyöllä");

	require("yöstä alkaen");
	require("aamuyöstä alkaen");
	require("aamusta alkaen");
	require("aamupäivästä alkaen");
	require("keskipäivästä alkaen");
	require("iltapäivästä alkaen");
	require("illasta alkaen");
	require("iltayöstä alkaen");
	require("keskiyöstä alkaen");

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

	require("1-vastaisena yönä","maanantain vastaisena yönä");
	require("2-vastaisena yönä","tiistain vastaisena yönä");
	require("3-vastaisena yönä","keskiviikon vastaisena yönä");
	require("4-vastaisena yönä","torstain vastaisena yönä");
	require("5-vastaisena yönä","perjantain vastaisena yönä");
	require("6-vastaisena yönä","lauantain vastaisena yönä");
	require("7-vastaisena yönä","sunnuntain vastaisena yönä");

	require("1-aamulla","maanantaina aamulla");
	require("2-aamulla","tiistaina aamulla");
	require("3-aamulla","keskiviikkona aamulla");
	require("4-aamulla","torstaina aamulla");
	require("5-aamulla","perjantaina aamulla");
	require("6-aamulla","lauantaina aamulla");
	require("7-aamulla","sunnuntaina aamulla");

	require("1-aamupäivällä","maanantaina aamupäivällä");
	require("2-aamupäivällä","tiistaina aamupäivällä");
	require("3-aamupäivällä","keskiviikkona aamupäivällä");
	require("4-aamupäivällä","torstaina aamupäivällä");
	require("5-aamupäivällä","perjantaina aamupäivällä");
	require("6-aamupäivällä","lauantaina aamupäivällä");
	require("7-aamupäivällä","sunnuntaina aamupäivällä");

	require("1-iltapäivällä","maanantaina iltapäivällä");
	require("2-iltapäivällä","tiistaina iltapäivällä");
	require("3-iltapäivällä","keskiviikkona iltapäivällä");
	require("4-iltapäivällä","torstaina iltapäivällä");
	require("5-iltapäivällä","perjantaina iltapäivällä");
	require("6-iltapäivällä","lauantaina iltapäivällä");
	require("7-iltapäivällä","sunnuntaina iltapäivällä");

	require("1-illalla","maanantaina illalla");
	require("2-illalla","tiistaina illalla");
	require("3-illalla","keskiviikkona illalla");
	require("4-illalla","torstaina illalla");
	require("5-illalla","perjantaina illalla");
	require("6-illalla","lauantaina illalla");
	require("7-illalla","sunnuntaina illalla");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief TemperatureStory::mean() requirements
   */
  // ----------------------------------------------------------------------

  void temperature_mean()
  {
	require("keskilämpötila");
	require("astetta");

	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief TemperatureStory::range() requirements
   */
  // ----------------------------------------------------------------------

  void temperature_range()
  {
	require("lämpötila");
	require("on");
	require("noin");
	require("astetta");

	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief DewPointStory::range() requirements
   */
  // ----------------------------------------------------------------------

  void dewpoint_range()
  {
	require("kastepiste");
	require("on");
	require("noin");
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
	require("keskimääräinen ylin lämpötila");
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
	require("keskimääräinen alin lämpötila");
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
	require("päivän ylin lämpötila");
	require("on");
	require("noin");
	require("astetta");
	require("seuraavana päivänä");
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
	require("yön alin lämpötila");
	require("on");
	require("noin");
	require("astetta");
	require("seuraavana yönä");
	require("huomattavasti korkeampi");
	require("korkeampi");
	require("hieman korkeampi");
	require("suunnilleen sama");
	require("hieman alempi");
	require("alempi");
	require("huomattavasti alempi");

	require("1-vastaisena yönä","maanantain vastaisena yönä");
	require("2-vastaisena yönä","tiistain vastaisena yönä");
	require("3-vastaisena yönä","keskiviikon vastaisena yönä");
	require("4-vastaisena yönä","torstain vastaisena yönä");
	require("5-vastaisena yönä","perjantain vastaisena yönä");
	require("6-vastaisena yönä","lauantain vastaisena yönä");
	require("7-vastaisena yönä","sunnuntain vastaisena yönä");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test TemperatureStory::weekly_minmax()
   */
  // ----------------------------------------------------------------------

  void temperature_weekly_minmax()
  {
	require("päivien ylin lämpötila");
	require("öiden alin lämpötila");
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
	require("päivälämpötila");
	require("yölämpötila");
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
	require("päivälämpötila");
	require("yölämpötila");
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
	require("millimetriä");

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
	require("on");
	require("yli");
	require("millimetriä");

	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief PrecipitationStory::sums() requirements
   */
  // ----------------------------------------------------------------------

  void precipitation_sums()
  {
	require("ensimmäisen 12 tunnin sademäärä");
	require("seuraavan 12 tunnin");
	require("seuraavan 24 tunnin sademäärä");
	require("on");
	require("noin");
	require("millimetriä");

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
	require("millimetriä");
	require("paikoin enemmän");
	require("monin paikoin enemmän");

	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief WeatherStory::shortoverview() requirements
   */
  // ----------------------------------------------------------------------

  void weather_shortoverview()
  {
	require("enimmäkseen");
	require("selkeää");
	require("puolipilvistä");
	require("pilvistä");
	require("vaihtelevaa pilvisyyttä");
	require("poutaa");
	require("paikoin");
	require("sadetta");
	require("ajoittain sateista");
	require("sää on epävakaista");
	require("tai");
	require("räntäsadetta");
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
	require("metriä sekunnissa");
	require("tyyntä");
	require("heikkoa");
	require("kohtalaista");
	require("navakkaa");
	require("kovaa");
	require("myrskyä");
	require("hirmumyrskyä");
	require("tyyneksi");
	require("heikoksi");
	require("kohtalaiseksi");
	require("navakaksi");
	require("kovaksi");
	require("myrskyksi");
	require("hirmumyrskyksi");
	require("tuuli");
	require("tuulta");
	require("yöllä");
	require("tuuli tyyntyy");
	require("tuuli heikkenee");
	require("tuuli voimistuu");
	require("edelleen");
	require("heikkenevää");
	require("voimistuvaa");
	require("suunnaltaan vaihtelevaa");

	require("1-tuulta","pohjoistuulta");
	require("2-tuulta","koillistuulta");
	require("3-tuulta","itätuulta");
	require("4-tuulta","kaakkoistuulta");
	require("5-tuulta","etelätuulta");
	require("6-tuulta","lounaistuulta");
	require("7-tuulta","länsituulta");
	require("8-tuulta","luoteistuulta");

	require("1-tuuleksi","pohjoistuuleksi");
	require("2-tuuleksi","koillistuuleksi");
	require("3-tuuleksi","itätuuleksi");
	require("4-tuuleksi","kaakkoistuuleksi");
	require("5-tuuleksi","etelätuuleksi");
	require("6-tuuleksi","lounaistuuleksi");
	require("7-tuuleksi","länsituuleksi");
	require("8-tuuleksi","luoteistuuleksi");

	require("1-puoleista tuulta","pohjoisen puoleista tuulta");
	require("2-puoleista tuulta","koillisen puoleista tuulta");
	require("3-puoleista tuulta","idän puoleista tuulta");
	require("4-puoleista tuulta","kaakon puoleista tuulta");
	require("5-puoleista tuulta","etelän puoleista tuulta");
	require("6-puoleista tuulta","lounaan puoleista tuulta");
	require("7-puoleista tuulta","lännen puoleista tuulta");
	require("8-puoleista tuulta","luoteen puoleista tuulta");

	require("1-puoleiseksi tuuleksi","pohjoisen puoleiseksi tuuleksi");
	require("2-puoleiseksi tuuleksi","koillisen puoleiseksi tuuleksi");
	require("3-puoleiseksi tuuleksi","idän puoleiseksi tuuleksi");
	require("4-puoleiseksi tuuleksi","kaakon puoleiseksi tuuleksi");
	require("5-puoleiseksi tuuleksi","etelän puoleiseksi tuuleksi");
	require("6-puoleiseksi tuuleksi","lounaan puoleiseksi tuuleksi");
	require("7-puoleiseksi tuuleksi","lännen puoleiseksi tuuleksi");
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
   * \brief WindStory::daily_ranges() requirements
   */
  // ----------------------------------------------------------------------

  void wind_daily_ranges()
  {
	require("m/s");
	require("metriä sekunnissa");
	require("suunnaltaan vaihtelevaa");

	require("1-tuulta","pohjoistuulta");
	require("2-tuulta","koillistuulta");
	require("3-tuulta","itätuulta");
	require("4-tuulta","kaakkoistuulta");
	require("5-tuulta","etelätuulta");
	require("6-tuulta","lounaistuulta");
	require("7-tuulta","länsituulta");
	require("8-tuulta","luoteistuulta");

	require("1-puoleista tuulta","pohjoisen puoleista tuulta");
	require("2-puoleista tuulta","koillisen puoleista tuulta");
	require("3-puoleista tuulta","idän puoleista tuulta");
	require("4-puoleista tuulta","kaakon puoleista tuulta");
	require("5-puoleista tuulta","etelän puoleista tuulta");
	require("6-puoleista tuulta","lounaan puoleista tuulta");
	require("7-puoleista tuulta","lännen puoleista tuulta");
	require("8-puoleista tuulta","luoteen puoleista tuulta");

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
   * \brief WindStory::ranges() requirements
   */
  // ----------------------------------------------------------------------

  void wind_range()
  {
	require("m/s");
	require("metriä sekunnissa");
	require("suunnaltaan vaihtelevaa");

	require("1-tuulta","pohjoistuulta");
	require("2-tuulta","koillistuulta");
	require("3-tuulta","itätuulta");
	require("4-tuulta","kaakkoistuulta");
	require("5-tuulta","etelätuulta");
	require("6-tuulta","lounaistuulta");
	require("7-tuulta","länsituulta");
	require("8-tuulta","luoteistuulta");

	require("1-puoleista tuulta","pohjoisen puoleista tuulta");
	require("2-puoleista tuulta","koillisen puoleista tuulta");
	require("3-puoleista tuulta","idän puoleista tuulta");
	require("4-puoleista tuulta","kaakon puoleista tuulta");
	require("5-puoleista tuulta","etelän puoleista tuulta");
	require("6-puoleista tuulta","lounaan puoleista tuulta");
	require("7-puoleista tuulta","lännen puoleista tuulta");
	require("8-puoleista tuulta","luoteen puoleista tuulta");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief FrostStory::mean() requirements
   */
  // ----------------------------------------------------------------------

  void frost_mean()
  {
	require("hallan todennäköisyys");
	require("ankaran hallan todennäköisyys");
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
	require("hallan todennäköisyys");
	require("ankaran hallan todennäköisyys");
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
	require("hallan todennäköisyys");
	require("ankaran hallan todennäköisyys");
	require("on");
	require("seuraavana yönä");
	require("sama");
	require("seuraava yö");
	require("lämpimämpi");
	require("huomattavasti lämpimämpi");

	require("1-vastaisena yönä","maanantain vastaisena yönä");
	require("2-vastaisena yönä","tiistain vastaisena yönä");
	require("3-vastaisena yönä","keskiviikon vastaisena yönä");
	require("4-vastaisena yönä","torstain vastaisena yönä");
	require("5-vastaisena yönä","perjantain vastaisena yönä");
	require("6-vastaisena yönä","lauantain vastaisena yönä");
	require("7-vastaisena yönä","sunnuntain vastaisena yönä");

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

  // ----------------------------------------------------------------------
  /*!
   * \brief RelativeHumidityStory::day() requirements
   */
  // ----------------------------------------------------------------------

  void relative_humidity_day()
  {
	require("alin suhteellinen kosteus");
	require("on");
	require("rannikolla");

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
   * \brief RelativeHumidityStory::range() requirements
   */
  // ----------------------------------------------------------------------

  void relative_humidity_range()
  {
	require("suhteellinen kosteus");
	require("on");
	require("noin");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief RoadStory::daynightranges requirements
   */
  // ----------------------------------------------------------------------

  void roadtemperature_daynightranges()
  {
	require("tienpintalämpötila");
	require("on");
	require("noin");
	require("astetta");
	require("päivällä");
	require("yöllä");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief RoadStory::condition_overview requirements
   */
  // ----------------------------------------------------------------------

  void roadcondition_overview()
  {
	require("tiet ovat");
	require("teillä on");
	require("jäisiä");
	require("osittain jäisiä");
	require("kuuraa");
	require("sohjoa");
	require("lunta");
	require("märkiä");
	require("kosteita");
	require("kuivia");
	require("kuuraisia");
	require("sohjoisia");
	require("lumisia");
	require("tiet ovat (sivulause)", "tiet ovat");
	require("yleisesti");
	require("monin paikoin");
	require("paikoin");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief PressureStory::mean requirements
   */
  // ----------------------------------------------------------------------

  void pressure_mean()
  {
	require("paine");
	require("on");
	require("hehtopascalia");

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
	  TEST(temperature_range);

	  TEST(dewpoint_range);

	  TEST(precipitation_total);
	  TEST(precipitation_range);
	  TEST(precipitation_classification);
	  TEST(precipitation_sums);

	  TEST(weather_shortoverview);

	  TEST(wind_overview);
	  TEST(wind_daily_ranges);
	  TEST(wind_range);

	  TEST(frost_mean);
	  TEST(frost_maximum);
	  TEST(frost_twonights);

	  TEST(relative_humidity_lowest);
	  TEST(relative_humidity_day);
	  TEST(relative_humidity_range);

	  TEST(roadtemperature_daynightranges);
	  TEST(roadcondition_overview);

	  TEST(pressure_mean);

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
