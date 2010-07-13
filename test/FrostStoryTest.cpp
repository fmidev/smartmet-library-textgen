#include <regression/tframe.h>
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Paragraph.h"
#include "PlainTextFormatter.h"
#include "FrostStory.h"
#include "Story.h"
#include "MessageLogger.h"

#include <newbase/NFmiSettings.h>

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;


namespace FrostStoryTest
{
  shared_ptr<TextGen::Dictionary> dict;
  TextGen::PlainTextFormatter formatter;
  

  void require(const TextGen::Story & theStory,
			   const string & theLanguage,
			   const string & theName,
			   const string & theExpected)
  {
	dict->init(theLanguage);
	formatter.dictionary(dict);

	TextGen::Paragraph para = theStory.makeStory(theName);
	const string value = para.realize(formatter);

	if(value != theExpected)
	  {
		const string msg = value + " <> " + theExpected;
		TEST_FAILED(msg.c_str());
	  }
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test FrostStory::mean()
   */
  // ----------------------------------------------------------------------

  void frost_mean()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("25,60");
	NFmiTime time1(2000,1,1);
	NFmiTime time2(2000,1,2);
	WeatherPeriod period(time1,time2);
	FrostStory story(time1,sources,area,period,"mean");

	const string fun = "frost_mean";

	NFmiSettings::Set("mean::precision","10");
	NFmiSettings::Set("mean::frost_limit","20");
	NFmiSettings::Set("mean::severe_frost_limit","10");

	NFmiSettings::Set("mean::fake::mean","0,0");
	NFmiSettings::Set("mean::fake::severe_mean","0,0");
	require(story,"fi",fun,"");
	require(story,"sv",fun,"");
	require(story,"en",fun,"");

	NFmiSettings::Set("mean::fake::mean","10,0");
	NFmiSettings::Set("mean::fake::severe_mean","0,0");
	require(story,"fi",fun,"");
	require(story,"sv",fun,"");
	require(story,"en",fun,"");

	NFmiSettings::Set("mean::fake::mean","20,0");
	NFmiSettings::Set("mean::fake::severe_mean","0,0");
	require(story,"fi",fun,"Hallan todennäköisyys on 20%.");
	require(story,"sv",fun,"Sannolikheten för nattfrost är 20%.");
	require(story,"en",fun,"Probability of frost is 20%.");

	NFmiSettings::Set("mean::fake::mean","20,0");
	NFmiSettings::Set("mean::fake::severe_mean","10,0");
	require(story,"fi",fun,"Ankaran hallan todennäköisyys on 10%.");
	require(story,"sv",fun,"Sannolikheten för sträng nattfrost är 10%.");
	require(story,"en",fun,"Probability of severe frost is 10%.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test FrostStory::maximum()
   */
  // ----------------------------------------------------------------------

  void frost_maximum()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("25,60");
	NFmiTime time1(2000,1,1);
	NFmiTime time2(2000,1,2);
	WeatherPeriod period(time1,time2);
	FrostStory story(time1,sources,area,period,"maximum");

	const string fun = "frost_maximum";

	NFmiSettings::Set("maximum::precision","10");
	NFmiSettings::Set("maximum::frost_limit","20");
	NFmiSettings::Set("maximum::severe_frost_limit","10");

	NFmiSettings::Set("maximum::fake::maximum","0,0");
	NFmiSettings::Set("maximum::fake::severe_maximum","0,0");
	require(story,"fi",fun,"");
	require(story,"sv",fun,"");
	require(story,"en",fun,"");

	NFmiSettings::Set("maximum::fake::maximum","10,0");
	NFmiSettings::Set("maximum::fake::severe_maximum","0,0");
	require(story,"fi",fun,"");
	require(story,"sv",fun,"");
	require(story,"en",fun,"");

	NFmiSettings::Set("maximum::fake::maximum","20,0");
	NFmiSettings::Set("maximum::fake::severe_maximum","0,0");
	require(story,"fi",fun,"Hallan todennäköisyys on 20%.");
	require(story,"sv",fun,"Sannolikheten för nattfrost är 20%.");
	require(story,"en",fun,"Probability of frost is 20%.");

	NFmiSettings::Set("maximum::fake::maximum","20,0");
	NFmiSettings::Set("maximum::fake::severe_maximum","10,0");
	require(story,"fi",fun,"Ankaran hallan todennäköisyys on 10%.");
	require(story,"sv",fun,"Sannolikheten för sträng nattfrost är 10%.");
	require(story,"en",fun,"Probability of severe frost is 10%.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test FrostStory::range()
   */
  // ----------------------------------------------------------------------

  void frost_range()
  {
	using namespace std;
	using namespace TextGen;
	using namespace 
WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("25,60");
	NFmiTime time1(2000,1,1);
	NFmiTime time2(2000,1,2);
	WeatherPeriod period(time1,time2);
	FrostStory story(time1,sources,area,period,"range");

	const string fun = "frost_range";

	NFmiSettings::Set("range::precision","10");
	NFmiSettings::Set("range::frost_limit","50");
	NFmiSettings::Set("range::severe_frost_limit","30");

	NFmiSettings::Set("range::fake::minimum","0,0");
	NFmiSettings::Set("range::fake::maximum","0,0");
	NFmiSettings::Set("range::fake::severe_minimum","0,0");
	NFmiSettings::Set("range::fake::severe_maximum","0,0");
	require(story,"fi",fun,"");
	require(story,"sv",fun,"");
	require(story,"en",fun,"");

	NFmiSettings::Set("range::fake::minimum","50,0");
	NFmiSettings::Set("range::fake::maximum","50,0");
	NFmiSettings::Set("range::fake::severe_minimum","0,0");
	NFmiSettings::Set("range::fake::severe_maximum","0,0");
	require(story,"fi",fun,"Hallan todennäköisyys on 50%.");
	require(story,"sv",fun,"Sannolikheten för nattfrost är 50%.");
	require(story,"en",fun,"Probability of frost is 50%.");

	NFmiSettings::Set("range::fake::minimum","40,0");
	NFmiSettings::Set("range::fake::maximum","60,0");
	NFmiSettings::Set("range::fake::severe_minimum","0,0");
	NFmiSettings::Set("range::fake::severe_maximum","0,0");
	require(story,"fi",fun,"Hallan todennäköisyys on 40-60%.");
	require(story,"sv",fun,"Sannolikheten för nattfrost är 40-60%.");
	require(story,"en",fun,"Probability of frost is 40-60%.");

	NFmiSettings::Set("range::fake::minimum","50,0");
	NFmiSettings::Set("range::fake::maximum","70,0");
	NFmiSettings::Set("range::fake::severe_minimum","10,0");
	NFmiSettings::Set("range::fake::severe_maximum","20,0");
	require(story,"fi",fun,"Hallan todennäköisyys on 50-70%.");
	require(story,"sv",fun,"Sannolikheten för nattfrost är 50-70%.");
	require(story,"en",fun,"Probability of frost is 50-70%.");

	NFmiSettings::Set("range::fake::minimum","40,0");
	NFmiSettings::Set("range::fake::maximum","60,0");
	NFmiSettings::Set("range::fake::severe_minimum","30,0");
	NFmiSettings::Set("range::fake::severe_maximum","40,0");
	require(story,"fi",fun,"Ankaran hallan todennäköisyys on 30-40%.");
	require(story,"sv",fun,"Sannolikheten för sträng nattfrost är 30-40%.");
	require(story,"en",fun,"Probability of severe frost is 30-40%.");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test FrostStory::twonights()
   */
  // ----------------------------------------------------------------------

  void frost_twonights()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("25,60");

	const string fun = "frost_twonights";

	NFmiSettings::Set("twonights::precision","10");
	NFmiSettings::Set("twonights::frost_limit","20");
	NFmiSettings::Set("twonights::severe_frost_limit","10");

	NFmiSettings::Set("twonights::night::starthour","21");
	NFmiSettings::Set("twonights::night::endhour","09");

	// One night
	{
	  NFmiTime time1(2003,6,3,12,0);
	  NFmiTime time2(2003,6,4,12,0);
	  WeatherPeriod period(time1,time2);
	  FrostStory story(time1,sources,area,period,"twonights");

	  NFmiSettings::Set("twonights::fake::day1::mean","10,0");
	  NFmiSettings::Set("twonights::fake::day1::severe_mean","0,0");
	  require(story,"fi",fun,"");
	  require(story,"sv",fun,"");
	  require(story,"en",fun,"");

	  NFmiSettings::Set("twonights::fake::day1::mean","20,0");
	  NFmiSettings::Set("twonights::fake::day1::severe_mean","0,0");
	  require(story,"fi",fun,"Hallan todennäköisyys on keskiviikon vastaisena yönä 20%.");
	  require(story,"sv",fun,"Sannolikheten för nattfrost är natten mot onsdagen 20%.");
	  require(story,"en",fun,"Probability of frost is on Wednesday night 20%.");

	  NFmiSettings::Set("twonights::fake::day1::mean","80,0");
	  NFmiSettings::Set("twonights::fake::day1::severe_mean","20,0");
	  require(story,"fi",fun,"Ankaran hallan todennäköisyys on keskiviikon vastaisena yönä 20%.");
	  require(story,"sv",fun,"Sannolikheten för sträng nattfrost är natten mot onsdagen 20%.");
	  require(story,"en",fun,"Probability of severe frost is on Wednesday night 20%.");

	}

	// Two nights, nothing on second day
	{
	  NFmiTime time1(2003,6,3,12,0);
	  NFmiTime time2(2003,6,5,12,0);
	  WeatherPeriod period(time1,time2);
	  FrostStory story(time1,sources,area,period,"twonights");

	  NFmiSettings::Set("twonights::fake::day1::mean","10,0");
	  NFmiSettings::Set("twonights::fake::day1::severe_mean","0,0");
	  NFmiSettings::Set("twonights::fake::day2::mean","0,0");
	  NFmiSettings::Set("twonights::fake::day2::severe_mean","0,0");
	  require(story,"fi",fun,"");
	  require(story,"sv",fun,"");
	  require(story,"en",fun,"");

	  NFmiSettings::Set("twonights::fake::day1::mean","30,0");
	  NFmiSettings::Set("twonights::fake::day1::severe_mean","0,0");
	  NFmiSettings::Set("twonights::fake::day2::mean","30,0");
	  NFmiSettings::Set("twonights::fake::day2::severe_mean","0,0");
	  require(story,"fi",fun,"Hallan todennäköisyys on keskiviikon vastaisena yönä 30%, seuraavana yönä sama.");
	  require(story,"sv",fun,"Sannolikheten för nattfrost är natten mot onsdagen 30%, följande natt densamma.");
	  require(story,"en",fun,"Probability of frost is on Wednesday night 30%, the following night the same.");

	  NFmiSettings::Set("twonights::fake::day1::mean","30,0");
	  NFmiSettings::Set("twonights::fake::day1::severe_mean","0,0");
	  NFmiSettings::Set("twonights::fake::day2::mean","40,0");
	  NFmiSettings::Set("twonights::fake::day2::severe_mean","0,0");
	  require(story,"fi",fun,"Hallan todennäköisyys on keskiviikon vastaisena yönä 30%, seuraavana yönä 40%.");
	  require(story,"sv",fun,"Sannolikheten för nattfrost är natten mot onsdagen 30%, följande natt 40%.");
	  require(story,"en",fun,"Probability of frost is on Wednesday night 30%, the following night 40%.");

	  NFmiSettings::Set("twonights::fake::day1::mean","20,0");
	  NFmiSettings::Set("twonights::fake::day1::severe_mean","0,0");
	  NFmiSettings::Set("twonights::fake::day2::mean","0,0");
	  NFmiSettings::Set("twonights::fake::day2::severe_mean","0,0");
	  require(story,"fi",fun,"Hallan todennäköisyys on keskiviikon vastaisena yönä 20%, seuraava yö on lämpimämpi.");
	  require(story,"sv",fun,"Sannolikheten för nattfrost är natten mot onsdagen 20%, följande natt är varmare.");
	  require(story,"en",fun,"Probability of frost is on Wednesday night 20%, the following night is warmer.");

	  NFmiSettings::Set("twonights::fake::day1::mean","80,0");
	  NFmiSettings::Set("twonights::fake::day1::severe_mean","20,0");
	  NFmiSettings::Set("twonights::fake::day2::mean","0,0");
	  NFmiSettings::Set("twonights::fake::day2::severe_mean","0,0");
	  require(story,"fi",fun,"Ankaran hallan todennäköisyys on keskiviikon vastaisena yönä 20%, seuraava yö on huomattavasti lämpimämpi.");
	  require(story,"sv",fun,"Sannolikheten för sträng nattfrost är natten mot onsdagen 20%, följande natt är betydligt varmare.");
	  require(story,"en",fun,"Probability of severe frost is on Wednesday night 20%, the following night is significantly warmer.");

	}

	// First day frost
	{
	  NFmiTime time1(2003,6,3,12,0);
	  NFmiTime time2(2003,6,5,12,0);
	  WeatherPeriod period(time1,time2);
	  FrostStory story(time1,sources,area,period,"twonights");

	  NFmiSettings::Set("twonights::fake::day1::mean","20,0");
	  NFmiSettings::Set("twonights::fake::day1::severe_mean","0,0");

	  NFmiSettings::Set("twonights::fake::day2::mean","0,0");
	  NFmiSettings::Set("twonights::fake::day2::severe_mean","0,0");
	  require(story,"fi",fun,"Hallan todennäköisyys on keskiviikon vastaisena yönä 20%, seuraava yö on lämpimämpi.");
	  require(story,"sv",fun,"Sannolikheten för nattfrost är natten mot onsdagen 20%, följande natt är varmare.");
	  require(story,"en",fun,"Probability of frost is on Wednesday night 20%, the following night is warmer.");

	  NFmiSettings::Set("twonights::fake::day2::mean","30,0");
	  NFmiSettings::Set("twonights::fake::day2::severe_mean","00,0");
	  require(story,"fi",fun,"Hallan todennäköisyys on keskiviikon vastaisena yönä 20%, seuraavana yönä 30%.");
	  require(story,"sv",fun,"Sannolikheten för nattfrost är natten mot onsdagen 20%, följande natt 30%.");
	  require(story,"en",fun,"Probability of frost is on Wednesday night 20%, the following night 30%.");

	  NFmiSettings::Set("twonights::fake::day2::mean","80,0");
	  NFmiSettings::Set("twonights::fake::day2::severe_mean","20,0");
	  require(story,"fi",fun,"Hallan todennäköisyys on keskiviikon vastaisena yönä 20%, seuraavana yönä ankaran hallan todennäköisyys on 20%.");
	  require(story,"sv",fun,"Sannolikheten för nattfrost är natten mot onsdagen 20%, följande natt sannolikheten för sträng nattfrost är 20%.");
	  require(story,"en",fun,"Probability of frost is on Wednesday night 20%, the following night probability of severe frost is 20%.");

	}

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test FrostStory::day()
   */
  // ----------------------------------------------------------------------

  void frost_day()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("25,60");
	NFmiTime time1(2000,6,1,18);
	NFmiTime time2(2000,6,2,6);
	WeatherPeriod period(time1,time2);
	FrostStory story(time1,sources,area,period,"day");

	const string fun = "frost_day";

	NFmiSettings::Set("day::night::starthour","18");
	NFmiSettings::Set("day::night::endhour","6");

	NFmiSettings::Set("day::precision","10");
	NFmiSettings::Set("day::frost_limit","50");
	NFmiSettings::Set("day::severe_frost_limit","30");

	NFmiSettings::Set("day::fake::area::frost","0,0");
	NFmiSettings::Set("day::fake::area::severe_frost","0,0");
	NFmiSettings::Set("day::fake::coast::value","0,0");
	NFmiSettings::Set("day::fake::inland::value","0,0");
	require(story,"fi",fun,"");
	require(story,"sv",fun,"");
	require(story,"en",fun,"");

	NFmiSettings::Set("day::fake::area::frost","60,0");
	NFmiSettings::Set("day::fake::area::severe_frost","0,0");
	NFmiSettings::Set("day::fake::coast::value","60,0");
	NFmiSettings::Set("day::fake::inland::value","60,0");
	require(story,"fi",fun,"Hallan todennäköisyys on 60%.");
	require(story,"sv",fun,"Sannolikheten för nattfrost är 60%.");
	require(story,"en",fun,"Probability of frost is 60%.");

	NFmiSettings::Set("day::fake::area::frost","60,0");
	NFmiSettings::Set("day::fake::area::severe_frost","40,0");
	NFmiSettings::Set("day::fake::coast::value","40,0");
	NFmiSettings::Set("day::fake::inland::value","40,0");
	require(story,"fi",fun,"Ankaran hallan todennäköisyys on 40%.");
	require(story,"sv",fun,"Sannolikheten för sträng nattfrost är 40%.");
	require(story,"en",fun,"Probability of severe frost is 40%.");

	NFmiSettings::Set("day::fake::area::frost","60,0");
	NFmiSettings::Set("day::fake::area::severe_frost","40,0");
	NFmiSettings::Set("day::fake::coast::value","10,0");
	NFmiSettings::Set("day::fake::inland::value","60,0");
	require(story,"fi",fun,"Ankaran hallan todennäköisyys on 60%, rannikolla 10%.");
	require(story,"sv",fun,"Sannolikheten för sträng nattfrost är 60%, vid kusten 10%.");
	require(story,"en",fun,"Probability of severe frost is 60%, on the coastal area 10%.");

	NFmiSettings::Set("day::fake::area::frost","60,0");
	NFmiSettings::Set("day::fake::area::severe_frost","90,0");
	NFmiSettings::Set("day::fake::coast::value","90,0");
	NFmiSettings::Set("day::fake::inland::value","90,0");
	require(story,"fi",fun,"");
	require(story,"sv",fun,"");
	require(story,"en",fun,"");


	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test FrostStory::onenight()
   */
  // ----------------------------------------------------------------------
	struct OneNightFrostTestParam 
	{
	  OneNightFrostTestParam(const char* tscoastal,
							 const char* tsinland,
							 const char* ngfrocoastal,
							 const char* ngfroinland,
							 const char* frocoastal,
							 const char* froinland,
							 const char* svfrocoastal,
							 const char* svfroinland,
							 const char* story)
	  {
		temperaturesumcoastal = tscoastal;
		temperaturesuminland = tsinland;
		nightfrostcoastal = ngfrocoastal;
		nightfrostinland = ngfroinland;
		frostcoastal = frocoastal;
		frostinland = froinland;
		severefrostcoastal = svfrocoastal;
		severefrostinland = svfroinland;
		froststory = story;
	  }

	  const char* temperaturesumcoastal;
	  const char* temperaturesuminland;
	  const char* nightfrostcoastal;
	  const char* nightfrostinland;
	  const char* frostcoastal;
	  const char* frostinland;
	  const char* severefrostcoastal;
	  const char* severefrostinland;
	  const char* froststory;
	};

	typedef std::map<int, OneNightFrostTestParam> FrostOneNightTestCases;

  const char* translation(const char* phrase, const string& language)
  {
	const char* retval = "";

	if(language == "fi" || strlen(phrase) == 0)
	  {
		retval = phrase;
	  }
	else if(language == "sv")
	  {
	  }
	else if(language == "en")
	  {
	  }

	return retval;
  }

  void create_testcases(FrostOneNightTestCases& testCases, const string& language)
  {
	int i = 1;


	// no areas included
	testCases.insert(make_pair(i++, OneNightFrostTestParam("32700,0","32700,0",
														   "", "",
														   "", "",
														   "", "",
														   translation("", language))));
	
	// inland area included, but growing season has not yet started
	// cell 1,1
	testCases.insert(make_pair(i++, OneNightFrostTestParam("32700,0", "15.0,0",
														   "", "",
														   "", "",
														   "", "",
														   translation("", language))));
	
	// growing season has started only on 10% of coastal and inland area
	// growing season has not yet started
	// cell 1,1
	testCases.insert(make_pair(i++, OneNightFrostTestParam("10.0,0", "10.0,0",
														   "", "",
														   "", "",
														   "", "",
														   translation("", language))));
	
	// growing season has started on coastal area, but not on inland area
	// probability for frost at coastal area is 5.0%
	// cell 2,1
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "10,0", 
														   "0,0", "", 
														   "5.0,0", "", 
														   "", "", 
														   translation("", language))));
	
	// growing season has started on coastal area, but not on inland area
	// probability for frost at coastal area is 9.9%
	// cell 2,1
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "10,0", 
														   "0,0", "", 
														   "9.9,0", "", 
														   "", "", 
														   translation("", language))));

	// growing season has started on coastal area, but not on inland area
	// probability for frost at coastal area is 10.0%
	// cell 2,1
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "10,0", 
														   "0,0", "", 
														   "10.0,0", "", 
														   "", "", 
														   translation("Rannikolla mahdollisesti hallaa.", language))));

	// growing season has started on coastal area, but not on inland area
	// probability for frost at coastal area is 15.0%
	// cell 3,1
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "10,0", 
														   "0,0", "", 
														   "15.0,0", "", 
														   "", "", 
														   translation("Rannikolla mahdollisesti hallaa.", language))));

	// growing season has started on coastal area, but not on inland area
	// probability for frost at coastal area is 24.9%
	// cell 3,1
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "10,0", 
														   "0,0", "", 
														   "24.9,0", "", 
														   "", "", 
														   translation("Rannikolla mahdollisesti hallaa.", language))));

	// growing season has started on coastal area, but not on inland area
	// probability for frost at coastal area is 25.0%
	// cell 4,1
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "10,0", 
														   "0,0", "", 
														   "25.0,0", "", 
														   "", "", 
														   translation("Rannikolla mahdollisesti hallaa.", language))));

	// growing season has started on coastal area, but not on inland area
	// probability for frost at coastal area is 35.0%
	// cell 4,1
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "10,0", 
														   "0,0", "", 
														   "35.0,0", "", 
														   "", "", 
														   translation("Rannikolla mahdollisesti hallaa.", language))));

	// growing season has started on coastal area, but not on inland area
	// probability for frost at coastal area is 44.9%
	// cell 4,1
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "10,0", 
														   "0,0", "", 
														   "44.9,0", "", 
														   "", "", 
														   translation("Rannikolla mahdollisesti hallaa.", language))));

	// growing season has started on coastal area, but not on inland area
	// probability for frost at coastal area is 45.0%
	// cell 5,1
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "10,0", 
														   "0,0", "", 
														   "45.0,0", "", 
														   "", "", 
														   translation("Rannikolla paikoin hallaa.", language))));

	// growing season has started on coastal area, but not on inland area
	// probability for frost at coastal area is 55.0%
	// cell 5,1
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "10,0", 
														   "0,0", "", 
														   "55.0,0", "", 
														   "", "", 
														   translation("Rannikolla paikoin hallaa.", language))));

	// growing season has started on coastal area, but not on inland area
	// probability for frost at coastal area is 64.9%
	// cell 5,1
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "10,0", 
														   "0,0", "", 
														   "64.9,0", "", 
														   "", "", 
														   translation("Rannikolla paikoin hallaa.", language))));

	// growing season has started on coastal area, but not on inland area
	// probability for frost at coastal area is 65.0%
	// cell 6,1
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "10,0", 
														   "0,0", "", 
														   "65.0,0", "", 
														   "", "", 
														   translation("Rannikolla monin paikoin hallaa.", language))));

	// growing season has started on coastal area, but not on inland area
	// probability for frost at coastal area is 75.0%
	// probability for severe frost is 15%
	// cell 6,1
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "10,0", 
														   "0,0", "", 
														   "75.0,0", "", 
														   "15.0,0", "", 
														   translation("Rannikolla monin paikoin hallaa.", language))));

	// growing season has started on coastal area, but not on inland area
	// probability for frost at coastal area is 84.9%
	// probability for severe frost is 20.0%
	// cell 6,1
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "10,0", 
														   "0,0", "", 
														   "84.9,0", "", 
														   "20.0,0", "", 
														   translation("Rannikolla monin paikoin hallaa.", language))));

	// growing season has started on coastal area, but not on inland area
	// probability for frost at coastal area is 85.0%
	// probability for severe frost is 20.1%
	// cell 7,1
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "10,0", 
														   "0,0", "", 
														   "85.0,0", "", 
														   "20.1,0", "", 
														   translation("Rannikolla hallaa, joka voi olla ankaraa.", language))));

	// growing season has started on coastal area, but not on inland area
	// probability for frost at coastal area is 95.0%
	// probability for severe frost is 45.0%
	// cell 7,1
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "10,0", 
														   "0,0", "", 
														   "95.0,0", "", 
														   "45.0,0", "", 
														   translation("Rannikolla hallaa, joka voi olla ankaraa.", language))));

	// growing season has started on coastal area, but not on inland area
	// probability for frost at coastal area is 99.9%
	// probability for severe frost is 75.0%
	// cell 7,1
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "10,0", 
														   "0,0", "", 
														   "99.9,0", "", 
														   "75.0,0", "", 
														   translation("Rannikolla hallaa, joka voi olla ankaraa.", language))));

	// growing season has started in exactly 1/3 of the coastal area
	// night frost exists 25% of the coastal area
	// cell 8,1
	testCases.insert(make_pair(i++, OneNightFrostTestParam("33.333,0","32700,0", 
														   "25.0,0","", 
														   "", "",
														   "", "",
														   translation("", language))));

	// growing season has started only on 10% of coastal area
	// growing season has started on 35% of inland area
	// probability for frost at inland area is 1.0%
	// cell 1,2
	testCases.insert(make_pair(i++, OneNightFrostTestParam("10.0,0", "35.0,0",
														   "", "0,0",
														   "", "1.0,0",
														   "", "",
														   translation("", language))));

	// growing season has started on both areas
	// probability for frost at coastal and inland area is 5.0%
	// cell 2,2
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "95.0,0", 
														   "0,0", "0,0", 
														   "5.0,0", "5.0,0", 
														   "", "", 
														   translation("", language))));

	// growing season has started
	// probability for frost at coastal area is 9.9%
	// probability for frost at inland area is 5.0%
	// cell 2,2
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "95.0,0", 
														   "0,0", "0,0", 
														   "9.9,0", "5.0,0", 
														   "", "", 
														   translation("", language))));

	// growing season has started
	// probability for frost at coastal area is 10.0%
	// probability for frost at inland area is 5.0%
	// cell 3,2
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "10.0,0", "5.0,0", 
														   "", "", 
														   translation("Alavilla mailla hallan vaara.", language))));

	// growing season has started
	// probability for frost at coastal area is 15.0%
	// probability for frost at inland area is 5.0%
	// cell 3,2
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "15.0,0", "5.0,0", 
														   "", "", 
														   translation("Alavilla mailla hallan vaara.", language))));

	// growing season has started
	// probability for frost at coastal area is 24.9%
	// probability for frost at inland area is 5.0%
	// cell 3,2
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "24.9,0", "5.0,0", 
														   "", "", 
														   translation("Alavilla mailla hallan vaara.", language))));

	// growing season has started
	// probability for frost at coastal area is 25.0%
	// probability for frost at inland area is 5.0%
	// cell 4,2
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "25.0,0", "5.0,0", 
														   "", "", 
														   translation("Rannikolla mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 35.0%
	// probability for frost at inland area is 5.0%
	// cell 4,2
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "35.0,0", "5.0,0", 
														   "", "", 
														   translation("Rannikolla mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 44.9%
	// probability for frost at inland area is 5.0%
	// cell 4,2
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "44.9,0", "5.0,0", 
														   "", "", 
														   translation("Rannikolla mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 45.0%
	// probability for frost at inland area is 5.0%
	// cell 5,2
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "45.0,0", "5.0,0", 
														   "", "", 
														   translation("Rannikolla paikoin hallaa.", language))));
															
	// growing season has started
	// probability for frost at coastal and area is 55.0%
	// probability for frost at inland area is 5.0%
	// cell 5,2
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "55.0,0", "5.0,0", 
														   "", "", 
														   translation("Rannikolla paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 64.9%
	// probability for frost at inland area is 5.0%
	// cell 5,2
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "64.9,0", "5.0,0", 
														   "", "", 
														   translation("Rannikolla paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 65.0%
	// probability for frost at inland area is 5.0%
	// cell 6,2
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "65.0,0", "5.0,0", 
														   "", "", 
														   translation("Rannikolla monin paikoin hallaa.", language))));
															
	// growing season has started
	// probability for frost at coastal and area is 75.0%
	// probability for frost at inland area is 5.0%
	// probability for severe frost at coastal area is 5.0%
	// cell 6,2
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "75.0,0", "5.0,0", 
														   "5.0,0", "", 
														   translation("Rannikolla monin paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 84.9%
	// probability for frost at inland area is 5.0%
	// probability for severe frost at coastal area is 20.1%
	// cell 6,2
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "84.9,0", "5.0,0", 
														   "20.1,0", "", 
														   translation("Rannikolla monin paikoin hallaa, joka voi olla ankaraa.", language))));

	// growing season has started
	// probability for frost at coastal area is 85.0%
	// probability for frost at inland area is 5.0%
	// probability for severe frost at coastal area is 20.0%
	// cell 7,2
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "85.0,0", "5.0,0", 
														   "20.0,0", "", 
														   translation("Rannikolla hallaa.", language))));
															
	// growing season has started
	// probability for frost at coastal and area is 95.0%
	// probability for frost at inland area is 5.0%
	// probability for severe frost at coastal area is 20.1%
	// cell 7,2
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "95.0,0", "5.0,0", 
														   "20.1,0", "", 
														   translation("Rannikolla hallaa, joka voi olla ankaraa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 99.9%
	// probability for frost at inland area is 5.0%
	// probability for severe frost at coastal area is 65.0%
	// cell 7,2
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "99.9,0", "5.0,0", 
														   "65.0,0", "", 
														   translation("Rannikolla hallaa, joka voi olla ankaraa.", language))));

	// growing season has started
	// night frost exists 25% of the coastal area
	// probability for frost at inland area is 5.0%
	// cell 8,2
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0",  
														   "25.0,0","0,0", 
														   "", "5.0,0",
														   "", "",
														   translation("", language))));


	// growing season has started only on 10% of coastal area
	// growing season has started on 35% of inland area
	// probability for frost at inland area is 15.0%
	// cell 1,3
	testCases.insert(make_pair(i++, OneNightFrostTestParam("10.0,0", "35.0,0",
														   "", "0,0",
														   "", "15.0,0",
														   "", "",
														   translation("Alavilla mailla hallan vaara.", language))));


	// growing season has started on both areas
	// probability for frost at coastal and inland area is 15.0%
	// cell 2,3
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "95.0,0", 
														   "0,0", "0,0", 
														   "5.0,0", "15.0,0", 
														   "", "", 
														   translation("Alavilla mailla hallan vaara.", language))));

	// growing season has started
	// probability for frost at coastal area is 9.9%
	// probability for frost at inland area is 15.0%
	// cell 2,3
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "95.0,0", 
														   "0,0", "0,0", 
														   "9.9,0", "15.0,0", 
														   "", "", 
														   translation("Alavilla mailla hallan vaara.", language))));
	// growing season has started
	// probability for frost at coastal area is 10.0%
	// probability for frost at inland area is 15.0%
	// cell 3,3
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "10.0,0", "15.0,0", 
														   "", "", 
														   translation("Alavilla mailla hallan vaara.", language))));

	// growing season has started
	// probability for frost at coastal area is 15.0%
	// probability for frost at inland area is 15.0%
	// cell 3,3
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "15.0,0", "15.0,0", 
														   "", "", 
														   translation("Alavilla mailla hallan vaara.", language))));

	// growing season has started
	// probability for frost at coastal area is 24.9%
	// probability for frost at inland area is 15.0%
	// cell 3,3
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "24.9,0", "15.0,0", 
														   "", "", 
														   translation("Alavilla mailla hallan vaara.", language))));

	// growing season has started
	// probability for frost at coastal area is 25.0%
	// probability for frost at inland area is 15.0%
	// cell 4,3
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "25.0,0", "15.0,0", 
														   "", "", 
														   translation("Mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 35.0%
	// probability for frost at inland area is 15.0%
	// cell 4,3
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "35.0,0", "15.0,0", 
														   "", "", 
														   translation("Mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 44.9%
	// probability for frost at inland area is 15.0%
	// cell 4,3
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "44.9,0", "15.0,0", 
														   "", "", 
														   translation("Mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 45.0%
	// probability for frost at inland area is 15.0%
	// cell 5,3
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "45.0,0", "15.0,0", 
														   "", "", 
														   translation("Paikoin hallaa.", language))));
															
	// growing season has started
	// probability for frost at coastal and area is 55.0%
	// probability for frost at inland area is 15.0%
	// cell 5,3
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "55.0,0", "15.0,0", 
														   "", "", 
														   translation("Paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 64.9%
	// probability for frost at inland area is 15.0%
	// cell 5,3
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "64.9,0", "15.0,0", 
														   "", "", 
														   translation("Paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 65.0%
	// probability for frost at inland area is 15.0%
	// cell 6,3
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "65.0,0", "15.0,0", 
														   "", "", 
														   translation("Sisämaassa alavilla mailla hallan vaara, rannikolla monin paikoin hallaa.", language))));
															
	// growing season has started
	// probability for frost at coastal and area is 75.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at coastal area is 15.0%
	// cell 6,3
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "75.0,0", "15.0,0", 
														   "5.0,0", "", 
														   translation("Sisämaassa alavilla mailla hallan vaara, rannikolla monin paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 84.9%
	// probability for frost at inland area is 15.0%
	// probability for severe frost at coastal area is 20.1%
	// cell 6,3
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "84.9,0", "15.0,0", 
														   "20.1,0", "", 
														   translation("Sisämaassa alavilla mailla hallan vaara, rannikolla monin paikoin hallaa, joka voi olla ankaraa.", language))));

	// growing season has started
	// probability for frost at coastal area is 85.0%
	// probability for frost at inland area is 15.0%
	// probability for severe frost at coastal area is 20.0%
	// cell 7,3
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "85.0,0", "15.0,0", 
														   "20.0,0", "", 
														   translation("Sisämaassa alavilla mailla hallan vaara, rannikolla hallaa.", language))));
															
	// growing season has started
	// probability for frost at coastal and area is 95.0%
	// probability for frost at inland area is 15.0%
	// probability for severe frost at coastal area is 20.1%
	// cell 7,3
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "95.0,0", "15.0,0", 
														   "20.1,0", "", 
														   translation("Sisämaassa alavilla mailla hallan vaara, rannikolla hallaa, joka voi olla ankaraa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 99.9%
	// probability for frost at inland area is 15.0%
	// probability for severe frost at coastal area is 65.0%
	// cell 7,3
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "99.9,0", "15.0,0", 
														   "65.0,0", "", 
														   translation("Sisämaassa alavilla mailla hallan vaara, rannikolla hallaa, joka voi olla ankaraa.", language))));

	// growing season has started
	// probability for frost at inland area is 15.0%
	// night frost exists 25% of the coastal area
	// cell 8,3
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0",  
														   "25.0,0","0,0", 
														   "", "15.0,0",
														   "", "",
														   translation("", language))));

	// growing season has started only on 10% of coastal area
	// growing season has started on 35% of inland area
	// probability for frost at inland area is 35.0%
	// cell 1,4
	testCases.insert(make_pair(i++, OneNightFrostTestParam("10.0,0", "35.0,0",
														   "", "0,0",
														   "", "35.0,0",
														   "", "",
														   translation("Mahdollisesti hallaa.", language))));

	// growing season has started on both areas
	// probability for frost at coastal area is 5.0%
	// probability for frost at inland area is 35.0%
	// cell 2,4
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "95.0,0", 
														   "0,0", "0,0", 
														   "5.0,0", "35.0,0", 
														   "", "", 
														   translation("Mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 9.9%
	// probability for frost at inland area is 35.0%
	// cell 2,4
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "95.0,0", 
														   "0,0", "0,0", 
														   "9.9,0", "35.0,0", 
														   "", "", 
														   translation("Mahdollisesti hallaa.", language))));
	// growing season has started
	// probability for frost at coastal area is 10.0%
	// probability for frost at inland area is 35.0%
	// cell 3,4
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "10.0,0", "35.0,0", 
														   "", "", 
														   translation("Mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 15.0%
	// probability for frost at inland area is 35.0%
	// cell 3,4
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "15.0,0", "35.0,0", 
														   "", "", 
														   translation("Mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 24.9%
	// probability for frost at inland area is 35.0%
	// cell 3,4
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "24.9,0", "35.0,0", 
														   "", "", 
														   translation("Mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 25.0%
	// probability for frost at inland area is 35.0%
	// cell 4,4
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "25.0,0", "35.0,0", 
														   "", "", 
														   translation("Mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 35.0%
	// probability for frost at inland area is 35.0%
	// cell 4,4
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "35.0,0", "35.0,0", 
														   "", "", 
														   translation("Mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 44.9%
	// probability for frost at inland area is 35.0%
	// cell 4,4
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "44.9,0", "35.0,0", 
														   "", "", 
														   translation("Mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 45.0%
	// probability for frost at inland area is 35.0%
	// cell 5,4
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "45.0,0", "35.0,0", 
														   "", "", 
														   translation("Paikoin hallaa.", language))));
															
	// growing season has started
	// probability for frost at coastal and area is 55.0%
	// probability for frost at inland area is 35.0%
	// cell 5,4
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "55.0,0", "35.0,0", 
														   "", "", 
														   translation("Paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 64.9%
	// probability for frost at inland area is 35.0%
	// cell 5,4
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "64.9,0", "35.0,0", 
														   "", "", 
														   translation("Paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 65.0%
	// probability for frost at inland area is 35.0%
	// cell 6,4
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "65.0,0", "35.0,0", 
														   "", "", 
														   translation("Rannikolla monin paikoin hallaa, sisämaassa mahdollisesti hallaa.", language))));
															
	// growing season has started
	// probability for frost at coastal and area is 75.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at coastal area is 5.0%
	// cell 6,4
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "75.0,0", "35.0,0", 
														   "5.0,0", "", 
														   translation("Rannikolla monin paikoin hallaa, sisämaassa mahdollisesti hallaa.", language))));


	// growing season has started
	// probability for frost at coastal and area is 84.9%
	// probability for frost at inland area is 35.0%
	// probability for severe frost at coastal area is 20.1%
	// cell 6,4
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "84.9,0", "35.0,0", 
														   "20.1,0", "", 
														   translation("Rannikolla monin paikoin hallaa, joka voi olla ankaraa, sisämaassa mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 85.0%
	// probability for frost at inland area is 35.0%
	// probability for severe frost at coastal area is 20.0%
	// cell 7,4
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "85.0,0", "35.0,0", 
														   "20.0,0", "", 
														   translation("Rannikolla hallaa, sisämaassa mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 95.0%
	// probability for frost at inland area is 35.0%
	// probability for severe frost at coastal area is 20.1%
	// cell 7,4
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "95.0,0", "35.0,0", 
														   "20.1,0", "", 
														   translation("Rannikolla hallaa, joka voi olla ankaraa, sisämaassa mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 99.9%
	// probability for frost at inland area is 35.0%
	// probability for severe frost at coastal area is 65.0%
	// cell 7,4
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "99.9,0", "35.0,0", 
														   "65.0,0", "", 
														   translation("Rannikolla hallaa, joka voi olla ankaraa, sisämaassa mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at inland area is 35.0%
	// night frost exists 25% of the coastal area
	// cell 8,4
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0",  
														   "25.0,0","0,0", 
														   "", "35.0,0",
														   "", "",
														   translation("", language))));

	// growing season has started only on 10% of coastal area
	// growing season has started on 35% of inland area
	// probability for frost at inland area is 55.0%
	// cell 1,5
	testCases.insert(make_pair(i++, OneNightFrostTestParam("10.0,0", "35.0,0",
														   "", "0,0",
														   "", "55.0,0",
														   "", "",
														   translation("Paikoin hallaa.", language))));

	// growing season has started on both areas
	// probability for frost at coastal area is 5.0%
	// probability for frost at inland area is 55.0%
	// cell 2,5
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "95.0,0", 
														   "0,0", "0,0", 
														   "5.0,0", "55.0,0", 
														   "", "", 
														   translation("Sisämaassa paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 9.9%
	// probability for frost at inland area is 55.0%
	// cell 2,5
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "95.0,0", 
														   "0,0", "0,0", 
														   "9.9,0", "55.0,0", 
														   "", "", 
														   translation("Sisämaassa paikoin hallaa.", language))));
	// growing season has started
	// probability for frost at coastal area is 10.0%
	// probability for frost at inland area is 55.0%
	// cell 3,5
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "10.0,0", "55.0,0", 
														   "", "", 
														   translation("Paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 15.0%
	// probability for frost at inland area is 55.0%
	// cell 3,5
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "15.0,0", "55.0,0", 
														   "", "", 
														   translation("Paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 24.9%
	// probability for frost at inland area is 55.0%
	// cell 3,5
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "24.9,0", "55.0,0", 
														   "", "", 
														   translation("Paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 25.0%
	// probability for frost at inland area is 55.0%
	// cell 4,5
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "25.0,0", "55.0,0", 
														   "", "", 
														   translation("Paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 35.0%
	// probability for frost at inland area is 55.0%
	// cell 4,5
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "35.0,0", "55.0,0", 
														   "", "", 
														   translation("Paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 44.9%
	// probability for frost at inland area is 55.0%
	// cell 4,5
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "44.9,0", "55.0,0", 
														   "", "", 
														   translation("Paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 45.0%
	// probability for frost at inland area is 55.0%
	// cell 5,5
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "45.0,0", "55.0,0", 
														   "", "", 
														   translation("Paikoin hallaa.", language))));
															
	// growing season has started
	// probability for frost at coastal and area is 55.0%
	// probability for frost at inland area is 55.0%
	// cell 5,5
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "55.0,0", "55.0,0", 
														   "", "", 
														   translation("Paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 64.9%
	// probability for frost at inland area is 55.0%
	// cell 5,5
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "64.9,0", "55.0,0", 
														   "", "", 
														   translation("Paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 65.0%
	// probability for frost at inland area is 55.0%
	// cell 6,5
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "65.0,0", "55.0,0", 
														   "", "", 
														   translation("Paikoin hallaa.", language))));
															
	// growing season has started
	// probability for frost at coastal and area is 75.0%
	// probability for frost at inland area is 55.0%
	// probability for severe frost at coastal area is 5.0%
	// cell 6,5
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "75.0,0", "55.0,0", 
														   "5.0,0", "", 
														   translation("Paikoin hallaa.", language))));


	// growing season has started
	// probability for frost at coastal and area is 84.9%
	// probability for frost at inland area is 55.0%
	// probability for severe frost at coastal area is 20.1%
	// cell 6,5
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "84.9,0", "55.0,0", 
														   "20.1,0", "", 
														   translation("Paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 85.0%
	// probability for frost at inland area is 55.0%
	// probability for severe frost at coastal area is 20.0%
	// cell 7,5
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "85.0,0", "55.0,0", 
														   "20.0,0", "", 
														   translation("Rannikolla hallaa, sisämaassa paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 95.0%
	// probability for frost at inland area is 55.0%
	// probability for severe frost at coastal area is 20.1%
	// cell 7,5
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "95.0,0", "55.0,0", 
														   "20.1,0", "", 
														   translation("Rannikolla hallaa, joka voi olla ankaraa, sisämaassa paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 99.9%
	// probability for frost at inland area is 55.0%
	// probability for severe frost at coastal area is 65.0%
	// cell 7,5
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "99.9,0", "55.0,0", 
														   "65.0,0", "", 
														   translation("Rannikolla hallaa, joka voi olla ankaraa, sisämaassa paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at inland area is 55.0%
	// night frost exists 25% of the coastal area
	// cell 8,5
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0",  
														   "25.0,0","0,0", 
														   "", "55.0,0",
														   "", "",
														   translation("", language))));

	// growing season has started only on 10% of coastal area
	// growing season has started on 35% of inland area
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 1,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("10.0,0", "35.0,0",
														   "", "0,0",
														   "", "75.0,0",
														   "", "5.0,0",
														   translation("Monin paikoin hallaa.", language))));

	// growing season has started on both areas
	// probability for frost at coastal area is 5.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 2,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "95.0,0", 
														   "0,0", "0,0", 
														   "5.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 9.9%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 2,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "95.0,0", 
														   "0,0", "0,0", 
														   "9.9,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa.", language))));
	// growing season has started
	// probability for frost at coastal area is 10.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 3,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "10.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 15.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 3,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "15.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 24.9%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 3,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "24.9,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 25.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 4,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "25.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa, rannikolla mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 35.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 4,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "35.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa, rannikolla mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 44.9%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 4,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "44.9,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa, rannikolla mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 45.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 5,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "45.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Monin paikoin hallaa.", language))));
															
	// growing season has started
	// probability for frost at coastal and area is 55.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 5,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "55.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Monin paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 64.9%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 5,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "64.9,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Monin paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 65.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 6,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "65.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Monin paikoin hallaa.", language))));
															
	// growing season has started
	// probability for frost at coastal and area is 75.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at coastal area is 5.0%
	// probability for severe frost at inland area is 5.0%
	// cell 6,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "75.0,0", "75.0,0", 
														   "5.0,0", "5.0,0", 
														   translation("Monin paikoin hallaa.", language))));


	// growing season has started
	// probability for frost at coastal and area is 84.9%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at coastal area is 20.1%
	// probability for severe frost at inland area is 5.0%
	// cell 6,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "84.9,0", "75.0,0", 
														   "20.1,0", "5.0,0", 
														   translation("Monin paikoin hallaa, joka voi olla ankaraa.", language))));

	// growing season has started
	// probability for frost at coastal area is 85.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at coastal area is 20.0%
	// probability for severe frost at inland area is 5.0%
	// cell 7,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "85.0,0", "75.0,0", 
														   "20.0,0", "5.0,0", 
														   translation("Hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 95.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at coastal area is 20.1%
	// probability for severe frost at inland area is 5.0%
	// cell 7,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "95.0,0", "75.0,0", 
														   "20.1,0", "5.0,0", 
														   translation("Hallaa, joka voi olla ankaraa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 99.9%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at coastal area is 65.0%
	// probability for severe frost at inland area is 5.0%
	// cell 7,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "99.9,0", "75.0,0", 
														   "65.0,0", "5.0,0", 
														   translation("Hallaa, joka voi olla ankaraa.", language))));

	// growing season has started
	// night frost exists 25% of the coastal area
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 8,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0",  
														   "25.0,0","0,0", 
														   "", "75.0,0",
														   "", "5.0,0",
														   translation("", language))));






	// growing season has started only on 10% of coastal area
	// growing season has started on 35% of inland area
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 1,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("10.0,0", "35.0,0",
														   "", "0,0",
														   "", "75.0,0",
														   "", "5.0,0",
														   translation("Monin paikoin hallaa.", language))));

	// growing season has started on both areas
	// probability for frost at coastal area is 5.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 2,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "95.0,0", 
														   "0,0", "0,0", 
														   "5.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 9.9%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 2,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "95.0,0", 
														   "0,0", "0,0", 
														   "9.9,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa.", language))));
	// growing season has started
	// probability for frost at coastal area is 10.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 3,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "10.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 15.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 3,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "15.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 24.9%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 3,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "24.9,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 25.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 4,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "25.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa, rannikolla mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 35.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 4,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "35.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa, rannikolla mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 44.9%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 4,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "44.9,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa, rannikolla mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 45.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 5,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "45.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Monin paikoin hallaa.", language))));
															
	// growing season has started
	// probability for frost at coastal and area is 55.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 5,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "55.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Monin paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 64.9%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 5,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "64.9,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Monin paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 65.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 6,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "65.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Monin paikoin hallaa.", language))));
															
	// growing season has started
	// probability for frost at coastal and area is 75.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at coastal area is 5.0%
	// probability for severe frost at inland area is 5.0%
	// cell 6,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "75.0,0", "75.0,0", 
														   "5.0,0", "5.0,0", 
														   translation("Monin paikoin hallaa.", language))));


	// growing season has started
	// probability for frost at coastal and area is 84.9%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at coastal area is 20.1%
	// probability for severe frost at inland area is 5.0%
	// cell 6,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "84.9,0", "75.0,0", 
														   "20.1,0", "5.0,0", 
														   translation("Monin paikoin hallaa, joka voi olla ankaraa.", language))));

	// growing season has started
	// probability for frost at coastal area is 85.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at coastal area is 20.0%
	// probability for severe frost at inland area is 5.0%
	// cell 7,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "85.0,0", "75.0,0", 
														   "20.0,0", "5.0,0", 
														   translation("Hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 95.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at coastal area is 20.1%
	// probability for severe frost at inland area is 5.0%
	// cell 7,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "95.0,0", "75.0,0", 
														   "20.1,0", "5.0,0", 
														   translation("Hallaa, joka voi olla ankaraa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 99.9%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at coastal area is 65.0%
	// probability for severe frost at inland area is 5.0%
	// cell 7,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "99.9,0", "75.0,0", 
														   "65.0,0", "5.0,0", 
														   translation("Hallaa, joka voi olla ankaraa.", language))));

	// growing season has started
	// night frost exists 25% of the coastal area
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 8,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0",  
														   "25.0,0","0,0", 
														   "", "75.0,0",
														   "", "5.0,0",
														   translation("", language))));



	// growing season has started only on 10% of coastal area
	// growing season has started on 35% of inland area
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 1,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("10.0,0", "35.0,0",
														   "", "0,0",
														   "", "75.0,0",
														   "", "5.0,0",
														   translation("Monin paikoin hallaa.", language))));

	// growing season has started on both areas
	// probability for frost at coastal area is 5.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 2,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "95.0,0", 
														   "0,0", "0,0", 
														   "5.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 9.9%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 2,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "95.0,0", 
														   "0,0", "0,0", 
														   "9.9,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa.", language))));
	// growing season has started
	// probability for frost at coastal area is 10.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 3,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "10.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 15.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 3,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "15.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 24.9%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 3,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "24.9,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 25.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 4,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "25.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa, rannikolla mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 35.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 4,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "35.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa, rannikolla mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 44.9%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 4,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "44.9,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa monin paikoin hallaa, rannikolla mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 45.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 5,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "45.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Monin paikoin hallaa.", language))));
															
	// growing season has started
	// probability for frost at coastal and area is 55.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 5,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "55.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Monin paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 64.9%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 5,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "64.9,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Monin paikoin hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 65.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 6,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "65.0,0", "75.0,0", 
														   "", "5.0,0", 
														   translation("Monin paikoin hallaa.", language))));
															
	// growing season has started
	// probability for frost at coastal and area is 75.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at coastal area is 5.0%
	// probability for severe frost at inland area is 5.0%
	// cell 6,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "75.0,0", "75.0,0", 
														   "5.0,0", "5.0,0", 
														   translation("Monin paikoin hallaa.", language))));


	// growing season has started
	// probability for frost at coastal and area is 84.9%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at coastal area is 20.1%
	// probability for severe frost at inland area is 5.0%
	// cell 6,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "84.9,0", "75.0,0", 
														   "20.1,0", "5.0,0", 
														   translation("Monin paikoin hallaa, joka voi olla ankaraa.", language))));

	// growing season has started
	// probability for frost at coastal area is 85.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at coastal area is 20.0%
	// probability for severe frost at inland area is 5.0%
	// cell 7,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "85.0,0", "75.0,0", 
														   "20.0,0", "5.0,0", 
														   translation("Hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 95.0%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at coastal area is 20.1%
	// probability for severe frost at inland area is 5.0%
	// cell 7,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "95.0,0", "75.0,0", 
														   "20.1,0", "5.0,0", 
														   translation("Hallaa, joka voi olla ankaraa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 99.9%
	// probability for frost at inland area is 75.0%
	// probability for severe frost at coastal area is 65.0%
	// probability for severe frost at inland area is 5.0%
	// cell 7,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "99.9,0", "75.0,0", 
														   "65.0,0", "5.0,0", 
														   translation("Hallaa, joka voi olla ankaraa.", language))));

	// growing season has started
	// night frost exists 25% of the coastal area
	// probability for frost at inland area is 75.0%
	// probability for severe frost at inland area is 5.0%
	// cell 8,6
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0",  
														   "25.0,0","0,0", 
														   "", "75.0,0",
														   "", "5.0,0",
														   translation("", language))));

	// growing season has started only on 10% of coastal area
	// growing season has started on 35% of inland area
	// probability for frost at inland area is 95.0%
	// probability for severe frost at inland area is 5.0%
	// cell 1,7
	testCases.insert(make_pair(i++, OneNightFrostTestParam("10.0,0", "35.0,0",
														   "", "0,0",
														   "", "95.0,0",
														   "", "5.0,0",
														   translation("Hallaa.", language))));

	// growing season has started on both areas
	// probability for frost at coastal area is 5.0%
	// probability for frost at inland area is 95.0%
	// probability for severe frost at inland area is 5.0%
	// cell 2,7
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "95.0,0", 
														   "0,0", "0,0", 
														   "5.0,0", "95.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 9.9%
	// probability for frost at inland area is 95.0%
	// probability for severe frost at inland area is 5.0%
	// cell 2,7
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "95.0,0", 
														   "0,0", "0,0", 
														   "9.9,0", "95.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 10.0%
	// probability for frost at inland area is 95.0%
	// probability for severe frost at inland area is 5.0%
	// cell 3,7
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "10.0,0", "95.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 15.0%
	// probability for frost at inland area is 95.0%
	// probability for severe frost at inland area is 5.0%
	// cell 3,7
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "15.0,0", "95.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 24.9%
	// probability for frost at inland area is 95.0%
	// probability for severe frost at inland area is 5.0%
	// cell 3,7
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "24.9,0", "95.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 25.0%
	// probability for frost at inland area is 95.0%
	// probability for severe frost at inland area is 5.0%
	// cell 4,7
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "25.0,0", "95.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa hallaa, rannikolla mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 35.0%
	// probability for frost at inland area is 95.0%
	// probability for severe frost at inland area is 5.0%
	// cell 4,7
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "35.0,0", "95.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa hallaa, rannikolla mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 44.9%
	// probability for frost at inland area is 95.0%
	// probability for severe frost at inland area is 5.0%
	// cell 4,7
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "44.9,0", "95.0,0", 
														   "", "5.0,0", 
														   translation("Sisämaassa hallaa, rannikolla mahdollisesti hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 45.0%
	// probability for frost at inland area is 95.0%
	// probability for severe frost at inland area is 5.0%
	// cell 5,7
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "45.0,0", "95.0,0", 
														   "", "5.0,0", 
														   translation("Hallaa.", language))));
															
	// growing season has started
	// probability for frost at coastal and area is 55.0%
	// probability for frost at inland area is 95.0%
	// probability for severe frost at inland area is 5.0%
	// cell 5,7
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "55.0,0", "95.0,0", 
														   "", "5.0,0", 
														   translation("Hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 64.9%
	// probability for frost at inland area is 95.0%
	// probability for severe frost at inland area is 5.0%
	// cell 5,7
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "64.9,0", "95.0,0", 
														   "", "5.0,0", 
														   translation("Hallaa.", language))));

	// growing season has started
	// probability for frost at coastal area is 65.0%
	// probability for frost at inland area is 95.0%
	// probability for severe frost at inland area is 5.0%
	// cell 6,7
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "65.0,0", "95.0,0", 
														   "", "5.0,0", 
														   translation("Hallaa.", language))));
															
	// growing season has started
	// probability for frost at coastal and area is 75.0%
	// probability for frost at inland area is 95.0%
	// probability for severe frost at coastal area is 5.0%
	// probability for severe frost at inland area is 5.0%
	// cell 6,7
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "75.0,0", "95.0,0", 
														   "5.0,0", "5.0,0", 
														   translation("Hallaa.", language))));


	// growing season has started
	// probability for frost at coastal and area is 84.9%
	// probability for frost at inland area is 95.0%
	// probability for severe frost at coastal area is 20.1%
	// probability for severe frost at inland area is 5.0%
	// cell 6,7
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "84.9,0", "95.0,0", 
														   "20.1,0", "5.0,0", 
														   translation("Hallaa, joka voi olla ankaraa.", language))));

	// growing season has started
	// probability for frost at coastal area is 85.0%
	// probability for frost at inland area is 95.0%
	// probability for severe frost at coastal area is 20.0%
	// probability for severe frost at inland area is 5.0%
	// cell 7,7
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "85.0,0", "95.0,0", 
														   "20.0,0", "5.0,0", 
														   translation("Hallaa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 95.0%
	// probability for frost at inland area is 95.0%
	// probability for severe frost at coastal area is 20.1%
	// probability for severe frost at inland area is 5.0%
	// cell 7,7
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "95.0,0", "95.0,0", 
														   "20.1,0", "5.0,0", 
														   translation("Hallaa, joka voi olla ankaraa.", language))));

	// growing season has started
	// probability for frost at coastal and area is 99.9%
	// probability for frost at inland area is 95.0%
	// probability for severe frost at coastal area is 65.0%
	// probability for severe frost at inland area is 5.0%
	// cell 7,7
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "0,0", 
														   "99.9,0", "95.0,0", 
														   "65.0,0", "5.0,0", 
														   translation("Hallaa, joka voi olla ankaraa.", language))));

	// growing season has started
	// night frost exists 25% of the coastal area
	// probability for frost at inland area is 95.0%
	// probability for severe frost at inland area is 5.0%
	// cell 8,7
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0",  
														   "25.0,0","0,0", 
														   "", "95.0,0",
														   "", "5.0,0",
														   translation("", language))));

	// growing season has started only on 10% of coastal area
	// growing season has started on 35% of inland area
	// night frost exists 25% of the inland area
	// probability for frost at inland area is 95.0%
	// probability for severe frost at inland area is 5.0%
	// cell 1,8
	testCases.insert(make_pair(i++, OneNightFrostTestParam("10.0,0", "35.0,0",
														   "", "25.0,0",
														   "", "",
														   "", "",
														   translation("", language))));

	// growing season has started on both areas
	// probability for frost at coastal area is 5.0%
	// night frost exists 25% of the inland area
	// cell 2,8
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "95.0,0", 
														   "0,0", "25.0,0", 
														   "5.0,0", "", 
														   "", "", 
														   translation("", language))));

	// growing season has started
	// probability for frost at coastal area is 9.9%
	// night frost exists 25% of the inland area
	// cell 2,8
	testCases.insert(make_pair(i++, OneNightFrostTestParam("95.0,0", "95.0,0", 
														   "0,0", "25.0,0", 
														   "9.9,0", "", 
														   "", "", 
														   translation("", language))));

	// growing season has started
	// probability for frost at coastal area is 10.0%
	// night frost exists 25% of the inland area
	// cell 3,8
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "25.0,0", 
														   "10.0,0", "", 
														   "", "", 
														   translation("", language))));

	// growing season has started
	// probability for frost at coastal area is 15.0%
	// night frost exists 25% of the inland area
	// cell 3,8
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "25.0,0", 
														   "15.0,0", "", 
														   "", "", 
														   translation("", language))));

	// growing season has started
	// probability for frost at coastal area is 24.9%
	// night frost exists 25% of the inland area
	// cell 3,8
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "25.0,0", 
														   "24.9,0", "", 
														   "", "", 
														   translation("", language))));

	// growing season has started
	// probability for frost at coastal area is 25.0%
	// night frost exists 25% of the inland area
	// cell 4,8
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "25.0,0", 
														   "25.0,0", "", 
														   "", "", 
														   translation("", language))));

	// growing season has started
	// probability for frost at coastal and area is 35.0%
	// night frost exists 25% of the inland area
	// cell 4,8
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "25.0,0", 
														   "35.0,0", "", 
														   "", "", 
														   translation("", language))));

	// growing season has started
	// probability for frost at coastal and area is 44.9%
	// night frost exists 25% of the inland area
	// cell 4,8
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "25.0,0", 
														   "44.9,0", "", 
														   "", "", 
														   translation("", language))));

	// growing season has started
	// probability for frost at coastal area is 45.0%
	// night frost exists 25% of the inland area
	// cell 5,8
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "25.0,0", 
														   "45.0,0", "", 
														   "", "", 
														   translation("", language))));

	// growing season has started
	// probability for frost at coastal and area is 55.0%
	// night frost exists 25% of the inland area
	// cell 5,8
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "25.0,0", 
														   "55.0,0", "", 
														   "", "", 
														   translation("", language))));

	// growing season has started
	// probability for frost at coastal and area is 64.9%
	// night frost exists 25% of the inland area
	// cell 5,8
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "25.0,0", 
														   "64.9,0", "", 
														   "", "", 
														   translation("", language))));

	// growing season has started
	// probability for frost at coastal area is 65.0%
	// night frost exists 25% of the inland area
	// cell 6,8
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "25.0,0", 
														   "65.0,0", "", 
														   "", "", 
														   translation("", language))));

	// growing season has started
	// probability for frost at coastal and area is 75.0%
	// night frost exists 25% of the inland area
	// cell 6,8
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "25.0,0", 
														   "75.0,0", "", 
														   "5.0,0", "", 
														   translation("", language))));

	// growing season has started
	// probability for frost at coastal and area is 84.9%
	// night frost exists 25% of the inland area
	// cell 6,8
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "25.0,0", 
														   "84.9,0", "", 
														   "15.0,0", "", 
														   translation("", language))));

	// growing season has started
	// probability for frost at coastal area is 85.0%
	// night frost exists 25% of the inland area
	// cell 7,8
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "25.0,0", 
														   "85.0,0", "", 
														   "25.0,0", "", 
														   translation("", language))));

	// growing season has started
	// probability for frost at coastal and area is 95.0%
	// night frost exists 25% of the inland area
	// cell 7,8
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "25.0,0", 
														   "95.0,0", "", 
														   "65.0,0", "", 
														   translation("", language))));

	// growing season has started
	// probability for frost at coastal and area is 99.9%
	// night frost exists 25% of the inland area
	// cell 7,8
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "0,0", "25.0,0", 
														   "99.9,0", "", 
														   "72.0,0", "", 
														   translation("", language))));

	// growing season has started
	// night frost exists 25% of the coastal and inland area
	// cell 7,8
	testCases.insert(make_pair(i++, OneNightFrostTestParam("100.0,0", "100.0,0", 
														   "25.0,0", "25.0,0", 
														   "", "", 
														   "", "", 
														   translation("", language))));

  }

  void frost_onenight()
  {
	using namespace std;
	using namespace TextGen;
	using namespace WeatherAnalysis;

	AnalysisSources sources;
	WeatherArea area("25,60");

	const string fun = "frost_onenight";

	NFmiSettings::Set("onenight::night::starthour","21");
	NFmiSettings::Set("onenight::night::endhour","09");
	NFmiSettings::Set("onenight::required_growing_season_percentage::default","33.333");
	NFmiSettings::Set("onenight::required_growing_season_percentage::lansi-lappi","50.0");
	NFmiSettings::Set("onenight::required_night_frost_percentage","20.0");
	NFmiSettings::Set("onenight::required_severe_frost_probability","20.0");

	NFmiTime time1(2003,6,3,12);
	NFmiTime time2(2003,6,4,12);
	WeatherPeriod period(time1,time2);
	FrostStory story(time1,sources,area,period,"onenight");

	FrostOneNightTestCases testCases;
	FrostOneNightTestCases::iterator iter;

	const char* languages [] = {"fi", "sv", "en"};

	for(int i = 0; i < 1; i++)
	  {
		create_testcases(testCases, languages[i]);
		for(iter = testCases.begin(); iter != testCases.end(); iter++)
		  {
			NFmiSettings::Set("onenight::fake::growing_season_percentange::coastal",iter->second.temperaturesumcoastal);
			NFmiSettings::Set("onenight::fake::growing_season_percentange::inland",iter->second.temperaturesuminland);
			NFmiSettings::Set("onenight::fake::night_frost_percentage::coastal",iter->second.nightfrostcoastal);
			NFmiSettings::Set("onenight::fake::night_frost_percentage::inland",iter->second.nightfrostinland);
			NFmiSettings::Set("onenight::fake::frost_probability::coastal",iter->second.frostcoastal);
			NFmiSettings::Set("onenight::fake::frost_probability::inland",iter->second.frostinland);
			NFmiSettings::Set("onenight::fake::severe_frost_probability::coastal",iter->second.severefrostcoastal);
			NFmiSettings::Set("onenight::fake::severe_frost_probability::inland",iter->second.severefrostinland);
			require(story,languages[i],fun,iter->second.froststory);
		  }
		cout << endl << "test cases(" << languages[i] << "): " << testCases.size();
		testCases.clear();
	  }
	cout << endl;
	
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
	virtual const char * const error_message_prefix() const
	{
	  return "\n\t";
	}

	//! Main test suite
	void test(void)
	{
	  TEST(frost_onenight);
	  TEST(frost_mean);
	  TEST(frost_maximum);
	  TEST(frost_range);
	  TEST(frost_twonights);
	  TEST(frost_day);
	}

  }; // class tests

} // namespace FrostStoryTest


int main(void)
{
  using namespace FrostStoryTest;


  MessageLogger::open("my.log");


  cout << endl
	   << "FrostStory tests" << endl
	   << "================" << endl;

  dict.reset(TextGen::DictionaryFactory::create("multimysql"));

  dict->init("fi");
  dict->init("sv");
  dict->init("en");

  NFmiSettings::Set("textgen::frostseason","true");

  tests t;
  return t.run();
}
