// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::PrecipitationStory::sums
 */
// ======================================================================

#include "PrecipitationStory.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PrecipitationStoryTools.h"
#include "RangeAcceptor.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "TimeTools.h"
#include "UnitFactory.h"
#include "WeatherResult.h"

#include "boost/lexical_cast.hpp"

#include <vector>

using namespace boost;
using namespace std;
using namespace WeatherAnalysis;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on total precipitation range
   *
   * \return The story
   */
  // ----------------------------------------------------------------------

  const Paragraph PrecipitationStory::sums() const
  {
	MessageLogger log("PrecipitationStory::sums");

	const double minrain = Settings::optional_double(itsVar+"::minrain",0);
	const int mininterval = Settings::optional_int(itsVar+"::mininterval",1);

	Paragraph paragraph;

	GridForecaster forecaster;

	const NFmiTime time1 = itsPeriod.localStartTime();
	const NFmiTime time2 = TimeTools::addHours(time1,12);
	const NFmiTime time3 = TimeTools::addHours(time1,24);

	log << "Period 1 = " << time1 << " ... " << time2 << endl;
	log << "Period 2 = " << time2 << " ... " << time3 << endl;

	vector<WeatherPeriod> periods;
	periods.push_back(WeatherPeriod(time1,time2));
	periods.push_back(WeatherPeriod(time2,time3));

	RangeAcceptor rainlimits;
	rainlimits.lowerLimit(minrain);

	vector<int> minima;
	vector<int> maxima;
	vector<int> means;

	for(vector<int>::size_type i = 0; i<periods.size(); i++)
	  {
		const string fake = itsVar+"::fake::period"+lexical_cast<string>(i+1);

		WeatherResult minresult = forecaster.analyze(fake+"::minimum",
													 itsSources,
													 Precipitation,
													 Minimum,
													 Sum,
													 itsArea,
													 periods[i],
													 DefaultAcceptor(),
													 rainlimits);
		
		WeatherResult maxresult = forecaster.analyze(fake+"::maximum",
													 itsSources,
													 Precipitation,
													 Maximum,
													 Sum,
													 itsArea,
													 periods[i],
													 DefaultAcceptor(),
													 rainlimits);

		WeatherResult meanresult = forecaster.analyze(fake+"::mean",
													  itsSources,
													  Precipitation,
													  Mean,
													  Sum,
													  itsArea,
													  periods[i],
													  DefaultAcceptor(),
													  rainlimits);
		
		log << "Precipitation Minimum(Sum) for period " << i+1 << " = " << minresult << endl;
		log << "Precipitation Maximum(Sum) for period " << i+1 << " = " << maxresult << endl;
		log << "Precipitation Mean(Sum) for period " << i+1 << " = " << meanresult << endl;

		if(minresult.value() == kFloatMissing ||
		   maxresult.value() == kFloatMissing ||
		   meanresult.value() == kFloatMissing)
		  throw TextGenError("Total precipitation not available");

		minima.push_back(FmiRound(minresult.value()));
		maxima.push_back(FmiRound(maxresult.value()));
		means.push_back(FmiRound(meanresult.value()));
	  }

	Sentence sentence;
	if(maxima[0]==0 && maxima[1]==0)
	  {
		sentence << "seuraavan 24 tunnin sademäärä"
				 << "on"
				 << PrecipitationStoryTools::sum_phrase(0,
														0,
														0,
														mininterval);
	  }
	else
	  {
		sentence << "ensimmäisen 12 tunnin sademäärä"
				 << "on"
				 << PrecipitationStoryTools::sum_phrase(minima[0],
														maxima[0],
														means[0],
														mininterval)
				 << Delimiter(",")
				 << "seuraavan 12 tunnin"
				 << PrecipitationStoryTools::sum_phrase(minima[1],
														maxima[1],
														means[1],
														mininterval);
	  }

	paragraph << sentence;
	log << paragraph;
	return paragraph;

  }

} // namespace TextGen
  
// ======================================================================
  
