// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::WeatherStory::thunderprobability
 */
// ======================================================================

#include "WeatherStory.h"
#include "GridForecaster.h"
#include "Integer.h"
#include "MathTools.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "UnitFactory.h"
#include "WeatherPeriodTools.h"
#include "WeatherResult.h"

using namespace WeatherAnalysis;
using namespace std;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on thunder probability
   *
   * \return The story
   *
   * \see page_weather_thunderprobability
   */
  // ----------------------------------------------------------------------

  Paragraph WeatherStory::thunderprobability() const
  {
	MessageLogger log("WeatherStory::thunderprobability");

	using namespace Settings;
	using namespace WeatherPeriodTools;

	Paragraph paragraph;

	const int precision = optional_percentage(itsVar+"::precision",10);
	const int limit = optional_percentage(itsVar+"::limit",10);

	GridForecaster forecaster;

	WeatherResult result = forecaster.analyze(itsVar+"::fake::probability",
											 itsSources,
											 Thunder,
											 Maximum,
											 Maximum,
											 itsArea,
											 itsPeriod);

	if(result.value() == kFloatMissing)
	  throw TextGenError("Thunder is not available");

	log << "Thunder Maximum(Maximum) = " << result << endl;

	const int probability = MathTools::to_precision(result.value(),
													precision);


	if(probability >= limit)
	  {
		Sentence sentence;
		sentence << "ukkosen todennakoisyys"
				 << "on"
				 << Integer(probability)
				 << *UnitFactory::create(Percent);
		paragraph << sentence;
	  }

	log << paragraph;
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
