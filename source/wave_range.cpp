// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::WaveStory::range
 */
// ======================================================================

#include "WaveStory.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "MathTools.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "Real.h"
#include "RealRange.h"
#include "Settings.h"
#include "TextGenError.h"
#include "UnitFactory.h"
#include "WeatherResult.h"

#include <boost/lexical_cast.hpp>

#include <vector>

using namespace WeatherAnalysis;
using namespace boost;
using namespace std;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Return wave_ranges story
   *
   * \return Paragraph containing the story
   */
  // ----------------------------------------------------------------------

  const Paragraph WaveStory::range() const
  {
	MessageLogger log("WaveStory::range");
	
	// Establish options
	
	using namespace Settings;
	
	// Generate the story
	
	Paragraph paragraph;
	
	GridForecaster forecaster;
	
	// Calculate wave speeds
	
	const WeatherResult minresult =
	  forecaster.analyze(itsVar+"::fake::height::minimum",
						 itsSources,
						 WaveHeight,
						 Minimum,
						 Maximum,
						 itsArea,
						 itsPeriod);
	
	const WeatherResult maxresult =
	  forecaster.analyze(itsVar+"::fake::height::maximum",
						 itsSources,
						 WaveHeight,
						 Maximum,
						 Maximum,
						 itsArea,
						 itsPeriod);
	
	const WeatherResult meanresult =
	  forecaster.analyze(itsVar+"::fake::height::mean",
						 itsSources,
						 WaveHeight,
						 Mean,
						 Maximum,
						 itsArea,
						 itsPeriod);
	
	
	if(minresult.value() == kFloatMissing ||
	   maxresult.value() == kFloatMissing ||
	   meanresult.value() == kFloatMissing)
	  {
		throw TextGenError("Wave height not available for story wave_range");
	  }
	
	log << "WaveHeight Minimum(Mean)  = " << minresult << endl;
	log << "WaveHeight Maximum(Mean)  = " << maxresult << endl;
	log << "WaveHeight Mean(Mean)     = " << meanresult << endl;
	
	const float roundingprecision = Settings::optional_double(itsVar+"::roundingprecision",0.1);
	const int precision = Settings::optional_int(itsVar+"::precision",1);
	const float mininterval = Settings::optional_double(itsVar+"::mininterval",0.5);
	const string rangeseparator = Settings::optional_string(itsVar+"::rangeseparator","-");

	float n = MathTools::to_float_precision(meanresult.value(),roundingprecision);
	float x = MathTools::to_float_precision(minresult.value(),roundingprecision);
	float y = MathTools::to_float_precision(maxresult.value(),roundingprecision);

	Sentence sentence;
	sentence << "aallonkorkeus"
			 << "on";

	if(x-y < mininterval)
	  {
		sentence << Real(n,precision)
				 << *UnitFactory::create_unit(Meters, static_cast<int>(n));
	  }
	else if(y != round(y))
	  {
		sentence << RealRange(x,y,precision)
				 << *UnitFactory::create(Meters);
	  }
	else
	  {
		int endvalue = static_cast<int>(round(y));
		sentence << RealRange(x,y,precision)
				 << *UnitFactory::create_unit(Meters,endvalue,false);
	  }

	paragraph << sentence;

	log << paragraph;
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
