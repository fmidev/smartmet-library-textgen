#include "regression/tframe.h"
#include "PrecipitationPeriodTools.h"
#include "AnalysisSources.h"
#include "RegularMaskSource.h"
#include "UserWeatherSource.h"
#include "MapSource.h"
#include "MaskSource.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"
#include "NFmiStreamQueryData.h"
#include "NFmiSvgPath.h"
#include "NFmiGrid.h"
#include "NFmiSettings.h"

#include "boost/shared_ptr.hpp"
#include "boost/lexical_cast.hpp"

#include <stdexcept>

using namespace std;
using namespace boost;

namespace PrecipitationPeriodToolsTest
{
  shared_ptr<NFmiQueryData> theQD;

  void read_querydata(const std::string & theFilename)
  {
	std::ifstream input(theFilename.c_str(),ios::in);
	if(!input)
	  throw runtime_error("Failed to read "+theFilename);
	theQD = shared_ptr<NFmiQueryData>(new NFmiQueryData);
	input >> *theQD;

  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Test PrecipitationPeriodTools::findRainTimes
   */
  // ----------------------------------------------------------------------

  void findRainTimes()
  {
	using namespace WeatherAnalysis;
	using namespace WeatherAnalysis::PrecipitationPeriodTools;
	using WeatherAnalysis::PrecipitationPeriodTools::findRainTimes;

	NFmiFastQueryInfo q(theQD.get());
	q.First();

	NFmiTime time1 = q.Time();
	NFmiTime time2 = time1;
	time2.ChangeByHours(100);
	WeatherPeriod period(time1,time2);


	AnalysisSources sources;
	shared_ptr<UserWeatherSource> weathersource(new UserWeatherSource());
	weathersource->insert("data",theQD);

	NFmiSettings::Set("textgen::precipitation_forecast","data");

	shared_ptr<MapSource> mapsource(new MapSource());
	shared_ptr<MaskSource> masksource(new RegularMaskSource(mapsource,10));
	sources.setWeatherSource(weathersource);
	sources.setMapSource(mapsource);
	sources.setMaskSource(masksource);

	{
	  RainTimes times = findRainTimes(sources,
									  WeatherArea("uusimaa"),
									  period,
									  "");
	  if(times.size() != 60)
		TEST_FAILED("Must get 60 rainy moments out of 100 for Uusimaa, got "+lexical_cast<string>(times.size()));

	}

	{
	  RainTimes times = findRainTimes(sources,
									  WeatherArea("ahvenanmaa"),
									  period,
									  "");
	  if(times.size() != 55)
		TEST_FAILED("Must get 55 rainy moments out of 100 for Ahvenanmaa, got "+lexical_cast<string>(times.size()));

	}

	{
	  RainTimes times = findRainTimes(sources,
									  WeatherArea("pohjois-lappi"),
									  period,
									  "");
	  if(times.size() != 1)
		TEST_FAILED("Must get 1 rainy moments out of 100 for Pohjois-Lappi, got "+lexical_cast<string>(times.size()));

	}

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PrecipitationPeriodTools::findRainPeriods()
   */
  // ----------------------------------------------------------------------

  void findRainPeriods()
  {
	using namespace WeatherAnalysis;
	using namespace WeatherAnalysis::PrecipitationPeriodTools;
	using WeatherAnalysis::PrecipitationPeriodTools::findRainTimes;
	using WeatherAnalysis::PrecipitationPeriodTools::findRainPeriods;

	NFmiFastQueryInfo q(theQD.get());
	q.First();

	NFmiTime time1 = q.Time();
	NFmiTime time2 = time1;
	time2.ChangeByHours(100);
	WeatherPeriod period(time1,time2);


	AnalysisSources sources;
	shared_ptr<UserWeatherSource> weathersource(new UserWeatherSource());
	weathersource->insert("data",theQD);
	NFmiSettings::Set("textgen::precipitation_forecast","data");


	shared_ptr<MapSource> mapsource(new MapSource());
	shared_ptr<MaskSource> masksource(new RegularMaskSource(mapsource,10));
	sources.setWeatherSource(weathersource);
	sources.setMapSource(mapsource);
	sources.setMaskSource(masksource);

	NFmiSettings::Set("a::rainyperiod::maximum_interval","1");
	NFmiSettings::Set("b::rainyperiod::maximum_interval","3");

	{
	  RainTimes times = findRainTimes(sources,
									  WeatherArea("uusimaa"),
									  period,
									  "");

	  RainPeriods periods1 = findRainPeriods(times,"a");
	  if(periods1.size() != 2)
		TEST_FAILED("Must find 2 rainy periods for Uusimaa with max separation 1");
	  RainPeriods periods3 = findRainPeriods(times,"b");
	  if(periods3.size() != 1)
		TEST_FAILED("Must find 1 rainy period for Uusimaa with max separation 3");
	  
	}
	
	{
	  RainTimes times = findRainTimes(sources,
									  WeatherArea("ahvenanmaa"),
									  period,
									  "");
	  
	  RainPeriods periods1 = findRainPeriods(times,"a");
	  if(periods1.size() != 6)
		TEST_FAILED("Must find 6 rainy periods for Ahvenanmaa with max separation 1");
	  
	  RainPeriods periods3 = findRainPeriods(times,"b");
	  if(periods3.size() != 4)
		TEST_FAILED("Must find 4 rainy periods for Ahvenanmaa with max separation 3");
	  
	}
	
	{
	  RainTimes times = findRainTimes(sources,
									  WeatherArea("pohjois-lappi"),
									  period,
									  "");
	  
	  RainPeriods periods1 = findRainPeriods(times,"a");
	  if(periods1.size() != 1)
		TEST_FAILED("Must find 1 rainy period for Pohjois-Lappi with max separation 1");

	  RainPeriods periods3 = findRainPeriods(times,"b");
	  if(periods3.size() != 1)
		TEST_FAILED("Must find 1 rainy period for Pohjois-Lappi with max separation 3");
	  
	}
	
	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PrecpitationPeriodTools::overlappingPeriods
   */
  // ----------------------------------------------------------------------

  void overlappingPeriods()
  {
	using namespace WeatherAnalysis;

	PrecipitationPeriodTools::RainPeriods periods;
	periods.push_back(WeatherPeriod(NFmiTime(2003,9,1,10),
									NFmiTime(2003,9,1,13)));
	periods.push_back(WeatherPeriod(NFmiTime(2003,9,2,10),
									NFmiTime(2003,9,2,13)));
	periods.push_back(WeatherPeriod(NFmiTime(2003,9,2,18),
									NFmiTime(2003,9,2,19)));
	periods.push_back(WeatherPeriod(NFmiTime(2003,9,3,18),
									NFmiTime(2003,9,3,19)));
	periods.push_back(WeatherPeriod(NFmiTime(2003,9,3,18),
									NFmiTime(2003,9,4,04)));
	periods.push_back(WeatherPeriod(NFmiTime(2003,9,4,18),
									NFmiTime(2003,9,6,04)));

	WeatherPeriod day1(NFmiTime(2003,9,1),NFmiTime(2003,9,2));
	if(PrecipitationPeriodTools::overlappingPeriods(periods,day1).size() != 1)
	  TEST_FAILED("Failed to get exactly one period for day 1");

	WeatherPeriod day2(NFmiTime(2003,9,2),NFmiTime(2003,9,3));
	if(PrecipitationPeriodTools::overlappingPeriods(periods,day2).size() != 2)
	  TEST_FAILED("Failed to get exactly two periods for day 2");

	WeatherPeriod day3(NFmiTime(2003,9,3),NFmiTime(2003,9,4));
	if(PrecipitationPeriodTools::overlappingPeriods(periods,day3).size() != 2)
	  TEST_FAILED("Failed to get exactly two periods for day 3");

	WeatherPeriod day4(NFmiTime(2003,9,4),NFmiTime(2003,9,5));
	if(PrecipitationPeriodTools::overlappingPeriods(periods,day4).size() != 2)
	  TEST_FAILED("Failed to get exactly two periods for day 4");

	WeatherPeriod day5(NFmiTime(2003,9,5),NFmiTime(2003,9,6));
	if(PrecipitationPeriodTools::overlappingPeriods(periods,day5).size() != 1)
	  TEST_FAILED("Failed to get exactly one period for day 5");

	WeatherPeriod day6(NFmiTime(2003,9,6),NFmiTime(2003,9,7));
	if(PrecipitationPeriodTools::overlappingPeriods(periods,day6).size() != 1)
	  TEST_FAILED("Failed to get exactly one period for day 6");
												  
	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PrecpitationPeriodTools::inclusivePeriods
   */
  // ----------------------------------------------------------------------

  void inclusivePeriods()
  {
	using namespace WeatherAnalysis;

	PrecipitationPeriodTools::RainPeriods periods;
	periods.push_back(WeatherPeriod(NFmiTime(2003,9,1,10),
									NFmiTime(2003,9,1,13)));
	periods.push_back(WeatherPeriod(NFmiTime(2003,9,2,10),
									NFmiTime(2003,9,2,13)));
	periods.push_back(WeatherPeriod(NFmiTime(2003,9,2,18),
									NFmiTime(2003,9,2,19)));
	periods.push_back(WeatherPeriod(NFmiTime(2003,9,3,18),
									NFmiTime(2003,9,3,19)));
	periods.push_back(WeatherPeriod(NFmiTime(2003,9,3,18),
									NFmiTime(2003,9,4,04)));
	periods.push_back(WeatherPeriod(NFmiTime(2003,9,4,18),
									NFmiTime(2003,9,6,04)));

	WeatherPeriod day1(NFmiTime(2003,9,1),NFmiTime(2003,9,2));
	if(PrecipitationPeriodTools::inclusivePeriods(periods,day1).size() != 1)
	  TEST_FAILED("Failed to get exactly one period for day 1");

	WeatherPeriod day2(NFmiTime(2003,9,2),NFmiTime(2003,9,3));
	if(PrecipitationPeriodTools::inclusivePeriods(periods,day2).size() != 2)
	  TEST_FAILED("Failed to get exactly two periods for day 2");

	WeatherPeriod day3(NFmiTime(2003,9,3),NFmiTime(2003,9,4));
	if(PrecipitationPeriodTools::inclusivePeriods(periods,day3).size() != 1)
	  TEST_FAILED("Failed to get exactly one period for day 3");

	WeatherPeriod day4(NFmiTime(2003,9,4),NFmiTime(2003,9,5));
	if(PrecipitationPeriodTools::inclusivePeriods(periods,day4).size() != 0)
	  TEST_FAILED("Failed to get exactly zero periods for day 4");

	WeatherPeriod day5(NFmiTime(2003,9,5),NFmiTime(2003,9,6));
	if(PrecipitationPeriodTools::inclusivePeriods(periods,day5).size() != 0)
	  TEST_FAILED("Failed to get exactly zero periods for day 5");

	WeatherPeriod day6(NFmiTime(2003,9,6),NFmiTime(2003,9,7));
	if(PrecipitationPeriodTools::inclusivePeriods(periods,day6).size() != 0)
	  TEST_FAILED("Failed to get exactly zero periods for day 6");
												  
	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test PrecipitationPeriodTools::mergeNigthlyRainPeriods
   */
  // ----------------------------------------------------------------------

  void mergeNightlyRainPeriods()
  {
	TEST_NOT_IMPLEMENTED();
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
	  TEST(findRainTimes);
	  TEST(findRainPeriods);
	  TEST(mergeNightlyRainPeriods);
	  TEST(overlappingPeriods);
	  TEST(inclusivePeriods);
	}

  }; // class tests

} // namespace PrecipitationPeriodToolsTest


int main(void)
{
  cout << endl
	   << "PrecipitationPeriodTools tester" << endl
	   << "===============================" << endl;

  NFmiSettings::Init();
  const string mapdir = NFmiSettings::Require<string>("textgen::mappath");
  NFmiSettings::Set("textgen::areas::map::uusimaa",mapdir+"/sonera/uusimaa.svg");
  NFmiSettings::Set("textgen::areas::map::ahvenanmaa",mapdir+"/sonera/ahvenanmaa.svg");
  NFmiSettings::Set("textgen::areas::map::pohjois-lappi",mapdir+"/sonera/pohjois-lappi.svg");
  
  PrecipitationPeriodToolsTest::read_querydata("data/skandinavia_pinta.sqd");

  PrecipitationPeriodToolsTest::tests t;
  return t.run();
}
