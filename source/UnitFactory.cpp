// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::UnitFactory
 */
// ======================================================================

#include "UnitFactory.h"
#include "Delimiter.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"

#include <string>

using namespace std;
using namespace boost;

namespace
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the DegreesCelsius sentence
   *
   * \return The sentence
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<TextGen::Sentence> degrees_celsius()
  {
	using namespace TextGen;

	const string var = "textgen::units::celsius::format";
	const string opt = Settings::optional_string(var,"SI");

	shared_ptr<Sentence> sentence(new Sentence);

	if(opt == "SI")
	  *sentence << Delimiter("\176C");
	else if(opt == "phrase")
	  *sentence << "astetta";
	else
	  throw TextGenError("Unknown format "+opt+" in variable "+var);

	return sentence;

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the MetersPerSecond sentence
   *
   * \return The sentence
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<TextGen::Sentence> meters_per_second()
  {
	using namespace TextGen;

	const string var = "textgen::units::meterspersecond::format";
	const string opt = Settings::optional_string(var,"SI");

	shared_ptr<Sentence> sentence(new Sentence);

	if(opt == "SI")
	  *sentence << "m/s";
	else if(opt == "phrase")
	  *sentence << "metriä sekunnissa";
	else
	  throw TextGenError("Unknown format "+opt+" in variable "+var);

	return sentence;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the Millimeters sentence
   *
   * \return The sentence
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<TextGen::Sentence> millimeters()
  {
	using namespace TextGen;

	const string var = "textgen::units::millimeters::format";
	const string opt = Settings::optional_string(var,"SI");

	shared_ptr<Sentence> sentence(new Sentence);

	if(opt == "SI")
	  *sentence << Delimiter("mm");
	else if(opt == "phrase")
	  *sentence << "millimetriä";
	else
	  throw TextGenError("Unknown format "+opt+" in variable "+var);

	return sentence;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the Percent sentence
   *
   * \return The sentence
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<TextGen::Sentence> percent()
  {
	using namespace TextGen;

	const string var = "textgen::units::percent::format";
	const string opt = Settings::optional_string(var,"SI");

	shared_ptr<Sentence> sentence(new Sentence);

	if(opt == "SI")
	  *sentence << Delimiter("%");
	else if(opt == "phrase")
	  *sentence << "prosenttia";
	else
	  throw TextGenError("Unknown format "+opt+" in variable "+var);
	return sentence;
  }

}

namespace TextGen
{


  namespace UnitFactory
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the formatted sentence for the given unit
	 *
	 * \param theUnit The desired unit
	 * \return The sentence
	 */
	// ----------------------------------------------------------------------

	boost::shared_ptr<Sentence> create(Units theUnit)
	{
	  switch(theUnit)
		{
		case DegreesCelsius:
		  return degrees_celsius();
		case MetersPerSecond:
		  return meters_per_second();
		case Millimeters:
		  return millimeters();
		case Percent:
		  return percent();
		}

	  throw TextGenError("UnitFactory::create failed - unknown unit");

	}

  } // namespace UnitFactory
} // namespace TextGen

// ======================================================================
