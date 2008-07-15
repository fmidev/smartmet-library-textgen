// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::TemperatureStory::meanmin
 */
// ======================================================================

#include "TemperatureStory.h"
#include "GridForecaster.h"
#include "HourPeriodGenerator.h"
#include "Integer.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "TextGenError.h"
#include "UnitFactory.h"
#include "WeatherResult.h"

using namespace WeatherAnalysis;
using namespace std;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on mean minimum temperature
   *
   * Throws if the weather period does not evenly divide into 24 hour
   * segments.
   *
   * \return The story
   *
   * \see page_temperature_meanmin
   */
  // ----------------------------------------------------------------------

  const Paragraph TemperatureStory::meanmin() const
  {
	MessageLogger log("TemperatureStory::meanmin");

	Paragraph paragraph;
	Sentence sentence;

	GridForecaster forecaster;

	HourPeriodGenerator periods(itsPeriod,18,06,18,06);

	WeatherResult result = forecaster.analyze(itsVar+"::fake::mean",
											  itsSources,
											  Temperature,
											  Mean,
											  Mean,
											  Minimum,
											  itsArea,
											  periods);

	if(result.value() == kFloatMissing)
	  throw TextGenError("Mean daily minimum temperature not available");

	log << "Temperature Mean(Mean(Minimum())) = " << result << endl;

	sentence << "keskimääräinen alin lämpötila"
			 << Integer(static_cast<int>(round(result.value())))
			 << *UnitFactory::create(DegreesCelsius);
	paragraph << sentence;
	log << paragraph;
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
