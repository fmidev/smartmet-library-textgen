// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::TemperatureStory::meanmax
 */
// ======================================================================

#include "ak_TemperatureStory.h"
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
   * \brief Generate story on mean maximum temperature
   *
   * \return The story
   *
   * \see page_temperature_meanmax
   */
  // ----------------------------------------------------------------------

  const Paragraph TemperatureStory::meanmax() const
  {
	MessageLogger log("TemperatureStory::meanmax");

	Paragraph paragraph;
	Sentence sentence;

	GridForecaster forecaster;

	HourPeriodGenerator periods(itsPeriod,06,18,06,18);

	WeatherResult result = forecaster.analyze(itsVar+"::fake::mean",
											  itsSources,
											  Temperature,
											  Mean,
											  Mean,
											  Maximum,
											  itsArea,
											  periods);

	if(result.value() == kFloatMissing)
	  throw TextGenError("Mean daily maximum temperature not available");

	log << "Temperature Mean(Mean(Maximum())) = " << result << endl;

	sentence << "keskimääräinen ylin lämpötila"
			 << Integer(static_cast<int>(round(result.value())))
			 << *UnitFactory::create(DegreesCelsius);
	paragraph << sentence;
	log << paragraph;
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
