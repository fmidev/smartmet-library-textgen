#include "tframe.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Phrase.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;


namespace SoneraTest
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

  void phrases()
  {

	require("o'clock","");
	require("pohjanmaa:lle","402");
	require("ahvenanmaa:lle","308");

	require("1-aamusta","200");
	require("1-aamuun","201");
	require("1-alkaen","202");
	require("1-illasta","203");
	require("1-iltaan","204");
	require("1-na","205");
	require("1-puoleiseksi tuuleksi","206");
	require("1-puoleista tuulta","207");
	require("1-suunnasta","208");
	require("1-suuntaan","209");
	require("1-tuuleksi","210");
	require("1-tuulta","211");
	require("1-vastaisena yönä","212");
	require("2-aamusta","213");
	require("2-aamuun","214");
	require("2-alkaen","215");
	require("2-illasta","216");
	require("2-iltaan","217");
	require("2-na","218");
	require("2-puoleiseksi tuuleksi","219");
	require("2-puoleista tuulta","220");
	require("2-suunnasta","221");
	require("2-suuntaan","222");
	require("2-tuuleksi","223");
	require("2-tuulta","224");
	require("2-vastaisena yönä","225");
	require("3-aamusta","226");
	require("3-aamuun","227");
	require("3-alkaen","228");
	require("3-illasta","229");
	require("3-iltaan","230");
	require("3-na","231");
	require("3-puoleiseksi tuuleksi","232");
	require("3-puoleista tuulta","233");
	require("3-suunnasta","234");
	require("3-suuntaan","235");
	require("3-tuuleksi","236");
	require("3-tuulta","237");
	require("3-vastaisena yönä","238");
	require("4-aamusta","239");
	require("4-aamuun","240");
	require("4-alkaen","241");
	require("4-illasta","242");
	require("4-iltaan","243");
	require("4-na","244");
	require("4-puoleiseksi tuuleksi","245");
	require("4-puoleista tuulta","246");
	require("4-suunnasta","247");
	require("4-suuntaan","248");
	require("4-tuuleksi","249");
	require("4-tuulta","250");
	require("4-vastaisena yönä","251");
	require("5-aamusta","252");
	require("5-aamuun","253");
	require("5-alkaen","254");
	require("5-illasta","255");
	require("5-iltaan","256");
	require("5-na","257");
	require("5-puoleiseksi tuuleksi","258");
	require("5-puoleista tuulta","259");
	require("5-suunnasta","260");
	require("5-suuntaan","261");
	require("5-tuuleksi","262");
	require("5-tuulta","263");
	require("5-vastaisena yönä","264");
	require("5-vuorokauden sää","265");
	require("6-aamusta","266");
	require("6-aamuun","267");
	require("6-alkaen","268");
	require("6-illasta","269");
	require("6-iltaan","270");
	require("6-na","271");
	require("6-puoleiseksi tuuleksi","272");
	require("6-puoleista tuulta","273");
	require("6-suunnasta","274");
	require("6-suuntaan","275");
	require("6-tuuleksi","276");
	require("6-tuulta","277");
	require("6-vastaisena yönä","278");
	require("7-aamusta","279");
	require("7-aamuun","280");
	require("7-alkaen","281");
	require("7-illasta","282");
	require("7-iltaan","283");
	require("7-na","284");
	require("7-puoleiseksi tuuleksi","285");
	require("7-puoleista tuulta","286");
	require("7-suunnasta","287");
	require("7-suuntaan","288");
	require("7-tuuleksi","289");
	require("7-tuulta","290");
	require("7-vastaisena yönä","291");
	require("8-puoleiseksi tuuleksi","292");
	require("8-puoleista tuulta","293");
	require("8-suunnasta","294");
	require("8-suuntaan","295");
	require("8-tuuleksi","296");
	require("8-tuulta","297");
	require("aamulla","298");
	require("aamupäivällä","299");
	require("aamupäivästä alkaen","300");
	require("aamusta alkaen","301");
	require("aamuyöllä","302");
	require("aamuyöstä alkaen","303");
	require("ajoittain lumisadetta","304");
	require("ajoittain sadetta","305");
	require("ajoittain sateista","306");
	require("ajoittain ukkosta","307");
	require("aland:lle","308");
	require("alempi","309");
	require("alin suhteellinen kosteus","310");
	require("alkavan","311");
	require("alueen yli","312");
	require("aluksi","313");
	require("ankaran hallan todennäköisyys","314");
	require("astetta","315");
	require("asti","316");
	require("edelleen","317");
	require("enimmäkseen","318");
	require("ensi yönä","319");
	require("etela-karjala:lle","320");
	require("etela-pohjanmaa:lle","321");
	require("etela-savo:lle","322");
	require("hallan todennäköisyys","323");
	require("hame:lle","324");
	require("heikkenevää","325");
	require("heikkoa sadetta","326");
	require("heikkoa","327");
	require("heikoksi","328");
	require("hieman alempi","329");
	require("hieman korkeampi","330");
	require("hieman pienempi","331");
	require("hieman suurempi","332");
	require("hirmumyrskyksi","333");
	require("hirmumyrskyä","334");
	require("huomattavasti alempi","335");
	require("huomattavasti korkeampi","336");
	require("huomattavasti lämpimämpi","337");
	require("huomattavasti pienempi","338");
	require("huomattavasti suurempi","339");
	require("huomenna","340");
	require("huomisaamuna","341");
	require("illalla","342");
	require("illasta alkaen","343");
	require("iltapäivällä","344");
	require("iltapäivästä alkaen","345");
	require("iltayöllä","346");
	require("iltayöstä alkaen","347");
	require("kainuu:lle","348");
	require("kello","349");
	require("keski-pohjanmaa:lle","350");
	require("keski-suomi:lle","351");
	require("keskilämpötila","352");
	require("keskimääräinen alin lämpötila","353");
	require("keskimääräinen ylin lämpötila","354");
	require("keskipäivällä","355");
	require("keskipäivästä alkaen","356");
	require("keskiyöllä","357");
	require("keskiyöstä alkaen","358");
	require("kohtalaiseksi","359");
	require("kohtalaista","360");
	require("koillismaa:lle","361");
	require("korkeampi","362");
	require("korkeapaine","363");
	require("korkeapaineen alue","364");
	require("kosteammaksi","365");
	require("kovaa","366");
	require("kovaksi","367");
	require("kuivemmaksi","368");
	require("kymenlaakso:lle","369");
	require("laaja","370");
	require("lansi-lappi:lle","371");
	require("leviää","372");
	require("liikkuu","373");
	require("loittonee","374");
	require("lopuksi","375");
	require("lumi-","376");
	require("lumikuuroja","377");
	require("lumisadetta","378");
	require("lähestyy","379");
	require("lämpenee","380");
	require("lämpimämpi","381");
	require("lämpötila","382");
	require("m/s","383");
	require("mahdollisesti ukkosta","384");
	require("matalapaine","385");
	require("matalapaineen alue","386");
	require("melko selkeää","387");
	require("metriä sekunnissa","388");
	require("miinus","389");
	require("millimetriä","390");
	require("monin paikoin enemmän","391");
	require("monin paikoin","392");
	require("muuttuu","393");
	require("myrskyksi","394");
	require("myrskyä","395");
	require("navakaksi","396");
	require("navakkaa","397");
	require("noin","398");
	require("nyland:lle","399");
	require("odotettavissa","400");
	require("on","401");
	require("osterbotten:lle","402");
	require("paijat-hame:lle","403");
	require("paikoin enemmän","404");
	require("paikoin","405");
	require("pienempi","406");
	require("pilvistyvää","407");
	require("pilvistä","408");
	require("pirkanmaa:lle","409");
	require("pohjois-karjala:lle","410");
	require("pohjois-lappi:lle","411");
	require("pohjois-pohjanmaa:lle","412");
	require("pohjois-savo:lle","413");
	require("poutaa","414");
	require("puolipilvistä","415");
	require("päivien ylin lämpötila","416");
	require("päivällä","417");
	require("päivälämpötila","418");
	require("päivän ylin lämpötila","419");
	require("rannikolla","420");
	require("runsasta sadetta","421");
	require("räntä-","422");
	require("räntäsadetta","423");
	require("sadealue","424");
	require("sadesumma","425");
	require("sadetta","426");
	require("sakeaa lumisadetta","427");
	require("sakeita lumikuuroja","428");
	require("sama","429");
	require("satakunta:lle","430");
	require("selkenevää","431");
	require("selkeää","432");
	require("seuraava yö","433");
	require("seuraavana päivänä","434");
	require("seuraavana yönä","435");
	require("suunnaltaan vaihtelevaa","436");
	require("suunnilleen sama","437");
	require("suurempi","438");
	require("syvenee","439");
	require("sää on epävakaista","440");
	require("sää","441");
	require("sääennuste","442");
	require("tai","443");
	require("tuuli heikkenee","444");
	require("tuuli tyyntyy","445");
	require("tuuli voimistuu","446");
	require("tuuli","447");
	require("tuulta","448");
	require("tyyneksi","449");
	require("tyyntä","450");
	require("tänään","451");
	require("uusimaa:lle","452");
	require("vahvistuu","453");
	require("vaihtelevaa pilvisyyttä","454");
	require("varsinais-suomi:lle","455");
	require("vesi-","456");
	require("vesisadetta","457");
	require("viilenee","458");
	require("viiva","459");
	require("voimakkaita kuuroja","460");
	require("voimistuvaa","461");
	require("vähäisiä kuuroja","462");
	require("vähäistä lumisadetta","463");
	require("yli","464");
	require("ylihuomenna","465");
	require("yöllä","466");
	require("yölämpötila","467");
	require("yön alin lämpötila","468");
	require("yöstä alkaen","469");
	require("öiden alin lämpötila","470");
	require("prosenttia","471");
	require("ajoittain","472");
	require("1-na kello","473");
	require("2-na kello","474");
	require("3-na kello","475");
	require("4-na kello","476");
	require("5-na kello","477");
	require("6-na kello","478");
	require("7-na kello","479");
	require("ei suuria muutoksia säässä","480");
	require("sää selkenee","481");
	require("sää muuttuu epävakaisemmaksi","482");
	require("sateinen sää jatkuu","483");
	require("sää muuttuu sateisemmaksi","484");
	require("epävakainen sää jatkuu","485");
	require("sää lauhtuu","486");
	require("sää on enimmäkseen poutaista","487");
	require("sää poutaantuu","488");
	require("sää muuttuu helteiseksi","489");
	require("sää lämpenee","490");
	require("sää viilenee","491");
	require("sää muuttuu koleaksi","492");
	require("sää kylmenee","493");
	require("helteinen sää jatkuu","494");
	require("lämmin sää jatkuu","495");
	require("viileä sää jatkuu","496");
	require("kolea sää jatkuu","497");
	require("kylmä sää jatkuu","498");
	require("virtaa","499");
	require("tilapäisesti","500");
	require("lämmintä","501");
	require("kylmää","502");
	require("viileää","503");
	require("viileämpää","504");
	require("lämpimämpää","505");
	require("kylmempää","506");
	require("hyvin","507");
	require("vähän","508");
	require("ilmaa","509");
	require("Suomeen","510");
	require("maan","511");
	require("eteläosiin","512");
	require("keskiosiin","513");
	require("pohjoisosiin","514");
	require("itäosiin","515");
	require("länsiosiin","516");
	require("etelä-","517");
	require("keski-","518");
	require("pohjois-","519");
	require("itä-","520");
	require("länsi-","521");
	require("kuivempaa","522");
	require("kosteampaa","523");
	require("sadealueita","524");
	require("Suomen yli","525");
	require("Etelä-Suomen yli","526");
	require("Keski-Suomen yli","527");
	require("Pohjois-Suomen yli","528");
	require("Itä-Suomen yli","529");
	require("Länsi-Suomen yli","530");
	require("Lounais-Suomen yli","531");
	require("Kaakkois-Suomen yli","532");
	require("uusi","533");
	require("saapuu","534");
	require("alue","535");
	require("sateineen","536");
	require("sadealueineen","537");
	require("hajanaisine sadealueineen","538");
	require("sateen todennäköisyys","539");
	require("kuurosateen todennäköisyys","540");
	require("ukkosen todennäköisyys","541");
	require("kuuluu","542");
	require("pakkanen jatkuu","543");
	require("pakkanen kiristyy","544");
	require("pakkanen heikkenee","545");

	// extras:
	// pohjanmaa:lle == osterbotten::lle
	// ahvenanmaa:lle == aland::lle
	// o'clock = <empty>

	const int extras = 3;
	if(dict->size() != 545-200+1+extras)
	  TEST_FAILED("Sonera dictionary should have 349 phrases");
	
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
	  TEST(phrases);
	}
	
  }; // class tests
  
} // namespace SoneraTest


int main(void)
{
  cout << endl
	   << "SoneraPhrases requirements" << endl
	   << "==========================" << endl;
  
  SoneraTest::dict = TextGen::DictionaryFactory::create("mysql");
  SoneraTest::dict->init("sonera");
  
  SoneraTest::tests t;
  return t.run();
}
