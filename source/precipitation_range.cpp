// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::PrecipitationStory::range
 */
// ======================================================================

#include "PrecipitationStory.h"
#include "GridForecaster.h"
#include "Integer.h"
#include "IntegerRange.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "RangeAcceptor.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "UnitFactory.h"
#include "WeatherResult.h"

using namespace boost;
using namespace std;
using namespace WeatherAnalysis;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on total precipitation range
   *
   * Sample story 1: "Sadesumma yli LIMIT millimetriä."	(when min>=LIMIT)
   * Sample story 2: "Sadesumma 4 millimetriä."			(when min==max)
   * Sample story 3: "Sadesumma 1-5 millimetriä."		(otherwise)
   *
   * where LIMIT is the value of textgen::precipitation_range::maxrain
   *
   * \return The story
   */
  // ----------------------------------------------------------------------

  const Paragraph PrecipitationStory::range() const
  {
	MessageLogger log("PrecipitationStory::range");

	Paragraph paragraph;
	Sentence sentence;

	GridForecaster forecaster;

	RangeAcceptor rainlimits;
	rainlimits.lowerLimit(Settings::optional_double(itsVar+"::minrain",0));

	WeatherResult minresult = forecaster.analyze(itsVar+"::fake::minimum",
												 itsSources,
												 Precipitation,
												 Minimum,
												 Sum,
												 itsArea,
												 itsPeriod,
												 DefaultAcceptor(),
												 rainlimits);

	WeatherResult maxresult = forecaster.analyze(itsVar+"::fake::maximum",
												 itsSources,
												 Precipitation,
												 Maximum,
												 Sum,
												 itsArea,
												 itsPeriod,
												 DefaultAcceptor(),
												 rainlimits);

	if(minresult.value() == kFloatMissing ||
	   maxresult.value() == kFloatMissing)
	  throw TextGenError("Total precipitation not available");

	log << "Precipitation Minimum(Sum) " << minresult << endl;
	log << "Precipitation Maximum(Sum) " << maxresult << endl;

	const int minrain = FmiRound(minresult.value());
	const int maxrain = FmiRound(maxresult.value());

	// optionaalinen maksimisade

	const string variable = itsVar + "::maxrain";
	const int rainlimit = Settings::optional_int(variable,-1);

	if(minrain >= rainlimit && rainlimit>0)
	  {
		sentence << "sadesumma"
				 << "on"
				 << "yli"
				 << Integer(rainlimit)
				 << *UnitFactory::create(Millimeters);
	  }
	else
	  {
		sentence << "sadesumma"
				 << "on"
				 << IntegerRange(minrain,maxrain)
				 << *UnitFactory::create(Millimeters);
	  }

	paragraph << sentence;
	log << paragraph;
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
  
