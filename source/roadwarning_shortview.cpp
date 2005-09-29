// ======================================================================
/*!
 * \file
 *†\brief Implementation of method TextGen::RoadStory::warning_shortview
 */
// ======================================================================

#include "RoadStory.h"
#include "DebugTextFormatter.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "MessageLogger.h"
#include "HourPeriodGenerator.h"
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
	 * \brief The various road warnings
	 *
	 * The numbers are fixed by the Ties‰‰-malli, and must not be changed.
	 */
	// ----------------------------------------------------------------------

	enum RoadWarningType
	  {
	        NORMAL = 0,
		FROSTY,
		PARTLY_ICING,
		ICING,
		FROSTSLIPPERY,
		FASTWORSENING,
		ICYRAIN,
		SLEET_TO_PARTLY_ICY,
		SLEET_TO_ICY,
		RAIN_TO_PARTLY_ICY,
		RAIN_TO_ICY
	  };

	const int min_warning = NORMAL;
	const int max_warning = RAIN_TO_ICY;

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the importance of a road warning
	 */
	// ----------------------------------------------------------------------

	unsigned int warning_importance(int theWarning)
	{
	  switch(RoadWarningType(theWarning))
		{
		case NORMAL: return 0;
		case FROSTY: return 1;
		case PARTLY_ICING: return 2;
		case ICING: return 3;
		case FROSTSLIPPERY: return 4;
		case FASTWORSENING: return 5;
		case ICYRAIN: return 6;
		case SLEET_TO_PARTLY_ICY: return 7;
		case SLEET_TO_ICY: return 8;
		case RAIN_TO_PARTLY_ICY: return 9;
		case RAIN_TO_ICY: return 10;
		}
	  throw TextGenError("Unknown road warning in warning_importance");
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Convert road warning enumeration to string
	 */
	// ----------------------------------------------------------------------

	const char * warning_name(RoadWarningType theWarning)
	{
	  switch(theWarning)
		{
		case NORMAL: return "normal";
                case FROSTY: return "frosty";
                case PARTLY_ICING: return "partly_icing";
		case ICING: return "icing";
		case FROSTSLIPPERY: return "frostslippery";
		case FASTWORSENING: return "fastworsening";
		case ICYRAIN: return "icyrain";
		case SLEET_TO_PARTLY_ICY: return "sleet_to_partly_icy";
		case SLEET_TO_ICY: return "sleet_to_icy";
		case RAIN_TO_PARTLY_ICY: return "rain_to_partly_icy";
		case RAIN_TO_ICY: return "rain_to_icy";
		}
	  throw TextGenError("Unknown road warning in warning_name");
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Road warning percentages
	 */
	// ----------------------------------------------------------------------

	class WarningPercentages
	{
	public:

	  ~WarningPercentages()
	  { }

	  WarningPercentages()
		: itsPercentages(max_warning-min_warning+1,0)
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
	 * \brief Calculate road warning percentages for given period
	 *
	 * \todo Optimize the integration loop so that you first calculate
	 *       the minimum and maximum road warning, or possibly just
	 *       the maximum, and then loop over those values only.
	 */
	// ----------------------------------------------------------------------

	const WarningPercentages
	calculate_percentages(const WeatherPeriod & thePeriod,
						  int thePeriodIndex,
						  const WeatherAnalysis::AnalysisSources & theSources,
						  const WeatherAnalysis::WeatherArea & theArea,
						  const std::string & theVar)
	{
	  GridForecaster forecaster;

	  WarningPercentages percentages;
	  for(int i=min_warning; i<=max_warning; i++)
		{
		  const RoadWarningType c = RoadWarningType(i);

		  const string fake = (theVar +
							   "::fake::period" +
							   lexical_cast<string>(thePeriodIndex) +
							   "::" +
							   warning_name(c) +
							   "::percentage");

		  ValueAcceptor warnfilter;
		  warnfilter.value(c);

		  WeatherResult result = forecaster.analyze(fake,
							theSources,
							RoadWarning,
							Maximum, //Mean -> Maximum PI 7.9.05
							Percentage,
							theArea,
							thePeriod,
							DefaultAcceptor(),
							DefaultAcceptor(),
							warnfilter);

		  percentages[c] = result.value();
		}

	  return percentages;

	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Find the most frequent warning type
	 *
	 * Equal cases are resolved by the importance of the type
	 */
	// ----------------------------------------------------------------------

	RoadWarningType
	find_most_general_warning(const WarningPercentages & thePercentages)
	{
	  int ibest = min_warning;
	  for(int i=min_warning+1; i<=max_warning; i++)
		{
		  if(thePercentages[i] > thePercentages[ibest])
			ibest = i;
		  else if(thePercentages[i] == thePercentages[ibest] &&
				  warning_importance(i) > warning_importance(ibest))
			ibest = i;
		}
	  return RoadWarningType(ibest);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Generate a places-phrase for a warning
	 */
	// ----------------------------------------------------------------------

	const char * warning_places_phrase(RoadWarningType theType,
										 double thePercentage,
										 int theGenerallyLimit,
										 int theManyPlacesLimit,
										 int theSomePlacesLimit)
	{
	  if(thePercentage < theSomePlacesLimit)
		return "";
	  else
	  if(thePercentage < theManyPlacesLimit)
		return "paikoin";
	  else
	  if(thePercentage < theGenerallyLimit)
		return "monin paikoin";
	  else
		  return "yleisesti";
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Generate a single road warning phrase
	 */
	// ----------------------------------------------------------------------

	const Sentence warning_phrase(RoadWarningType theType,
									double thePercentage,
									int theGenerallyLimit,
									int theManyPlacesLimit,
									int theSomePlacesLimit)
	{
	  const char * places_phrase =  warning_places_phrase(theType,thePercentage,
															theGenerallyLimit,
															theManyPlacesLimit,
			
															theSomePlacesLimit);

	  Sentence tienpinnat_muuttuvat;
	  Sentence teilla_on;
	  Sentence keli_on;
	  Sentence keli_muuttuu;
	  tienpinnat_muuttuvat << "tienpinnat muuttuvat" << places_phrase;
	  teilla_on << "teill‰ on" << places_phrase;
	  keli_on << "keli on" <<  places_phrase;
	  keli_muuttuu << "keli muuttuu" << places_phrase;

	  switch(theType)
		{
		case NORMAL:			return (keli_on << "normaali");
		case FROSTY:			return (tienpinnat_muuttuvat << "kuuraisiksi");
		case PARTLY_ICING:		return (tienpinnat_muuttuvat << "osittain j‰isiksi");
		case ICING:			return (tienpinnat_muuttuvat << "j‰isiksi");
		case FROSTSLIPPERY:		return (teilla_on << "pakkasliukkautta");
		case FASTWORSENING:		return (keli_muuttuu << "lumisateesta");
		case ICYRAIN:			return (tienpinnat_muuttuvat << "liukkaiksi j‰‰t‰v‰st‰ sateesta");
		case SLEET_TO_PARTLY_ICY:	return (keli_on << "r‰nn‰st‰ ja osittain liukkaista");
		case SLEET_TO_ICY:		return (keli_on << "r‰nn‰st‰ ja liukkaista");
		case RAIN_TO_PARTLY_ICY:	return (keli_on << "sateesta ja osittain liukkaista");
		case RAIN_TO_ICY: 		return (keli_on << "sateesta ja liukkaista");
		}

	  // Unreachable

	  throw TextGenError("Internal error in roadwarning_shortview warning_phrase function");

	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Generate the second phrase of the manyplaces-type sentence
	 */
	// ----------------------------------------------------------------------

	const Sentence second_places_sentence(RoadWarningType thePrimaryType,
										  RoadWarningType theSecondaryType)
	{
	  Sentence sentence;


	  /*
	   case RAIN_TO_ICY:
	   case RAIN_TO_PARTLY_ICY:
	   case SLEET_TO_ICY:
	   case SLEET_TO_PARTLY_ICY:
	   case ICYRAIN:
	   case FASTWORSENING:
	   case FROSTSLIPPERY:
	   case ICING:
	   case PARTLY_ICING:
	   case FROSTY:
	                                                                                                                                                                   */
	  switch(thePrimaryType)
		{
		case RAIN_TO_ICY:
		case RAIN_TO_PARTLY_ICY:
		case SLEET_TO_ICY:
		case SLEET_TO_PARTLY_ICY:
		case ICYRAIN:
		case FASTWORSENING:
		  switch(theSecondaryType)
			{
          		     case RAIN_TO_ICY:
		             case RAIN_TO_PARTLY_ICY:
		             case SLEET_TO_ICY:
		             case SLEET_TO_PARTLY_ICY:
		             case ICYRAIN:
		             case FASTWORSENING:
		             case FROSTSLIPPERY:
				     sentence << "paikoin" << "teill‰ on" << "pakkasliukkautta";
				     break;
		             case ICING:
				     sentence << "paikoin" << "tienpinnat muuttuvat" << "j‰isiksi";
				     break;
		             case PARTLY_ICING:
				     sentence << "paikoin" << "tienpinnat muuttuvat" << "osittain j‰isiksi";
				     break;
		             case FROSTY:
				     sentence << "paikoin" << "tienpinnat muuttuvat" << "kuuraisiksi";
				     break;
			     case NORMAL:
				     break;
			}
		  break;
		case FROSTSLIPPERY:
		case ICING:
		  switch(theSecondaryType)
		  {
			 case RAIN_TO_ICY:
			 case RAIN_TO_PARTLY_ICY:
			 case SLEET_TO_ICY:
			 case SLEET_TO_PARTLY_ICY:
			 case ICYRAIN:
			 case FASTWORSENING:
			 case FROSTSLIPPERY:
			 case ICING:
			  case PARTLY_ICING:
				  sentence << "paikoin" << "osittain j‰isiksi";
				  break;
			 case FROSTY:
				  sentence << "paikoin" << "kuuraisiksi";
				  break;
			 case NORMAL:
				  break;
		  }
		  break;
		case PARTLY_ICING:
		  switch(theSecondaryType)
		  {
          		case RAIN_TO_ICY:
		        case RAIN_TO_PARTLY_ICY:
		        case SLEET_TO_ICY:
		        case SLEET_TO_PARTLY_ICY:
		        case ICYRAIN:
		        case FASTWORSENING:
		        case FROSTSLIPPERY:
		        case ICING:
			case PARTLY_ICING:
			  case FROSTY:
				  sentence << "paikoin" << "kuuraisiksi";
				  break;
			  case NORMAL:
				  break;
		  }
		  break;
		case FROSTY:
		case NORMAL:
		  break;
		}
	  return sentence;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Generate a sentence from road warning percentages
	 */
	// ----------------------------------------------------------------------

	const Sentence warning_sentence(const WarningPercentages & thePercentages,
									  const string & theVar)
	{
	  Sentence sentence;

	  // Read the related configuration settings

	  using Settings::optional_percentage;
	  const int generally_limit = optional_percentage(theVar+"::generally_limit",90);
	  const int manyplaces_limit = optional_percentage(theVar+"::manyplaces_limit",50);
	  const int someplaces_limit = optional_percentage(theVar+"::someplaces_limit",10);

	  // Find the most frequent warning

	  RoadWarningType firsttype = find_most_general_warning(thePercentages);

	  // Handle the case when the type dominates the others

	  if(thePercentages[firsttype] >= generally_limit)
		{
		  sentence << warning_phrase(firsttype,
									   thePercentages[firsttype],
									   generally_limit,
									   manyplaces_limit,
									   someplaces_limit);
		  return sentence;
		}

	  // List all "someplaces" types that occur in order of importance
	  // The set may include firsttype, if no condition occurs in many places

	  map<int,RoadWarningType,std::greater<int> > someplacestypes;

	  for(int i=min_warning; i<=max_warning; i++)
		{
		  const RoadWarningType warning = RoadWarningType(i);
		  
		  if(thePercentages[warning] >= someplaces_limit &&
			 thePercentages[warning] < manyplaces_limit)
			{
			  const int importance = warning_importance(warning);
			  someplacestypes.insert(make_pair(importance,warning));
			}
		}

	  // Handle the case when there is one type in many places
	  // Note that we always ignore it if the "many places" type is NORMAL,
	  // instead we report on two most important "places" types

	  if(thePercentages[firsttype] >= manyplaces_limit &&
		 firsttype != NORMAL)
		{
		  sentence << warning_phrase(firsttype,
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
		  sentence << warning_phrase(firsttype,
									   thePercentages[firsttype],
									   generally_limit,
									   manyplaces_limit,
									   someplaces_limit);
		  return sentence;
		}

	  firsttype = someplacestypes.begin()->second;

	  sentence << warning_phrase(firsttype,
								   thePercentages[firsttype],
								   generally_limit,
								   manyplaces_limit,
								   someplaces_limit);

	  if(someplacestypes.size() > 1)
		{
		  RoadWarningType secondtype = (++someplacestypes.begin())->second;

		  if(firsttype == ICING)
			{
			  if(secondtype == PARTLY_ICING)
				sentence << "tai" << "osittain j‰isiksi";
			  else if(secondtype == FROSTY)
				sentence << "tai" << "kuuraisiksi";
			}
		}
	  return sentence;
	}

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on road warnings
   *
   * \return The story
   *
   *†\see page_roadcondition_warning
   */
  // ----------------------------------------------------------------------
  
  const Paragraph RoadStory::warning_shortview() const
  {
	MessageLogger log("RoadStory::warning_shortview");

	Paragraph paragraph;

	// the period we wish to analyze is at most 30 hours,
	// but of course must not exceed the period length itself


	const int maxhours = Settings::optional_int(itsVar+"::maxhours",30);

	const NFmiTime time1(itsPeriod.localStartTime());
	NFmiTime time2 = TimeTools::addHours(time1,maxhours);
	if(itsPeriod.localEndTime().IsLessThan(time2))
	  time2 = itsPeriod.localEndTime();

	const WeatherPeriod fullperiod(time1,time2);


	log << "Analysis period is " << time1 << " ... " << time2 << endl;

	// Calculate the results for each period

	HourPeriodGenerator generator(fullperiod, itsVar);

	if(generator.size()==0)
	  {
		log << "Analysis period is too short!" << endl;
		log << paragraph;
		return paragraph;
	  }

	// Calculate the percentages, initial sentences and their
	// dummy realizations

	vector<WeatherPeriod> periods;
	vector<Sentence> sentences;

	DebugTextFormatter formatter;

	log << "Individual period results:" << endl;
	for(unsigned int i=1; i<=generator.size(); i++)
	  {
		const WeatherPeriod period = generator.period(i);

		const WarningPercentages result = calculate_percentages(period,
									i,
									itsSources,
									itsArea,
									itsVar);


		for(int j=min_warning; j<=max_warning; j++)
		{
			if(result[j] == kFloatMissing)
			{
				Sentence s;
				s << "tieto puuttuu";
				paragraph << s;
				log << paragraph;
				return paragraph;
			}
		}

		const Sentence sentence = warning_sentence(result,itsVar);
		const string realization = formatter.format(sentence);

		periods.push_back(period);
		sentences.push_back(sentence);

		log << period.localStartTime()
			<< " ... "
			<< period.localEndTime()
			<< ": "
			<< realization
			<< endl;

		for(int j=min_warning; j<=max_warning; j++)
		  log << '\t'
			  << warning_name(RoadWarningType(j))
			  << '\t'
			  << result[j]
			  << " %"
			  << endl;
	}
 

	// Algorithm:
	// For each start period
	//   Find the number of similar periods
	//   Generate common text for the periods
	//   Skip the similar periods
	// Next

	// Some old date guaranteed to be different than any
	// period to be handled:

	NFmiTime last_mentioned_date(1970,1,1);

	Sentence sentence;

	for(unsigned int i=0; i<periods.size(); i++)
	  {
		sentence << sentences[i];

	  }
	paragraph << sentence;

	log << paragraph;
	return paragraph;

  }

} // namespace TextGen
  
// ======================================================================
