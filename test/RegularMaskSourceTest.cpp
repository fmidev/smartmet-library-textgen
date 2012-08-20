#include <regression/tframe.h>
#include <newbase/NFmiIndexMask.h>
#include <newbase/NFmiStringTools.h>
#include "RegularMaskSource.h"
#include "UserWeatherSource.h"
#include "WeatherArea.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost;

namespace RegularMaskSourceTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test RegularMaskSource::mask
   */
  // ----------------------------------------------------------------------

  void mask()
  {
	using namespace TextGen;
	using NFmiStringTools::Convert;
	typedef RegularMaskSource::mask_type mask_type;

	const string datafile = "data/skandinavia_pinta.sqd";

	const WeatherArea uusimaa("data/uusimaa.svg");
	const WeatherArea ahvenanmaa("data/ahvenanmaa.svg");
	const WeatherArea ahvenanmaa15("data/ahvenanmaa.svg:15");
	const WeatherArea pohjois_lappi("data/pohjois-lappi.svg");

	// Initialize all the data

	shared_ptr<NFmiStreamQueryData> sqd(new NFmiStreamQueryData());
	if(!sqd->ReadData(datafile))
	  throw runtime_error("Failed to read '"+datafile+"'");

	UserWeatherSource wsource;
	wsource.insert("data",sqd);

	// The mask source itself

	RegularMaskSource source;
	
	// Begin testing

	{
	  mask_type m = source.mask(uusimaa,"data",wsource);
	  if(m->size() != 8)
		TEST_FAILED("Size of mask uusimaa should be 8, not "+Convert(m->size()));
	}

	{
	  mask_type m = source.mask(ahvenanmaa,"data",wsource);
	  if(m->size() != 0)
		TEST_FAILED("Size of mask ahvenanmaa should be 0, not "+Convert(m->size()));
	}

	{
	  mask_type m = source.mask(ahvenanmaa15,"data",wsource);
	  if(m->size() != 5)
		TEST_FAILED("Size of mask ahvenanmaa:15 should be 5, not "+Convert(m->size()));
	}

	{
	  mask_type m = source.mask(pohjois_lappi,"data",wsource);
	  if(m->size() != 26)
		TEST_FAILED("Size of mask pohjois_lappi should be 26, not "+Convert(m->size()));
	}

	TEST_PASSED();

  }

  //! The actual test driver
  class tests : public tframe::tests
  {
	//! Overridden message separator
	virtual const char * error_message_prefix() const
	{
	  return "\n\t";
	}

	//! Main test suite
	void test(void)
	{
	  TEST(mask);
	}

  }; // class tests

} // namespace RegularMaskSourceTest


int main(void)
{
  cout << endl
	   << "RegularMaskSource tester" << endl
	   << "========================" << endl;
  RegularMaskSourceTest::tests t;
  return t.run();
}
