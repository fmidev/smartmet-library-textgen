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

	  TEST(precipitation_total);
	  TEST(precipitation_range);
	  TEST(precipitation_classification);

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
