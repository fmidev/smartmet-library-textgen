// ======================================================================
/*!
 * \file
 *�\brief Implementation of method TextGen::RoadStory::condition_overview
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

#include <map>

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
	 * The numbers are fixed by the Ties��-malli, and must not be changed.
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
	 * \brief Return the importance of a road condition
	 */
	// ----------------------------------------------------------------------

	unsigned int condition_importance(int theCondition)
	{
	  switch(RoadConditionType(theCondition))
		{
		case DRY: return 0;
		case MOIST: return 1;
		case WET: return 2;
		case SNOW: return 3;
		case SLUSH: return 4;
		case FROST: return 5;
		case PARTLY_ICY: return 6;
		case ICY: return 7;
		}
	  throw TextGenError("Unknown road condition in condition_importance");
	}

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

	  const double & operator[](int i) const
	  {
		return itsPercentages[i];
	  }

	  double & operator[](int i)
	  {
		return itsPercentages[i];
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

	const ConditionPercentages
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

	// ----------------------------------------------------------------------
	/*!
	 * \brief Find the most frequent condition type
	 *
	 * Equal cases are resolved by the importance of the type
	 */
	// ----------------------------------------------------------------------

	RoadConditionType
	find_most_general_condition(const ConditionPercentages & thePercentages)
	{
	  int ibest = 0;
	  for(int i=1; i<different_conditions; i++)
		{
		  if(thePercentages[i] > thePercentages[ibest])
			ibest = i;
		  else if(thePercentages[i] == thePercentages[ibest] &&
				  condition_importance(i) > condition_importance(ibest))
			ibest = i;
		}
	  return RoadConditionType(ibest);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Generate a places-phrase for a condition
	 */
	// ----------------------------------------------------------------------

	const char * condition_places_phrase(RoadConditionType theType,
										 double thePercentage,
										 int theGenerallyLimit,
										 int theManyPlacesLimit,
										 int theSomePlacesLimit)
	{
	  if(thePercentage < theSomePlacesLimit)
		return "";
	  if(thePercentage < theManyPlacesLimit)
		return "paikoin";
	  if(thePercentage < theGenerallyLimit)
		return "monin paikoin";

	  switch(theType)
		{
		case DRY:
		case MOIST:
		case WET:
		case SNOW:
		case SLUSH:
		  return "";
		case FROST:
		case PARTLY_ICY:
		case ICY:
		  return "yleisesti";
		}

	  // should never happen
	  return "";
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Generate a single road condition phrase
	 */
	// ----------------------------------------------------------------------

	const Sentence condition_phrase(RoadConditionType theType,
									double thePercentage,
									int theGenerallyLimit,
									int theManyPlacesLimit,
									int theSomePlacesLimit)
	{
	  const char * places_phrase =  condition_places_phrase(theType,thePercentage,
															theGenerallyLimit,
															theManyPlacesLimit,
															theSomePlacesLimit);

	  Sentence tiet_ovat;
	  Sentence teilla_on;
	  tiet_ovat << "tiet ovat" << places_phrase;
	  teilla_on << "teill� on" << places_phrase;

	  switch(theType)
		{
		case DRY:			return (tiet_ovat << "kuivia");
		case MOIST:			return (tiet_ovat << "kosteita");
		case WET:			return (tiet_ovat << "m�rki�");
		case SNOW:			return (teilla_on << "lunta");
		case SLUSH:			return (teilla_on << "sohjoa");
		case FROST:			return (teilla_on << "kuuraa");
		case PARTLY_ICY:	return (tiet_ovat << "osittain j�isi�");
		case ICY:			return (tiet_ovat << "j�isi�");
		}

	  // Unreachable

	  throw TextGenError("Internal error in roadcondition_overview condition_phrase function");

	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Generate the second phrase of the manyplaces-type sentence
	 */
	// ----------------------------------------------------------------------

	const Sentence second_places_sentence(RoadConditionType thePrimaryType,
										  RoadConditionType theSecondaryType)
	{
	  Sentence sentence;
	  switch(thePrimaryType)
		{
		case ICY:
		case PARTLY_ICY:
		  switch(theSecondaryType)
			{
			case PARTLY_ICY:
			  sentence << "paikoin" << "osittain j�isi�";
			  break;
			case FROST:
			  sentence << "paikoin" << "kuuraisia";
			  break;
			case ICY:
			  sentence << "paikoin" << "j�isi�";
			  break;
			case SLUSH:
			case SNOW:
			case WET:
			case MOIST:
			case DRY:
			  break;
			}
		  break;
		case FROST:
		  switch(theSecondaryType)
			{
			case ICY:
			  sentence << "paikoin" << "tiet ovat (sivulause)" << "j�isi�";
			  break;
			case PARTLY_ICY:
			  sentence << "paikoin" << "tiet ovat (sivulause)" << "osittain j�isi�";
			  break;
			case FROST:
			case SLUSH:
			case SNOW:
			case WET:
			case MOIST:
			case DRY:
			  break;
			}
		  break;
		case SLUSH:
		  switch(theSecondaryType)
			{
			case ICY:
			  sentence << "paikoin" << "tiet ovat (sivulause)" << "j�isi�";
			  break;
			case PARTLY_ICY:
			  sentence << "paikoin" << "tiet ovat (sivulause)" << "osittain j�isi�";
			  break;
			case FROST:
			  sentence << "paikoin" << "kuuraa";
			  break;
			  break;
			case SNOW:
			  sentence << "paikoin" << "lunta";
			  break;
			case SLUSH:
			case WET:
			case MOIST:
			case DRY:
			  break;
			}
		  break;
		case SNOW:
		  switch(theSecondaryType)
			{
			case ICY:
			  sentence << "paikoin" << "tiet ovat (sivulause)" << "j�isi�";
			  break;
			case PARTLY_ICY:
			  sentence << "paikoin" << "tiet ovat (sivulause)" << "osittain j�isi�";
			  break;
			case FROST:
			  sentence << "paikoin" << "kuuraa";
			  break;
			case SLUSH:
			  sentence << "paikoin" << "sohjoa";
			  break;
			case SNOW:
			case WET:
			case MOIST:
			case DRY:
			  break;
			}
		  break;
		case WET:
		case MOIST:
		  switch(theSecondaryType)
			{
			case ICY:
			  sentence << "paikoin" << "j�isi�";
			  break;
			case PARTLY_ICY:
			  sentence << "paikoin" << "osittain j�isi�";
			  break;
			case FROST:
			  sentence << "paikoin" << "kuuraisia";
			  break;
			case SLUSH:
			  sentence << "paikoin" << "sohjoisia";
			  break;
			case SNOW:
			  sentence << "paikoin" << "lumisia";
			  break;
			case WET:
			  sentence << "paikoin" << "m�rki�";
			  break;
			case MOIST:
			  sentence << "paikoin" << "kosteita";
			  break;
			case DRY:
			  break;
			}
		  break;
		case DRY:
		  // should never happen
		  break;
		}
	  return sentence;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Generate a sentence from road condition percentages
	 */
	// ----------------------------------------------------------------------

	const Sentence condition_sentence(const ConditionPercentages & thePercentages,
									  const string & theVar)
	{
	  Sentence sentence;

	  // Read the related configuration settings

	  using Settings::optional_percentage;
	  const int generally_limit = optional_percentage(theVar+"::generally_limit",90);
	  const int manyplaces_limit = optional_percentage(theVar+"::manyplaces_limit",50);
	  const int someplaces_limit = optional_percentage(theVar+"::someplaces_limit",10);

	  // Find the most frequent condition

	  RoadConditionType firsttype = find_most_general_condition(thePercentages);

	  // Handle the case when the type dominates the others

	  if(thePercentages[firsttype] >= generally_limit)
		{
		  sentence << condition_phrase(firsttype,
									   thePercentages[firsttype],
									   generally_limit,
									   manyplaces_limit,
									   someplaces_limit);
		  return sentence;
		}

	  // List all "someplaces" types that occur in order of importance
	  // The set may include firsttype, if no condition occurs in many places

	  map<int,RoadConditionType> someplacestypes;

	  for(int i=0; i<different_conditions; i++)
		{
		  const RoadConditionType condition = RoadConditionType(i);
		  
		  if(thePercentages[condition] >= someplaces_limit &&
			 thePercentages[condition] < manyplaces_limit)
			{
			  const int importance = condition_importance(condition);
			  someplacestypes.insert(make_pair(importance,condition));
			}
		}

	  // Handle the case when there is one type in many places
	  // Note that we always ignore it if the "many places" type is DRY,
	  // instead we report on two most important "places" types

	  if(thePercentages[firsttype] >= manyplaces_limit &&
		 firsttype != DRY)
		{
		  sentence << condition_phrase(firsttype,
									   thePercentages[firsttype],
									   generally_limit,
									   manyplaces_limit,
									   someplaces_limit);

		  // Then report the most important "someplaces" condition

		  if(!someplacestypes.empty())
			{
			  Sentence s = second_places_sentence(firsttype,
												  someplacestypes.begin()->second);
			  if(!s.empty())
				sentence << Delimiter(",") << s;
			}
		  return sentence;
		}

	  // Report on the two most important types

	  if(someplacestypes.empty())
		{
		  sentence << condition_phrase(firsttype,
									   thePercentages[firsttype],
									   generally_limit,
									   manyplaces_limit,
									   someplaces_limit);
		  return sentence;
		}

	  firsttype = someplacestypes.begin()->second;

	  sentence << condition_phrase(firsttype,
								   thePercentages[firsttype],
								   generally_limit,
								   manyplaces_limit,
								   someplaces_limit);

	  if(someplacestypes.size() > 1)
		{
		  RoadConditionType secondtype = (++someplacestypes.begin())->second;

		  if(firsttype == ICY || firsttype == PARTLY_ICY)
			{
			  if(secondtype == PARTLY_ICY)
				sentence << "tai" << "osittain j�isi�";
			  else if(secondtype == FROST)
				sentence << "tai" << "kuuraisia";
			}
		  else if(firsttype == SLUSH && secondtype == SNOW)
			sentence << "tai" << "lunta";
		  else if(firsttype == WET && secondtype == MOIST)
			sentence << "tai" << "kosteita";
		}

	  return sentence;
	}

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on road conditions
   *
   * \return The story
   *
   *�\see page_roadcondition_overview
   */
  // ----------------------------------------------------------------------
  
  const Paragraph RoadStory::condition_overview() const
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
