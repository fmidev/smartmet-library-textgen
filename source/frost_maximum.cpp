// ======================================================================
/*!
 * \file
 *�\brief Implementation of method TextGen::FrostStory::maximum
 */
// ======================================================================

#include "FrostStory.h"
#include "GridForecaster.h"
#include "Integer.h"
#include "MathTools.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "UnitFactory.h"
#include "WeatherResult.h"

using namespace std;
using namespace WeatherAnalysis;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on maximum frost
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  
  Paragraph FrostStory::maximum() const
  {
	MessageLogger log("FrostStory::maximum");

	using MathTools::to_precision;

	Paragraph paragraph;
	Sentence sentence;

	const string var1 = itsVar+"::precision";
	const string var2 = itsVar+"::severe_frost_limit";
	const string var3 = itsVar+"::frost_limit";

	const int precision = Settings::require_percentage(var1);
	const int severelimit = Settings::require_percentage(var2);
	const int normallimit = Settings::require_percentage(var3);

	GridForecaster forecaster;

	WeatherResult frost = forecaster.analyze(itsVar+"::fake::maximum",
											 itsSources,
											 Frost,
											 Maximum,
											 Maximum,
											 itsArea,
											 itsPeriod);

	if(frost.value() == kFloatMissing)
	  throw TextGenError("Frost is not available");

	log << "Frost Maximum(Maximum) is " << frost << endl;

	// Quick exit if the mean is zero
	
	if(frost.value() == 0)
	  {
		log << paragraph;
		return paragraph;
	  }

	// Severe frost

	WeatherResult severefrost = forecaster.analyze(itsVar+"::fake::severe_maximum",
												   itsSources,
												   SevereFrost,
												   Maximum,
												   Maximum,
												   itsArea,
												   itsPeriod);

	if(severefrost.value() == kFloatMissing)
	  throw TextGenError("SevereFrost is not available");

	log << "SevereFrost Maximum(Maximum) is " << severefrost << endl;

	const int frost_value = to_precision(frost.value(),precision);

	const int severe_frost_value = to_precision(severefrost.value(),precision);

	if(severe_frost_value >= severelimit)
	  {
		sentence << "ankaran hallan todenn�k�isyys"
				 << "on"
				 << Integer(severe_frost_value)
				 << *UnitFactory::create(Percent);
		paragraph << sentence;
	  }
	else if(frost_value >= normallimit)
	  {
		sentence << "hallan todenn�k�isyys"
				 << "on"
				 << Integer(frost_value)
				 << *UnitFactory::create(Percent);
		paragraph << sentence;
	  }

	log << paragraph;
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
  