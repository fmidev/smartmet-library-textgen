// ======================================================================
/*!
 * \file
 *†\brief Implementation of method TextGen::RoadStory::condition_overview
 */
// ======================================================================

#include "RoadStory.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "MessageLogger.h"
#include "MorningAndEveningPeriodGenerator.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "TimeTools.h"
#include "ValueAcceptor.h"
#include "WeatherResult.h"

#include "boost/lexical_cast.hpp"

using namespace WeatherAnalysis;
using namespace boost;
using namespace std;

namespace TextGen
{

  namespace
  {
	// ----------------------------------------------------------------------
	/*!
	 * \brief The various road conditions
	 *
	 * The numbers are fixed by the Ties‰‰-malli, and must not be changed.
	 */
	// ----------------------------------------------------------------------

	enum RoadConditionType
	  {
		DRY,
		MOIST,
		WET,
		SLUSH,
		FROST,
		PARTLY_ICY,
		ICY,
		SNOW
	  };

	const int different_conditions = SNOW+1;

	// ----------------------------------------------------------------------
	/*!
	 * \brief Convert road condition enumeration to string
	 */
	// ----------------------------------------------------------------------

	const char * condition_name(RoadConditionType theCondition)
	{
	  switch(theCondition)
		{
		case DRY: return "dry";
		case MOIST: return "moist";
		case WET: return "wet";
		case SLUSH: return "slush";
		case FROST: return "frost";
		case PARTLY_ICY: return "partly_icy";
		case ICY: return "icy";
		case SNOW: return "snow";
		}
	  throw TextGenError("Unknown road condition in condition_name");
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Road condition percentages
	 */
	// ----------------------------------------------------------------------

	class ConditionPercentages
	{
	public:

	  ~ConditionPercentages()
	  { }

	  ConditionPercentages()
		: itsPercentages(different_conditions,0)
	  { }

	  const double & operator[](RoadConditionType c) const
	  {
		return itsPercentages[c];
	  }

	  double & operator[](RoadConditionType c)
	  {
		return itsPercentages[c];
	  }

	private:

	  std::vector<double> itsPercentages;
	};

	// ----------------------------------------------------------------------
	/*!
	 * \brief Calculate road condition percentages for given period
	 *
	 * \todo Optimize the integration loop so that you first calculate
	 *       the minimum and maximum road condition, or possibly just
	 *       the maximum, and then loop over those values only.
	 */
	// ----------------------------------------------------------------------

	ConditionPercentages
	calculate_percentages(const WeatherPeriod & thePeriod,
						  int thePeriodIndex,
						  const WeatherAnalysis::AnalysisSources & theSources,
						  const WeatherAnalysis::WeatherArea & theArea,
						  const std::string & theVar)
	{
	  GridForecaster forecaster;

	  ConditionPercentages percentages;
	  for(int i=0; i<different_conditions; i++)
		{
		  const RoadConditionType c = RoadConditionType(i);

		  const string fake = (theVar +
							   "::fake::period" +
							   lexical_cast<string>(thePeriodIndex) +
							   "::" +
							   condition_name(c) +
							   "::percentage");

		  ValueAcceptor condfilter;
		  condfilter.value(c);

		  WeatherResult result = forecaster.analyze(fake,
													theSources,
													RoadCondition,
													Mean,
													Percentage,
													theArea,
													thePeriod,
													DefaultAcceptor(),
													DefaultAcceptor(),
													condfilter);

		  percentages[c] = result.value();
		}

	  return percentages;

	}

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on road conditions
   *
   * \return The story
   *
   *†\see page_roadcondition_overview
   */
  // ----------------------------------------------------------------------

  Paragraph RoadStory::condition_overview() const
  {
	MessageLogger log("RoadStory::condition_overview");

	Paragraph paragraph;

	// the period we wish to analyze is at most 30 hours,
	// but of course must not exceed the period length itself

	const NFmiTime time1(itsPeriod.localStartTime());
	NFmiTime time2 = TimeTools::addHours(time1,30);
	if(itsPeriod.localEndTime().IsLessThan(time2))
	  time2 = itsPeriod.localEndTime();

	const WeatherPeriod fullperiod(time1,time2);

	log << "Analysis period is " << time1 << " ... " << time2 << endl;

	// Calculate the results for each period

	MorningAndEveningPeriodGenerator generator(fullperiod, itsVar);

	if(generator.size()==0)
	  {
		log << "Analysis period is too short!" << endl;
		log << paragraph;
		return paragraph;
	  }

	vector<WeatherPeriod> periods;
	vector<ConditionPercentages> percentages;

	for(unsigned int i=1; i<=generator.size(); i++)
	  {
		const WeatherPeriod period = generator.period(i);

		ConditionPercentages result = calculate_percentages(period,
															i,
															itsSources,
															itsArea,
															itsVar);
		periods.push_back(period);
		percentages.push_back(result);
	  }

	log << paragraph;
	return paragraph;

  }

} // namespace TextGen
  
// ======================================================================
