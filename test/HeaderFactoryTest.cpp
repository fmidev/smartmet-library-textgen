#include "regression/tframe.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "Header.h"
#include "HeaderFactory.h"
#include "PlainTextFormatter.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"

#include "NFmiSettings.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;

namespace HeaderFactoryTest
{
  shared_ptr<TextGen::Dictionary> dict;
  TextGen::PlainTextFormatter formatter;
  

  string require(const string & theLanguage,
				 const WeatherAnalysis::WeatherArea & theArea,
				 const WeatherAnalysis::WeatherPeriod & thePeriod,
				 const string & theName,
				 const string & theResult)
  {
	dict->init(theLanguage);
	formatter.dictionary(dict);

	TextGen::Header header = TextGen::HeaderFactory::create(theArea,
															thePeriod,
															theName);

	const string value = header.realize(formatter);

	if(value != theResult)
	  return (value + " <> " + theResult);
	else
	  return "";
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test HeaderFactory::create() with type "none"
   */
  // ----------------------------------------------------------------------

  void header_none()
  {
	using namespace WeatherAnalysis;

	string var = "variable";
	NFmiSettings::instance().set(var+"::type","none");

	WeatherArea area("");
	WeatherPeriod period(NFmiTime(2003,6,1),NFmiTime(2003,6,2));

	string result;

	result = require("fi",area,period,var,"");
	if(!result.empty()) TEST_FAILED(result.c_str());

	result = require("sv",area,period,var,"");
	if(!result.empty()) TEST_FAILED(result.c_str());

	result = require("en",area,period,var,"");
	if(!result.empty()) TEST_FAILED(result.c_str());

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test HeaderFactory::create() with type "until"
   */
  // ----------------------------------------------------------------------

  void header_until()
  {
	using namespace WeatherAnalysis;

	string var = "variable";
	NFmiSettings::instance().set(var+"::type","until");

	WeatherArea area("");
	WeatherPeriod period1(NFmiTime(2003,6,1,6,0),NFmiTime(2003,6,2,6));
	WeatherPeriod period2(NFmiTime(2003,6,1,6,0),NFmiTime(2003,6,2,18));

	string result;

	result = require("fi",area,period1,var,"Odotettavissa maanantaiaamuun asti:");
	if(!result.empty()) TEST_FAILED(result.c_str());

	result = require("sv",area,period1,var,"Utsikter till måndag morgon:");
	if(!result.empty()) TEST_FAILED(result.c_str());

	result = require("en",area,period1,var,"Expected weather until Monday morning:");
	if(!result.empty()) TEST_FAILED(result.c_str());

	result = require("fi",area,period2,var,"Odotettavissa maanantai-iltaan asti:");
	if(!result.empty()) TEST_FAILED(result.c_str());

	result = require("sv",area,period2,var,"Utsikter till måndag kväll:");
	if(!result.empty()) TEST_FAILED(result.c_str());

	result = require("en",area,period2,var,"Expected weather until Monday evening:");
	if(!result.empty()) TEST_FAILED(result.c_str());

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test HeaderFactory::create() with type "from_until"
   */
  // ----------------------------------------------------------------------

  void header_from_until()
  {
	using namespace WeatherAnalysis;

	string var = "variable";
	NFmiSettings::instance().set(var+"::type","from_until");

	WeatherArea area("");
	WeatherPeriod period1(NFmiTime(2003,6,1,6,0),NFmiTime(2003,6,2,6));
	WeatherPeriod period2(NFmiTime(2003,6,1,6,0),NFmiTime(2003,6,2,18));

	string result;

	result = require("fi",area,period1,var,"Odotettavissa sunnuntaiaamusta maanantaiaamuun:");
	if(!result.empty()) TEST_FAILED(result.c_str());

	result = require("sv",area,period1,var,"Utsikter från söndag morgon till måndag morgon:");
	if(!result.empty()) TEST_FAILED(result.c_str());

	result = require("en",area,period1,var,"Expected weather from Sunday morning until Monday morning:");
	if(!result.empty()) TEST_FAILED(result.c_str());

	result = require("fi",area,period2,var,"Odotettavissa sunnuntaiaamusta maanantai-iltaan:");
	if(!result.empty()) TEST_FAILED(result.c_str());

	result = require("sv",area,period2,var,"Utsikter från söndag morgon till måndag kväll:");
	if(!result.empty()) TEST_FAILED(result.c_str());

	result = require("en",area,period2,var,"Expected weather from Sunday morning until Monday evening:");
	if(!result.empty()) TEST_FAILED(result.c_str());

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test HeaderFactory::create() with type "several_days"
   */
  // ----------------------------------------------------------------------

  void header_several_days()
  {
	using namespace WeatherAnalysis;

	string var = "variable";
	NFmiSettings::instance().set(var+"::type","several_days");

	WeatherArea area("");
	WeatherPeriod period(NFmiTime(2003,6,1,6,0),NFmiTime(2003,6,3,6));

	string result;

	result = require("fi",area,period,var,"Sunnuntaiaamusta alkavan kahden vuorokauden sää:");
	if(!result.empty()) TEST_FAILED(result.c_str());

	result = require("sv",area,period,var,"Från söndag morgon för de följande två dygnen:");
	if(!result.empty()) TEST_FAILED(result.c_str());

	result = require("en",area,period,var,"From Sunday morning for the next two days:");
	if(!result.empty()) TEST_FAILED(result.c_str());

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test HeaderFactory::create() with type "report_area"
   */
  // ----------------------------------------------------------------------

  void header_report_area()
  {
	using namespace WeatherAnalysis;

	string var = "variable";
	NFmiSettings::instance().set(var+"::type","report_area");

	WeatherArea area("uusimaa");
	WeatherPeriod period(NFmiTime(2003,6,1,6,0),NFmiTime(2003,6,3,6));

	string result;

	result = require("fi",area,period,var,"Sääennuste Uudellemaalle sunnuntaina kello 6:");
	if(!result.empty()) TEST_FAILED(result.c_str());

	result = require("sv",area,period,var,"Väderraport för Nyland söndag klockan 6:");
	if(!result.empty()) TEST_FAILED(result.c_str());

	result = require("en",area,period,var,"Weather report for Uusimaa on Sunday 6 o'clock:");
	if(!result.empty()) TEST_FAILED(result.c_str());

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
	  TEST(header_none);
	  TEST(header_until);
	  TEST(header_from_until);
	  TEST(header_several_days);
	  TEST(header_report_area);
	}

  }; // class tests

} // namespace HeaderFactoryTest


int main(void)
{
  using namespace HeaderFactoryTest;

  cout << endl
	   << "HeaderFactory tests" << endl
	   << "======================" << endl;

  dict = TextGen::DictionaryFactory::create("multimysql");

  dict->init("fi");
  dict->init("sv");
  dict->init("en");

  tests t;
  return t.run();
}
