#include "regression/tframe.h"
#include "QueryDataIntegrator.h"
#include "SumCalculator.h"
#include "MaximumCalculator.h"
#include "MinimumCalculator.h"
#include "MeanCalculator.h"
#include "NFmiIndexMask.h"
#include "NFmiIndexMaskSource.h"
#include "NFmiIndexMaskTools.h"
#include "NFmiStreamQueryData.h"
#include "NFmiSvgPath.h"
#include "boost/lexical_cast.hpp"

using namespace std;
using namespace boost;
using namespace WeatherAnalysis;
using namespace WeatherAnalysis::QueryDataIntegrator;

namespace QueryDataIntegratorTest
{

  NFmiStreamQueryData theQD;
  const NFmiGrid * theGrid;

  NFmiSvgPath theArea;

  void read_querydata(const std::string & theFilename)
  {
	theQD.ReadData(theFilename);
	theGrid = theQD.QueryInfoIter()->Grid();
  }

  void read_svg()
  {
	std::ifstream input("data/uusimaa.svg",std::ios::in);
	input >> theArea;
	input.close();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test Integrate() over time
   */
  // ----------------------------------------------------------------------

  void integrate_time(void)
  {
	using namespace std;

	NFmiFastQueryInfo * q = theQD.QueryInfoIter();
	q->First();
	q->Param(kFmiTemperature);
	NFmiMetTime time1 = q->Time();
	NFmiMetTime time2 = time1;
	time2.ChangeByHours(10);

	{
	  MaximumCalculator modifier;
	  float result = Integrate(*q,time1,time2,modifier);
	  float expected = 24.55971;

	  if(std::abs(result-expected) > 0.1)
		TEST_FAILED("max over time failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}

	{
	  MinimumCalculator modifier;
	  float result = Integrate(*q,time1,time2,modifier);
	  float expected = 16.02036;

	  if(std::abs(result-expected) > 0.1)
		TEST_FAILED("min over time failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}

	{
	  SumCalculator modifier;
	  float result = Integrate(*q,time1,time2,modifier);
	  float expected = 219.6678;
	  
	  if(std::abs(result-expected) > 0.1)
		TEST_FAILED("sum over time failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}

	{
	  MeanCalculator modifier;
	  float result = Integrate(*q,time1,time2,modifier);
	  float expected = 19.9698;
	  
	  if(std::abs(result-expected) > 0.1)
		TEST_FAILED("average over time failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}


	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test Integrate() over space
   */
  // ----------------------------------------------------------------------

  void integrate_space(void)
  {
	using namespace std;
	using namespace WeatherAnalysis::QueryDataIntegrator;
	using namespace NFmiIndexMaskTools;

	NFmiFastQueryInfo * q = theQD.QueryInfoIter();
	q->First();
	q->Param(kFmiTemperature);

	NFmiIndexMask helsinki = MaskDistance(*theGrid,NFmiPoint(25,60),50);

	// too many to check manually
	NFmiIndexMask uusimaa = MaskInside(*theGrid,theArea);

	{
	  MaximumCalculator modifier;
	  float result = Integrate(*q,helsinki,modifier);
	  float expected = 16.58555;

	  if(std::abs(result-expected)>0.01)
		TEST_FAILED("maximum over helsinki failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}

	{
	  MinimumCalculator modifier;
	  float result = Integrate(*q,helsinki,modifier);
	  float expected = 11.56234;

	  if(std::abs(result-expected)>0.01)
		TEST_FAILED("minimum over helsinki failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}

	{
	  MeanCalculator modifier;
	  float result = Integrate(*q,helsinki,modifier);
	  float expected = 14.92822;
	  
	  if(std::abs(result-expected)>0.01)
		TEST_FAILED("average over helsinki failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}


	{
	  MaximumCalculator modifier;
	  float result = Integrate(*q,uusimaa,modifier);
	  float expected = 14.86401;

	  if(std::abs(result-expected)>0.01)
		TEST_FAILED("maximum over uusimaa failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}

	{
	  MinimumCalculator modifier;
	  float result = Integrate(*q,uusimaa,modifier);
	  float expected = 9.620665;

	  if(std::abs(result-expected)>0.01)
		TEST_FAILED("minimum over uusimaa failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}


	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test Integrate() over time and space
   */
  // ----------------------------------------------------------------------

  void integrate_time_space(void)
  {
	using namespace std;
	using namespace WeatherAnalysis::QueryDataIntegrator;
	using namespace NFmiIndexMaskTools;

	NFmiFastQueryInfo * q = theQD.QueryInfoIter();
	q->First();
	q->Param(kFmiTemperature);
	NFmiMetTime time1 = q->Time();
	NFmiMetTime time2 = time1;
	time2.ChangeByHours(10);

	NFmiIndexMask helsinki = MaskDistance(*theGrid,NFmiPoint(25,60),50);
	NFmiIndexMask uusimaa = MaskInside(*theGrid,theArea);

	{
	  MaximumCalculator spacemodifier;
	  MaximumCalculator timemodifier;
	  float result = Integrate(*q,helsinki,spacemodifier,
							   time1,time2,timemodifier);

	  float expected = 20.90618;

	  if(std::abs(result-expected)>00.1)
		TEST_FAILED("maximum over helsinki and time failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}

	{
	  MinimumCalculator spacemodifier;
	  MinimumCalculator timemodifier;
	  float result = Integrate(*q,helsinki,spacemodifier,
							   time1,time2,timemodifier);

	  float expected = 11.56234;

	  if(std::abs(result-expected)>0.01)
		TEST_FAILED("minimum over helsinki and time failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}

	{
	  MaximumCalculator spacemodifier;
	  MaximumCalculator timemodifier;
	  float result = Integrate(*q,uusimaa,spacemodifier,
							   time1,time2,timemodifier);

	  float expected = 20.90618;

	  if(std::abs(result-expected)>0.01)
		TEST_FAILED("maximum over uusimaa and time failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}

	{
	  MinimumCalculator spacemodifier;
	  MinimumCalculator timemodifier;
	  float result = Integrate(*q,uusimaa,spacemodifier,
							   time1,time2,timemodifier);

	  float expected = 9.620665;

	  if(std::abs(result-expected)>0.01)
		TEST_FAILED("minimum over uusimaa and time failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}

	{
	  MaximumCalculator spacemodifier;
	  MinimumCalculator timemodifier;
	  float result = Integrate(*q,
							   helsinki,spacemodifier,
							   time1,time2,timemodifier);

	  float expected = 16.58555;

	  if(std::abs(result-expected)>00.1)
		TEST_FAILED("maxmin over helsinki and time failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}

	{
	  MinimumCalculator spacemodifier;
	  MaximumCalculator timemodifier;
	  float result = Integrate(*q,
							   helsinki,spacemodifier,
							   time1,time2,timemodifier);

	  float expected = 19.25029;

	  if(std::abs(result-expected)>0.01)
		TEST_FAILED("minmax over helsinki and time failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}

	{
	  MaximumCalculator spacemodifier;
	  MinimumCalculator timemodifier;
	  float result = Integrate(*q,
							   uusimaa,spacemodifier,
							   time1,time2,timemodifier);

	  float expected = 14.86401;

	  if(std::abs(result-expected)>0.01)
		TEST_FAILED("maxmin over uusimaa and time failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}

	{
	  MinimumCalculator spacemodifier;
	  MaximumCalculator timemodifier;
	  float result = Integrate(*q,
							   uusimaa,spacemodifier,
							   time1,time2,timemodifier);

	  float expected = 18.1207;

	  if(std::abs(result-expected)>0.01)
		TEST_FAILED("minmax over uusimaa and time failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}


	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test Integrate() over space and time
   */
  // ----------------------------------------------------------------------

  void integrate_space_time(void)
  {
	using namespace std;
	using namespace WeatherAnalysis::QueryDataIntegrator;
	using namespace NFmiIndexMaskTools;

	NFmiFastQueryInfo * q = theQD.QueryInfoIter();
	q->First();
	q->Param(kFmiTemperature);
	NFmiMetTime time1 = q->Time();
	NFmiMetTime time2 = time1;
	time2.ChangeByHours(10);

	NFmiIndexMask helsinki = MaskDistance(*theGrid,NFmiPoint(25,60),50);
	NFmiIndexMask uusimaa = MaskInside(*theGrid,theArea);

	{
	  MaximumCalculator spacemodifier;
	  MinimumCalculator timemodifier;
	  float result = Integrate(*q,
							   time1,time2,timemodifier,
							   helsinki,spacemodifier);

	  float expected = 16.58555;

	  if(std::abs(result-expected)>00.1)
		TEST_FAILED("maxmin over time and helsinki failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}

	{
	  MinimumCalculator spacemodifier;
	  MaximumCalculator timemodifier;
	  float result = Integrate(*q,
							   time1,time2,timemodifier,
							   helsinki,spacemodifier);

	  float expected = 19.25029;

	  if(std::abs(result-expected)>0.01)
		TEST_FAILED("minmax over time and helsinki failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}

	{
	  MaximumCalculator spacemodifier;
	  MinimumCalculator timemodifier;
	  float result = Integrate(*q,
							   time1,time2,timemodifier,
							   uusimaa,spacemodifier);

	  float expected = 14.86401;

	  if(std::abs(result-expected)>0.01)
		TEST_FAILED("maxmin over time and uusimaa failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}

	{
	  MinimumCalculator spacemodifier;
	  MaximumCalculator timemodifier;
	  float result = Integrate(*q,
							   time1,time2,timemodifier,
							   uusimaa,spacemodifier);

	  float expected = 18.1207;

	  if(std::abs(result-expected)>0.01)
		TEST_FAILED("minmax over time and uusimaa failed, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}


	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test Integrate() over time and space with changing area
   */
  // ----------------------------------------------------------------------

  void integrate_time_space_masks(void)
  {
	using namespace std;
	using namespace WeatherAnalysis::QueryDataIntegrator;
	using namespace NFmiIndexMaskTools;

	NFmiFastQueryInfo * q = theQD.QueryInfoIter();
	q->First();
	q->Param(kFmiTemperature);
	NFmiMetTime time1 = q->Time();
	NFmiMetTime time2 = time1;
	time2.ChangeByHours(1);

	NFmiIndexMask uusimaa1 = MaskExpand(*theGrid,theArea,0);
	NFmiIndexMask uusimaa2 = MaskExpand(*theGrid,theArea,100);

	NFmiIndexMaskSource source;
	source.Insert(time1,uusimaa1);
	source.Insert(time2,uusimaa2);
	
	{
	  MaximumCalculator spacemodifier;
	  MaximumCalculator timemodifier;
	  float result = Integrate(*q,source,spacemodifier,
							   time1,time2,timemodifier);

	  float expected1 = Integrate(*q,uusimaa1,spacemodifier,
								  time1,time1,timemodifier);
	  float expected2 = Integrate(*q,uusimaa2,spacemodifier,
								  time2,time2,timemodifier);
								
	  float expected = std::max(expected1,expected2);

	  if(std::abs(result-expected)>0.01)
		TEST_FAILED("maximum over uusimaa and time failed with expanding masks, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}

	{
	  MinimumCalculator spacemodifier;
	  MinimumCalculator timemodifier;
	  float result = Integrate(*q,source,spacemodifier,
							   time1,time2,timemodifier);

	  float expected1 = Integrate(*q,uusimaa1,spacemodifier,
								  time1,time1,timemodifier);
	  float expected2 = Integrate(*q,uusimaa2,spacemodifier,
								  time2,time2,timemodifier);
								
	  float expected = std::min(expected1,expected2);

	  if(std::abs(result-expected)>0.01)
		TEST_FAILED("minimum over uusimaa and time failed with expanding masks, got result "+lexical_cast<string>(result)+" instead of "+lexical_cast<string>(expected));
	}


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
	  TEST(integrate_time);
	  TEST(integrate_space);
	  TEST(integrate_time_space);
	  TEST(integrate_space_time);
	  TEST(integrate_time_space_masks);
	}

  }; // class tests

} // QueryDataIntegratorTest


int main(void)
{
  cout << endl
	   << "QueryDataIntegrator tester" << endl
	   << "=========================" << endl;

  QueryDataIntegratorTest::read_querydata("data/skandinavia_pinta.sqd");
  QueryDataIntegratorTest::read_svg();

  QueryDataIntegratorTest::tests t;
  return t.run();
}
